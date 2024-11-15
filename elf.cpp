#include "elf.h"

namespace riscy::elf {

std::shared_ptr<ELFHeader> readELFHeader(buffer::Buffer &buf) {
  uint32_t magic = buf.pop_u32();
  assert(magic == 0x7F454c46);

  uint8_t cls = buf.pop_u8();
  assert(cls == 1 || cls == 2);

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
  assert(abi < (uint8_t)ELFHeader::ABI::_MAX);

  uint8_t abiVersion = buf.pop_u8();

  buf.skip(7); // padding

  uint16_t type = buf.pop_u16();
  assert(type < (uint16_t)ELFHeader::FileType::_MAX);

  uint16_t machine = buf.pop_u16();
  assert(machine < (uint16_t)ELFHeader::ISA::_MAX);

  uint8_t version2 = buf.pop_u8();
  assert(version2 == 1);

  uint64_t entry;
  if (cls == (uint8_t)ELFHeader::Bitness::k32Bit)
    entry = buf.pop_u64();
  else
    entry = buf.pop_u32();

  uint64_t phoff;
  if (cls == (uint8_t)ELFHeader::Bitness::k64Bit)
    phoff = buf.pop_u64();
  else
    phoff = buf.pop_u32();

  uint64_t shoff;
  if (cls == (uint8_t)ELFHeader::Bitness::k64Bit)
    shoff = buf.pop_u64();
  else
    shoff = buf.pop_u32();

  uint32_t flags = buf.pop_u32();

  uint16_t ehsize = buf.pop_u16();

  uint16_t phentsize = buf.pop_u16();

  uint16_t phnum = buf.pop_u16();

  uint16_t shentsize = buf.pop_u16();

  uint16_t shnum = buf.pop_u16();

  uint16_t shstrndx = buf.pop_u16();

  return std::make_shared<ELFHeader>(
      (ELFHeader::Bitness)cls, (ELFHeader::Endianness)endian,
      (ELFHeader::ABI)abi, abiVersion, (ELFHeader::FileType)type,
      (ELFHeader::ISA)machine, entry, phoff, shoff, flags, ehsize, phentsize,
      phnum, shentsize, shnum, shstrndx);
}

std::shared_ptr<ELF> readELF(buffer::Buffer &buf) {
  auto header = readELFHeader(buf);
  assert(header);
  return std::make_shared<ELF>(header);
}

} // namespace riscy::elf
