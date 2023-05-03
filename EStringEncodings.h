#pragma once
#define EString_EStringEncodings_h_

/*
* This file constains basic encoding traits.
* You can see example of encoding traits class in AsciiEncodingTraits.
* If encoding/decoding failed, 'encoding_failed' should be throwen.
* 
* NOTE: UTF16/UTF32/WIDE encodings doesn't support different endianness.
*/

#include <stddef.h>

#include <exception>

template <typename CharType>
struct EncodingTraits;

// Throws when String::encode() or String::decode() failed.
class encoding_failed : public std::exception {
public:
  encoding_failed(const char* target_encoding, const char* info)
    : std::exception(info), m_target_encoding(target_encoding) {}

  const char* get_target_encoding() const noexcept { return m_target_encoding; }

private:
  const char* m_target_encoding;
};

// Example of encoding traits.
struct AsciiEncodingTraits {
  using encoded_char_type = char; // Minimal encoding unit.

  using size_type = size_t;

  // Maximum value that 'char_length()' or 'char_from_utf32()' can return.
  static constexpr size_type max_encoded_size = 1;

  // When 'encoding_failed' is throwen, 'e.get_target_encoding()' should be equal to this string,
  //  where 'e' is an exception of type 'encoding_failed'.
  static constexpr const char* encoding_name = "ascii";

  // Encode single utf32 character to given encoding.
  // Returns number of 'encoded_char_type', used in 'dest'.
  static constexpr size_type char_from_utf32(char32_t original_char, encoded_char_type* dest) {
    if (original_char > 0x7F)
      throw encoding_failed(encoding_name, "Trying to encode non-ASCII character.");

    dest[0] = static_cast<encoded_char_type>(original_char);

    return 1;
  }

  // Decode single character from given encoding to utf32.
  // Returns decoded character.
  static constexpr char32_t char_to_utf32(const encoded_char_type* encoded_char) {
    return static_cast<char32_t>(encoded_char[0]);
  }

  // Encode utf32 string to given encoding.
  // Note: 'decoded_string_size_in_utf32_chars' is a size
  //  of 'decoded_string' in 'char32_t
  // Returns number of 'encoded_char_type' written to 'dest'
  static constexpr size_type from_utf32(const char32_t* decoded_string, size_type decoded_string_size_in_utf32_chars, encoded_char_type* dest) {
    for (size_type index = 0; index < decoded_string_size_in_utf32_chars; ++index, ++dest) {
      char_from_utf32(decoded_string[index], dest);
    }

    return decoded_string_size_in_utf32_chars;
  }

  // Decode string from given encoding to utf32.
  // Note: 'decoded_string_size_in_chars' is a size
  //  of 'encoded_string' in 'encoded_char_type'
  // Returns number of 'char32_t' written to 'dest'
  static constexpr size_type to_utf32(const encoded_char_type* encoded_string, size_type decoded_string_size_in_chars, char32_t* dest) {
    for (size_type index = 0; index < decoded_string_size_in_chars; ++index, ++encoded_string) {
      dest[index] = char_to_utf32(encoded_string);
    }

    return decoded_string_size_in_chars;
  }

  // Get length of abstract character in 'encoded_char_type',
  //  i.e. for 2-word UTF-16 character this function should
  //  return 2.
  static constexpr size_type char_length(const encoded_char_type* encoded_char) {
    (void)encoded_char;
    return 1;
  }

  // Get length of string in 'encoded_char_type'
  static constexpr size_type str_length(const encoded_char_type* string) {
    size_type length = 0;

    // Iterate until null-terminating character is reached.
    for (; *string; ++length)
      ++string;

    return length;
  }
};

struct Utf8EncodingTraits {
  using encoded_char_type = char8_t;

  using size_type = size_t;

  static constexpr size_type max_encoded_size = 4;

  static constexpr const char* encoding_name = "utf8";

