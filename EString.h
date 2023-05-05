#pragma once
#define EString_EString_h_

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include <string>
#include <istream>

#include "EStringEncodings.h"

class EString {
public:
  using size_type = size_t;

public:
  constexpr EString() = default;

  constexpr EString(const char32_t* utf32_string) : EString() {
    size_type string_size = Utf32EncodingTraits::str_length(utf32_string);

    _construct_with_string_and_size(utf32_string, string_size);
  }

  constexpr EString(const char32_t* utf32_string, size_type string_size_in_chars) : EString() {
    _construct_with_string_and_size(utf32_string, string_size_in_chars);
  }

  template <typename CharType>
  constexpr EString(const CharType* encoded_string, size_type encoded_string_length_in_chars) : EString() {
    decode(encoded_string, encoded_string_length_in_chars);
  }

  constexpr EString(size_type count, char32_t character = 0) : EString() {
    _need_allocated(count);

    for (size_type i = 0; i < count; ++i)
      m_buffer[i] = character;
  }

  template <typename CharType>
  constexpr EString(const CharType* encoded_string) : EString(encoded_string, EncodingTraits<CharType>::str_length(encoded_string)) {}

  template <typename CharType>
  constexpr EString(std::basic_string<CharType, std::char_traits<CharType>> const& encoded_string) : EString(encoded_string.c_str(), encoded_string.length()) {}

  constexpr EString(EString const& other) : EString() {
    operator=(other);
  }

  constexpr EString(EString&& other) noexcept {
    operator=(std::move(other));
  }

  constexpr EString& operator=(EString const& other) {
    _need_allocated(other.m_length + 1);

    for (size_type i = 0; i < other.m_length; ++i)
      m_buffer[i] = other.m_buffer[i];

    m_buffer[other.m_length] = 0;

    return *this;
  }

  constexpr EString& operator=(EString&& other) noexcept {
    m_buffer = other.m_buffer;
    m_length = other.m_length;
    m_allocated = other.m_allocated;
    other.m_buffer = nullptr;
    other.m_length = 0;
    other.m_allocated = 0;

    return *this;
  }

