#pragma once

#include <cassert>
#include <cstdint>
#include <string>
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

  [[nodiscard]] inline std::string pop_null_string() {
    std::string str;
    while (_data[_index] != 0) {
      str.push_back(_data[_index]);
      skip(1);
    }
    skip(1);
    return str;
  }

  inline void skip(size_t n) {
    assert(_data.size() >= n);
    _index += n;
  }

  inline void seek(size_t new_index) {
    assert(_data.size() == 0 || new_index < _data.size());
    _index = new_index;
  }

  [[nodiscard]] inline size_t index() const { return _index; }

  [[nodiscard]] inline Buffer slice(size_t start, size_t end) const {
    if (start == end) {
      return Buffer();
    }
    assert(start < end);
    assert(end <= _data.size());
    auto result = Buffer(_data.begin() + start, _data.begin() + end);
    result.setEndianness(endian);
    return result;
  }

  [[nodiscard]] inline size_t size() const { return _data.size(); }

  [[nodiscard]] inline bool empty() const { return _data.empty(); }

  [[nodiscard]] inline const uint8_t *data() const { return _data.data(); }

  [[nodiscard]] inline uint8_t *data() { return _data.data(); }

  [[nodiscard]] inline uint8_t operator[](size_t i) const {
    return _data.at(i);
  }

  [[nodiscard]] inline uint8_t &operator[](size_t i) { return _data.at(i); }
};

} // namespace riscy::buffer