  static constexpr size_type char_from_utf32(char32_t original_char, encoded_char_type* dest) {
    if (original_char <= 0x7F) {
      dest[0] = static_cast<char8_t>(original_char & 0x7F);
      return 1;
    }
    else if (original_char <= 0x7FF) {
      dest[1] = static_cast<char8_t>(0x80 | (original_char & 0x3F));
      original_char >>= 6;
      dest[0] = static_cast<char8_t>(0xC0 | (original_char & 0x1F));
      return 2;
    }
    else if (original_char <= 0xFFFF) {
      dest[2] = static_cast<char8_t>(0x80 | (original_char & 0x3F));
      original_char >>= 6;
      dest[1] = static_cast<char8_t>(0x80 | (original_char & 0x3F));
      original_char >>= 6;
      dest[0] = static_cast<char8_t>(0xE0 | (original_char & 0x0F));
      return 3;
    }
    else if (original_char <= 0x10FFFF) {
      dest[3] = static_cast<char8_t>(0x80 | (original_char & 0x3F));
      original_char >>= 6;
      dest[2] = static_cast<char8_t>(0x80 | (original_char & 0x3F));
      original_char >>= 6;
      dest[1] = static_cast<char8_t>(0x80 | (original_char & 0x3F));
      original_char >>= 6;
      dest[0] = static_cast<char8_t>(0xF0 | (original_char & 0x07));
      return 4;
    }
    else
      throw encoding_failed(encoding_name, "Invalid UNICODE character.");
  }

  static constexpr char32_t char_to_utf32(const encoded_char_type* encoded_char) {
    encoded_char_type first_byte = encoded_char[0];

    if ((first_byte & 0x80) == 0)
      return static_cast<char32_t>(encoded_char[0]);

    char32_t result = 0;

    if ((first_byte & 0xE0) == 0xC0) {
      result = static_cast<char32_t>(first_byte & 0x1F);
      result <<= 6;
      result |= static_cast<char32_t>(encoded_char[1] & 0x3F);
    }
    else if ((first_byte & 0xF0) == 0xE0) {
      result = static_cast<char32_t>(first_byte & 0x0F);
      result <<= 6;
      result |= static_cast<char32_t>(encoded_char[1] & 0x3F);
      result <<= 6;
      result |= static_cast<char32_t>(encoded_char[2] & 0x3F);
    }
    else if ((first_byte & 0xF8) == 0xF0) {
      result = static_cast<char32_t>(first_byte & 0x07);
      result <<= 6;
      result |= static_cast<char32_t>(encoded_char[1] & 0x3F);
      result <<= 6;
      result |= static_cast<char32_t>(encoded_char[2] & 0x3F);
      result <<= 6;
      result |= static_cast<char32_t>(encoded_char[3] & 0x3F);
    }
    else
      throw encoding_failed(encoding_name, "Invalid UTF-8 character.");

    return result;
  }

  static constexpr size_type from_utf32(const char32_t* decoded_string, size_type decoded_string_size_in_utf32_chars, encoded_char_type* dest) {
    const encoded_char_type* begin = dest;

    for (
      const char32_t* end = decoded_string + decoded_string_size_in_utf32_chars;
      decoded_string != end;
      ++decoded_string, dest += char_length(dest)
      )
      char_from_utf32(decoded_string[0], dest);

    return static_cast<size_type>(dest - begin);
  }

  static constexpr size_type to_utf32(const encoded_char_type* encoded_string, size_type decoded_string_size_in_chars, char32_t* dest) {
    const char32_t* begin = dest;

    for (
      const encoded_char_type* end = encoded_string + decoded_string_size_in_chars;
      encoded_string != end;
      ++dest, encoded_string += char_length(encoded_string)
      )
      dest[0] = char_to_utf32(encoded_string);

    return static_cast<size_type>(dest - begin);
  }

