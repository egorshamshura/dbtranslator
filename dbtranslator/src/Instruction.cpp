#include "Instruction.h"
#include "CPU.h"
#include <cstdint>
#include <llvm-20/llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

namespace riscv {

namespace {

static void updatePC(IRData& Data) {
  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *CPUArg = Data.CurrentFunction->getArg(0);
  llvm::Value *PCPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 1);
  llvm::Value *PCVal = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), PCPtr);  
  llvm::Value *NewPCVal = Data.Builder.CreateAdd(PCVal, Data.Builder.getInt32(4));
  Data.Builder.CreateStore(NewPCVal, PCPtr);
}

} // end anonymous namespace
  
char const* InstrToLiteral(Instr I) {
  switch (I) {
  case Instr::UNKNOWN:
    return "UNKNOWN";
  case Instr::LUI:
    return "LUI";
  case Instr::AUIPC:
    return "AUIPC";
  case Instr::JAL:
    return "JAL";
  case Instr::JALR:
    return "JALR";
  case Instr::BEQ:
    return "BEQ";
  case Instr::BNE:
    return "BNE";
  case Instr::BLT:
    return "BLT";
  case Instr::BGE:
    return "BGE";
  case Instr::BLTU:
    return "BLTU";
  case Instr::BGEU:
    return "BGEU";
  case Instr::LB:
    return "LB";
  case Instr::LH:
    return "LH";
  case Instr::LW:
    return "LW";
  case Instr::LBU:
    return "LBU";
  case Instr::LHU:
    return "LHU";
  case Instr::SB:
    return "SB";
  case Instr::SH:
    return "SH";
  case Instr::SW:
    return "SW";
  case Instr::ADDI:
    return "ADDI";
  case Instr::SLTI:
    return "SLTI";
  case Instr::SLTIU:
    return "SLTIU";
  case Instr::XORI:
    return "XORI";
  case Instr::ORI:
    return "ORI";
  case Instr::ANDI:
    return "ANDI";
  case Instr::SLLI:
    return "SLLI";
  case Instr::SRLI:
    return "SRLI";
  case Instr::SRAI:
    return "SRAI";
  case Instr::ADD:
    return "ADD";
  case Instr::SUB:
    return "SUB";
  case Instr::SLL:
    return "SLL";
  case Instr::SLT:
    return "SLT";
  case Instr::SLTU:
    return "SLTU";
  case Instr::XOR:
    return "XOR";
  case Instr::SRL:
    return "SRL";
  case Instr::SRA:
    return "SRA";
  case Instr::OR:
    return "OR";
  case Instr::AND:
    return "AND";
  case Instr::FENCE:
    return "FENCE";
  case Instr::FENCETSO:
    return "FENCE.TSO";
  case Instr::PAUSE:
    return "PAUSE";
  case Instr::ECALL:
    return "ECALL";
  case Instr::EBREAK:
    return "EBREAK";
  default:
    return "UNKNOWN";
  }
}

void LUIInstruction::build_ir(IRData& Data) {
  uint32_t Immediate = InstructionData & 0xFFFFF000;
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUPtrTy = riscv::getCPUStatePointerType(Data.Builder.getContext());
  auto *CPUArg = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);
  llvm::Value *RegDestPtr  = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                        {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  Data.Builder.CreateStore(Data.Builder.getInt32(Immediate), RegDestPtr);

  updatePC(Data);
}

void AUIPCInstruction::build_ir(IRData& Data) {
  uint32_t Immediate = InstructionData & 0xFFFFF000;
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUPtrTy = riscv::getCPUStatePointerType(Data.Builder.getContext());
  auto *CPUArg = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);
  llvm::Value *RegDestPtr  = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                        {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});

  llvm::Value *PCPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 1);
  llvm::Value *PCVal = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), PCPtr);                                      
  Data.Builder.CreateStore(Data.Builder.CreateAdd(Data.Builder.getInt32(Immediate), PCVal), RegDestPtr);
  
  llvm::Value *NewPCVal = Data.Builder.CreateAdd(PCVal, Data.Builder.getInt32(4));
  Data.Builder.CreateStore(NewPCVal, PCPtr);
}

void JALInstruction::build_ir(IRData& Data) {
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;

  uint32_t Offset = ((InstructionData >> 31) & 0x1) << 20
                | ((InstructionData >> 12) & 0xFF) << 12
                | ((InstructionData >> 20) & 0x1) << 11
                | ((InstructionData >> 21) & 0x3FF) << 1;

  if (Offset & 0x80000) {
    Offset |= 0xFFF00000;
  }

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUPtrTy = riscv::getCPUStatePointerType(Data.Builder.getContext());
  auto *CPUArg = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);
  llvm::Value *RegDestPtr  = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                        {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});

  llvm::Value *PCPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 1);
  llvm::Value *PCVal = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), PCPtr);                                      
  Data.Builder.CreateStore(Data.Builder.CreateAdd(Data.Builder.getInt32(4), PCVal), RegDestPtr);
  Data.Builder.CreateStore(Data.Builder.CreateAdd(Data.Builder.getInt32(Offset), PCVal), PCPtr);
}

