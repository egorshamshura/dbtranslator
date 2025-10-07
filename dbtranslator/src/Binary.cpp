#include <cstddef>
#include <cstdint>
#include <elfio/elfio.hpp>
#include <iostream>
#include <elfio/elfio_dump.hpp>
#include "Binary.h"
#include "elfio/elf_types.hpp"
#include "elfio/elfio_section.hpp"
#include "elfio/elfio_segment.hpp"
#include "Memory.h"

namespace riscv {


std::pair<MemoryManager*, uint32_t> parseElf(char const* FileName, bool Debug) {
  ELFIO::elfio Reader;
  Reader.load(FileName);
  if (Debug) {
    ELFIO::dump::segment_headers(std::cerr, Reader);
    ELFIO::dump::segment_datas(std::cerr, Reader);
  }
  MemoryManager* Result = new MemoryManager();
  Result->NumSegments = Reader.segments.size() + 1;
  Result->SegmentData = static_cast<SegmentManager*>(operator new(sizeof(SegmentManager) * (Result->NumSegments)));
  for (size_t I = 0; I < Reader.segments.size(); ++I) {
    new (Result->SegmentData + I) SegmentManager();
    SegmentManager* Manager = Result->SegmentData + I;
    if (Reader.segments[I]->get_flags() == ELFIO::PT_LOAD || true) {
      Manager->MemorySize = Reader.segments[I]->get_memory_size();
      Manager->Memory = static_cast<uint8_t*>(operator new(Manager->MemorySize));
      Manager->GuestAddress = Reader.segments[I]->get_virtual_address();
      std::memcpy(Manager->Memory, Reader.segments[I]->get_data(), Manager->MemorySize);
    }
  }
  SegmentManager* Manager = Result->SegmentData + Result->NumSegments - 1;
  new (Manager) SegmentManager();
  Manager->MemorySize = 1 << 24;
  Manager->Memory = static_cast<uint8_t*>(operator new(Manager->MemorySize));
  Manager->GuestAddress = -1 - Manager->MemorySize;
  return {Result, Reader.get_entry()};
}
} // end namespace riscv
