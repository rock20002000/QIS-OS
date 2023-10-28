

/** \file malloc.h
  *
  * Copyright 2022, KunyuOS Software Systems. All Rights Reserved.
  *
  * @author   	WangYang
  * @email   	yang.wang@kunyuos.com
  * @version	V1.0
  * @date    	2022-09-20
  * @brief   	Malloc define & interface
  *
  * Version|Auther|Date|Describe
  * ------|----|------|--------
  * V1.0|WangYang|2022-09-20|Create File
  */
#ifndef _KLIBC_MALLOC_H_
#define _KLIBC_MALLOC_H_

#include <stdint.h>

/**
    @brief Get heap start address
    @return heap start address
    @note for debug
*/
void* heap_base(void);

/**
    @brief Get heap size
    @return heap size
    @note for debug
*/
size_t heap_size(void);

/**
    @brief Malloc memory in heap
    @param size malloc bytes
    @return Pointer of malloc memory
*/
void* malloc(size_t size);

/**
    @brief alloc memory in heap
    @param size alloc bytes
    @return Pointer of alloc memory
*/
void *alloc(size_t size);

/**
    @brief realloc memory in heap
    @param size realloc bytes
    @return Pointer of realloc memory
*/
void *realloc(void *ptr, size_t size);

/**
    @brief Free malloced memory by a pointer
    @param ptr pointer in heap memory
    @return None
*/
void free(void* ptr);

#endif