void JALRInstruction::build_ir(IRData& Data) {
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc  = (InstructionData >> 15) & 0x1F;
  uint32_t Offset  = (InstructionData >> 20) & 0xFFF;

  if (Offset & 0x800) {
    Offset |= 0xFFFFF000;
  }

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUPtrTy    = riscv::getCPUStatePointerType(Data.Builder.getContext());
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);
  llvm::Value *RegSrcPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc)});
  llvm::Value *RegSrcVal = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrcPtr);

  llvm::Value *PCPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 1);
  llvm::Value *PCVal = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), PCPtr);

  llvm::Value *RegDestPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  Data.Builder.CreateStore(Data.Builder.CreateAdd(PCVal, Data.Builder.getInt32(4)), RegDestPtr);

  llvm::Value *Target = Data.Builder.CreateAdd(RegSrcVal, Data.Builder.getInt32(Offset));
  llvm::Value *TargetAligned = Data.Builder.CreateAnd(Target, Data.Builder.getInt32(~1U));
  Data.Builder.CreateStore(TargetAligned, PCPtr);
}

void BEQInstruction::build_ir(IRData& Data) {
  uint32_t RegSrc1 = (InstructionData >> 15) & 0x1F;
  uint32_t RegSrc2 = (InstructionData >> 20) & 0x1F;
  
  uint32_t Offset = ((InstructionData >> 31) & 0x1) << 12
                  | ((InstructionData >> 7) & 0x1) << 11
                  | ((InstructionData >> 25) & 0x3F) << 5
                  | ((InstructionData >> 8) & 0xF) << 1;
  if (Offset & 0x1000) {
    Offset |= 0xFFFFE000;
  }

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);
  llvm::Value *Reg1Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc1)});
  llvm::Value *Reg2Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc2)});
  
  llvm::Value *Reg1Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), Reg1Ptr);
  llvm::Value *Reg2Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), Reg2Ptr);

  llvm::Value *PCPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 1);
  llvm::Value *PCVal = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), PCPtr);

  llvm::Value *Cond = Data.Builder.CreateICmpEQ(Reg1Val, Reg2Val);
  llvm::Value *PCPlusOffset = Data.Builder.CreateAdd(PCVal, Data.Builder.getInt32(Offset));
  llvm::Value *PCNext = Data.Builder.CreateSelect(Cond, PCPlusOffset,
                                                  Data.Builder.CreateAdd(PCVal, Data.Builder.getInt32(4)));
  Data.Builder.CreateStore(PCNext, PCPtr);
}

void BNEInstruction::build_ir(IRData& Data) {
  uint32_t RegSrc1 = (InstructionData >> 15) & 0x1F;
  uint32_t RegSrc2 = (InstructionData >> 20) & 0x1F;
  
  uint32_t Offset = ((InstructionData >> 31) & 0x1) << 12
                  | ((InstructionData >> 7) & 0x1) << 11
                  | ((InstructionData >> 25) & 0x3F) << 5
                  | ((InstructionData >> 8) & 0xF) << 1;
  if (Offset & 0x1000) {
    Offset |= 0xFFFFE000;
  }

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);
  llvm::Value *Reg1Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc1)});
  llvm::Value *Reg2Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc2)});
  
  llvm::Value *Reg1Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), Reg1Ptr);
  llvm::Value *Reg2Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), Reg2Ptr);

  llvm::Value *PCPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 1);
  llvm::Value *PCVal = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), PCPtr);

  llvm::Value *Cond = Data.Builder.CreateICmpNE(Reg1Val, Reg2Val);
  llvm::Value *PCPlusOffset = Data.Builder.CreateAdd(PCVal, Data.Builder.getInt32(Offset));
  llvm::Value *PCNext = Data.Builder.CreateSelect(Cond, PCPlusOffset,
                                                  Data.Builder.CreateAdd(PCVal, Data.Builder.getInt32(4)));
  Data.Builder.CreateStore(PCNext, PCPtr);
}

void BLTInstruction::build_ir(IRData& Data) {
  uint32_t RegSrc1 = (InstructionData >> 15) & 0x1F;
  uint32_t RegSrc2 = (InstructionData >> 20) & 0x1F;
  
  uint32_t Offset = ((InstructionData >> 31) & 0x1) << 12
                  | ((InstructionData >> 7) & 0x1) << 11
                  | ((InstructionData >> 25) & 0x3F) << 5
                  | ((InstructionData >> 8) & 0xF) << 1;
  if (Offset & 0x1000) {
    Offset |= 0xFFFFE000;
  }

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);
  llvm::Value *Reg1Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc1)});
  llvm::Value *Reg2Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc2)});
  
  llvm::Value *Reg1Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), Reg1Ptr);
  llvm::Value *Reg2Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), Reg2Ptr);

  llvm::Value *PCPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 1);
  llvm::Value *PCVal = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), PCPtr);

  llvm::Value *Cond = Data.Builder.CreateICmpSLT(Reg1Val, Reg2Val);
  llvm::Value *PCPlusOffset = Data.Builder.CreateAdd(PCVal, Data.Builder.getInt32(Offset));
  llvm::Value *PCNext = Data.Builder.CreateSelect(Cond, PCPlusOffset,
                                                  Data.Builder.CreateAdd(PCVal, Data.Builder.getInt32(4)));
  Data.Builder.CreateStore(PCNext, PCPtr);
}

