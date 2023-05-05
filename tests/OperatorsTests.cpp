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