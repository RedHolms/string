#include <gtest/gtest.h>

#include <EString.h>

namespace ComparisonOperatorsTests {

  TEST(ComparisonOperatorsTests, ComprasionWithOtherEString) {
    EString first = "Hello, world!";
    EString second = "Hello, world!";

    EXPECT_TRUE(first == second);
    EXPECT_FALSE(first != second);

    second.append(U'!');

    EXPECT_FALSE(first == second);
    EXPECT_TRUE(first != second);
  }

  TEST(ComparisonOperatorsTests, ComprasionWithUtf32String) {
    EString first = "Hello, world!";
    const char32_t* second = U"Hello, world!";

    EXPECT_TRUE(first == second);
    EXPECT_FALSE(first != second);

    second = U"Hello  world!";

    EXPECT_FALSE(first == second);
    EXPECT_TRUE(first != second);
  }

  TEST(ComparisonOperatorsTests, ComprasionWithUtf8String) {
    EString first = "Hello, world!";
    const char8_t* second = u8"Hello, world!";

    EXPECT_TRUE(first == second);
    EXPECT_FALSE(first != second);

    second = u8"Hello  world!";

    EXPECT_FALSE(first == second);
    EXPECT_TRUE(first != second);
  }

}

namespace ConcatenatingTests {

  TEST(ConcatenatingTests, ConcatenateWithEString) {
    EString string = "Hello";
    EString other = ", world!";

    EXPECT_STREQ((string + other).encode<char>().c_str(), "Hello, world!");
  }

  TEST(ConcatenatingTests, ConcatenateWithUTF32) {
    EString string = "Hello";

    EXPECT_STREQ((string + U", world!").encode<char>().c_str(), "Hello, world!");
  }

  TEST(ConcatenatingTests, ConcatenateWithUTF8) {
    EString string = "Hello";

    EXPECT_STREQ((string + u8", world!").encode<char>().c_str(), "Hello, world!");
  }

}