void BGEInstruction::build_ir(IRData& Data) {
  uint32_t RegSrc1 = (InstructionData >> 15) & 0x1F;
  uint32_t RegSrc2 = (InstructionData >> 20) & 0x1F;
  
  uint32_t Offset = ((InstructionData >> 31) & 0x1) << 12
                  | ((InstructionData >> 7) & 0x1) << 11
                  | ((InstructionData >> 25) & 0x3F) << 5
                  | ((InstructionData >> 8) & 0xF) << 1;
  if (Offset & 0x1000) {
    Offset |= 0xFFFFE000;
  }

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);
  llvm::Value *Reg1Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc1)});
  llvm::Value *Reg2Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc2)});
  
  llvm::Value *Reg1Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), Reg1Ptr);
  llvm::Value *Reg2Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), Reg2Ptr);

  llvm::Value *PCPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 1);
  llvm::Value *PCVal = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), PCPtr);

  llvm::Value *Cond = Data.Builder.CreateICmpSGE(Reg1Val, Reg2Val);
  llvm::Value *PCPlusOffset = Data.Builder.CreateAdd(PCVal, Data.Builder.getInt32(Offset));
  llvm::Value *PCNext = Data.Builder.CreateSelect(Cond, PCPlusOffset,
                                                  Data.Builder.CreateAdd(PCVal, Data.Builder.getInt32(4)));
  Data.Builder.CreateStore(PCNext, PCPtr);
}

void BLTUInstruction::build_ir(IRData& Data) {
  uint32_t RegSrc1 = (InstructionData >> 15) & 0x1F;
  uint32_t RegSrc2 = (InstructionData >> 20) & 0x1F;
  
  uint32_t Offset = ((InstructionData >> 31) & 0x1) << 12
                  | ((InstructionData >> 7) & 0x1) << 11
                  | ((InstructionData >> 25) & 0x3F) << 5
                  | ((InstructionData >> 8) & 0xF) << 1;
  if (Offset & 0x1000) {
    Offset |= 0xFFFFE000;
  }

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);
  llvm::Value *Reg1Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc1)});
  llvm::Value *Reg2Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc2)});
  
  llvm::Value *Reg1Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), Reg1Ptr);
  llvm::Value *Reg2Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), Reg2Ptr);

  llvm::Value *PCPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 1);
  llvm::Value *PCVal = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), PCPtr);

  llvm::Value *Cond = Data.Builder.CreateICmpULT(Reg1Val, Reg2Val);
  llvm::Value *PCPlusOffset = Data.Builder.CreateAdd(PCVal, Data.Builder.getInt32(Offset));
  llvm::Value *PCNext = Data.Builder.CreateSelect(Cond, PCPlusOffset,
                                                  Data.Builder.CreateAdd(PCVal, Data.Builder.getInt32(4)));
  Data.Builder.CreateStore(PCNext, PCPtr);
} 

void BGEUInstruction::build_ir(IRData& Data) {
  uint32_t RegSrc1 = (InstructionData >> 15) & 0x1F;
  uint32_t RegSrc2 = (InstructionData >> 20) & 0x1F;
  
  uint32_t Offset = ((InstructionData >> 31) & 0x1) << 12
                  | ((InstructionData >> 7) & 0x1) << 11
                  | ((InstructionData >> 25) & 0x3F) << 5
                  | ((InstructionData >> 8) & 0xF) << 1;
  if (Offset & 0x1000) {
    Offset |= 0xFFFFE000;
  }

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);
  llvm::Value *Reg1Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc1)});
  llvm::Value *Reg2Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc2)});
  
  llvm::Value *Reg1Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), Reg1Ptr);
  llvm::Value *Reg2Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), Reg2Ptr);

  llvm::Value *PCPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 1);
  llvm::Value *PCVal = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), PCPtr);

  llvm::Value *Cond = Data.Builder.CreateICmpUGE(Reg1Val, Reg2Val);
  llvm::Value *PCPlusOffset = Data.Builder.CreateAdd(PCVal, Data.Builder.getInt32(Offset));
  llvm::Value *PCNext = Data.Builder.CreateSelect(Cond, PCPlusOffset,
                                                  Data.Builder.CreateAdd(PCVal, Data.Builder.getInt32(4)));
  Data.Builder.CreateStore(PCNext, PCPtr);
}

void LBInstruction::build_ir(IRData& Data) {
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc = (InstructionData >> 15) & 0x1F;
  uint32_t Offset = (InstructionData >> 20) & 0xFFF;
  
  if (Offset & 0x800) {
    Offset |= 0xFFFFF000;
  }
  
  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);
  llvm::Value *MemoryManagerPtrPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 2);
  llvm::Value *MemoryManagerPtr = Data.Builder.CreateLoad(Data.Builder.getPtrTy(), MemoryManagerPtrPtr);
  llvm::Value *RegDestPtr  = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                        {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  llvm::Value *RegSrcPtr  = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                        {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc)});
  llvm::Value *RegSrcVal = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrcPtr);
  llvm::Value *Address = Data.Builder.CreateAdd(RegSrcVal, Data.Builder.getInt32(Offset));
  llvm::Value *ReadMemory = Data.Builder.CreateCall(Data.MemoryFunctions[0], {MemoryManagerPtr, Address});
  
  Data.Builder.CreateStore(Data.Builder.CreateSExt(ReadMemory, Data.Builder.getInt32Ty()), RegDestPtr);
  
  updatePC(Data);
}

