#pragma once

#include <stddef.h>
#include <string.h>

#include <type_traits>

template <class StringType, typename CharType>
struct string_iterator {
  using encoding = typename StringType::encoding;
  using char_type = CharType;
  
  using size_type = typename StringType::size_type;
  using difference_type = typename StringType::difference_type;
  
  /* reference emulator */
  struct _CharDescriptor {
    _CharDescriptor(char_type* p, StringType* s) : Ptr(p), StringContainer(s) {}

    string_iterator operator&() {
      return string_iterator(Ptr, StringContainer);
    }

    operator char_type&() {
      return *Ptr;
    }

    operator char_type() {
      return *Ptr;
    }

    _CharDescriptor& operator=(const char_type* chr) {
      auto chr_len = encoding::chrlen(chr);
      auto orig_chr_len = encoding::chrlen(Ptr);
      
      if (chr_len < orig_chr_len) {
        difference_type diff = orig_chr_len - chr_len;

        StringContainer->_move_left(Ptr + chr_len, diff);
      }
      else if (chr_len > orig_chr_len) {
        difference_type diff = chr_len - orig_chr_len;

        StringContainer->_want_size(diff);
        StringContainer->_move_right(Ptr + chr_len, diff);
      }

      memcpy(Ptr, chr, chr_len * sizeof(char_type));
    }

    char_type* Ptr = nullptr;
    StringType* StringContainer = nullptr;
  };
  
  using char_descriptor = typename std::conditional<
    encoding::need_char_descriptor,
    _CharDescriptor,
    char_type&
  >::type;

  string_iterator(char_type* p, StringType* s) : Ptr(p), StringContainer(s) {}

  char_descriptor operator*() {
    if constexpr (StringType::encoding::need_char_descriptor) {
      return char_descriptor(Ptr, StringContainer);
    }
    else {
      return *Ptr;
    }
  }

  operator char_type* () {
    return Ptr;
  }

  char_descriptor operator[](difference_type off) const noexcept {
    char_type* chr_ptr = StringType::encoding::index(Ptr, off);
    
    if constexpr (StringType::encoding::need_char_descriptor) {
      return char_descriptor(chr_ptr, StringContainer);
    }
    else {
      return *chr_ptr;
    }
  }
  
  string_iterator& operator++() noexcept {
    ++Ptr;
    return *this;
  }

  string_iterator operator++(int) noexcept {
    string_iterator temp = *this;
    ++Ptr;
    return temp;
  }

  string_iterator& operator--() noexcept {
    --Ptr;
    return *this;
  }

  string_iterator operator--(int) noexcept {
    string_iterator temp = *this;
    --Ptr;
    return temp;
  }

  string_iterator& operator+=(difference_type off) noexcept {
    Ptr += off;
    return *this;
  }

  string_iterator& operator-=(difference_type off) noexcept {
    Ptr -= off;
    return *this;
  }

  string_iterator operator+(difference_type off) const noexcept {
    return string_iterator(Ptr + off);
  }

  string_iterator operator-(difference_type off) const noexcept {
    return string_iterator(Ptr - off);
  }

  char_type* Ptr = nullptr;
  StringType* StringContainer = nullptr;
};

template <class StringType, typename CharType>
string_iterator<StringType, CharType> operator+(
  typename string_iterator<StringType, CharType>::difference_type off,
  string_iterator<StringType, CharType> next
) noexcept {
  next += off;
  return next;
}

template <typename Encoding>
class string {
public:
  using _Type = string<Encoding>;
  
  using encoding      = Encoding;
  using char_table    = typename Encoding::char_table;
  using char_type     = typename Encoding::char_type;
  using max_char_type = typename Encoding::max_char_type;

  using size_type = size_t;
  using difference_type = ptrdiff_t;

  using iterator = string_iterator<_Type, char_type>;
  using const_iterator = string_iterator<_Type, const char_type>;

public:
  string() = default;

  string(const char_type* str) : string(str, Encoding::strlen(str)) {}

  string(const char_type* str, size_type length) : string() {
    append(str, length);
  }

  string(const string& o) : string() { *this = o; }
  string(string&& o) : string() { *this = o; }

  ~string() {
    _reallocate(0);
  }

public:
  string& operator=(const string& o) {
    this->~string();
    append(o.m_buffer);
    return *this;
  }

  string& operator=(string&& o) {
    this->~string();
    m_buffer = o.m_buffer;
    m_allocated = o.m_allocated;

    o.m_buffer = nullptr;
    o.m_allocated = 0;

    return *this;
  }

