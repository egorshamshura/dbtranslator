#ifndef DBTRANSLATOR_MEMORY_H
#define DBTRANSLATOR_MEMORY_H

#include <cstdint>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>

namespace riscv {

llvm::Type* getSegmentType(llvm::LLVMContext& Ctx);
llvm::Type* getMemoryType(llvm::LLVMContext& Ctx);
llvm::Type* getMemoryPointerType(llvm::LLVMContext& Ctx);

struct SegmentManager {
  uint8_t* Memory;
  uint32_t MemorySize;
  uint32_t GuestAddress;
};

struct MemoryManager {
  SegmentManager* SegmentData;
  uint32_t NumSegments;
};


uint8_t read8(MemoryManager*, uint32_t Addr);
uint16_t read16(MemoryManager*, uint32_t Addr);
uint32_t read32(MemoryManager*, uint32_t Addr);
void write8(MemoryManager*, uint32_t Addr, uint8_t Data);
void write16(MemoryManager*, uint32_t Addr, uint16_t Data);
void write32(MemoryManager*, uint32_t Addr, uint32_t Data);

template<typename T>
T* mapAddress(MemoryManager* Managers, uint32_t Addr) {
  for (size_t I = 0; I < Managers->NumSegments; ++I) {
    uint32_t GuestAddress = Managers->SegmentData[I].GuestAddress;
    if (GuestAddress <= Addr && Addr < GuestAddress + Managers->SegmentData[I].MemorySize) 
      return reinterpret_cast<T*>(Managers->SegmentData[I].Memory + (Addr - GuestAddress));
  }
  return nullptr;
}

} // end namespace riscv

#endif // DBTRANSLATOR_MEMORY_H