void LHInstruction::build_ir(IRData& Data) {
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc = (InstructionData >> 15) & 0x1F;
  uint32_t Offset = (InstructionData >> 20) & 0xFFF;
  
  if (Offset & 0x800) {
    Offset |= 0xFFFFF000;
  }
  
  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);
  llvm::Value *MemoryManagerPtrPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 2);
  llvm::Value *MemoryManagerPtr = Data.Builder.CreateLoad(Data.Builder.getPtrTy(), MemoryManagerPtrPtr);
  llvm::Value *RegDestPtr  = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                        {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  llvm::Value *RegSrcPtr  = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                        {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc)});
  llvm::Value *RegSrcVal = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrcPtr);
  llvm::Value *Address = Data.Builder.CreateAdd(RegSrcVal, Data.Builder.getInt32(Offset));
  llvm::Value *ReadMemory = Data.Builder.CreateCall(Data.MemoryFunctions[1], {MemoryManagerPtr, Address});
  
  Data.Builder.CreateStore(Data.Builder.CreateSExt(ReadMemory, Data.Builder.getInt32Ty()), RegDestPtr);
  
  updatePC(Data);
}

void LWInstruction::build_ir(IRData& Data) {
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc = (InstructionData >> 15) & 0x1F;
  uint32_t Offset = (InstructionData >> 20) & 0xFFF;
  
  if (Offset & 0x800) {
    Offset |= 0xFFFFF000;
  }
  
  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);
  llvm::Value *MemoryManagerPtrPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 2);
  llvm::Value *MemoryManagerPtr = Data.Builder.CreateLoad(Data.Builder.getPtrTy(), MemoryManagerPtrPtr);
  llvm::Value *RegDestPtr  = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                        {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  llvm::Value *RegSrcPtr  = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                        {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc)});
  llvm::Value *RegSrcVal = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrcPtr);
  llvm::Value *Address = Data.Builder.CreateAdd(RegSrcVal, Data.Builder.getInt32(Offset));
  llvm::Value *ReadMemory = Data.Builder.CreateCall(Data.MemoryFunctions[2], {MemoryManagerPtr, Address});
  
  Data.Builder.CreateStore(Data.Builder.CreateSExt(ReadMemory, Data.Builder.getInt32Ty()), RegDestPtr);
  
  updatePC(Data);
}

void LBUInstruction::build_ir(IRData& Data) {
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc = (InstructionData >> 15) & 0x1F;
  uint32_t Offset = (InstructionData >> 20) & 0xFFF;
  
  if (Offset & 0x800) {
    Offset |= 0xFFFFF000;
  }

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);
  llvm::Value *MemoryManagerPtrPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 2);
  llvm::Value *MemoryManagerPtr = Data.Builder.CreateLoad(Data.Builder.getPtrTy(), MemoryManagerPtrPtr);
  llvm::Value *RegDestPtr  = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                        {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  llvm::Value *RegSrcPtr  = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                        {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc)});
  llvm::Value *RegSrcVal = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrcPtr);
  llvm::Value *Address = Data.Builder.CreateAdd(RegSrcVal, Data.Builder.getInt32(Offset));
  llvm::Value *ReadMemory = Data.Builder.CreateCall(Data.MemoryFunctions[0], {MemoryManagerPtr, Address});
  
  Data.Builder.CreateStore(Data.Builder.CreateZExt(ReadMemory, Data.Builder.getInt32Ty()), RegDestPtr);
  
  updatePC(Data);
}

void LHUInstruction::build_ir(IRData& Data) {
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc = (InstructionData >> 15) & 0x1F;
  uint32_t Offset = (InstructionData >> 20) & 0xFFF;
  
  if (Offset & 0x800) {
    Offset |= 0xFFFFF000;
  }
  
  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);
  llvm::Value *MemoryManagerPtrPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 2);
  llvm::Value *MemoryManagerPtr = Data.Builder.CreateLoad(Data.Builder.getPtrTy(), MemoryManagerPtrPtr);
  llvm::Value *RegDestPtr  = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                        {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  llvm::Value *RegSrcPtr  = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                        {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc)});
  llvm::Value *RegSrcVal = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrcPtr);
  llvm::Value *Address = Data.Builder.CreateAdd(RegSrcVal, Data.Builder.getInt32(Offset));
  llvm::Value *ReadMemory = Data.Builder.CreateCall(Data.MemoryFunctions[1], {MemoryManagerPtr, Address});
  
  Data.Builder.CreateStore(Data.Builder.CreateZExt(ReadMemory, Data.Builder.getInt32Ty()), RegDestPtr);
  
  updatePC(Data);
}

void SBInstruction::build_ir(IRData& Data) {
  uint32_t RegSrc1 = (InstructionData >> 15) & 0x1F;
  uint32_t RegSrc2 = (InstructionData >> 20) & 0x1F;
  
  uint32_t Offset = ((InstructionData >> 25) & 0x7F) << 5
                  | ((InstructionData >> 7)  & 0x1F);
  
  if (Offset & 0x800) {
    Offset |= 0xFFFFF000;
  }
  
  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);
  llvm::Value *MemoryManagerPtrPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 2);
  llvm::Value *RegSrc1Ptr  = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                        {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc1)});
  llvm::Value *RegSrc2Ptr  = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                        {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc2)});

                                          
  llvm::Value *RegSrc1Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrc1Ptr);
  llvm::Value *RegSrc2Val = Data.Builder.CreateLoad(Data.Builder.getInt8Ty(), RegSrc2Ptr);
  llvm::Value *MemoryManagerPtr = Data.Builder.CreateLoad(Data.Builder.getPtrTy(), MemoryManagerPtrPtr);
  llvm::Value *Address = Data.Builder.CreateAdd(RegSrc1Val, Data.Builder.getInt32(Offset));

  Data.Builder.CreateCall(Data.MemoryFunctions[3], {MemoryManagerPtr, Address, RegSrc2Val});

  updatePC(Data);
}

