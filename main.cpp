#include <cstdint>
#include <fstream>
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
}
