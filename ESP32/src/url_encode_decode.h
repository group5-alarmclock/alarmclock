#ifndef URL_ENCODE_DECODE_H_
#define URL_ENCODE_DECODE_H_

#include <Arduino.h>

// FIXME "free wifi" -> "free+wifi"

unsigned char hex2int(char c)
{
  if (c >= '0' && c <= '9')
  {
    return (unsigned char)c - '0';
  }
  if (c >= 'a' && c <= 'f')
  {
    return (unsigned char)c - 'a' + 10;
  }
  if (c >= 'A' && c <= 'F')
  {
    return (unsigned char)c - 'A' + 10;
  }
  return 0;
}

String urldecode(String str)
{
  String encodedString = "";
  char c;
  char code0;
  char code1;
  for (int i = 0; i < str.length(); i++)
  {
    c = str.charAt(i);
    if (c == '%')
    {
      i++;
      code0 = str.charAt(i);
      i++;
      code1 = str.charAt(i);
      c = (hex2int(code0) << 4) | hex2int(code1);
      encodedString += c;
    }
    else if (c == '+')
    {
      encodedString += ' ';
    }
    else
    {
      encodedString += c;
    }
  }

  return encodedString;
}

String urlencode(String str)
{
  String encodedString = "";
  char c;
  char code0;
  char code1;
  char code2;
  for (int i = 0; i < str.length(); i++)
  {
    c = str.charAt(i);
    if (c == ' ')
    {
      encodedString += '+';
    }
    else if (isalnum(c))
    {
      encodedString += c;
    }
    else
    {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9)
      {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9)
      {
        code0 = c - 10 + 'A';
      }
      code2 = '\0';
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
      // encodedString+=code2;
    }
    yield();
  }
  return encodedString;
}

#endif // URL_ENCODE_DECODE_H_