void SHInstruction::build_ir(IRData& Data) {
  uint32_t RegSrc1 = (InstructionData >> 15) & 0x1F;
  uint32_t RegSrc2 = (InstructionData >> 20) & 0x1F;
  
  uint32_t Offset = ((InstructionData >> 25) & 0x7F) << 5
                  | ((InstructionData >> 7)  & 0x1F);
  
  if (Offset & 0x800) {
    Offset |= 0xFFFFF000;
  }
  
  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);
  llvm::Value *MemoryManagerPtrPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 2);
  llvm::Value *RegSrc1Ptr  = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                        {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc1)});
  llvm::Value *RegSrc2Ptr  = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                        {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc2)});

                                          
  llvm::Value *RegSrc1Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrc1Ptr);
  llvm::Value *RegSrc2Val = Data.Builder.CreateLoad(Data.Builder.getInt16Ty(), RegSrc2Ptr);
  llvm::Value *MemoryManagerPtr = Data.Builder.CreateLoad(Data.Builder.getPtrTy(), MemoryManagerPtrPtr);
  llvm::Value *Address = Data.Builder.CreateAdd(RegSrc1Val, Data.Builder.getInt32(Offset));

  Data.Builder.CreateCall(Data.MemoryFunctions[4], {MemoryManagerPtr, Address, RegSrc2Val});

  updatePC(Data);
}

void SWInstruction::build_ir(IRData& Data) {
  uint32_t RegSrc1 = (InstructionData >> 15) & 0x1F;
  uint32_t RegSrc2 = (InstructionData >> 20) & 0x1F;
  
  uint32_t Offset = ((InstructionData >> 25) & 0x7F) << 5
                  | ((InstructionData >> 7)  & 0x1F);
  
  if (Offset & 0x800) {
    Offset |= 0xFFFFF000;
  }

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);
  llvm::Value *MemoryManagerPtrPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 2);
  llvm::Value *RegSrc1Ptr  = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                        {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc1)});
  llvm::Value *RegSrc2Ptr  = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                        {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc2)});

                                          
  llvm::Value *RegSrc1Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrc1Ptr);
  llvm::Value *RegSrc2Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrc2Ptr);
  llvm::Value *MemoryManagerPtr = Data.Builder.CreateLoad(Data.Builder.getPtrTy(), MemoryManagerPtrPtr);
  llvm::Value *Address = Data.Builder.CreateAdd(RegSrc1Val, Data.Builder.getInt32(Offset));

  Data.Builder.CreateCall(Data.MemoryFunctions[5], {MemoryManagerPtr, Address, RegSrc2Val});

  updatePC(Data);
}

void ADDIInstruction::build_ir(IRData& Data) {
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc  = (InstructionData >> 15) & 0x1F;
  uint32_t Imm      = (InstructionData >> 20) & 0xFFF;

  if (Imm & 0x800) {
    Imm |= 0xFFFFF000;
  }

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);

  llvm::Value *RegSrcPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc)});
  llvm::Value *RegSrcVal = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrcPtr);

  llvm::Value *Result = Data.Builder.CreateAdd(RegSrcVal, Data.Builder.getInt32(Imm));

  llvm::Value *RegDestPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  Data.Builder.CreateStore(Result, RegDestPtr);

  updatePC(Data);
}

void SLTIInstruction::build_ir(IRData& Data) {
  uint32_t Immediate = (InstructionData >> 20) & 0xFFF;
  if (Immediate & 0x800) {
    Immediate |= 0xFFFFF000;
  }
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc = (InstructionData >> 15) & 0x1F;

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);
  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);

  llvm::Value *RegDestPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  llvm::Value *RegSrcPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc)});
  llvm::Value *SrcValue = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrcPtr);  
  llvm::Value *Cond = Data.Builder.CreateICmpSLT(SrcValue, Data.Builder.getInt32(Immediate));
  Data.Builder.CreateStore(Data.Builder.CreateZExt(Cond, Data.Builder.getInt32Ty()), RegDestPtr);

  updatePC(Data);
}

void SLTIUInstruction::build_ir(IRData& Data) {
  uint32_t Immediate = (InstructionData >> 20) & 0xFFF;
  if (Immediate & 0x800) {
    Immediate |= 0xFFFFF000;
  }
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc = (InstructionData >> 15) & 0x1F;

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);
  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);

  llvm::Value *RegDestPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  llvm::Value *RegSrcPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc)});
  llvm::Value *SrcValue = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrcPtr);  
  llvm::Value *Cond = Data.Builder.CreateICmpULT(SrcValue, Data.Builder.getInt32(Immediate));
  Data.Builder.CreateStore(Data.Builder.CreateZExt(Cond, Data.Builder.getInt32Ty()), RegDestPtr);

  updatePC(Data);
}

void XORIInstruction::build_ir(IRData& Data) {
  uint32_t Immediate = (InstructionData >> 20) & 0xFFF;
  if (Immediate & 0x800) {
    Immediate |= 0xFFFFF000;
  }
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc = (InstructionData >> 15) & 0x1F;

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);

  llvm::Value *RegSrcPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc)});
  llvm::Value *RegSrcVal = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrcPtr);

  llvm::Value *Result = Data.Builder.CreateXor(RegSrcVal, Data.Builder.getInt32(Immediate));

  llvm::Value *RegDestPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  Data.Builder.CreateStore(Result, RegDestPtr);

  updatePC(Data);
}

