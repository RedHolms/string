#include <gtest/gtest.h>

#include <string>

#include <EString.h>

namespace EncodingTests {

  TEST(EncodingTests, EncodeToUtf8) {
    EString string = U"Привет, мир!";

    std::u8string encoded_string = string.encode<char8_t>();

    // To make EXPECT_STREQ available
    const char* encoded_string_ = reinterpret_cast<const char*>(encoded_string.c_str());

    const char* expected_result = (const char*)u8"Привет, мир!";

    EXPECT_STREQ(encoded_string_, expected_result);
  }

  TEST(EncodingTests, EncodeToUtf16) {
    EString string = U"Привет, мир!";

    std::u16string encoded_string = string.encode<char16_t>();

    std::u16string_view expected_result = u"Привет, мир!";

    EXPECT_EQ(encoded_string, expected_result);
  }

}