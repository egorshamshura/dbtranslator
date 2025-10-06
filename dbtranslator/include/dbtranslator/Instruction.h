#ifndef DBTRANSLATOR_INSTRUCTION_H
#define DBTRANSLATOR_INSTRUCTION_H

#include "llvm/IR/Instruction.h"
#include "llvm/IR/IRBuilder.h"
#include <cstdint>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>

namespace riscv {

namespace constants {
static constexpr uint64_t REG_SIZE = 32; // 32 regs in rv32i
} // end namespace constants 

enum class Instr {
  UNKNOWN = 0,
  LUI,
  AUIPC,
  JAL,
  JALR,
  BEQ,
  BNE,
  BLT,
  BGE,
  BLTU,
  BGEU,
  LB,
  LH,
  LW,
  LBU,
  LHU,
  SB,
  SH,
  SW,
  ADDI,
  SLTI,
  SLTIU,
  XORI,
  ORI,
  ANDI,
  SLLI,
  SRLI,
  SRAI,
  ADD,
  SUB,
  SLL,
  SLT,
  SLTU,
  XOR,
  SRL,
  SRA,
  OR,
  AND,
  FENCE,
  FENCETSO,
  PAUSE,
  ECALL,
  EBREAK,
};

char const* InstrToLiteral(Instr I);

struct IRData {
  llvm::Module& Module;
  llvm::IRBuilder<>& Builder;
  llvm::Function* CurrentFunction;
  llvm::FunctionCallee MemoryFunctions[6];
};    

struct Instruction {
  uint32_t InstructionData;

  Instruction(uint32_t Instr) : InstructionData(Instr) {}

  // PC is part of RegFile
  virtual void build_ir(IRData&) = 0;
};

struct LUIInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct AUIPCInstruction : Instruction {
  using Instruction::Instruction;
  
  void build_ir(IRData&) override;
};

struct JALInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct JALRInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct BEQInstruction : Instruction {
  using Instruction::Instruction;
  
  void build_ir(IRData&) override;
};

struct BNEInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct BLTInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct BGEInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct BLTUInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct BGEUInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct LBInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct LHInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct LWInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct LBUInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct LHUInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct SBInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct SHInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct SWInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct ADDIInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct SLTIInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct SLTIUInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct XORIInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct ORIInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct ANDIInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct SLLIInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct SRLIInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct SRAIInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct ADDInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct SUBInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct SLLInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct SLTInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct SLTUInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct XORInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct SRLInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct SRAInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct ORInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct ANDInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct FENCEInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct FENCETSOInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct PAUSEInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct ECALLInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

struct EBREAKInstruction : Instruction {
  using Instruction::Instruction;

  void build_ir(IRData&) override;
};

Instr decode(uint32_t InstructionData);
void generate(Instr InstrType, uint32_t InstructionData, IRData& Data);

} // end namespace riscv

#endif // DBTRANSLATOR_INSTRUCTION_H