void ORIInstruction::build_ir(IRData& Data) {
  uint32_t Immediate = (InstructionData >> 20) & 0xFFF;
  if (Immediate & 0x800) {
    Immediate |= 0xFFFFF000;
  }
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc = (InstructionData >> 15) & 0x1F;

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);

  llvm::Value *RegSrcPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc)});
  llvm::Value *RegSrcVal = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrcPtr);

  llvm::Value *Result = Data.Builder.CreateOr(RegSrcVal, Data.Builder.getInt32(Immediate));

  llvm::Value *RegDestPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  Data.Builder.CreateStore(Result, RegDestPtr);

  updatePC(Data);
}

void ANDIInstruction::build_ir(IRData& Data) {
  uint32_t Immediate = (InstructionData >> 20) & 0xFFF;
  if (Immediate & 0x800) {
    Immediate |= 0xFFFFF000;
  }
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc = (InstructionData >> 15) & 0x1F;

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);
  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);

  llvm::Value *RegSrcPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc)});
  llvm::Value *RegSrcVal = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrcPtr);

  llvm::Value *Result = Data.Builder.CreateAnd(RegSrcVal, Data.Builder.getInt32(Immediate));

  llvm::Value *RegDestPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  Data.Builder.CreateStore(Result, RegDestPtr);

  updatePC(Data);
}

void SLLIInstruction::build_ir(IRData& Data) {
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc = (InstructionData >> 15) & 0x1F;
  uint32_t Shamt = (InstructionData >> 20) & 0x1F;

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);
  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);

  llvm::Value *RegSrcPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc)});
  llvm::Value *RegSrcVal = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrcPtr);

  llvm::Value *Result = Data.Builder.CreateShl(RegSrcVal, Data.Builder.getInt32(Shamt));

  llvm::Value *RegDestPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  Data.Builder.CreateStore(Result, RegDestPtr);

  updatePC(Data);
}

void SRLIInstruction::build_ir(IRData& Data) {
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc = (InstructionData >> 15) & 0x1F;
  uint32_t Shamt = (InstructionData >> 20) & 0x1F;

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);
  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);

  llvm::Value *RegSrcPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc)});
  llvm::Value *RegSrcVal = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrcPtr);

  llvm::Value *Result = Data.Builder.CreateLShr(RegSrcVal, Data.Builder.getInt32(Shamt));

  llvm::Value *RegDestPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  Data.Builder.CreateStore(Result, RegDestPtr);

  updatePC(Data);
}

void SRAIInstruction::build_ir(IRData& Data) {
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc = (InstructionData >> 15) & 0x1F;
  uint32_t Shamt = (InstructionData >> 20) & 0x1F;

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);
  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);

  llvm::Value *RegSrcPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc)});
  llvm::Value *RegSrcVal = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrcPtr);

  llvm::Value *Result = Data.Builder.CreateAShr(RegSrcVal, Data.Builder.getInt32(Shamt));

  llvm::Value *RegDestPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  Data.Builder.CreateStore(Result, RegDestPtr);

  updatePC(Data);
}

void ADDInstruction::build_ir(IRData& Data) {
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc1 = (InstructionData >> 15) & 0x1F;
  uint32_t RegSrc2 = (InstructionData >> 20) & 0x1F;

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);

  llvm::Value *Reg1Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc1)});
  llvm::Value *Reg2Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc2)});
  llvm::Value *Reg1Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), Reg1Ptr);
  llvm::Value *Reg2Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), Reg2Ptr);

  llvm::Value *Result = Data.Builder.CreateAdd(Reg1Val, Reg2Val);

  llvm::Value *RegDestPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  Data.Builder.CreateStore(Result, RegDestPtr);

  updatePC(Data);
}

void SUBInstruction::build_ir(IRData& Data) {
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc1 = (InstructionData >> 15) & 0x1F;
  uint32_t RegSrc2 = (InstructionData >> 20) & 0x1F;

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);

  llvm::Value *Reg1Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc1)});
  llvm::Value *Reg2Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc2)});
  llvm::Value *Reg1Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), Reg1Ptr);
  llvm::Value *Reg2Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), Reg2Ptr);

  llvm::Value *Result = Data.Builder.CreateSub(Reg1Val, Reg2Val);

  llvm::Value *RegDestPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  Data.Builder.CreateStore(Result, RegDestPtr);

  updatePC(Data);
}

void SLLInstruction::build_ir(IRData& Data) {
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc1 = (InstructionData >> 15) & 0x1F;
  uint32_t RegSrc2 = (InstructionData >> 20) & 0x1F;

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);
  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);

  llvm::Value *RegSrc1Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc1)});
  llvm::Value *RegSrc1Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrc1Ptr);
  llvm::Value *RegSrc2Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc2)});
  llvm::Value *RegSrc2Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrc2Ptr);

  llvm::Value *Result = Data.Builder.CreateShl(RegSrc1Val, RegSrc2Val);

  llvm::Value *RegDestPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  Data.Builder.CreateStore(Result, RegDestPtr);

  updatePC(Data);
}

void SRLInstruction::build_ir(IRData& Data) {
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc1 = (InstructionData >> 15) & 0x1F;
  uint32_t RegSrc2 = (InstructionData >> 20) & 0x1F;

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);
  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);

  llvm::Value *RegSrc1Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc1)});
  llvm::Value *RegSrc1Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrc1Ptr);
  llvm::Value *RegSrc2Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc2)});
  llvm::Value *RegSrc2Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrc2Ptr);

  llvm::Value *Result = Data.Builder.CreateLShr(RegSrc1Val, RegSrc2Val);

  llvm::Value *RegDestPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  Data.Builder.CreateStore(Result, RegDestPtr);

  updatePC(Data);
}

