#include <gtest/gtest.h>

#include <string>

#include <EString.h>

#define ESTR(s) s.encode<char>().c_str()

namespace InsertingTests {

  namespace StringsInsertingTests {

    TEST(StringsInsertingTests, InsertingStringAtBegin) {
      EString string = ", world!";
      EString second_string = "Hello";

      string.insert(0, second_string);

      EXPECT_STREQ(ESTR(string), "Hello, world!");
    }

    TEST(StringsInsertingTests, InsertingStringInMiddle) {
      EString string = "Herld!";
      EString second_string = "llo, wo";

      string.insert(2, second_string);

      EXPECT_STREQ(ESTR(string), "Hello, world!");
    }

    TEST(StringsInsertingTests, InsertingStringToEnd) {
      EString string = "Hello";
      const char32_t* second_string = U", world!";

      string.insert(string.length(), second_string);

      EXPECT_STREQ(ESTR(string), "Hello, world!");
    }

    TEST(StringsInsertingTests, InsertingEncodedStringInMiddle) {
      EString string = "Herld!";
      const char16_t* second_string = u"llo, wo";

      string.insert(2, second_string);

      EXPECT_STREQ(ESTR(string), "Hello, world!");
    }

  }

  namespace CharactersInsertingTests {

    TEST(CharactersInsertingTests, InsertingCharactersAtBegin) {
      EString string = "BBB";

      string.insert(0, 3, U'A');

      EXPECT_STREQ(ESTR(string), "AAABBB");
    }

    TEST(CharactersInsertingTests, InsertingCharactersInMiddle) {
      EString string = "AAAA";

      string.insert(2, 5, U'B');

      std::string string_encoded = string.encode<char>();
      EXPECT_STREQ(string_encoded.c_str(), "AABBBBBAA");
    }

    TEST(CharactersInsertingTests, InsertingCharactersToEnd) {
      EString string = "AAAA";

      string.insert(string.length(), 4, U'B');

      EXPECT_STREQ(ESTR(string), "AAAABBBB");
    }

  }

}

namespace ErasingTests {

  TEST(ErasingTests, EraseAtBegin) {
    EString string = "hhHHello, world!";

    string.erase(0, 3);

    EXPECT_STREQ(ESTR(string), "Hello, world!");
  }

  TEST(ErasingTests, EraseInMiddle) {
    EString string = "Helllo, world!";

    string.erase(3, 1);

    EXPECT_STREQ(ESTR(string), "Hello, world!");
  }

  TEST(ErasingTests, EraseAtEnd) {
    EString string = "Hello, world!!!";

    string.erase(string.length() - 2, 2);

    EXPECT_STREQ(ESTR(string), "Hello, world!");
  }

  TEST(ErasingTests, EraseOutOfBuffer) {
    EString string = "Hello, world!";
    
    // Will cause assertation failure.
    EXPECT_DEATH(string.erase(string.length() + 1, 5), "Trying to erase characters out of string buffer");
  }

}