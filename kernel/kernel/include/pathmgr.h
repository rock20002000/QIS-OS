/** \file mm.h
  *
  * Copyright 2022, QIS 0.11 Software Systems. All Rights Reserved.
  *
  * @author   	WangYang
  * @email   	yang.wang@kunyuos.com
  * @version	V1.0
  * @date    	2022-10-09
  * @brief   	Path manager define & interface
  *
  * Version|Auther|Date|Describe
  * ------|----|------|--------
  * V1.0|WangYang|2022-10-09|Create File
  */
#ifndef __PATHMGR_H__
#define __PATHMGR_H__
#include <types.h>
#include <platform/include/pathobj.h>

#define CONFIG_PATHMGR

// pathmgr.c
/**
    @brief Path manager initialize
    @return None
*/
extern void pathmgr_init(void);

/**
    @brief Register path-obj
    @param path path string
    @param p_obj path object pointer
    @param reg_obj true for register object to process
    @return error code, 'EOK' is successful
*/
extern error_t pathmgr_link(const char* path, path_object_t* p_obj, bool reg_obj);

/**
    @brief Unregister path
    @param path path string
    @return error code, 'EOK' is successful
*/
extern error_t pathmgr_unlink(const char* path);

/**
    @brief get path_object by path
    @param path path string
    @return path_object pointer
*/
extern path_object_t* pathmgr_object(const char* path);

/**
    @brief process get full path in kernel space from user path
    @param proc process struct pointer
    @param user_path path name string in user space
    @param len user path length
    @param absolute user path is a absolute path
    @param kernel_path path name string in kernel space, NULL means alloc
    @return error code, 'EOK' is successful
*/
char* path_from_user(PROCESS_t* proc,
    const void* user_path, size_t len, bool absolute, char* kernel_path);

/**
    @brief Open object with path
    @param path path string
    @return fd number, < 0 is error code
*/
extern int pathobj_open(const char* path);

/**
    @brief Close path object
    @param fd fd number
    @return error code, 'EOK' is successful
*/
extern error_t pathobj_close(int fd);

/**
    @brief Do ioctl of path object
    @param fd fd number
    @param param ioctl parameter pointer
    @param size ioctl parameter size
    @return error code, 'EOK' is successful
*/
extern error_t pathobj_ioctl(int fd, const void* param, size_t size);

#endif  // __PATHMGR_H__
