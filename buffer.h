#pragma once

#include <cassert>
#include <cstdint>
#include <vector>

namespace riscy::buffer {

enum class Endianness {
  Big,
  Little,
};

class Buffer {
private:
  std::vector<uint8_t> _data;
  size_t _index = 0;
  Endianness endian = Endianness::Big;

  template <typename T>
  [[nodiscard]]
  T fromBigEndian(T v) {
    if (endian == Endianness::Big)
      return v;
    constexpr size_t N = sizeof(T);
    T res = 0;
    if constexpr (N > 1) {
      for (size_t i = 0; i < N; i++) {
        res <<= 8;
        res |= (v & 0xff);
        v >>= 8;
      }
    } else {
      res = v;
    }
    return res;
  }

public:
  Buffer() = default;

  template <typename It> Buffer(It begin, It end) : _data(begin, end) {}

  Buffer(std::vector<uint8_t> &&v) : _data(std::move(v)) {}

  inline void setEndianness(Endianness e) { endian = e; }

  template <typename T> [[nodiscard]] T pop() {
    constexpr size_t N = sizeof(T);
    assert(_data.size() >= N);
    T value = 0;
    for (size_t i = 0; i < N; ++i) {
      value = (value << 8) | _data[i + _index];
    }
    skip(N);
    return fromBigEndian(value);
  }

  [[nodiscard]] inline uint8_t pop_u8() { return pop<uint8_t>(); }
  [[nodiscard]] inline uint16_t pop_u16() { return pop<uint16_t>(); }
  [[nodiscard]] inline uint32_t pop_u32() { return pop<uint32_t>(); }
  [[nodiscard]] inline uint32_t pop_u64() { return pop<uint64_t>(); }

  inline void skip(size_t n) {
    assert(_data.size() >= n);
    _index += n;
  }

  inline void seek(ptrdiff_t n) {
    ptrdiff_t new_index = static_cast<ptrdiff_t>(_index) + n;
    assert(new_index >= 0 && new_index < _data.size());
    _index = new_index;
  }

  inline size_t index() const { return _index; }
};

} // namespace riscy::buffer