  string& operator=(const char_type* str) {
    this->~string();
    append(str);
    return *this;
  }

public:
  // convert string to `NewEncoding` encoding
  template <typename NewEncoding>
  string<NewEncoding> convert() const {
    using new_char_type = typename NewEncoding::char_type;
    
    static_assert(std::is_same<char_table, typename NewEncoding::char_table>::value, "convertation impossible");
    
    if constexpr (std::is_same<Encoding, NewEncoding>::value) {
      return *this;
    }

    string<NewEncoding> result;

    if (m_buffer && m_buffer[0]) {

      if constexpr (std::is_same<char_table, typename NewEncoding::char_table>::value) {
        size_type size = Encoding::strsize(m_buffer);
        for (size_type i = 0; i < size;) {
          max_char_type decoded = Encoding::decode_char(m_buffer + i);

          new_char_type encoded[NewEncoding::max_char_length];
          size_type encoded_length;

          NewEncoding::encode_char(decoded, encoded, &encoded_length);

          result.append(encoded, 1);

          i += Encoding::chrlen(m_buffer + i);
        }
      }
    }

    return result;
  }

public:
  iterator operator[](size_type i) {
    return *iterator(Encoding::index(m_buffer, i), this);
  }

  const_iterator operator[](size_type i) const {
    return *const_iterator(Encoding::index(m_buffer, i), this);
  }

  // get pointer to the first element in the buffer
  iterator begin() {
    return iterator(m_buffer, this);
  }

  const_iterator begin() const {
    return const_iterator(m_buffer, this);
  }

  // get pointer to the last element in the buffer
  iterator end() {
    return iterator(Encoding::index(m_buffer, Encoding::strlen(m_buffer)), this);
  }

  const_iterator end() const {
    return const_iterator(Encoding::index(m_buffer, Encoding::strlen(m_buffer)), this);
  }

public:
  // get LENGTH of string (i.e. COUNT of elements)
  const size_type length() const {
    return m_buffer ? Encoding::strlen(m_buffer) : 0;
  }

  // get SIZE IN BYTES of string
  const size_type raw_size() const {
    return Encoding::strsize(m_buffer) * sizeof(char_type);
  }

  // get raw string pointer
  const char_type* c_str() const {
    return m_buffer;
  }

  char_type* data() {
    return m_buffer;
  }

  const char_type* data() const {
    return m_buffer;
  }

public:
  //template <typename AnotherEncoding>
  //bool operator==(const string<AnotherEncoding>& o) {
  //  return *this == o.convert<Encoding>();
  //}

  bool operator==(const string& o) {
    return *this == o.m_buffer;
  }

  bool operator==(const char_type* s) {
    auto buff_size = Encoding::strsize(m_buffer);
    auto str_size = Encoding::strsize(s);

    if (buff_size != str_size) return false;

    return memcmp(m_buffer, s, buff_size * sizeof(char_type)) == 0;
  }

  //template <typename AnotherEncoding>
  //bool operator!=(const string<AnotherEncoding>& o) {
  //  return *this != o.convert<Encoding>();
  //}

  bool operator!=(const string& o) {
    return *this != o.m_buffer;
  }

  bool operator!=(const char_type* s) {
    return !(*this == s);
  }

public:
  // append data to the end of buffer
  void append(const char_type* str) {
    append(str, Encoding::strlen(str));
  }

  void append(const char_type* str, size_type length) {
    auto size = Encoding::strsize(str, length);

    _want_size(size + 1);

    auto start_place = end().Ptr;
    
    memcpy(start_place, str, size * sizeof(char_type));
    start_place[size] = 0;
  }

  // insert data in the buffer
  void insert(size_type i, const char_type* str) {
    insert(i, str, Encoding::strlen(str));
  }

  void insert(size_type i, const char_type* str, size_type length) {
    auto size = Encoding::strsize(str, length);
    bool contained_string = this->length() > 0;

    _want_size(size + 1);

    _move_right(i, size);
    memcpy(Encoding::index(m_buffer, i), str, size * sizeof(char_type));
    
    if (!contained_string)
      Encoding::index(m_buffer, i)[size] = 0;
  }

  void insert(const_iterator it, const char_type* str) {
    insert(it, str, Encoding::strlen(str));
  }

  void insert(const_iterator it, const char_type* str, size_type length) {
    insert(static_cast<size_type>(it.Ptr - m_buffer), str, length);
  }

private:
  // want space with size `size` at the end of buffer
  void _want_size(size_type size) {
    auto buffer_size = Encoding::strsize(m_buffer);
    auto gap = m_allocated - buffer_size;

    if (gap < size)
      _growth(m_allocated + size);
  }

