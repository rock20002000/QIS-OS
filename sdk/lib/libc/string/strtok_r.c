#include <types.h>
#include <libc/include/string.h>
#include <utils/include/print_macros.h>

#if 0 // test NG
/* Find the first occurrence of C in S or the final NUL byte.  */
char* strchrnul(const char *s, int c_in)
{
  const unsigned char *char_ptr;
  const unsigned long int *longword_ptr;
  unsigned long int longword, magic_bits, charmask;
  unsigned char c;

  c = (unsigned char) c_in;

  /* Handle the first few characters by reading one character at a time.
     Do this until CHAR_PTR is aligned on a longword boundary.  */
  for (char_ptr = (const unsigned char *) s;
       ((unsigned long int) char_ptr & (sizeof (longword) - 1)) != 0;
       ++char_ptr)
    if (*char_ptr == c || *char_ptr == '\0')
      return (void *) char_ptr;

  /* All these elucidatory comments refer to 4-byte longwords,
     but the theory applies equally well to 8-byte longwords.  */

  longword_ptr = (unsigned long int *) char_ptr;

  /* Bits 31, 24, 16, and 8 of this number are zero.  Call these bits
     the "holes."  Note that there is a hole just to the left of
     each byte, with an extra at the end:

     bits:  01111110 11111110 11111110 11111111
     bytes: AAAAAAAA BBBBBBBB CCCCCCCC DDDDDDDD

     The 1-bits make sure that carries propagate to the next 0-bit.
     The 0-bits provide holes for carries to fall into.  */
  magic_bits = -1;
  magic_bits = magic_bits / 0xff * 0xfe << 1 >> 1 | 1;

  /* Set up a longword, each of whose bytes is C.  */
  charmask = c | (c << 8);
  charmask |= charmask << 16;
  if (sizeof (longword) > 4)
    /* Do the shift in two steps to avoid a warning if long has 32 bits.  */
    charmask |= (charmask << 16) << 16;
#if 0
  if (sizeof (longword) > 8)
    abort ();
#else
  ASSERT(sizeof(longword) <= 8);
#endif
  /* Instead of the traditional loop which tests each character,
     we will test a longword at a time.  The tricky part is testing
     if *any of the four* bytes in the longword in question are zero.  */
  for (;;)
    {
      /* We tentatively exit the loop if adding MAGIC_BITS to
	 LONGWORD fails to change any of the hole bits of LONGWORD.

	 1) Is this safe?  Will it catch all the zero bytes?
	 Suppose there is a byte with all zeros.  Any carry bits
	 propagating from its left will fall into the hole at its
	 least significant bit and stop.  Since there will be no
	 carry from its most significant bit, the LSB of the
	 byte to the left will be unchanged, and the zero will be
	 detected.

	 2) Is this worthwhile?  Will it ignore everything except
	 zero bytes?  Suppose every byte of LONGWORD has a bit set
	 somewhere.  There will be a carry into bit 8.  If bit 8
	 is set, this will carry into bit 16.  If bit 8 is clear,
	 one of bits 9-15 must be set, so there will be a carry
	 into bit 16.  Similarly, there will be a carry into bit
	 24.  If one of bits 24-30 is set, there will be a carry
	 into bit 31, so all of the hole bits will be changed.

	 The one misfire occurs when bits 24-30 are clear and bit
	 31 is set; in this case, the hole at bit 31 is not
	 changed.  If we had access to the processor carry flag,
	 we could close this loophole by putting the fourth hole
	 at bit 32!

	 So it ignores everything except 128's, when they're aligned
	 properly.

	 3) But wait!  Aren't we looking for C as well as zero?
	 Good point.  So what we do is XOR LONGWORD with a longword,
	 each of whose bytes is C.  This turns each byte that is C
	 into a zero.  */

      longword = *longword_ptr++;

      /* Add MAGIC_BITS to LONGWORD.  */
      if ((((longword + magic_bits)

	    /* Set those bits that were unchanged by the addition.  */
	    ^ ~longword)

	   /* Look at only the hole bits.  If any of the hole bits
	      are unchanged, most likely one of the bytes was a
	      zero.  */
	   & ~magic_bits) != 0

	  /* That caught zeroes.  Now test for C.  */
	  || ((((longword ^ charmask) + magic_bits) ^ ~(longword ^ charmask))
	      & ~magic_bits) != 0)
	{
	  /* Which of the bytes was C or zero?
	     If none of them were, it was a misfire; continue the search.  */

	  const unsigned char *cp = (const unsigned char *) (longword_ptr - 1);

	  if (*cp == c || *cp == '\0')
	    return (char *) cp;
	  if (*++cp == c || *cp == '\0')
	    return (char *) cp;
	  if (*++cp == c || *cp == '\0')
	    return (char *) cp;
	  if (*++cp == c || *cp == '\0')
	    return (char *) cp;
	  if (sizeof (longword) > 4)
	    {
	      if (*++cp == c || *cp == '\0')
		return (char *) cp;
	      if (*++cp == c || *cp == '\0')
		return (char *) cp;
	      if (*++cp == c || *cp == '\0')
		return (char *) cp;
	      if (*++cp == c || *cp == '\0')
		return (char *) cp;
	    }
	}
    }

  /* This should never happen.  */
  return NULL;
}

