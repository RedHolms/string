#include "EString.h"

#ifdef _WIN32
#include <Windows.h>
#endif

static void decode_data_from_console(std::string const& buffer, EString& out_str);

std::basic_istream<char, std::char_traits<char>>& operator>>(std::basic_istream<char, std::char_traits<char>>& stream, EString& out_str) {
  std::string buffer;
  stream >> buffer;

  decode_data_from_console(buffer, out_str);

  return stream;
}

#ifdef _WIN32

static void decode_data_from_console(std::string const& buffer, EString& out_str) {
  unsigned int console_code_page = GetConsoleCP();

  wchar_t* wide_buffer = new wchar_t[buffer.size()];

  int return_value = MultiByteToWideChar(
    console_code_page, 0,
    buffer.c_str(), buffer.size(),
    wide_buffer, buffer.size()
  );

  if (return_value == 0) {
    throw encoding_failed("console", "Failed to decode string from console");
  }

  out_str.decode<wchar_t>(wide_buffer, return_value);

  delete[] wide_buffer;
}

#endif

// ANSI encoding
#ifdef _WIN32 

using size_type = typename AnsiEncodingTraits::size_type;
using encoded_char_type = typename AnsiEncodingTraits::encoded_char_type;

size_type AnsiEncodingTraits::char_from_utf32(char32_t original_char, encoded_char_type* dest) {
  throw encoding_failed(AnsiEncodingTraits::encoding_name, "Ansi not implimented.");
}

char32_t AnsiEncodingTraits::char_to_utf32(const encoded_char_type* encoded_char) {
  throw encoding_failed(AnsiEncodingTraits::encoding_name, "Ansi not implimented.");
}

size_type AnsiEncodingTraits::from_utf32(const char32_t* decoded_string, size_type decoded_string_size_in_utf32_chars, encoded_char_type* dest) {
  throw encoding_failed(AnsiEncodingTraits::encoding_name, "Ansi not implimented.");
}

size_type AnsiEncodingTraits::to_utf32(const encoded_char_type* encoded_string, size_type encoded_string_size_in_chars, char32_t* dest) {
  throw encoding_failed(AnsiEncodingTraits::encoding_name, "Ansi not implimented.");
}

size_type AnsiEncodingTraits::char_length(const encoded_char_type* encoded_char) {
  throw encoding_failed(AnsiEncodingTraits::encoding_name, "Ansi not implimented.");
}

size_type AnsiEncodingTraits::str_length(const encoded_char_type* string) {
  throw encoding_failed(AnsiEncodingTraits::encoding_name, "Ansi not implimented.");
}

#endif
