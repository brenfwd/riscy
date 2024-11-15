#pragma once

#include <bitset>
#include <cstdint>
#include <memory>

#include "buffer.h"

namespace riscy::elf {

struct ELFHeader {

  enum class Endianness : uint8_t {
    kLittle = 0x01,
    kBig = 0x02,
  };
  Endianness endianness;

  enum class ABI : uint8_t {
    System_V = 0x00,
    HP_UX = 0x01,
    NetBSD = 0x02,
    Linux = 0x03,
    GNU_Hurd = 0x04,
    Solaris = 0x06,
    AIX_Monterey = 0x07,
    IRIX = 0x08,
    FreeBSD = 0x09,
    Tru64 = 0x0A,
    Novell_Modesto = 0x0B,
    OpenBSD = 0x0C,
    OpenVMS = 0x0D,
    NonStop_Kernel = 0x0E,
    AROS = 0x0F,
    FenixOS = 0x10,
    Nuxi_CloudABI = 0x11,
    Stratus_Technologies_OpenVOS = 0x12,
  };

  // EI_OSABI
  ABI abi;
  // EI_ABIVERSION
  uint8_t abiVersion;

  enum class FileType : uint16_t {
    None = 0x00,
    Relocatable = 0x01,
    Executable = 0x02,
    Shared = 0x03,
    Core = 0x04,
  };

  // e_type
  FileType type;

  enum class ISA : uint16_t {
    Undefined = 0x00,
    ATT_WE_32100 = 0x01,
    SPARC = 0x02,
    x86 = 0x03,
    Motorola_68000_M68k = 0x04,
    Motorola_88000_M88k = 0x05,
    Intel_MCU = 0x06,
    Intel_80860 = 0x07,
    MIPS = 0x08,
    IBM_System_370 = 0x09,
    MIPS_RS3000_Little_endian = 0x0A,
    Hewlett_Packard_PA_RISC = 0x0F,
    Intel_80960 = 0x13,
    PowerPC = 0x14,
    PowerPC_64_bit = 0x15,
    S390_including_S390x = 0x16,
    IBM_SPU_SPC = 0x17,
    NEC_V800 = 0x24,
    Fujitsu_FR20 = 0x25,
    TRW_RH_32 = 0x26,
    Motorola_RCE = 0x27,
    Arm_up_to_Armv7_AArch32 = 0x28,
    Digital_Alpha = 0x29,
    SuperH = 0x2A,
    SPARC_Version_9 = 0x2B,
    Siemens_TriCore_embedded_processor = 0x2C,
    Argonaut_RISC_Core = 0x2D,
    Hitachi_H8_300 = 0x2E,
    Hitachi_H8_300H = 0x2F,
    Hitachi_H8S = 0x30,
    Hitachi_H8_500 = 0x31,
    IA_64 = 0x32,
    Stanford_MIPS_X = 0x33,
    Motorola_ColdFire = 0x34,
    Motorola_M68HC12 = 0x35,
    Fujitsu_MMA_Multimedia_Accelerator = 0x36,
    Siemens_PCP = 0x37,
    Sony_nCPU_embedded_RISC_processor = 0x38,
    Denso_NDR1_microprocessor = 0x39,
    Motorola_Star_Core_processor = 0x3A,
    Toyota_ME16_processor = 0x3B,
    STMicroelectronics_ST100_processor = 0x3C,
    Advanced_Logic_Corp_TinyJ_embedded_processor_family = 0x3D,
    AMD_x86_64 = 0x3E,
    Sony_DSP_Processor = 0x3F,
    Digital_Equipment_Corp_PDP_10 = 0x40,
    Digital_Equipment_Corp_PDP_11 = 0x41,
    Siemens_FX66_microcontroller = 0x42,
    STMicroelectronics_ST9_8_16_bit_microcontroller = 0x43,
    STMicroelectronics_ST7_8_bit_microcontroller = 0x44,
    Motorola_MC68HC16_Microcontroller = 0x45,
    Motorola_MC68HC11_Microcontroller = 0x46,
    Motorola_MC68HC08_Microcontroller = 0x47,
    Motorola_MC68HC05_Microcontroller = 0x48,
    Silicon_Graphics_SVx = 0x49,
    STMicroelectronics_ST19_8_bit_microcontroller = 0x4A,
    Digital_VAX = 0x4B,
    Axis_Communications_32_bit_embedded_processor = 0x4C,
    Infineon_Technologies_32_bit_embedded_processor = 0x4D,
    Element_14_64_bit_DSP_Processor = 0x4E,
    LSI_Logic_16_bit_DSP_Processor = 0x4F,
    TMS320C6000_Family = 0x8C,
    MCST_Elbrus_e2k = 0xAF,
    Arm_64_bits_Armv8_AArch64 = 0xB7,
    Zilog_Z80 = 0xDC,
    RISC_V = 0xF3,
    Berkeley_Packet_Filter = 0xF7,
    WDC_65C816 = 0x101,
    LoongArch = 0x102,
  };

  // e_machine
  ISA isa;

  // e_entry
  uint64_t entry;

  // e_phoff - program header offset
  uint64_t phOffset;

  uint64_t shOffset;
  // e_shoff - section header offset

  // e_flags
  uint32_t flags;

  // e_ehsize - size of this header
  uint16_t headerSize;

  // e_phentsize - size of program header table entry
  uint16_t phEntrySize;

  // e_phnum
  uint16_t phEntryCount;

  // e_shentsize - size of a section header table entry
  uint16_t sectionEntrySize;

  // e_shnum
  uint16_t sectionEntryCount;

  // e_shstrndx
  uint16_t sectionNameEntryIndex;

  //////////

