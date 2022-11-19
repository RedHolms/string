#ifdef _WIN32
#include <Windows.h>
#endif

#include <iostream>

#include "string.h"

#define COLOR_FAIL "\033[91m"
#define COLOR_SUCCESS "\033[92m"
#define COLOR_INFO "\033[97m"
#define COLOR_RESTORE "\033[39m"

string<UTF8> creation_test() {
  return u8"Привет, мир!";
}

string<UTF8> appending_test() {
  string<UTF8> str = u8"Привет, ";
  
  str.append(u8"мир!");
  
  return str;
}

string<UTF8> inserting_test() {
  string<UTF8> str = u8"мир!";
  
  str.insert(0, u8"Привет, ");

  return str;
}

string<UTF8> conversion_test() {
  string<UTF32> str = U"嶲 (CJK Compatibility Ideograph)";

  return str.convert<UTF8>();
}

#define string_test(fn, expected) \
  do {\
    std::cout << COLOR_INFO "Test '" #fn "'\t..." COLOR_RESTORE << std::endl;\
    auto received = fn();\
    if (received != expected) {\
      std::cerr << COLOR_FAIL "Test '" #fn "'\tfailed!" << std::endl\
        << "\tExpected output: " << (const char*)expected << std::endl\
        << "\tReceived output: " << (const char*)received.c_str() << COLOR_RESTORE << std::endl;\
    } else {\
      std::cout << COLOR_SUCCESS "Test '" #fn "'\tpassed!" COLOR_RESTORE << std::endl;\
    }\
  } while(0)

int main() {
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  setvbuf(stdout, nullptr, _IOFBF, 1000);
#endif

  std::cout << std::endl;

  string_test(creation_test, u8"Привет, мир!");
  string_test(appending_test, u8"Привет, мир!");
  string_test(inserting_test, u8"Привет, мир!");
  string_test(conversion_test, u8"嶲 (CJK Compatibility Ideograph)");

  std::cout << std::endl;

  return 0;
}