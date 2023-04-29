#include "EString.h"

static void decode_data_from_console(std::string const& buffer, String& out_str);

std::basic_istream<char, std::char_traits<char>>& operator>>(std::basic_istream<char, std::char_traits<char>>& stream, String& out_str) {
  std::string buffer;
  stream >> buffer;

  decode_data_from_console(buffer, out_str);

  return stream;
}

#ifdef _WIN32
#include <Windows.h>

static void decode_data_from_console(std::string const& buffer, String& out_str) {
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