  ELFHeader(Endianness endianness, ABI abi, uint8_t abiVersion, FileType type,
            ISA isa, uint64_t entry, uint64_t phOffset, uint64_t shOffset,
            uint32_t flags, uint16_t headerSize, uint16_t phEntrySize,
            uint16_t phEntryCount, uint16_t sectionEntrySize,
            uint16_t sectionEntryCount, uint16_t sectionNameEntryIndex)
      : endianness(endianness), abi(abi), abiVersion(abiVersion), type(type),
        isa(isa), entry(entry), phOffset(phOffset), shOffset(shOffset),
        flags(flags), headerSize(headerSize), phEntrySize(phEntrySize),
        phEntryCount(phEntryCount), sectionEntrySize(sectionEntrySize),
        sectionEntryCount(sectionEntryCount),
        sectionNameEntryIndex(sectionNameEntryIndex) {}
};

struct ProgramHeaderEntry {
  enum class SegmentType : uint32_t {
    Null = 0x00000000,
    Loadable = 0x00000001,
    Dynamic = 0x00000002,
    Interpreter = 0x00000003,
    Auxiliary = 0x00000004,
    _Reserved = 0x00000005,
    ProgramHeader = 0x00000006,
    ThreadLocalStorage = 0x00000007,
    PT_HIPROC = 0x7FFFFFFF,
  };

  SegmentType type;

  struct Flags {
    std::bitset<3> bits;

#define FIELD(name, index)                                                     \
  bool name() const { return bits[index]; }                                    \
  void name(bool v) { bits[index] = v; }

    FIELD(R, 0);
    FIELD(W, 1);
    FIELD(X, 2);

    Flags(uint8_t v) : bits(v) {}
  };

  Flags flags;

  uint64_t fileOffset;

  uint64_t virtAddr;

  uint64_t physAddr;

  uint64_t size;

  uint64_t sizeInMemory;

  uint64_t alignment;

  ProgramHeaderEntry(SegmentType type, Flags flags, uint64_t fileOffset,
                     uint64_t virtAddr, uint64_t physAddr, uint64_t size,
                     uint64_t sizeInMemory, uint64_t alignment)
      : type(type), flags(flags), fileOffset(fileOffset), virtAddr(virtAddr),
        physAddr(physAddr), size(size), sizeInMemory(sizeInMemory),
        alignment(alignment) {}
};

struct SectionHeaderEntry {
  uint32_t nameOffset;

  enum class Type : uint32_t {
    Null = 0x0,
    ProgramData = 0x1,
    SymbolTable = 0x2,
    StringTable = 0x3,
    RelocEntriesAddends = 0x4,
    SymbolHashTable = 0x5,
    DynamicLinkingInfo = 0x6,
    Notes = 0x7,
    ProgramSpaceNoData = 0x8,
    RelocEntries = 0x9,
    Reserved = 0x0A,
    DynamicLinkerSymbolTable = 0x0B,
    ConstructorArray = 0x0E,
    DestructorArray = 0x0F,
    PreConstructorArray = 0x10,
    SectionGroup = 0x11,
    ExtendedSectionIndices = 0x12,
    NumDefinedTypes = 0x13,
  };
  Type type;

  struct Flags {
    std::bitset<14> bits;

#define FIELD(name, index)                                                     \
  bool name() const { return bits[index]; }                                    \
  void name(bool v) { bits[index] = v; }

    FIELD(WRITE, 13);
    FIELD(ALLOC, 12);
    FIELD(EXECINSTR, 11);
    FIELD(MERGE, 10);
    FIELD(STRINGS, 9);
    FIELD(INFO_LINK, 8);
    FIELD(LINK_ORDER, 7);
    FIELD(OS_NONCONFORMING, 6);
    FIELD(GROUP, 5);
    FIELD(TLS, 4);
    // -- cut --
#undef FIELD

    Flags(uint64_t v) : bits(v) {}
  };
  Flags flags;

  uint64_t virtAddr;
  uint64_t fileOffset;
  uint64_t size;
  uint32_t linkIndex;
  uint32_t info;
  uint64_t alignment;
  uint64_t entrySize;

  SectionHeaderEntry(uint32_t nameOffset, Type type, Flags flags,
                     uint64_t virtAddr, uint64_t fileOffset, uint64_t size,
                     uint32_t linkIndex, uint32_t info, uint64_t alignment,
                     uint64_t entrySize)
      : nameOffset(nameOffset), type(type), flags(flags), virtAddr(virtAddr),
        fileOffset(fileOffset), size(size), linkIndex(linkIndex), info(info),
        alignment(alignment), entrySize(entrySize) {}
};

struct ELF {
  std::shared_ptr<ELFHeader> header;
  std::vector<std::shared_ptr<ProgramHeaderEntry>> programHeaders;
  std::vector<std::shared_ptr<SectionHeaderEntry>> sectionHeaders;

  ELF(std::shared_ptr<ELFHeader> header,
      std::vector<std::shared_ptr<ProgramHeaderEntry>> &programHeaders,
      std::vector<std::shared_ptr<SectionHeaderEntry>> &sectionHeaders)
      : header(header), programHeaders(programHeaders),
        sectionHeaders(sectionHeaders) {}
};

[[nodiscard]] std::shared_ptr<ELFHeader> readELFHeader(buffer::Buffer &buf);

[[nodiscard]] std::shared_ptr<ProgramHeaderEntry>
readProgramHeaderEntry(buffer::Buffer &buf);

[[nodiscard]] std::shared_ptr<SectionHeaderEntry>
readSectionHeaderEntry(buffer::Buffer &buf);

[[nodiscard]] std::shared_ptr<ELF> readELF(buffer::Buffer &buf);

} // namespace riscy::elf
