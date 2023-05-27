#include <gtest/gtest.h>

#include <EString.h>

namespace ConstructingTests {

  TEST(ConstructingTests, ConstructWithASCII) {
    EString string = "Hello, world!";

    EXPECT_EQ(string.encode<char>(), "Hello, world!");
  }

  TEST(ConstructingTests, ConstructWithUTF8) {
    EString string = u8"Привет, мир!";

    EXPECT_EQ(string.encode<char32_t>(), U"Привет, мир!");
  }

  TEST(ConstructingTests, ConstructWithUTF32) {
    EString string = U"Привет, мир!";

    EXPECT_EQ(string.encode<char32_t>(), U"Привет, мир!");
  }

  TEST(ConstructingTests, CopyEString) {
    EString string = U"Привет, мир!";
    EString copy = EString(string);

    EXPECT_EQ(string.length(), copy.length());
    EXPECT_EQ(string.encode<char32_t>(), copy.encode<char32_t>());
  }

  TEST(ConstructingTests, MoveEString) {
    EString string = U"Привет, мир!";
    char32_t* previous_data = string.data();

    EString moved = std::move(string);

    EXPECT_EQ(string.length(), 0);
    EXPECT_EQ(moved.data(), previous_data);
  }

}