  static constexpr size_type char_length(const encoded_char_type* encoded_char) {
    encoded_char_type first_byte = encoded_char[0];

    if ((first_byte & 0x80) == 0)
      return 1;
    else if ((first_byte & 0xE0) == 0xC0)
      return 2;
    else if ((first_byte & 0xF0) == 0xE0)
      return 3;
    else if ((first_byte & 0xF8) == 0xF0)
      return 4;

    throw encoding_failed(encoding_name, "Invalid UTF-8 character.");
  }

  static constexpr size_type str_length(const encoded_char_type* string) {
    size_type length = 0;

    for (; *string; ++length)
      ++string;

    return length;
  }
};

struct Utf16EncodingTraits {
  using encoded_char_type = char16_t;

  using size_type = size_t;

  static constexpr size_type max_encoded_size = 2;

  static constexpr const char* encoding_name = "utf16";

  struct _internal {
    static constexpr char16_t W1 = 0xD800, W2 = 0xDC00;
  };

  static constexpr size_type char_from_utf32(char32_t original_char, encoded_char_type* dest) {
    if (original_char <= 0xFFFF) {
      dest[0] = static_cast<char16_t>(original_char & 0xFFFF);

      return 1;
    }
    else {

      char16_t character2 = static_cast<char16_t>(original_char - 0x10000);

      dest[1] = _internal::W2 | (character2 & 0xFF);
      character2 >>= 8;
      dest[0] = _internal::W1 | (character2 & 0xFF);

      return 2;
    }
  }

  static constexpr char32_t char_to_utf32(const encoded_char_type* encoded_char) {
    encoded_char_type first_word = encoded_char[0];

    if ((first_word & 0xFF00) == _internal::W1) {
      char16_t first_byte = first_word & 0xFF;
      char16_t second_byte = encoded_char[1] & 0xFF;

      char32_t result = first_byte;
      result <<= 8;
      result |= second_byte;
      result += 0x10000;

      return result;
    }
    else {
      return static_cast<char32_t>(first_word);
    }
  }

  static constexpr size_type from_utf32(const char32_t* decoded_string, size_type decoded_string_size_in_utf32_chars, encoded_char_type* dest) {
    const encoded_char_type* begin = dest;

    for (
      const char32_t* end = decoded_string + decoded_string_size_in_utf32_chars;
      decoded_string != end;
      ++decoded_string, dest += char_length(dest)
      )
      char_from_utf32(decoded_string[0], dest);

    return static_cast<size_type>(dest - begin);
  }

  static constexpr size_type to_utf32(const encoded_char_type* encoded_string, size_type encoded_string_size_in_chars, char32_t* dest) {
    const char32_t* begin = dest;

    for (
      const encoded_char_type* end = encoded_string + encoded_string_size_in_chars;
      encoded_string != end;
      ++dest, encoded_string += char_length(encoded_string)
      )
      dest[0] = char_to_utf32(encoded_string);

    return static_cast<size_type>(dest - begin);
  }

  static constexpr size_type char_length(const encoded_char_type* encoded_char) {
    if ((encoded_char[0] & 0xFF00) == _internal::W1)
      return 2;
    else
      return 1;
  }

  static constexpr size_type str_length(const encoded_char_type* string) {
    size_type length = 0;

    for (; *string; ++length)
      ++string;

    return length;
  }
};

struct Utf32EncodingTraits {
  using encoded_char_type = char32_t;

  using size_type = size_t;

  static constexpr size_type max_encoded_size = 1;

  static constexpr const char* encoding_name = "utf32";

  static constexpr size_type char_from_utf32(char32_t original_char, encoded_char_type* dest) {
    dest[0] = original_char;
    return 1;
  }

  static constexpr char32_t char_to_utf32(const encoded_char_type* encoded_char) {
    return encoded_char[0];
  }

  static constexpr size_type from_utf32(const char32_t* decoded_string, size_type decoded_string_size_in_utf32_chars, encoded_char_type* dest) {
    for (size_type index = 0; index < decoded_string_size_in_utf32_chars; ++index)
      dest[index] = decoded_string[index];

    return decoded_string_size_in_utf32_chars;
  }

