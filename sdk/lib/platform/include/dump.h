#ifndef __DUMP_H__
#define __DUMP_H__

#include <libc/include/stdio.h>

typedef volatile uint8_t        RW8_t;
typedef volatile const uint8_t  RO8_t;
typedef volatile uint32_t       RW32_t;
typedef volatile const uint32_t RO32_t;

/**
    @brief Read IO byte
    @param addr read address
    @return byte
*/
static inline uint8_t read8(uintptr_t addr)
{
    return *(RO8_t*)addr;
}

/**
    @brief Write IO byte
    @param addr write address
    @param val byte
    @return None
*/
static inline void write8(uintptr_t addr, uint32_t val)
{
    *(RW8_t*)addr = val;
}

/**
    @brief Read IO word
    @param addr read address
    @return word
*/
static inline uint32_t read32(uintptr_t addr)
{
    return *(RO32_t*)addr;
}

/**
    @brief Write IO word
    @param addr write address
    @param val word
    @return None
*/
static inline void write32(uintptr_t addr, uint32_t val)
{
    *(RW32_t*)addr = val;
}

/**
    @brief Dump memory in byte
    @param title print head
    @param data dump memory address
    @param bytes dump bytes
    @param vaddr show address
    @return None
*/
static inline void dump_bytes(const char* title, uintptr_t data, size_t bytes, vaddr_t vaddr)
{
  size_t i;
  RO8_t* p = (RO8_t*)data;

  if (title)
    printf("%s:\n", title);
  if (NULL_VADDR == vaddr)
    vaddr = data;
  for (i = 0; i < bytes; i++)
  {
    if (!(i&15))
      printf("%p:", vaddr);
    printf(" %02x", *p++);

    if (15 == (i&15))
      printf("\n");
    vaddr += sizeof(*p);
  }
  if (15 != (i&15))
    printf("\n");
}

/**
    @brief Dump memory in word
    @param title print head
    @param data dump memory address
    @param words dump words
    @param vaddr show address
    @return None
*/
static inline void dump_words(const char* title, uintptr_t data, size_t words, vaddr_t vaddr)
{
  size_t i;
  RO32_t* p = (RO32_t*)data;

  if (title)
    printf("%s:\n", title);
  if (NULL_VADDR == vaddr)
    vaddr = data;
  for (i = 0; i < words; i++)
  {
    if (!(i&3))
      printf("%p:", p);
    printf(" %08x", *p++);

    if (3 == (i&3))
      printf("\n");
    vaddr += sizeof(*p);
  }
  if (3 != (i&3))
    printf("\n");
}

#endif
