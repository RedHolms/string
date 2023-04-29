#include "EString.h"

#include <Windows.h>

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

size_type AnsiEncodingTraits::to_utf32(const encoded_char_type* encoded_string, size_type decoded_string_size_in_chars, char32_t* dest) {
  throw encoding_failed(AnsiEncodingTraits::encoding_name, "Ansi not implimented.");
}

size_type AnsiEncodingTraits::char_length(const encoded_char_type* encoded_char) {
  throw encoding_failed(AnsiEncodingTraits::encoding_name, "Ansi not implimented.");
}

size_type AnsiEncodingTraits::str_length(const encoded_char_type* string) {
  throw encoding_failed(AnsiEncodingTraits::encoding_name, "Ansi not implimented.");
}