void SRAInstruction::build_ir(IRData& Data) {
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc1 = (InstructionData >> 15) & 0x1F;
  uint32_t RegSrc2 = (InstructionData >> 20) & 0x1F;

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);
  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);

  llvm::Value *RegSrc1Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc1)});
  llvm::Value *RegSrc1Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrc1Ptr);
  llvm::Value *RegSrc2Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc2)});
  llvm::Value *RegSrc2Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrc2Ptr);

  llvm::Value *Result = Data.Builder.CreateAShr(RegSrc1Val, RegSrc2Val);

  llvm::Value *RegDestPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  Data.Builder.CreateStore(Result, RegDestPtr);

  updatePC(Data);
}

void XORInstruction::build_ir(IRData& Data) {
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc1 = (InstructionData >> 15) & 0x1F;
  uint32_t RegSrc2 = (InstructionData >> 20) & 0x1F;

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);

  llvm::Value *Reg1Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc1)});
  llvm::Value *Reg2Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc2)});
  llvm::Value *Reg1Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), Reg1Ptr);
  llvm::Value *Reg2Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), Reg2Ptr);

  llvm::Value *Result = Data.Builder.CreateXor(Reg1Val, Reg2Val);

  llvm::Value *RegDestPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  Data.Builder.CreateStore(Result, RegDestPtr);

  updatePC(Data);
}

void ORInstruction::build_ir(IRData& Data) {
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc1 = (InstructionData >> 15) & 0x1F;
  uint32_t RegSrc2 = (InstructionData >> 20) & 0x1F;

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);

  llvm::Value *Reg1Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc1)});
  llvm::Value *Reg2Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc2)});
  llvm::Value *Reg1Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), Reg1Ptr);
  llvm::Value *Reg2Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), Reg2Ptr);

  llvm::Value *Result = Data.Builder.CreateOr(Reg1Val, Reg2Val);

  llvm::Value *RegDestPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  Data.Builder.CreateStore(Result, RegDestPtr);

  updatePC(Data);
}

void ANDInstruction::build_ir(IRData& Data) {
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc1 = (InstructionData >> 15) & 0x1F;
  uint32_t RegSrc2 = (InstructionData >> 20) & 0x1F;

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);

  llvm::Value *Reg1Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc1)});
  llvm::Value *Reg2Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc2)});
  llvm::Value *Reg1Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), Reg1Ptr);
  llvm::Value *Reg2Val = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), Reg2Ptr);

  llvm::Value *Result = Data.Builder.CreateAnd(Reg1Val, Reg2Val);

  llvm::Value *RegDestPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  Data.Builder.CreateStore(Result, RegDestPtr);

  updatePC(Data);
}

void SLTInstruction::build_ir(IRData& Data) {
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc1 = (InstructionData >> 15) & 0x1F;
  uint32_t RegSrc2 = (InstructionData >> 20) & 0x1F;

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);
  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);

  llvm::Value *RegDestPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  llvm::Value *RegSrc1Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc1)});
  llvm::Value *Src1Value = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrc1Ptr);  
  llvm::Value *RegSrc2Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc1)});
  llvm::Value *Src2Value = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrc2Ptr); 
  llvm::Value *Cond = Data.Builder.CreateICmpSLT(Src1Value, Src2Value);
  Data.Builder.CreateStore(Data.Builder.CreateZExt(Cond, Data.Builder.getInt32Ty()), RegDestPtr);

  updatePC(Data); 
}

void SLTUInstruction::build_ir(IRData& Data) {
  uint32_t RegDest = (InstructionData >> 7) & 0x1F;
  uint32_t RegSrc1 = (InstructionData >> 15) & 0x1F;
  uint32_t RegSrc2 = (InstructionData >> 20) & 0x1F;

  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);
  llvm::Value *RegsPtr = Data.Builder.CreateStructGEP(CPUStructTy, CPUArg, 0);

  llvm::Value *RegDestPtr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegDest)});
  llvm::Value *RegSrc1Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc1)});
  llvm::Value *Src1Value = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrc1Ptr);  
  llvm::Value *RegSrc2Ptr = Data.Builder.CreateInBoundsGEP(RegsArrTy, RegsPtr,
                                      {Data.Builder.getInt32(0), Data.Builder.getInt32(RegSrc1)});
  llvm::Value *Src2Value = Data.Builder.CreateLoad(Data.Builder.getInt32Ty(), RegSrc2Ptr); 
  llvm::Value *Cond = Data.Builder.CreateICmpULT(Src1Value, Src2Value);
  Data.Builder.CreateStore(Data.Builder.CreateZExt(Cond, Data.Builder.getInt32Ty()), RegDestPtr);

  updatePC(Data);
}

void FENCEInstruction::build_ir(IRData& Data) {
  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  updatePC(Data);
}
void FENCETSOInstruction::build_ir(IRData& Data) {
  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  updatePC(Data); 
}
void PAUSEInstruction::build_ir(IRData& Data) {
  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  updatePC(Data);
}
void ECALLInstruction::build_ir(IRData& Data) {}
void EBREAKInstruction::build_ir(IRData& Data) {
  auto *CPUStructTy = riscv::getCPUStateType(Data.Builder.getContext());
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Data.Builder.getContext()), 32);
  auto *CPUArg      = Data.CurrentFunction->getArg(0);

  updatePC(Data);
}

