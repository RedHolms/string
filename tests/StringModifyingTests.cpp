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
    
    size_t prev_allocated = string.capacity();
    size_t prev_size = string.length();
    char32_t* prev_data = string.data();

    string.erase(11, 5);
    EXPECT_EQ(string.capacity(), prev_allocated);
    EXPECT_EQ(string.length(), prev_size);
    EXPECT_EQ(string.data(), prev_data);

    string.erase(20, 3);
    EXPECT_EQ(string.capacity(), prev_allocated);
    EXPECT_EQ(string.length(), prev_size);
    EXPECT_EQ(string.data(), prev_data);
  }

}

namespace AppendingTests {

  TEST(AppendingTests, AppendUtf32String) {
    EString string = U"Hello";

    string.append(", world!");

    EXPECT_STREQ(ESTR(string), "Hello, world!");
  }

  TEST(AppendingTests, AppendCharacters) {
    EString string = U"Hello, world!";

    string.append(2, U'!');

    EXPECT_STREQ(ESTR(string), "Hello, world!!!");
  }

  TEST(AppendingTests, AppendEncodedString) {
    EString string = U"Hello";

    string.append(u8", мир!");

    std::u8string encoded_string = string.encode<char8_t>();

    const char* gared_string = reinterpret_cast<const char*>(encoded_string.c_str());

    EXPECT_STREQ(gared_string, "Hello, мир!");
  }

}

namespace PushPopTests {

  TEST(PushPopTests, PushBackASCIITest) {
    EString string = U"Привет, мир";

    string.push_back('!');

    EXPECT_EQ(string.encode<char32_t>(), U"Привет, мир!");
  }

  TEST(PushPopTests, PushBackUTF8Test) {
    EString string = U"Привет, ми";

    string.push_back(u8"р");

    EXPECT_EQ(string.encode<char32_t>(), U"Привет, мир");
  }

  TEST(PushPopTests, NormalPopBackTest) {
    EString string = U"Привет, мир!!";

    EXPECT_EQ(string.pop_back(), U'!');
    EXPECT_EQ(string.encode<char32_t>(), U"Привет, мир!");
  }

  TEST(PushPopTests, PopBackOutOfBufferTest) {
    EString string;

    EXPECT_EQ(string.pop_back(), 0);
  }

}