  constexpr ~EString() {
    m_length = 0;
    _reallocate(0);
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
  constexpr char32_t& front() noexcept {
    return m_buffer[0];
  }

  constexpr char32_t front() const noexcept {
    return m_buffer[0];
  }

  constexpr char32_t& back() noexcept {
    return m_buffer[m_length - 1];
  }

  constexpr char32_t back() const noexcept {
    return m_buffer[m_length - 1];
  }

  constexpr char32_t* data() noexcept {
    return m_buffer;
  }

  constexpr const char32_t* data() const noexcept {
    return m_buffer;
  }

  constexpr char32_t* begin() noexcept {
    return m_buffer;
  }

  constexpr const char32_t* begin() const noexcept {
    return m_buffer;
  }

  constexpr const char32_t* cbegin() const noexcept {
    return m_buffer;
  }

  constexpr char32_t* end() noexcept {
    return m_buffer;
  }

  constexpr const char32_t* end() const noexcept {
    return m_buffer;
  }

  constexpr const char32_t* cend() const noexcept {
    return m_buffer;
  }

  constexpr bool is_empty() const noexcept {
    return !m_buffer || m_length == 0;
  }

  constexpr size_type length() const noexcept {
    return m_length;
  }

  constexpr size_type size() const noexcept {
    return m_length;
  }

  constexpr size_type max_size() const noexcept {
    return static_cast<size_type>(~0) / sizeof(char32_t);
  }

  constexpr void reserve(size_type count) {
    _need_allocated(count);
  }

  constexpr size_type capacity() const noexcept {
    return m_allocated;
  }

  constexpr void shrink_to_fit() {
    if (m_allocated > (m_length + 1)) {
      _reallocate(m_length + 1);
    }
  }

  constexpr void clear() noexcept {
    m_buffer[0] = 0;
    m_length = 0;
  }

  constexpr EString& insert(size_type index, size_type count, char32_t character) {
    _need_allocated(m_length + count + 1);
    _move_right(index, m_length - index, count);

    for (size_type i = 0; i < count; ++i, ++index)
      m_buffer[index] = character;

    m_length += count;
    m_buffer[m_length] = 0;

    return *this;
  }

  constexpr EString& insert(size_type index, char32_t character) {
    return insert(index, 1, character);
  }

  constexpr EString& insert(size_type index, const char32_t* string, size_type string_length_in_characters) {
    _need_allocated(m_length + string_length_in_characters + 1);
    _move_right(index, m_length - index, string_length_in_characters);

    for (size_type i = 0; i < string_length_in_characters; ++i, ++index)
      m_buffer[index] = string[i];

    m_length += string_length_in_characters;
    m_buffer[m_length] = 0;

    return *this;
  }

  constexpr EString& insert(size_type index, const char32_t* string) {
    return insert(index, string, Utf32EncodingTraits::str_length(string));
  }

  constexpr EString& insert(size_type index, EString const& string) {
    return insert(index, string.m_buffer, string.m_length);
  }

  template <typename CharType>
  constexpr EString& insert(size_type index, const CharType* string, size_type string_length_in_characters) {
    using encoding_traits = EncodingTraits<CharType>;

    char32_t* buffer = new char32_t[string_length_in_characters];
    size_type buffer_size = encoding_traits::to_utf32(string, string_length_in_characters, buffer);

    insert(index, buffer, buffer_size);

    delete[] buffer;

    return *this;
  }

  template <typename CharType>
  constexpr EString& insert(size_type index, const CharType* string) {
    using encoding_traits = EncodingTraits<CharType>;

    return insert(index, string, encoding_traits::str_length(string));
  }

  constexpr EString& erase(size_type index, size_type count) {
    if (index >= m_length) {
      // Debug report.
      assert(!"Trying to erase characters out of string buffer");
      return *this;
    }

    if (index + count > m_length) {
      // Debug report.
      assert(!"Trying to erase characters out of string buffer");
      return *this;
    }

    _move_left(index + count, m_length - (index + count), count);

    m_length -= count;
    m_buffer[m_length] = 0;

    return *this;
  }

  constexpr EString& append(size_type count, char32_t character) {
    _need_allocated(m_length + count + 1);

    for (size_type counter = 0, index = m_length; counter < count; ++counter, ++index)
      m_buffer[index] = character;

    m_length += count;
    m_buffer[m_length] = 0;

    return *this;
  }

  constexpr EString& append(char32_t character) {
    return append(1, character);
  }

  constexpr EString& append(const char32_t* string, size_type string_length_in_characters) {
    _need_allocated(m_length + string_length_in_characters + 1);

    for (size_type i = 0, index = m_length; i < string_length_in_characters; ++i, ++index)
      m_buffer[index] = string[i];

    m_length += string_length_in_characters;
    m_buffer[m_length] = 0;

    return *this;
  }

  constexpr EString& append(const char32_t* string) {
    return append(string, Utf32EncodingTraits::str_length(string));
  }

  constexpr EString& append(EString const& string) {
    return append(string.m_buffer, string.m_length);
  }

  template <typename CharType>
  constexpr EString& append(const CharType* string, size_type string_length_in_characters) {
    using encoding_traits = EncodingTraits<CharType>;

    char32_t* buffer = new char32_t[string_length_in_characters];
    size_type buffer_size = encoding_traits::to_utf32(string, string_length_in_characters, buffer);

    append(buffer, buffer_size);

    delete[] buffer;

    return *this;
  }

  template <typename CharType>
  constexpr EString& append(const CharType* string) {
    using encoding_traits = EncodingTraits<CharType>;

    return append(string, encoding_traits::str_length(string));
  }

public:
  constexpr bool operator==(EString const& string) const noexcept {
    if ((&string) == this)
      return true; // this == this

    return _is_equal(string.m_buffer, string.m_length);
  }

  constexpr bool operator==(const char32_t* string) const noexcept {
    return _is_equal(string, Utf32EncodingTraits::str_length(string));
  }

  template <typename CharType>
  constexpr bool operator==(const CharType* string) const {
    using encoding_traits = EncodingTraits<CharType>;

    return _is_equal_encoded(string, encoding_traits::str_length(string));
  }

  template <typename CharType>
  constexpr bool operator==(std::basic_string<CharType, std::char_traits<CharType>> const& string) const {
    return _is_equal_encoded(string.c_str(), string.length());
  }


  constexpr bool operator!=(EString const& string) const noexcept {
    return !operator==(string);
  }

  constexpr bool operator!=(const char32_t* string) const noexcept {
    return !operator==(string);
  }

  template <typename CharType>
  constexpr bool operator!=(const CharType* string) const {
    return !operator==(string);
  }

  template <typename CharType>
  constexpr bool operator!=(std::basic_string<CharType, std::char_traits<CharType>> const& string) const {
    return !operator==(string);
  }


  template <typename CharType>
  constexpr EString& operator=(const CharType* encoded_string) {
    decode(encoded_string);
    return *this;
  }

  template <typename CharType>
  constexpr EString& operator=(std::basic_string<CharType, std::char_traits<CharType>> const& encoded_string) {
    decode(encoded_string);
    return *this;
  }

  constexpr char32_t& operator[](size_type index) noexcept {
    return m_buffer[index];
  }

  constexpr char32_t operator[](size_type index) const noexcept {
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

    m_allocated = new_size;
    m_buffer = nullptr;

    if (new_size > 0) {
      m_buffer = new char32_t[new_size];
      memset(m_buffer, 0, m_allocated * sizeof(char32_t));

      if (prev_buffer) {
        memcpy(m_buffer, prev_buffer, prev_allocated * sizeof(char32_t));
      }
    }

    if (prev_buffer) {
      delete[] prev_buffer;
    }
  }

  constexpr void _construct_with_string_and_size(const char32_t* utf32_string, size_type string_size_in_chars) {
    _need_allocated(string_size_in_chars + 1);

    for (size_type index = 0; index < string_size_in_chars; ++index)
      m_buffer[index] = utf32_string[index];

    m_buffer[string_size_in_chars] = 0;
    m_length = string_size_in_chars;
  }

  // Move all characters in range [index, index + count) by 'amount' characters to right
  constexpr void _move_right(size_type index, size_type count, size_type amount) noexcept {
    char32_t* rbegin = m_buffer + index + count - 1;
    char32_t* rend = m_buffer + index - 1;

    for (char32_t* it = rbegin; it != rend; --it) {
      it[amount] = it[0];
    }
  }

  // Move all characters in range [index, index + count) by 'amount' characters to left
  constexpr void _move_left(size_type index, size_type count, size_type amount) noexcept {
    char32_t* begin = m_buffer + index;
    char32_t* end = m_buffer + index + count;

    for (char32_t* it = begin; it != end; ++it) {
      it[-amount] = it[0];
    }
  }

  constexpr bool _is_equal(const char32_t* string, size_type string_size) const noexcept {
    if (string_size != m_length)
      return false;

    for (size_type index = 0; index < m_length; ++index)
      if (string[index] != m_buffer[index])
        return false;

    return true;
  }

  template <typename CharType>
  constexpr bool _is_equal_encoded(const CharType* string, size_type string_size) const {
    using encoding_traits = EncodingTraits<CharType>;

    char32_t* buffer = new char32_t[string_size];
    size_type buffer_size = encoding_traits::to_utf32(string, string_size, buffer);

    bool result = _is_equal(buffer, buffer_size);

    delete[] buffer;

    return result;
  }

private:
  // Length of string in chars, not including null-terminating char.
  size_type m_length = 0;
  size_type m_allocated = 0;
  char32_t* m_buffer = nullptr;
};

std::basic_istream<char, std::char_traits<char>>& operator>>(std::basic_istream<char, std::char_traits<char>>& stream, EString& out_str);