namespace {
constexpr uint32_t opcode(uint32_t instr) { return instr & 0x7F; }
constexpr uint32_t funct3(uint32_t instr) { return (instr >> 12) & 0x7; }
constexpr uint32_t funct7(uint32_t instr) { return (instr >> 25) & 0x7F; }
} // end anonymous namespace

Instr decode(uint32_t InstructionData) {
  uint32_t op = opcode(InstructionData);
  uint32_t f3 = funct3(InstructionData);
  uint32_t f7 = funct7(InstructionData);
  
  switch (op) {
    case 0x37: return Instr::LUI;
    case 0x17: return Instr::AUIPC;
    
    case 0x6F: return Instr::JAL;
    
    case 0x67: 
      if (f3 == 0x0) return Instr::JALR;
      break;
      
    case 0x03:
      switch (f3) {
        case 0x0: return Instr::LB;
        case 0x1: return Instr::LH;
        case 0x2: return Instr::LW;
        case 0x4: return Instr::LBU;
        case 0x5: return Instr::LHU;
      }
      break;
    
    case 0x13:
      switch (f3) {
        case 0x0: return Instr::ADDI;
        case 0x1: 
          if ((InstructionData >> 25) == 0x00) return Instr::SLLI;
          break;
        case 0x2: return Instr::SLTI;
        case 0x3: return Instr::SLTIU;
        case 0x4: return Instr::XORI;
        case 0x5:
          if ((InstructionData >> 25) == 0x00) return Instr::SRLI;
          else if ((InstructionData >> 25) == 0x20) return Instr::SRAI;
          break;
        case 0x6: return Instr::ORI;
        case 0x7: return Instr::ANDI;
      }
      break;
    
    case 0x63:
      switch (f3) {
        case 0x0: return Instr::BEQ;
        case 0x1: return Instr::BNE;
        case 0x4: return Instr::BLT;
        case 0x5: return Instr::BGE;
        case 0x6: return Instr::BLTU;
        case 0x7: return Instr::BGEU;
      }
      break;
    
    case 0x23:
      switch (f3) {
        case 0x0: return Instr::SB;
        case 0x1: return Instr::SH;
        case 0x2: return Instr::SW;
      }
      break;
    
    case 0x33:
      switch (f3) {
        case 0x0:
          if (f7 == 0x00) return Instr::ADD;
          else if (f7 == 0x20) return Instr::SUB;
          break;
        case 0x1: return Instr::SLL;
        case 0x2: return Instr::SLT;
        case 0x3: return Instr::SLTU;
        case 0x4: return Instr::XOR;
        case 0x5:
          if (f7 == 0x00) return Instr::SRL;
          else if (f7 == 0x20) return Instr::SRA;
          break;
        case 0x6: return Instr::OR;
        case 0x7: return Instr::AND;
      }
      break;
    
    case 0x0F:
      if (f3 == 0x0) {
        uint32_t fm = (InstructionData >> 28) & 0xF;
        uint32_t pred = (InstructionData >> 24) & 0xF;
        uint32_t succ = (InstructionData >> 20) & 0xF;
        
        if (fm == 0x8 && pred == 0x3 && succ == 0x3) {
          return Instr::FENCETSO;
        }
        
        if ((InstructionData & 0xFFFFF000) == 0x0000100F) {
          return Instr::PAUSE;
        }
        
        return Instr::FENCE;
      }
      break;
    
    case 0x73:
      if (f3 == 0x0) {
        uint32_t imm12 = (InstructionData >> 20) & 0xFFF;
        if (imm12 == 0x000) return Instr::ECALL;
        else if (imm12 == 0x001) return Instr::EBREAK;
      }
      break;
  }
  
  return Instr::UNKNOWN;
}

void generate(Instr InstrType, uint32_t InstructionData, IRData& Data) {
  switch (InstrType) {
    case Instr::UNKNOWN:
      return;
    case Instr::LUI:
      LUIInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::AUIPC:
      AUIPCInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::JAL:
      JALInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::JALR:
      JALRInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::BEQ:
      BEQInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::BNE:
      BNEInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::BLT:
      BLTInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::BGE:
      BGEInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::BLTU:
      BLTUInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::BGEU:
      BGEUInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::LB:
      LBInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::LH:
      LHInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::LW:
      LWInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::LBU:
      LBUInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::LHU:
      LHUInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::SB:
      SBInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::SH:
      SHInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::SW:
      SWInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::ADDI:
      ADDIInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::SLTI:
      SLTIInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::SLTIU:
      SLTIUInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::XORI:
      XORIInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::ORI:
      ORIInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::ANDI:
      ANDIInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::SLLI:
      SLLIInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::SRLI:
      SRLIInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::SRAI:
      SRAIInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::ADD:
      ADDInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::SUB:
      SUBInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::SLL:
      SLLInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::SLT:
      SLTInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::SLTU:
      SLTUInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::XOR:
      XORInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::SRL:
      SRLInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::SRA:
      SRAInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::OR:
      ORInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::AND:
      ANDInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::FENCE:
      FENCEInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::FENCETSO:
      FENCETSOInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::PAUSE:
      PAUSEInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::ECALL:
      ECALLInstruction{InstructionData}.build_ir(Data);
      return;
    case Instr::EBREAK:
      EBREAKInstruction{InstructionData}.build_ir(Data);
      return;
  }
}

} // end namespace riscv
