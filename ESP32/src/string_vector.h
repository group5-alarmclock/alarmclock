#ifndef STRING_VECTOR_H_
#define STRING_VECTOR_H_

#include <Arduino.h>

class StringVector
{
  const char delim = '-';
  const char escape = '\\';
  String str;
  unsigned int size;

  unsigned int getStrIndexStart(unsigned int index) const
  {
    if (index >= size)
    {
      return size;
    }
    unsigned int word_idx = 0;
    unsigned int str_index_start = 0;
    while (word_idx < index)
    {
      if (str[str_index_start] == escape)
      { // next is a literal, so increment the index twice
        ++str_index_start;
      }
      else if (str[str_index_start] == delim)
      { // this is in-between words
        ++word_idx;
      }
      ++str_index_start;
    }
    return str_index_start;
  }
  unsigned int getStrIndexBack(unsigned int index) const
  {
    if (index >= size)
    {
      return size;
    }
    unsigned int str_index_start = getStrIndexStart(index);
    unsigned int str_index_back = str_index_start;
    unsigned int n = str.length();
    while (str_index_back < n)
    {
      if (str[str_index_back] == escape)
      { // next is a literal
        ++str_index_back;
      }
      else if (str[str_index_back] == delim)
      { // this is in-between words
        --str_index_back;
        break;
      }
      ++str_index_back;
    }
    return str_index_back;
  }
  String escapeStr(const String &s)
  {
    String s_escaped;
    unsigned int n = s.length();
    for (unsigned int i = 0; i < n; ++i)
    {
      if (s[i] == delim || s[i] == escape)
      {
        s_escaped.concat(escape);
      }
      s_escaped.concat(s[i]);
    }
    return s_escaped;
  }
  String getWord(unsigned int index) const
  {
    if (index >= size)
    {
      return "";
    }

    unsigned int str_index = getStrIndexStart(index);
    String res;
    unsigned int n = str.length();
    while (str_index < n)
    {
      if (str[str_index] == escape)
      { // next is a literal
        ++str_index;
        res.concat(str[str_index]);
      }
      else if (str[str_index] == delim)
      { // this is in-between words
        return res;
      }
      else
      {
        res.concat(str[str_index]);
      }
      ++str_index;
    }

    return res; // last word won't have delimiter after it, so it will reach here
  }

public:
  StringVector() { size = 0; }
  StringVector(const StringVector &other) = default;
  StringVector &operator=(const StringVector &other)
  {
    if (this != &other)
    {
      str = other.str;
      size = other.size;
    }
    return *this;
  }
  ~StringVector() = default;
  unsigned int length() const { return size; }
  void insert(const String &s)
  {
    String s_escaped = escapeStr(s);
    if (size > 0)
    {
      str.concat(delim);
    }
    str.concat(s_escaped);
    ++size;
  }
  String operator[](unsigned int index) const
  {
    return this->getWord(index);
  }
  void replace(unsigned int index, const String &word)
  {
    if (index >= size)
    {
      return;
    }

    String word_escaped = escapeStr(word);
    unsigned int str_index_start = getStrIndexStart(index);
    unsigned int str_index_back = getStrIndexBack(index);

    // Serial.println("part1: "+str.substring(0,str_index_start));
    // Serial.println("old word: "+str.substring(str_index_start,str_index_back+1));
    // Serial.println("new word: "+word_escaped);
    // Serial.println("part2: "+str.substring(str_index_back+1));
    str = str.substring(0, str_index_start) + word_escaped + str.substring(str_index_back + 1);
  }
  void remove(unsigned int index)
  {
    if (index >= size)
    {
      return;
    }

    unsigned int str_index_start = getStrIndexStart(index);
    unsigned int str_index_back = getStrIndexBack(index);

    String part1;
    String the_delimiter;
    String part2 = str.substring(str_index_back + 1);
    if (str_index_start > 0)
    {
      part1 = str.substring(0, str_index_start - 1);
      the_delimiter = str.substring(str_index_start - 1, str_index_start);
    }
    else
    {
      part2 = part2.substring(1);
    }
    Serial.println("part1: " + part1);
    Serial.println("the delimiter: " + the_delimiter);
    Serial.println("old word: " + str.substring(str_index_start, str_index_back + 1));
    Serial.println("part2: " + str.substring(str_index_back + 1));
    str = part1 + part2;
    --size;
  }
  void insertBefore(unsigned int index, const String &word)
  {
    if (index >= size)
    {
      insert(word);
      return;
    }

    const unsigned int oldSize = size;
    insert(getWord(size - 1));
    for (unsigned int i = oldSize - 1; i > index; --i)
    {
      replace(i, getWord(i - 1));
    }
    replace(index, word);
  }
  void print() const
  {
    Serial.println("str: " + str);
    for (unsigned int i = 0; i < size; ++i)
    {
      Serial.println(String(i) + ": " + this->getWord(i));
    }
  }
};

#endif // STRING_VECTOR_H_