/* Return the length of the maximum initial segment
   of S which contains only characters in ACCEPT.  */
size_t strspn(const char *str, const char *accept)
{
  if (accept[0] == '\0')
    return 0;
  if (accept[1] == '\0')
    {
      const char *a = str;
      for (; *str == *accept; str++);
      return str - a;
    }

  /* Use multiple small memsets to enable inlining on most targets.  */
  unsigned char table[256];
  unsigned char *p = memset (table, 0, 64);
  memset (p + 64, 0, 64);
  memset (p + 128, 0, 64);
  memset (p + 192, 0, 64);

  unsigned char *s = (unsigned char*) accept;
  /* Different from strcspn it does not add the NULL on the table
     so can avoid check if str[i] is NULL, since table['\0'] will
     be 0 and thus stopping the loop check.  */
  do
    p[*s++] = 1;
  while (*s);

  s = (unsigned char*) str;
  if (!p[s[0]]) return 0;
  if (!p[s[1]]) return 1;
  if (!p[s[2]]) return 2;
  if (!p[s[3]]) return 3;

  s = (unsigned char *)ALIGN_DOWN((uintptr_t)s, 4);

  unsigned int c0, c1, c2, c3;
  do {
      s += 4;
      c0 = p[s[0]];
      c1 = p[s[1]];
      c2 = p[s[2]];
      c3 = p[s[3]];
  } while ((c0 & c1 & c2 & c3) != 0);

  size_t count = s - (unsigned char *) str;
  return (c0 & c1) == 0 ? count + c0 : count + c2 + 2;
}

/* Return the length of the maximum initial segment of S
   which contains no characters from REJECT.  */
size_t strcspn(const char *str, const char *reject)
{
  if ((reject[0] == '\0') || (reject[1] == '\0'))
    return strchrnul(str, reject [0]) - str;

  /* Use multiple small memsets to enable inlining on most targets.  */
  unsigned char table[256];
  unsigned char *p = memset (table, 0, 64);
  memset (p + 64, 0, 64);
  memset (p + 128, 0, 64);
  memset (p + 192, 0, 64);

  unsigned char *s = (unsigned char*) reject;
  unsigned char tmp;
  do
    p[tmp = *s++] = 1;
  while (tmp);

  s = (unsigned char*) str;
  if (p[s[0]]) return 0;
  if (p[s[1]]) return 1;
  if (p[s[2]]) return 2;
  if (p[s[3]]) return 3;

  s = (unsigned char *)ALIGN_DOWN((uintptr_t)s, 4);

  unsigned int c0, c1, c2, c3;
  do
    {
      s += 4;
      c0 = p[s[0]];
      c1 = p[s[1]];
      c2 = p[s[2]];
      c3 = p[s[3]];
    }
  while ((c0 | c1 | c2 | c3) == 0);

  size_t count = s - (unsigned char *) str;
  return (c0 | c1) != 0 ? count - c0 + 1 : count - c2 + 3;
}


/* Parse S into tokens separated by characters in DELIM.
   If S is NULL, the saved pointer in SAVE_PTR is used as
   the next starting point.  For example:
	char s[] = "-abc-=-def";
	char *sp;
	x = strtok_r(s, "-", &sp);	// x = "abc", sp = "=-def"
	x = strtok_r(NULL, "-=", &sp);	// x = "def", sp = NULL
	x = strtok_r(NULL, "=", &sp);	// x = NULL
		// s = "abc\0-def\0"
*/
char* strtok_r(char *s, const char *delim, char **save_ptr)
{
  char *end;

  if (s == NULL)
    s = *save_ptr;

  if (*s == '\0')
  {
      *save_ptr = s;
      return NULL;
  }

  /* Scan leading delimiters.  */
  s += strspn (s, delim);
  if (*s == '\0')
  {
      *save_ptr = s;
      return NULL;
  }

  /* Find the end of the token.  */
  end = s + strcspn (s, delim);
  if (*end == '\0')
  {
      *save_ptr = end;
      return s;
  }

  /* Terminate the token and make *SAVE_PTR point past it.  */
  *end = '\0';
  *save_ptr = end + 1;
  return s;
}

char* strtok(char *s, const char *delim)
{
    static char *olds;

    return strtok_r (s, delim, &olds);
}
#endif
