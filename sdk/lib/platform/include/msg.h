/** \file msg.h
  *
  * Copyright 2022, KunyuOS Software Systems. All Rights Reserved.
  *
  * @author   	WangYang
  * @email   	yang.wang@kunyuos.com
  * @version	V1.0
  * @date    	2022-09-20
  * @brief   	Message define & interface
  *
  * Version|Auther|Date|Describe
  * ------|----|------|--------
  * V1.0|WangYang|2022-09-28|Create File
  */
#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <types.h>

 //	typedef void*(*start_rtn)(void*);
 typedef void(*start_rtn)(void);

#define MSG_READ    1
#define MSG_WRITE   2
#define MSG_FAILED  3
#define MSG_CANCEL	4
#define MSG_DONE    5

typedef struct _msg_data{
  int   iMsgType;
  int   iMsgLen;
  char  szMsg[1024];	//	char* 	pszMsg;
}MSG_DATA;

typedef struct _msg_context{
  int fdApplication;
}MSG_CONTEXT;


typedef struct _dev_func{
  int     (*devopen)	();//char *pathname, char *mode);
  size_t  (*devread)	();//int fd, void *buff, size_t nbytes);
  size_t  (*devwrite)	();//int fd, void *buff, size_t nbytes);
  int     (*devclose)	();//int fd);
}DEVICE_FUNCTIONS;

typedef struct iovec { \
 	void *		iov_base; \
 	uint32_t 	iov_len; \
} iov_t;

#ifndef SETIOV
 #define SETIOV(_iov, _addr, _len)  ((_iov)->iov_base = (void *)(_addr), (_iov)->iov_len = (_len))
#endif
#ifndef GETIOVBASE
 #define GETIOVBASE(_iov)           ((_iov)->iov_base)
#endif
#ifndef GETIOVLEN
 #define GETIOVLEN(_iov)            ((_iov)->iov_len)
#endif

#endif