  void _move_right(size_type index, size_type off) {
    auto buffer_size = Encoding::strsize(m_buffer);
    for (difference_type i = buffer_size - 1; i >= static_cast<difference_type>(index); --i)
      m_buffer[i + off] = m_buffer[i];
  }

  void _move_left(size_type index, size_type off) {
    auto buffer_size = Encoding::strsize(m_buffer);
    for (difference_type i = index; i < buffer_size; ++i)
      m_buffer[i - off] = m_buffer[i];
  }

  // growth the buffer (assert new allocated size >= `size`)
  void _growth(size_type size) {
    auto toalloc = m_allocated * 2;

    if (toalloc < size)
      toalloc = size;

    _reallocate(toalloc);
  }

  // reallocate buffer to `toalloc` size (assert `toalloc` >= `raw_size()`) and copy all data from previous buffer
  // if `toalloc` == 0, than buffer will be deleted
  void _reallocate(size_type toalloc) {
    size_type buffer_size = Encoding::strsize(m_buffer);
    char_type* prev_buffer = m_buffer;
    size_type prev_allocated = m_allocated;

    if (toalloc < buffer_size)
      return;

    m_buffer = nullptr;
    m_allocated = toalloc;

    if (m_allocated > 0) {
      size_type toalloc_bytes = m_allocated * sizeof(char_type);

      m_buffer = (char_type*) ::operator new(toalloc_bytes);
      memset(m_buffer, 0, toalloc_bytes);

      if (prev_buffer)
        memcpy(m_buffer, prev_buffer, buffer_size * sizeof(char_type));
    }

    if (prev_buffer)
      ::operator delete(prev_buffer);
  }

private:
   char_type* m_buffer = nullptr; // String buffer (null-terminated)
   size_type m_allocated = 0;     // Size in bytes of space allocated for buffer
};

namespace CharTable {

  struct ASCII {
    using max_char_type = char;
  };

  struct UNICODE {
    using max_char_type = char32_t;
  };

}

struct ASCII {
  using char_table = CharTable::ASCII;

  using char_type = char;
  using max_char_type = char_table::max_char_type;
};

struct UTF8 {
  using char_table = CharTable::UNICODE;

#ifdef __cpp_char8_t
  using char_type = char8_t;
#else
  using char_type = char;
#endif

  using max_char_type = char_table::max_char_type;

  using size_type = size_t;

  static constexpr size_type max_char_length = 4;
  static constexpr bool need_char_descriptor = true;

  static constexpr char_type* index(char_type* s, size_type i) {
    for (size_type j = 0; s && j < i; j++, s += chrlen(s));
    return s;
  }

  static constexpr const char_type* index(const char_type* s, size_type i) {
    for (size_type j = 0; s && j <= i; j++, s += chrlen(s));
    return s;
  }

  static constexpr size_type chrlen(const char_type* p) {
    if (!p) return 0;

    const char_type chr = *p;
    size_type size = 1;

    if (chr & 0x80) {
      for (
        size_type shift = 1;
        (chr << shift) & 0x80;
        shift++, size++
        );
    }

    return size;
  }

  static constexpr size_type strlen(const char_type* s) {
    size_type length = 0;
    for (;
      s && *s;
      length++, s += chrlen(s)
      );
    return length;
  }

  static constexpr size_type strsize(const char_type* s, size_type length) {
    size_type size = 0;
    for (size_type i = 0; s && i < length; i++) {
      auto chr_len = chrlen(s);
      s += chr_len;
      size += chr_len;
    }
    return size;
  }

  static constexpr size_type strsize(const char_type* s) {
    size_type size = 0;
    for (; s && *s;) {
      auto chr_len = chrlen(s);
      s += chr_len;
      size += chr_len;
    }
    return size;
  }

  static constexpr max_char_type decode_char(const char_type* p) {
    max_char_type decoded = 0;
    char_type chr = p[0];

    if (!(chr & 0x80)) {
      decoded = p[0];
    }
    else if ((chr & 0xE0) == 0xC0) {
      decoded =
        (p[0] & 0x1F) << 6 |
        (p[1] & 0x3F);
    }
    else if ((chr & 0xF0) == 0xE0) {
      decoded =
        (p[0] & 0x0F) << 12 |
        (p[1] & 0x3F) << 6 |
        (p[2] & 0x3F);
    }
    else if ((chr & 0xF8) == 0xF0) {
      decoded =
        (p[0] & 0x07) << 18 |
        (p[1] & 0x3F) << 12 |
        (p[2] & 0x3F) << 6 |
        (p[3] & 0x3F);
    }

    return decoded;
  }

