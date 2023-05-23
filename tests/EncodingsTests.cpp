#include <gtest/gtest.h>

#include <string>

#include <EString.h>

#define ENCODING_TEST(test_suite_name, test_name, target_char_type, target_string, expected_string) \
TEST(test_suite_name, test_name) {\
  using string_type = std::basic_string<target_char_type, std::char_traits<target_char_type>>;\
  using string_view_type = std::basic_string_view<target_char_type, std::char_traits<target_char_type>>;\
  EString string = target_string;\
  string_type encoded_string = string.encode< target_char_type >();\
  string_view_type expected_result = expected_string;\
  EXPECT_EQ(encoded_string, expected_result);\
}

#define DECODING_TEST(test_suite_name, test_name, target_char_type, target_string, expected_string) \
TEST(test_suite_name, test_name) {\
  using string_view_type = std::basic_string_view<target_char_type, std::char_traits<target_char_type>>;\
  string_view_type original_string = target_string;\
  EString string;\
  string.decode<target_char_type>(original_string.data(), original_string.size());\
  std::u32string_view decoded_string = std::u32string_view(string.data());\
  std::u32string_view expected_result = expected_string;\
  EXPECT_EQ(decoded_string, expected_result);\
}

namespace EncodingTests {

  TEST(EncodingTests, EncodeToUtf8) {
    EString string = U"Привет, мир!";

    std::u8string encoded_string = string.encode<char8_t>();

    // To make EXPECT_STREQ available
    const char* encoded_string_ = reinterpret_cast<const char*>(encoded_string.c_str());

    const char* expected_result = (const char*)u8"Привет, мир!";

    EXPECT_STREQ(encoded_string_, expected_result);
  }

  ENCODING_TEST(EncodingTests, EncodeToUtf16, char16_t, U"Привет, мир!", u"Привет, мир!")

  ENCODING_TEST(EncodingTests, EncodeToUtf32, char32_t, U"Привет, мир!", U"Привет, мир!")

  ENCODING_TEST(EncodingTests, EncodeToWide, wchar_t, U"Привет, мир!", L"Привет, мир!")

}

namespace DecodingTests {

  DECODING_TEST(DecodingTests, DecodeFromUtf8, char8_t, u8"Привет, мир!", U"Привет, мир!")

  DECODING_TEST(DecodingTests, DecodeFromUtf16, char16_t, u"Привет, мир!", U"Привет, мир!")

  DECODING_TEST(DecodingTests, DecodeFromUtf32, char32_t, U"Привет, мир!", U"Привет, мир!")

  DECODING_TEST(DecodingTests, DecodeFromWide, wchar_t, L"Привет, мир!", U"Привет, мир!")

}
