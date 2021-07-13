/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Copyright 2021, Wabi.
 */

/**
 * @file
 * KRAKEN Library.
 * Gadget Vault.
 */

#include <ctype.h>
#include <inttypes.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "KLI_api.h"

#include "KLI_string_utils.h" /* own include. */

/* ------------------------------------------------------------ CLASSIC C STRING UTILITIES ----- */

/**
 * Looks for a numeric suffix preceded by delim character on the end of
 * name, puts preceding part into *left and value of suffix into *nr.
 * Returns the length of *left.
 *
 * Foo.001 -> "Foo", 1
 * Returning the length of "Foo"
 *
 * @param left: Where to return copy of part preceding delim.
 * @param nr: Where to return value of numeric suffix.
 * @param name: String to split.
 * @param delim: Delimiter character.
 * @returns length: Of left.
 */
size_t KLI_split_name_num(char *left, int *nr, const char *name, const char delim)
{
  const size_t name_len = strlen(name);

  *nr = 0;
  memcpy(left, name, (name_len + 1) * sizeof(char));

  /* name doesn't end with a delimiter "foo." */
  if ((name_len > 1 && name[name_len - 1] == delim) == 0)
  {
    size_t a = name_len;
    while (a--)
    {
      if (name[a] == delim)
      {
        left[a] = '\0'; /* truncate left part here */
        *nr = atol(name + a + 1);
        /* casting down to an int, can overflow for large numbers */
        if (*nr < 0)
        {
          *nr = 0;
        }
        return a;
      }
      if (isdigit(name[a]) == 0)
      {
        /* non-numeric suffix - give up */
        break;
      }
    }
  }

  return name_len;
}

/**
 * Like strncpy but ensures dst is always '\0' terminated.
 *
 * @param dst: Destination for copy.
 * @param src: Source string to copy.
 * @param maxncpy: Maximum number of characters to copy. (generally the size of dst)
 * @returns dst
 */
char *KLI_strncpy(char *__restrict dst, const char *__restrict src, const size_t maxncpy)
{
  size_t srclen = KLI_strnlen(src, maxncpy - 1);
  KLI_assert(maxncpy != 0);

  memcpy(dst, src, srclen);
  dst[srclen] = '\0';
  return dst;
}

int KLI_strcasecmp(const char *s1, const char *s2)
{
  int i;
  char c1, c2;

  for (i = 0;; i++)
  {
    c1 = tolower(s1[i]);
    c2 = tolower(s2[i]);

    if (c1 < c2)
    {
      return -1;
    }
    if (c1 > c2)
    {
      return 1;
    }
    if (c1 == 0)
    {
      break;
    }
  }

  return 0;
}

int KLI_strncasecmp(const char *s1, const char *s2, size_t len)
{
  size_t i;
  char c1, c2;

  for (i = 0; i < len; i++)
  {
    c1 = tolower(s1[i]);
    c2 = tolower(s2[i]);

    if (c1 < c2)
    {
      return -1;
    }
    if (c1 > c2)
    {
      return 1;
    }
    if (c1 == 0)
    {
      break;
    }
  }

  return 0;
}

char *KLI_strcasestr(const char *s, const char *find)
{
  char c, sc;
  size_t len;

  if ((c = *find++) != 0)
  {
    c = tolower(c);
    len = strlen(find);
    do
    {
      do
      {
        if ((sc = *s++) == 0)
        {
          return NULL;
        }
        sc = tolower(sc);
      } while (sc != c);
    } while (KLI_strncasecmp(s, find, len) != 0);
    s--;
  }
  return ((char *)s);
}

/* array copied from glib's gutf8.c, */
/* Note: last two values (0xfe and 0xff) are forbidden in utf-8,
 * so they are considered 1 byte length too. */
static const size_t utf8_skip_data[256] = {
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  5,
  5,
  5,
  5,
  6,
  6,
  1,
  1,
};

#define KLI_STR_UTF8_CPY(dst, src, maxncpy) \
  { \
    size_t utf8_size; \
    while (*src != '\0' && (utf8_size = utf8_skip_data[*src]) < maxncpy) \
    { \
      maxncpy -= utf8_size; \
      switch (utf8_size) \
      { \
        case 6: \
          *dst++ = *src++; \
          ATTR_FALLTHROUGH; \
        case 5: \
          *dst++ = *src++; \
          ATTR_FALLTHROUGH; \
        case 4: \
          *dst++ = *src++; \
          ATTR_FALLTHROUGH; \
        case 3: \
          *dst++ = *src++; \
          ATTR_FALLTHROUGH; \
        case 2: \
          *dst++ = *src++; \
          ATTR_FALLTHROUGH; \
        case 1: \
          *dst++ = *src++; \
      } \
    } \
    *dst = '\0'; \
  } \
  (void)0

/**
 * Like strncpy but for utf8 and ensures dst is always '\0' terminated.
 *
 * @param dst: Destination for copy.
 * @param src: Source string to copy.
 * @param maxncpy: Maximum number of characters to copy. (generally the size of dst)
 * @returns num: bytes written.
 */