  static constexpr size_type to_utf32(const encoded_char_type* encoded_string, size_type decoded_string_size_in_chars, char32_t* dest) {
    for (size_type index = 0; index < decoded_string_size_in_chars; ++index)
      dest[index] = encoded_string[index];

    return decoded_string_size_in_chars;
  }

  static constexpr size_type char_length(const encoded_char_type* encoded_char) {
    (void)encoded_char;
    return 1;
  }

  static constexpr size_type str_length(const encoded_char_type* string) {
    size_type length = 0;

    for (; *string; ++length)
      ++string;

    return length;
  }
};

struct WideEncodingTraits {
  using encoded_char_type = wchar_t;

  using size_type = size_t;

  static constexpr size_type max_encoded_size = 1;

  static constexpr const char* encoding_name = "wide";

  struct _internal {
    static constexpr char32_t max_char = static_cast<char32_t>(static_cast<wchar_t>(~0L));
  };

  static constexpr size_type char_from_utf32(char32_t original_char, encoded_char_type* dest) {
    if (original_char > _internal::max_char)
      throw encoding_failed(encoding_name, "Too big character to encode it as wide character");

    dest[0] = static_cast<wchar_t>(original_char & _internal::max_char);

    return 1;
  }

  static constexpr char32_t char_to_utf32(const encoded_char_type* encoded_char) {
    return static_cast<char32_t>(encoded_char[0]);
  }

  static constexpr size_type from_utf32(const char32_t* decoded_string, size_type decoded_string_size_in_utf32_chars, encoded_char_type* dest) {
    for (size_type index = 0; index < decoded_string_size_in_utf32_chars; ++index, ++dest)
      char_from_utf32(decoded_string[index], dest);

    return decoded_string_size_in_utf32_chars;
  }

  static constexpr size_type to_utf32(const encoded_char_type* encoded_string, size_type decoded_string_size_in_chars, char32_t* dest) {
    for (size_type index = 0; index < decoded_string_size_in_chars; ++index, ++encoded_string)
      dest[index] = char_to_utf32(encoded_string);

    return decoded_string_size_in_chars;
  }

  static constexpr size_type char_length(const encoded_char_type* encoded_char) {
    (void)encoded_char;
    return 1;
  }

  static constexpr size_type str_length(const encoded_char_type* string) {
    size_type length = 0;

    for (; *string; ++length)
      ++string;

    return length;
  }
};

template<>
struct EncodingTraits<char> : AsciiEncodingTraits {};

template<>
struct EncodingTraits<char8_t> : Utf8EncodingTraits {};

template<>
struct EncodingTraits<char16_t> : Utf16EncodingTraits {};

template<>
struct EncodingTraits<char32_t> : Utf32EncodingTraits {};

template<>
struct EncodingTraits<wchar_t> : WideEncodingTraits {};

#ifdef _WIN32

using ansichar_t = unsigned char;

struct AnsiEncodingTraits {
  using encoded_char_type = ansichar_t;

  using size_type = size_t;

  // ANSI can also be UTF-8, so let's assume that it is.
  static constexpr size_type max_encoded_size = 4;

  static constexpr const char* encoding_name = "ansi";

  // Implimented in 'EString_Ansi.cpp' to not include 'Windows.h'.
  static size_type char_from_utf32(char32_t original_char, encoded_char_type* dest);
  static char32_t char_to_utf32(const encoded_char_type* encoded_char);
  static size_type from_utf32(const char32_t* decoded_string, size_type decoded_string_size_in_utf32_chars, encoded_char_type* dest);
  static size_type to_utf32(const encoded_char_type* encoded_string, size_type decoded_string_size_in_chars, char32_t* dest);
  static size_type char_length(const encoded_char_type* encoded_char);
  static size_type str_length(const encoded_char_type* string);
};

template<>
struct EncodingTraits<ansichar_t> : AnsiEncodingTraits {};

#endif