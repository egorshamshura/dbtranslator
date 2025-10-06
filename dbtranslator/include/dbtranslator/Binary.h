#ifndef DBTRANSLATOR_BINARY_H
#define DBTRANSLATOR_BINARY_H

#include "Memory.h"

namespace riscv {


std::pair<MemoryManager*, uint32_t> parseElf(char const* FileName);

} // end namespace riscv

#endif // DBTRANSLATOR_BINARY_H
