#pragma once

#include <stddef.h>
#include <string.h>

#include <string>
#include <istream>

#include "EStringEncodings.h"

class String {
public:
  using size_type = size_t;

public:
  constexpr String() = default;

  constexpr String(const char32_t* utf32_string) : String() {
    // Count string size
    size_type string_size;
    for (string_size = 0; utf32_string[string_size]; ++string_size);

    _construct_with_string_and_size(utf32_string, string_size);
  }

  constexpr String(const char32_t* utf32_string, size_type string_size_in_chars) : String() {
    _construct_with_string_and_size(utf32_string, string_size_in_chars);
  }

  template <typename CharType>
  constexpr String(const CharType* encoded_string, size_type encoded_string_length_in_chars) : String() {
    decode(encoded_string, encoded_string_length_in_chars);
  }

  template <typename CharType>
  constexpr String(const CharType* encoded_string) : String(encoded_string, EncodingTraits<CharType>::str_length(encoded_string)) {}

  template <typename CharType>
  constexpr String(std::basic_string<CharType, std::char_traits<CharType>> const& encoded_string) : String(encoded_string.c_str(), encoded_string.length()) {}

public:
  template <typename CharType>
  constexpr std::basic_string<CharType, std::char_traits<CharType>, std::allocator<CharType>> encode() const {
    using encoding_traits = EncodingTraits<CharType>;
    using string_type = std::basic_string<CharType, std::char_traits<CharType>, std::allocator<CharType>>;

    if (m_length == 0)
      return string_type();

    string_type encoded_string;
    encoded_string.resize((m_length * encoding_traits::max_encoded_size) + 1);

    size_type encoded_length = encoding_traits::from_utf32(m_buffer, m_length, encoded_string.data());
    encoded_string.resize(encoded_length);

    return std::move(encoded_string);
  }

  template <typename CharType>
  constexpr void decode(const CharType* encoded_string, size_type encoded_string_length_in_chars) {
    using encoding_traits = EncodingTraits<CharType>;

    _need_allocated(encoded_string_length_in_chars + 1);
    m_length = encoded_string_length_in_chars;

    size_type written = encoding_traits::to_utf32(encoded_string, encoded_string_length_in_chars, m_buffer);
    m_buffer[written] = 0;
  }

  template <typename CharType>
  constexpr void decode(const CharType* encoded_string) {
    decode(encoded_string, EncodingTraits<CharType>::str_length(encoded_string));
  }

  template <typename CharType>
  constexpr void decode(std::basic_string<CharType, std::char_traits<CharType>> const& encoded_string) {
    decode(encoded_string.c_str(), encoded_string.length());
  }

public:
  template <typename CharType>
  constexpr String& operator=(const CharType* encoded_string) {
    decode(encoded_string);
    return *this;
  }

  template <typename CharType>
  constexpr String& operator=(std::basic_string<CharType, std::char_traits<CharType>> const& encoded_string) {
    decode(encoded_string);
    return *this;
  }

private:
  constexpr void _need_allocated(size_type size) {
    if (m_allocated < size) {
      _reallocate(size + size / 2);
    }
  }

  void _reallocate(size_type new_size) {
    char32_t* prev_buffer = m_buffer;
    size_type prev_allocated = m_allocated;

    m_buffer = new char32_t[new_size];
    m_allocated = new_size;

    memset(m_buffer, 0, m_allocated * sizeof(char32_t));

    if (prev_buffer) {
      memcpy(m_buffer, prev_buffer, prev_allocated * sizeof(char32_t));
      delete[] prev_buffer;
    }
  }

  constexpr void _construct_with_string_and_size(const char32_t* utf32_string, size_type string_size_in_chars) {
    _need_allocated(string_size_in_chars + 1);

    for (size_type index = 0; index < string_size_in_chars; ++string_size_in_chars)
      m_buffer[index * sizeof(char32_t)] = utf32_string[index * sizeof(char32_t)];

    m_buffer[string_size_in_chars] = 0;
    m_length = string_size_in_chars;
  }

private:
  // Length of string in chars, not including null-terminating char.
  size_type m_length = 0;
  size_type m_allocated = 0;
  char32_t* m_buffer = nullptr;
};

// Implimented in EString_Streams.cpp because
//  of complicated decoding logic.
// May be pretty slow.
std::basic_istream<char, std::char_traits<char>>& operator>>(std::basic_istream<char, std::char_traits<char>>& stream, String& out_str);