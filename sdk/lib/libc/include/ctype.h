/** \file ctype.h
  *
  * Copyright 2022, KunyuOS Software Systems. All Rights Reserved.
  *
  * @author     WangYang
  * @email      yang.wang@kunyuos.com
  * @version    V1.0
  * @date       2022-08-31
  * @brief      Character handling define & interface
  *
  * Version|Auther|Date|Describe
  * ------|----|------|--------
  * V1.0|WangYang|2022-09-26|Create File
  */
#ifndef __CTYPE_H__
#define __CTYPE_H__

/* ctype code bits */
#define _XB     0x400 /* extra blank */
#define _XA     0x200 /* extra alphabetic */
#define _XS     0x100 /* extra space */
#define _BB     0x80 /* BEL, BS, etc. */
#define _CN     0x40 /* CR, FF, HT, NL, VT */
#define _DI     0x20 /* '0'-'9' */
#define _LO     0x10 /* 'a'-'z' */
#define _PU     0x08 /* punctuation */
#define _SP     0x04 /* space */
#define _UP     0x02 /* 'A'-'Z' */
#define _XD     0x01 /* '0'-'9', 'A'-'F', 'a'-'f' */

const unsigned short* CtypeTable(void);

static inline short GetCharType(int ch)
{
    return CtypeTable()[ch & 0xff];
}

static inline int isalnum(int ch)
{
    return (GetCharType(ch) & (_DI|_LO|_UP|_XA));
}

static inline int isalpha(int ch)
{
    return (GetCharType(ch) & (_LO|_UP|_XA));
}

static inline int iscntrl(int ch)
{
    return (GetCharType(ch) & _BB);
}

static inline int isdigit(int ch)
{
    return (GetCharType(ch) & _DI);
}

static inline int isgraph(int ch)
{
    return (GetCharType(ch) & (_DI|_LO|_PU|_UP|_XA));
}

static inline int islower(int ch)
{
    return (GetCharType(ch) & _LO);
}

static inline int isprint(int ch)
{
    return (GetCharType(ch) & (_DI|_LO|_PU|_SP|_UP|_XA));
}

static inline int ispunct(int ch)
{
    return (GetCharType(ch) & _PU);
}

static inline int _Isspace(int ch)
{
    return (GetCharType(ch) & (_CN|_SP|_XS));
}

static inline int isspace(int ch)
{
    return (_Isspace(ch));
}

static inline int isupper(int ch)
{
    return (GetCharType(ch) & _UP);
}

static inline int isxdigit(int ch)
{
    return (GetCharType(ch) & _XD);
}

static inline int isblank(int ch)
{
    return (GetCharType(ch) & (_SP|_XB));
}

#endif
