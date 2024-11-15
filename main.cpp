#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include "buffer.h"
#include "elf.h"

int main() {
  std::ifstream is("examples/quad.so", std::ios_base::binary | std::ios::ate);
  assert(is.is_open());

  std::streamsize ss = is.tellg();
  is.seekg(0, std::ios::beg);

  std::vector<uint8_t> vec(static_cast<size_t>(ss));
  if (!is.read(reinterpret_cast<char *>(vec.data()), ss)) {
    throw std::ios_base::failure("failed to read");
  }

  riscy::buffer::Buffer buf(std::move(vec));

  auto elf = riscy::elf::readELF(buf);
  if (!elf) {
    std::cerr << "Failed to read ELF!" << std::endl;
    return 1;
  }

  std::cout << "Read ELF file OK.\n";
  std::cout << "\t- 0x" << std::hex << (uint16_t)elf->header->isa << std::dec
            << " ISA\n";

  std::cout << "\t- " << elf->programHeaders.size() << " Program Header(s)\n";

  std::cout << "\t- " << elf->sectionHeaders.size() << " Section Header(s)\n";
  for (auto &section : elf->sectionHeaders) {
    std::cout << "\t\t- flags = 0x" << std::hex << section->flags << std::endl;

    std::cout << "\t\t- strings? "
              << (section->flags & riscy::elf::SectionHeaderEntry::SHF_STRINGS)
              << std::endl;

    std::cout << "\t\t- exec? "
              << (section->flags &
                  riscy::elf::SectionHeaderEntry::SHF_EXECINSTR)
              << std::endl;

    std::cout << "\t\t- alloc? "
              << (section->flags & riscy::elf::SectionHeaderEntry::SHF_ALLOC)
              << std::endl;

    std::cout << "\t\t- type=0x" << std::hex << (uint32_t)section->type
              << std::dec << std::endl;
    buf.seek(section->fileOffset);
    std::cout << "\t\t\t[" << std::hex;
    for (int i = 0; i < section->size; i++) {
      std::cout << (uint32_t)buf.pop_u8() << " ";
    }
    buf.seek(section->fileOffset);
    std::cout << "]" << std::endl << "\t\t\t[";
    for (int i = 0; i < section->size; i++) {
      std::cout << (char)buf.pop_u8();
    }
    std::cout << "]" << std::endl << std::endl;
  }

  std::cout << std::dec;

  auto text = elf->getSectionByName(".text");
  std::cout << "Found .text section: " << text->size << " bytes\n";

  auto symt = elf->getSymbolTable();
  std::cout << "Found symbol table: " << symt->size << " bytes\n";

  auto pos = elf->getSymbolLocation("quad");
  std::cout << "Found symbol 'quad' at position " << pos.value() << std::endl;
}
