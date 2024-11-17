#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include "buffer.h"
#include "elf.h"
#include "risc.h"

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

  std::cout << std::dec;

  auto text = elf->getSectionByName(".text");
  std::cout << "Found .text section: " << text->size << " bytes\n";

  auto symt = elf->getSymbolTable();
  std::cout << "Found symbol table: " << symt->size << " bytes\n";

  auto pos = elf->getSymbolLocation("quad").value();
  std::cout << "Found symbol 'quad' at position " << pos.value << " with size "
            << pos.size << std::endl;

  buf.seek(pos.value);
  for (int i = 0; i < pos.size; i += 4) {
    auto instr_int = buf.pop_u32();
    std::cout << std::hex << std::setfill('0') << std::setw(8) << instr_int
              << " " << std::dec;
    auto instr = riscy::risc::decode_instr(instr_int);
    instr->operator<<(std::cout) << "\n";
    // std::cout << (uint32_t)buf.pop_u8() << " ";
  }
}
