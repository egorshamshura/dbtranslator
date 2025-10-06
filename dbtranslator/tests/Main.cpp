#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <memory>
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <string>
#include <sys/types.h>
#include <unordered_map>
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

#include "Binary.h"
#include "CPU.h"
#include "Instruction.h"
#include "Memory.h"

using namespace llvm;
using namespace llvm::orc;

static constexpr size_t THRESHOLD = 16;

static ThreadSafeModule optimizeModuleSimple(ThreadSafeModule TSM) {
  TSM.withModuleDo([](Module &M) {
    auto FPM = std::make_unique<legacy::FunctionPassManager>(&M);
    FPM->add(createInstructionCombiningPass());
    FPM->add(createReassociatePass());
    FPM->add(createGVNPass());
    FPM->add(createCFGSimplificationPass());
    FPM->doInitialization();
    for (auto &F : M) FPM->run(F);
  });
  return TSM;
}

static void addMemoryInterface(riscv::IRData& Data) {
  Module& M = Data.Module;
  LLVMContext& Ctx = Data.Builder.getContext();
  auto *MemType = riscv::getMemoryType(Ctx);
  
  auto *Read8Ty = FunctionType::get(Type::getInt8Ty(Ctx), {riscv::getMemoryPointerType(Ctx), llvm::Type::getInt32Ty(Ctx)}, false);
  auto *Read16Ty = FunctionType::get(Type::getInt16Ty(Ctx), {riscv::getMemoryPointerType(Ctx), llvm::Type::getInt32Ty(Ctx)}, false);
  auto *Read32Ty = FunctionType::get(Type::getInt32Ty(Ctx), {riscv::getMemoryPointerType(Ctx), llvm::Type::getInt32Ty(Ctx)}, false);
  
  auto *Write8Ty = FunctionType::get(Type::getVoidTy(Ctx), {riscv::getMemoryPointerType(Ctx), llvm::Type::getInt32Ty(Ctx), llvm::Type::getInt8Ty(Ctx)}, false);
  auto *Write16Ty = FunctionType::get(Type::getVoidTy(Ctx), {riscv::getMemoryPointerType(Ctx), llvm::Type::getInt32Ty(Ctx), llvm::Type::getInt16Ty(Ctx)}, false);
  auto *Write32Ty = FunctionType::get(Type::getVoidTy(Ctx), {riscv::getMemoryPointerType(Ctx), llvm::Type::getInt32Ty(Ctx), llvm::Type::getInt32Ty(Ctx)}, false);

  Data.MemoryFunctions[0] = M.getOrInsertFunction("read8", Read8Ty);
  Data.MemoryFunctions[1] = M.getOrInsertFunction("read16", Read16Ty);
  Data.MemoryFunctions[2] = M.getOrInsertFunction("read32", Read32Ty);
  
  Data.MemoryFunctions[3] = M.getOrInsertFunction("write8", Write8Ty);
  Data.MemoryFunctions[4] = M.getOrInsertFunction("write16", Write16Ty);
  Data.MemoryFunctions[5] = M.getOrInsertFunction("write32", Write32Ty);
}

static std::string generateFunc(riscv::CPUState const* State, std::unordered_map<uint32_t, std::string>& PCToFunc, LLJIT& JIT) {

  auto CtxPtr = std::make_unique<LLVMContext>();
  auto MPtr   = std::make_unique<Module>("Module " + std::to_string(State->PC), *CtxPtr);

  LLVMContext& Ctx = *CtxPtr;
  Module& M = *MPtr;
  
  auto *cpuStructTy = riscv::getCPUStateType(Ctx);
  auto *regsArrTy   = ArrayType::get(Type::getInt32Ty(Ctx), 32);
  auto *cpuPtrTy = riscv::getCPUStatePointerType(Ctx);

  std::string FuncName = "block_" + std::to_string(State->PC);

  auto *fnTy = FunctionType::get(Type::getVoidTy(Ctx), {cpuPtrTy}, false);
  auto *F    = Function::Create(fnTy, Function::ExternalLinkage, FuncName, &M);
  
  PCToFunc.insert({State->PC, FuncName});

  llvm::IRBuilder<> B{Ctx};
  auto *BB = BasicBlock::Create(Ctx, "entry", F);
  B.SetInsertPoint(BB);
  riscv::IRData IRData_{M, B, F};
  addMemoryInterface(IRData_);
  bool Continue = true;
  uint32_t TempPC = State->PC;
  int NumInstrs = 0;
  while (Continue && NumInstrs < THRESHOLD) {
    uint32_t InstructionData = riscv::read32(State->Manager, TempPC);
    TempPC += 4;
    riscv::Instr CurrentInstruction = riscv::decode(InstructionData);
    std::cout << riscv::InstrToLiteral(CurrentInstruction) << std::endl;
    riscv::generate(CurrentInstruction, InstructionData, IRData_);
    ++NumInstrs;
    switch (CurrentInstruction) {
      case riscv::Instr::BEQ:
      case riscv::Instr::BNE:
      case riscv::Instr::BLT:
      case riscv::Instr::BGE:
      case riscv::Instr::BLTU:
      case riscv::Instr::BGEU:
      case riscv::Instr::JAL:
      case riscv::Instr::JALR:
        Continue = false;
        break;
      default:
        break;
    }
  }
  B.CreateRetVoid();
  ThreadSafeModule TSM(std::move(MPtr), std::move(CtxPtr));
  TSM = optimizeModuleSimple(std::move(TSM));

  TSM.getModuleUnlocked()->dump();
  if (auto Err = JIT.addIRModule(std::move(TSM))) {
    logAllUnhandledErrors(std::move(Err), errs(), "Error adding module: ");
    exit(200);
  }
  return FuncName;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    std::printf("Usage: %s <path-to-elf> <path-to-memory-impl>.\n", argv[0]);
    return EXIT_FAILURE;
  }

  InitLLVM X(argc, argv);
  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  InitializeNativeTargetAsmParser();

  using BlockFunc = void(*)(riscv::CPUState*);

  auto JITOrErr = LLJITBuilder().create();
  if (!JITOrErr) {
    std::printf("Error creating JIT.\n");
    return EXIT_FAILURE;
  }
  auto JIT = std::move(*JITOrErr);
  
  auto CtxPtr = std::make_unique<LLVMContext>();
  SMDiagnostic Err;
  auto M = parseIRFile(argv[2], Err, *CtxPtr);

  ThreadSafeModule TSM(std::move(M), std::move(CtxPtr));
  TSM = optimizeModuleSimple(std::move(TSM));

  TSM.getModuleUnlocked()->dump();
  if (auto Err = JIT->addIRModule(std::move(TSM))) {
    logAllUnhandledErrors(std::move(Err), errs(), "Error adding module: ");
    return EXIT_FAILURE;
  }

  riscv::MemoryManager* Manager;
  uint32_t EntryPoint;

  std::tie(Manager, EntryPoint) = riscv::parseElf(argv[1]);
  riscv::CPUState state{{}, EntryPoint, Manager};
  state.Registers[2] = -16;

  std::unordered_map<uint32_t, std::string> PCToFunc;
  int i = 100;
  while (i > 0) {
    auto FuncIt = PCToFunc.find(state.PC);
    if (FuncIt == PCToFunc.end()) {
      PCToFunc.insert({state.PC, generateFunc(&state, PCToFunc, *JIT.get())});
    }
    BlockFunc Fn = JIT->lookup(PCToFunc[state.PC])->toPtr<BlockFunc>();
    Fn(&state);
    --i;
  }
  riscv::dump(&state);
  return 0;
}
