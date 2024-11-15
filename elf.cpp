#include "elf.h"

#include <iostream>

namespace riscy::elf {

std::shared_ptr<ELFHeader> readELFHeader(buffer::Buffer &buf) {
  uint32_t magic = buf.pop_u32();
  assert(magic == 0x7F454c46);

  uint8_t cls = buf.pop_u8();
  assert(cls == 2); // 64-bit only

  uint8_t endian = buf.pop_u8();
  if (endian == (uint8_t)ELFHeader::Endianness::kBig)
    buf.setEndianness(buffer::Endianness::Big);
  else if (endian == (uint8_t)ELFHeader::Endianness::kLittle)
    buf.setEndianness(buffer::Endianness::Little);
  else
    assert(false);

  uint8_t version = buf.pop_u8();
  assert(version == 1);

  uint8_t abi = buf.pop_u8();
  uint8_t abiVersion = buf.pop_u8();

  buf.skip(7); // padding

  uint16_t type = buf.pop_u16();
  uint16_t machine = buf.pop_u16();

  uint8_t version2 = buf.pop_u32();
  assert(version2 == 1);

  uint64_t entry = buf.pop_u64();
  uint64_t phoff = buf.pop_u64();
  uint64_t shoff = buf.pop_u64();
  uint32_t flags = buf.pop_u32();
  uint16_t ehsize = buf.pop_u16();
  uint16_t phentsize = buf.pop_u16();
  uint16_t phnum = buf.pop_u16();
  uint16_t shentsize = buf.pop_u16();
  uint16_t shnum = buf.pop_u16();
  uint16_t shstrndx = buf.pop_u16();

  return std::make_shared<ELFHeader>(
      (ELFHeader::Endianness)endian, (ELFHeader::ABI)abi, abiVersion,
      (ELFHeader::FileType)type, (ELFHeader::ISA)machine, entry, phoff, shoff,
      flags, ehsize, phentsize, phnum, shentsize, shnum, shstrndx);
}

std::shared_ptr<ProgramHeaderEntry>
readProgramHeaderEntry(buffer::Buffer &buf) {
  uint32_t type = buf.pop_u32();

  uint32_t flags = buf.pop_u32();

  uint64_t fileOffset = buf.pop_u64();
  uint64_t virtAddr = buf.pop_u64();
  uint64_t physAddr = buf.pop_u64();
  uint64_t size = buf.pop_u64();
  uint64_t sizeInMemory = buf.pop_u64();
  uint64_t alignment = buf.pop_u64();

  return std::make_shared<ProgramHeaderEntry>(
      (ProgramHeaderEntry::SegmentType)type, flags, fileOffset, virtAddr,
      physAddr, size, sizeInMemory, alignment);
}

std::shared_ptr<SectionHeaderEntry>
readSectionHeaderEntry(buffer::Buffer &buf) {
  uint32_t nameOffset = buf.pop_u32();
  uint32_t type = buf.pop_u32();
  uint64_t flags = buf.pop_u64();
  uint64_t virtAddr = buf.pop_u64();
  uint64_t fileOffset = buf.pop_u64();
  uint64_t size = buf.pop_u64();
  uint32_t linkIndex = buf.pop_u32();
  uint32_t info = buf.pop_u32();
  uint64_t alignment = buf.pop_u64();
  uint64_t entrySize = buf.pop_u64();

  auto sectionBuf = buf.slice(fileOffset, fileOffset + size);

  return std::make_shared<SectionHeaderEntry>(
      nameOffset, (SectionHeaderEntry::Type)type, flags, virtAddr, fileOffset,
      size, linkIndex, info, alignment, entrySize, sectionBuf);
}

std::shared_ptr<ELF> readELF(buffer::Buffer &buf) {
  auto header = readELFHeader(buf);
  assert(header);

  std::vector<std::shared_ptr<ProgramHeaderEntry>> programHeaders;
  std::vector<std::shared_ptr<SectionHeaderEntry>> sectionHeaders;

  programHeaders.reserve(header->phEntryCount);
  for (int i = 0; i < header->phEntryCount; i++) {
    buf.seek(header->phOffset + header->phEntrySize * i);
    programHeaders.push_back(readProgramHeaderEntry(buf));
  }

  sectionHeaders.reserve(header->sectionEntryCount);
  for (int i = 0; i < header->sectionEntryCount; i++) {
    buf.seek(header->shOffset + header->sectionEntrySize * i);
    sectionHeaders.push_back(readSectionHeaderEntry(buf));
  }

  return std::make_shared<ELF>(header, programHeaders, sectionHeaders);
}

} // namespace riscy::elf
