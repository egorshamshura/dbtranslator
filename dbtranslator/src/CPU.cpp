#include "CPU.h"
#include "Memory.h"
#include <cstddef>
#include <iostream>
#include <llvm/IR/Type.h>
#include "llvm/IR/DerivedTypes.h"

namespace riscv {

llvm::Type* getCPUStateType(llvm::LLVMContext& Ctx) {
  if (auto* Type = llvm::StructType::getTypeByName(Ctx, "CPUState")) {
    return Type;
  }
  auto *CPUStructTy = llvm::StructType::create(Ctx, "CPUState");
  auto *RegsArrTy   = llvm::ArrayType::get(llvm::Type::getInt32Ty(Ctx), 32);
  CPUStructTy->setBody({RegsArrTy, llvm::Type::getInt32Ty(Ctx), getMemoryPointerType(Ctx)});
  return CPUStructTy;
}

llvm::Type* getCPUStatePointerType(llvm::LLVMContext& Ctx) {
  return llvm::PointerType::getUnqual(getCPUStateType(Ctx));
}

void dump(CPUState* State) {
  for (size_t I = 0; I != 32; ++I) {
    std::cout << "R" << I << " " << State->Registers[I] << std::endl;
  }
  std::cout << "PC " << State->PC << std::endl;
}

} // end namespace riscv