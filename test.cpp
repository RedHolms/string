#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

#include <iostream>

#include "string.h"

int main() {
  // i realy hate this
#ifdef _WIN32
  _setmode(_fileno(stdout), _O_U16TEXT);
#endif

   string<UTF8> str = u8"Привет, мир!"; // Hello, world!

   // NOTE: UTF-16 is NOT wide string, but i dont give a fuck
   auto wstr = str.convert<UTF16>();

#ifdef _WIN32
   std::wcout << (const wchar_t*)wstr.c_str() << std::endl;
#else
   // make sure you set ur terminal to UTF-8
   std::cout << (const char*)str.c_str() << std::endl;
#endif

   return 0;
}