  static void encode_char(max_char_type decoded, char_type* dest, size_type* used) {
    size_type& used_ref = *used;
    used_ref = 0;

    if (decoded <= 0x7F) {
      dest[0] = (char_type)(decoded & 0x7F);
      used_ref = 1;
    }
    else if (decoded <= 0x7FF) {
      dest[0] = 0xC0 | (char_type)((decoded >> 5) & 0x1F);
      dest[1] = 0x80 | (char_type)((decoded) & 0x3F);
      used_ref = 2;
    }
    else if (decoded <= 0xFFFF) {
      dest[0] = 0xE0 | (char_type)((decoded >> 12) & 0x0F);
      dest[1] = 0x80 | (char_type)((decoded >> 6) & 0x3F);
      dest[2] = 0x80 | (char_type)((decoded) & 0x3F);
      used_ref = 3;
    }
    else if (decoded <= 0x10FFFF) {
      dest[0] = 0xF0 | (char_type)((decoded >> 18) & 0x07);
      dest[1] = 0x80 | (char_type)((decoded >> 12) & 0x3F);
      dest[2] = 0x80 | (char_type)((decoded >> 6) & 0x3F);
      dest[3] = 0x80 | (char_type)((decoded) & 0x3F);
      used_ref = 4;
    }
  }
};

struct UTF16 {
  using char_table = CharTable::UNICODE;

  using char_type = char16_t;
  using max_char_type = char_table::max_char_type;

  using size_type = size_t;

  static constexpr size_type max_char_length = 2;
  static constexpr bool need_char_descriptor = true;

  static constexpr char_type* index(char_type* s, size_type i) {
    for (size_type j = 0; s && j < i; j++, s += chrlen(s));
    return s;
  }

  static constexpr const char_type* index(const char_type* s, size_type i) {
    for (size_type j = 0; s && j <= i; j++, s += chrlen(s));
    return s;
  }

  static constexpr size_type chrlen(const char_type* p) {
    if (p[0] >= 0xD800 && p[0] <= 0xDBFF)
      return 2;
    else
      return 1;
  }

  static constexpr size_type strlen(const char_type* s) {
    size_type length = 0;
    for (;
      s && *s;
      length++, s += chrlen(s)
      );
    return length;
  }

  static constexpr size_type strsize(const char_type* s, size_type length) {
    size_type size = 0;
    for (size_type i = 0; s && i < length; i++) {
      auto chr_len = chrlen(s);
      s += chr_len;
      size += chr_len;
    }
    return size;
  }

  static constexpr size_type strsize(const char_type* s) {
    size_type size = 0;
    for (; s && *s;) {
      auto chr_len = chrlen(s);
      s += chr_len;
      size += chr_len;
    }
    return size;
  }

  static constexpr max_char_type decode_char(const char_type* p) {
    size_type chr_len = chrlen(p);

    if (chr_len == 2) {
      max_char_type
        w1 = p[0] - 0xD800,
        w2 = p[0] - 0xDC00;

      max_char_type decoded =
        ((w1 & 0xFFFF) << 10) | (w2 & 0xFFFF);

      decoded += 0x10000;

      return decoded;
    }
    else
      return (max_char_type)p[0];
  }

  static void encode_char(max_char_type decoded, char_type* dest, size_type* used) {
    if (decoded >= 0x10000) {
      max_char_type U = decoded - 0x10000;

      max_char_type
        w1 = (U >> 10) & 0xFFFF,
        w2 = 0xFFFF;

      dest[0] = 0xD800 + w1;
      dest[1] = 0xDC00 + w2;

      *used = 2;
    }
    else {
      dest[0] = (max_char_type)(decoded & 0xFFFF);
      *used = 1;
    }
  }
};

struct UTF32 {
  using char_table = CharTable::UNICODE;

  using char_type = char32_t;
  using max_char_type = char_table::max_char_type;

  using size_type = size_t;

  static constexpr size_type max_char_length = 1;
  static constexpr bool need_char_descriptor = false;

  static constexpr char_type* index(char_type* s, size_type i) {
    return s + i;
  }

  static constexpr const char_type* index(const char_type* s, size_type i) {
    return s + i;
  }

  static constexpr size_type chrlen(const char_type* p) {
    return 1;
  }

  static constexpr size_type strlen(const char_type* s) {
    size_type length = 0;
    for (; s && *s; length++, s++);
    return length;
  }

  static constexpr size_type strsize(const char_type* s, size_type length) {
    return length;
  }

  static constexpr size_type strsize(const char_type* s) {
    return strlen(s);
  }

  static constexpr max_char_type decode_char(const char_type* p) {
    return p[0];
  }

  static void encode_char(max_char_type decoded, char_type* dest, size_type* used) {
    dest[0] = decoded;
    *used = 1;
  }
};