/*
 * iomsg.h
 *
 *  Created on: 2022Äê12ÔÂ21ÈÕ
 *      Author: Test
 */

#ifndef SDK_LIB_LIBC_INCLUDE_IOMSG_H_
#define SDK_LIB_LIBC_INCLUDE_IOMSG_H_

#define _MSG_MAX 256

#define MAX_MSG_SRPARTS	7

enum _file_operation_types {
	_IO_READ,
	_IO_WRITE,
	_IO_DEVCTL,
	_IO_DEVCTL_REPLY,
	_IO_CLOSE
};

typedef struct _tag_msgHeader{
	uint32_t type;
}msg_header_t;

typedef struct _tag_ioRead{
	uint32_t type;
	uint32_t msglen;
	uint32_t nbytes;
} io_read_t;

typedef struct _tag_ioWrite{
	uint32_t type;
	uint32_t msglen;
	uint32_t nbytes;
} io_write_t;


typedef struct _tag_ioDevCtrl{
	uint32_t type;
	uint32_t msglen;
	uint32_t nbytes;
	uint32_t devCmd;
} io_devctl_t;


typedef struct _tag_ioDevCtrlReply{
	uint32_t type;
	uint32_t msglen;
	uint32_t nbytes;
	uint32_t return_value;
} io_devctl_reply_t;

typedef struct _tag_ioClose{
	uint32_t type;
	uint32_t msglen;
	uint32_t nbytes;
} io_close_t;


#endif /* SDK_LIB_LIBC_INCLUDE_IOMSG_H_ */
