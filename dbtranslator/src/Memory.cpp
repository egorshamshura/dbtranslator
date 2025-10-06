#include <Memory.h>
#include <cstdint>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/DerivedTypes.h>
#include <memory>

namespace riscv {

std::unique_ptr<SegmentManager[]> Managers;
uint32_t ManagersSize = 0;

llvm::Type* getSegmentType(llvm::LLVMContext& Ctx) {
  if (auto* Type = llvm::StructType::getTypeByName(Ctx, "SegmentManager")) {
    return Type;
  }

  llvm::StructType *SegTy = llvm::StructType::create(Ctx, "SegmentManager");
  llvm::Type *i8Ty = llvm::Type::getInt8Ty(Ctx);
  llvm::Type *i8PtrTy = llvm::PointerType::getUnqual(i8Ty);

  SegTy->setBody({ i8PtrTy, llvm::Type::getInt32Ty(Ctx), llvm::Type::getInt32Ty(Ctx) });
  return SegTy;
}

llvm::Type* getMemoryType(llvm::LLVMContext& Ctx) {
  if (auto* Type = llvm::StructType::getTypeByName(Ctx, "MemoryManager")) {
    return Type;
  }
  llvm::StructType *MemTy = llvm::StructType::create(Ctx, "MemoryManager");

  MemTy->setBody({ llvm::PointerType::getUnqual(llvm::PointerType::getUnqual(getSegmentType(Ctx))), llvm::Type::getInt32Ty(Ctx) });
  return MemTy;
}

llvm::Type* getMemoryPointerType(llvm::LLVMContext& Ctx) {
  return llvm::PointerType::getUnqual(getMemoryType(Ctx));
}

uint8_t read8(MemoryManager* Manager, uint32_t Addr) {
  uint8_t* MappedAddr = mapAddress<uint8_t>(Manager, Addr);
  return *MappedAddr;
}

uint16_t read16(MemoryManager* Manager, uint32_t Addr) {
  uint16_t* MappedAddr = mapAddress<uint16_t>(Manager, Addr);
  return *MappedAddr;
}

uint32_t read32(MemoryManager* Manager, uint32_t Addr) {
  uint32_t* MappedAddr = mapAddress<uint32_t>(Manager, Addr);
  return *MappedAddr;
}

void write8(MemoryManager* Manager, uint32_t Addr, uint8_t Data) {
  uint8_t* MappedAddr = mapAddress<uint8_t>(Manager, Addr);
  *MappedAddr = Data;
}

void write16(MemoryManager* Manager, uint32_t Addr, uint16_t Data) {
  uint16_t* MappedAddr = mapAddress<uint16_t>(Manager, Addr);
  *MappedAddr = Data;
}

void write32(MemoryManager* Manager, uint32_t Addr, uint32_t Data) {
  uint32_t* MappedAddr = mapAddress<uint32_t>(Manager, Addr);
  *MappedAddr = Data;
}

} // end namespace riscv