size_t KLI_strncpy_utf8_rlen(char *__restrict dst, const char *__restrict src, size_t maxncpy)
{
  char *r_dst = dst;

  KLI_assert(maxncpy != 0);

  /* note: currently we don't attempt to deal with invalid utf8 chars */
  KLI_STR_UTF8_CPY(dst, src, maxncpy);

  return (size_t)(dst - r_dst);
}

size_t KLI_strncpy_wchar_as_utf8(char *__restrict dst,
                                 const wchar_t *__restrict src,
                                 const size_t maxncpy)
{
  const size_t maxlen = maxncpy - 1;
  /* 6 is max utf8 length of an unicode char. */
  const int64_t maxlen_secured = (int64_t)maxlen - 6;
  size_t len = 0;

  KLI_assert(maxncpy != 0);

#ifdef DEBUG_STRSIZE
  memset(dst, 0xff, sizeof(*dst) * maxncpy);
#endif

  while (*src && len <= maxlen_secured) {
    len += KLI_str_utf8_from_unicode((uint)*src++, dst + len);
  }

  /* We have to be more careful for the last six bytes,
   * to avoid buffer overflow in case utf8-encoded char would be too long for our dst buffer. */
  while (*src) {
    char t[6];
    size_t l = KLI_str_utf8_from_unicode((uint)*src++, t);
    KLI_assert(l <= 6);
    if (len + l > maxlen) {
      break;
    }
    memcpy(dst + len, t, l);
    len += l;
  }

  dst[len] = '\0';

  return len;
}

size_t KLI_str_utf8_from_unicode(uint c, char *outbuf)
{
  /* If this gets modified, also update the copy in g_string_insert_unichar() */
  uint len = 0;
  uint first;
  uint i;

  if (c < 0x80) {
    first = 0;
    len = 1;
  }
  else if (c < 0x800) {
    first = 0xc0;
    len = 2;
  }
  else if (c < 0x10000) {
    first = 0xe0;
    len = 3;
  }
  else if (c < 0x200000) {
    first = 0xf0;
    len = 4;
  }
  else if (c < 0x4000000) {
    first = 0xf8;
    len = 5;
  }
  else {
    first = 0xfc;
    len = 6;
  }

  if (outbuf) {
    for (i = len - 1; i > 0; i--) {
      outbuf[i] = (c & 0x3f) | 0x80;
      c >>= 6;
    }
    outbuf[0] = c | first;
  }

  return len;
}

/**
 * Determine the length of a fixed length string.
 *
 * @param s: String to check.
 * @param maxlen: Maximum length to measure. (generally max - 1)
 * @returns len: The length of a string.
 */
size_t KLI_strnlen(const char *s, const size_t maxlen)
{
  size_t len;

  for (len = 0; len < maxlen; len++, s++)
  {
    if (!*s)
    {
      break;
    }
  }
  return len;
}

/**
 * Portable replacement for ``vsnprintf``.
 *
 * @param buffer: Pointer to a buffer where the resulting string is stored.
 * @param maxncpy: Maximum number of bytes to copy.
 * @param format: ``printf()`` style format string.
 * @param arg: A value identifying a variable arguments list.
 * @returns n: The number of bytes written.
 */
size_t KLI_vsnprintf(char *__restrict buffer, size_t maxncpy, const char *__restrict format, va_list arg)
{
  size_t n;

  KLI_assert(buffer != NULL);
  KLI_assert(maxncpy > 0);
  KLI_assert(format != NULL);

  n = (size_t)vsnprintf(buffer, maxncpy, format, arg);

  if (n != -1 && n < maxncpy)
  {
    buffer[n] = '\0';
  }
  else
  {
    buffer[maxncpy - 1] = '\0';
  }

  return n;
}

/**
 * A version of #KLI_vsnprintf that returns ``strlen(buffer)``
 *
 * @param buffer: Pointer to a buffer where the resulting string is stored.
 * @param maxncpy: Maximum number of bytes to copy.
 * @param format: ``printf()`` style format string.
 * @returns n: The number of bytes written.
 */
size_t KLI_vsnprintf_rlen(char *__restrict buffer,
                          size_t maxncpy,
                          const char *__restrict format,
                          va_list arg)
{
  size_t n;

  KLI_assert(buffer != NULL);
  KLI_assert(maxncpy > 0);
  KLI_assert(format != NULL);

  n = (size_t)vsnprintf(buffer, maxncpy, format, arg);

  if (n != -1 && n < maxncpy)
  {
    /* pass */
  }
  else
  {
    n = maxncpy - 1;
  }
  buffer[n] = '\0';

  return n;
}

/**
 * Portable Replacement for ``snprintf``.
 *
 * @param dst: Pointer to a buffer where the resulting string is stored.
 * @param maxncpy: Maximum number of bytes to copy.
 * @param format: ``printf()`` style format string.
 * @returns n: The number of bytes written.
 */
size_t KLI_snprintf(char *__restrict dst, size_t maxncpy, const char *__restrict format, ...)
{
  size_t n;
  va_list arg;

  va_start(arg, format);
  n = KLI_vsnprintf(dst, maxncpy, format, arg);
  va_end(arg);

  return n;
}

