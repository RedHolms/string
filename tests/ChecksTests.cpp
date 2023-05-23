#include <gtest/gtest.h>

#include <EString.h>

namespace StartsWithTests {

  TEST(StartsWithTests, StartsWithEString) {
    EString first = "Hello, world!";
    EString second = "Hello";

    EXPECT_TRUE(first.startswith(second));
    EXPECT_TRUE(first.startswith(first));
  }

  TEST(StartsWithTests, StartsWithUtf32String) {
    EString first = "Hello, world!";
    const char32_t* second = U"Hello";

    EXPECT_TRUE(first.startswith(second));
    EXPECT_TRUE(first.startswith(first.data()));
  }

  TEST(StartsWithTests, StartsWithUtf8String) {
    EString first = U"Привет, world!";
    const char8_t* second = u8"Привет";

    EXPECT_TRUE(first.startswith(second));
    
    second = u8"Прувет!";

    EXPECT_FALSE(first.startswith(second));
  }

}

namespace EndsWithTests {

  TEST(EndsWithTests, EndsWithEString) {
    EString first = "Hello, world!";
    EString second = "world!";

    EXPECT_TRUE(first.endswith(second));
    EXPECT_TRUE(first.endswith(first));
  }

  TEST(EndsWithTests, EndsWithUtf32String) {
    EString first = "Hello, world!";
    const char32_t* second = U"world!";

    EXPECT_TRUE(first.endswith(second));
    EXPECT_TRUE(first.endswith(first.data()));
  }

  TEST(EndsWithTests, EndsWithUtf8String) {
    EString first = U"Hello, мир!";
    const char8_t* second = u8"мир!";

    EXPECT_TRUE(first.endswith(second));

    second = u8"рим!";

    EXPECT_FALSE(first.endswith(second));
  }

}
