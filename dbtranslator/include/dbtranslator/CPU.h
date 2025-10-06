#ifndef DBTRANSLATOR_CPU_H
#define DBTRANSLATOR_CPU_H

#include "Memory.h"
#include <cstdint>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>

namespace riscv {

llvm::Type* getCPUStateType(llvm::LLVMContext& Ctx);
llvm::Type* getCPUStatePointerType(llvm::LLVMContext& Ctx);

struct CPUState {
  uint32_t Registers[32];
  uint32_t PC;
  MemoryManager* Manager;
};

void dump(CPUState* State);

} // end namespace riscv

#endif // DBTRANSLATOR_CPU_H