/**
 * A version of #KLI_snprintf that returns ``strlen(dst)``
 *
 * @param dst: Pointer to a buffer where the resulting string is stored.
 * @param maxncpy: Maximum number of bytes to copy.
 * @param format: ``printf()`` style format string.
 * @returns n: The number of bytes written.
 */
size_t KLI_snprintf_rlen(char *__restrict dst, size_t maxncpy, const char *__restrict format, ...)
{
  size_t n;
  va_list arg;

  va_start(arg, format);
  n = KLI_vsnprintf_rlen(dst, maxncpy, format, arg);
  va_end(arg);

  return n;
}

/* Unique name utils. */

static bool uniquename_find_dupe(std::vector<void *> list, void *vlink, const char *name, int name_offset)
{
  for (auto link : list)
  {
    if (link != vlink)
    {
      if (STREQ((const char *)POINTER_OFFSET((const char *)link, name_offset), name))
      {
        return true;
      }
    }
  }

  return false;
}

static bool uniquename_unique_check(void *arg, const char *name)
{
  struct Data
  {
    std::vector<void *> lb;
    void *vlink;
    int name_offset;

    Data()
      : lb(EMPTY),
        vlink(POINTER_ZERO),
        name_offset(VALUE_ZERO)
    {}
  };

  Data *data = (Data *)arg;

  return uniquename_find_dupe(data->lb, data->vlink, name, data->name_offset);
}

bool KLI_uniquename(std::vector<void *> list, void *vlink, const char *defname, char delim, int name_offset, size_t name_len)
{
  struct Data
  {
    std::vector<void *> lb;
    void *vlink;
    int name_offset;

    Data()
      : lb(EMPTY),
        vlink(POINTER_ZERO),
        name_offset(VALUE_ZERO)
    {}
  };

  Data data;

  data.lb = list;
  data.vlink = vlink;
  data.name_offset = name_offset;

  KLI_assert(name_len > 1);

  /* See if we are given an empty string */
  if (vlink == POINTER_ZERO || defname == POINTER_ZERO)
  {
    return false;
  }

  return KLI_uniquename_cb(uniquename_unique_check,
                           &data,
                           defname,
                           delim,
                           (char *)POINTER_OFFSET(vlink, name_offset),
                           name_len);
}

/**
 * Ensures name is unique (according to criteria specified by caller in unique_check callback),
 * incrementing its numeric suffix as necessary. Returns true if name had to be adjusted.
 *
 * @param unique_check: Return true if name is not unique.
 * @param arg: Additional arg to unique_check--meaning is up to caller.
 * @param defname: To initialize name if latter is empty.
 * @param delim: Delimits numeric suffix in name.
 * @param name: Name to be ensured unique.
 * @param name_len: Maximum length of name area.
 * @returns true: If name was changed.
 */
bool KLI_uniquename_cb(UniquenameCheckCallback unique_check,
                       void *arg,
                       const char *defname,
                       char delim,
                       char *name,
                       size_t name_len)
{
  if (name[0] == '\0')
  {
    KLI_strncpy(name, defname, name_len);
  }

  if (unique_check(arg, name))
  {
    char numstr[16];
    char *tempname = (char *)alloca(name_len);
    char *left = (char *)alloca(name_len);
    int number;
    size_t len = KLI_split_name_num(left, &number, name, delim);
    do
    {
      /* add 1 to account for \0 */
      const size_t numlen = KLI_snprintf(numstr, sizeof(numstr), "%c%03d", delim, ++number) + 1;

      /* highly unlikely the string only has enough room for the number
       * but support anyway */
      if ((len == 0) || (numlen >= name_len))
      {
        /* number is know not to be utf-8 */
        KLI_strncpy(tempname, numstr, name_len);
      }
      else
      {
        char *tempname_buf;
        tempname_buf = tempname + KLI_strncpy_utf8_rlen(tempname, left, name_len - numlen);
        memcpy(tempname_buf, numstr, numlen);
      }
    } while (unique_check(arg, tempname));

    KLI_strncpy(name, tempname, name_len);

    return true;
  }

  return false;
}

/* ------------------------------------------------------ MODERN CXX STD::STRING UTILITIES ----- */

std::string KLI_str_CapSpaceAmmender(std::string str)
{
  std::string ret;

  for (int i = 0; i < str.length(); i++)
  {
    if (str[i] >= 'A' && str[i] <= 'Z')
    {
      str[i] = str[i] + 32;
      if (i != 0)
        ret += " ";

      ret += toupper(str[i]);
    }

    else
    {
      if (i == 0)
      {
        ret += toupper(str[i]);
      }
      else
      {
        ret += str[i];
      }
    }
  }

  return ret;
}

std::string KLI_str_UpperCamel(std::string str)
{
  std::string ret;

  bool active = true;

  for (int i = 0; i < str.length(); i++)
  {

    if (std::isalpha(str[i]))
    {
      if (active)
      {
        ret += std::toupper(str[i]);
        active = false;
      }
      else
      {
        ret += std::tolower(str[i]);
      }
    }
    else if (str[i] == ' ')
    {
      active = true;
    }
  }

  return ret;
}
