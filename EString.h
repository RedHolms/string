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
    size_type string_size = Utf32EncodingTraits::str_length(utf32_string);

    _construct_with_string_and_size(utf32_string, string_size);
  }

  constexpr String(const char32_t* utf32_string, size_type string_size_in_chars) : String() {
    _construct_with_string_and_size(utf32_string, string_size_in_chars);
  }

  template <typename CharType>
  constexpr String(const CharType* encoded_string, size_type encoded_string_length_in_chars) : String() {
    decode(encoded_string, encoded_string_length_in_chars);
  }

  constexpr String(size_type count, char32_t character = 0) : String() {
    _need_allocated(count);

    for (size_type i = 0; i < count; ++i)
      m_buffer[i] = character;
  }

  template <typename CharType>
  constexpr String(const CharType* encoded_string) : String(encoded_string, EncodingTraits<CharType>::str_length(encoded_string)) {}

  template <typename CharType>
  constexpr String(std::basic_string<CharType, std::char_traits<CharType>> const& encoded_string) : String(encoded_string.c_str(), encoded_string.length()) {}

  constexpr String(String const& other) : String() {
    _need_allocated(other.m_length + 1);

    for (size_type i = 0; i < other.m_length; ++i)
      m_buffer[i] = other.m_buffer[i];

    m_buffer[other.m_length] = 0;
  }

  constexpr String(String&& other) {
    m_buffer = other.m_buffer;
    m_length = other.m_length;
    m_allocated = other.m_allocated;
    other.m_buffer = nullptr;
    other.m_length = 0;
    other.m_allocated = 0;
  }

public:
  template <typename CharType>
  constexpr std::basic_string<CharType, std::char_traits<CharType>, std::allocator<CharType>> encode() const {
    using encoding_traits = EncodingTraits<CharType>;
    using string_type = std::basic_string<CharType, std::char_traits<CharType>, std::allocator<CharType>>;

    if constexpr (std::is_same_v<CharType, char32_t>) {
      // Optimization
      return string_type(m_buffer, m_length);
    }

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
  constexpr char32_t& front() {
    return m_buffer[0];
  }

  constexpr char32_t front() const {
    return m_buffer[0];
  }

  constexpr char32_t& back() {
    return m_buffer[m_length - 1];
  }

  constexpr char32_t back() const {
    return m_buffer[m_length - 1];
  }

  constexpr char32_t* data() {
    return m_buffer;
  }

  constexpr const char32_t* data() const {
    return m_buffer;
  }

  constexpr char32_t* begin() {
    return m_buffer;
  }

  constexpr const char32_t* begin() const {
    return m_buffer;
  }

  constexpr const char32_t* cbegin() const {
    return m_buffer;
  }

  constexpr char32_t* end() {
    return m_buffer;
  }

  constexpr const char32_t* end() const {
    return m_buffer;
  }

  constexpr const char32_t* cend() const {
    return m_buffer;
  }

  constexpr bool is_empty() const {
    return !m_buffer || m_length == 0;
  }

  constexpr size_type length() const {
    return m_length;
  }

  constexpr size_type size() const {
    return m_length;
  }

  constexpr size_type max_size() const {
    return static_cast<size_type>(~0) / sizeof(char32_t);
  }

  constexpr void reserve(size_type count) {
    _need_allocated(count);
  }

  constexpr size_type capacity() const {
    return m_allocated;
  }

  constexpr void shrink_to_fit() {
    if (m_allocated > (m_length + 1)) {
      _reallocate(m_length + 1);
    }
  }

  constexpr void clear() {
    m_buffer[0] = 0;
    m_length = 0;
  }

  constexpr String& insert(size_type index, size_type count, char32_t character) {
    _need_allocated(m_length + count + 1);
    _move_right(index, count);

    for (size_type i = 0; i < count; ++i, ++index)
      m_buffer[index] = character;
    m_length = index;

    return *this;
  }

  constexpr String& insert(size_type index, const char32_t* string) {
    return insert(index, string, Utf32EncodingTraits::str_length(string));
  }

  constexpr String& insert(size_type index, const char32_t* string, size_type string_length_in_characters) {
    _need_allocated(m_length + string_length_in_characters + 1);
    _move_right(index, string_length_in_characters);

    for (size_type i = 0; i < string_length_in_characters; ++i, ++index)
      m_buffer[index] = string[i];
    m_length = index;

    return *this;
  }

  constexpr String& insert(size_type index, String const& string) {
    return insert(index, string.m_buffer, string.m_length);
  }

  constexpr String& erase(size_type index, size_type count) {
    _move_left(index, count);
    m_length = index + count;
    m_buffer[m_length] = 0;

    return *this;
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

  constexpr char32_t& operator[](size_type index) {
    return m_buffer[index];
  }

  constexpr char32_t operator[](size_type index) const {
    return m_buffer[index];
  }

  // Cast to STL string.
  template <typename CharType>
  constexpr operator std::basic_string<CharType, std::char_traits<CharType>, std::allocator<CharType>>() {
    return encode<CharType>();
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

  constexpr void _move_right(size_type index, size_type count) {
    for (size_type i = 0; i < count; ++i, ++index)
      m_buffer[index + count] = m_buffer[index];
  }

  constexpr void _move_left(size_type index, size_type count) {
    for (size_type i = 0; i < count; ++i, ++index)
      m_buffer[index - count] = m_buffer[index];
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