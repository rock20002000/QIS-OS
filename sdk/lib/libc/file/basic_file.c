/*
 * basic_file.c
 *
 *  Created by:
 *      Author: Test
 */
#include <types.h>
#include <libc/include/iomsg.h>
#include <libc/include/basic_file.h>
#include <platform/include/platform.h>

int open(const char *path, int oflag, ...) {
	//	va_list				ap;
	unsigned			fd = 0;

	path_object_t obj;
	char szTmp[1024] = {0};
	int iRet = PathmgrRequire(path, &obj);
	sprintf(szTmp, "Open Path %s: get pid = %d, chid = %x iRet:%d\n", path, obj.pid, obj.chid, iRet);
	printf_s(szTmp);
	if(iRet != EOK) return iRet;

	return ConnectAttach(0, obj.pid, obj.chid, 0, 0);

	//va_start(ap, oflag);
	//fd = _vopen(path, oflag, SH_DENYNO, ap);
	//va_end(ap);
	//return fd;
}

int name_open( const char * name, int flags ){
	return open(name, flags);
}


size_t read(int fd, void *buff, size_t nbytes) {
	io_read_t					msg;
	iov_t						iov_s;
	iov_t						iov_r;

	msg.type = _IO_READ;
	msg.msglen = sizeof(msg);
	msg.nbytes = nbytes;

	SETIOV(&iov_s, &(msg), sizeof(msg) );
	SETIOV(&iov_r, buff, nbytes);

	return MsgSendv(fd, &iov_s, 1, &iov_r, 1); //MsgSend(fd, &(msg.i), sizeof(msg.i), buff, nbytes);
}



size_t write(int fd, const void *buff, size_t nbytes) {
	io_write_t					msg;
	iov_t						iov[2];

	msg.type = _IO_WRITE;
	msg.msglen = sizeof(msg);
	msg.nbytes = nbytes;

	SETIOV(iov + 0, &msg, sizeof(msg) );
	SETIOV(iov + 1, buff, nbytes);
	return MsgSendv(fd, iov, 2, 0, 0);
}



#define _POSIX_DEVDIR_TO		0x80000000
#define _POSIX_DEVDIR_FROM		0x40000000
int devctl(int fd, int iDevCmd, void *data, size_t nbytes, int *info) {
	io_devctl_t			msg;
	io_devctl_reply_t	msgReply;
	iov_t				iov[4];
	int					status;


	// Stuff the message.
	msg.type = _IO_DEVCTL;
	msg.msglen = sizeof(msg);
	msg.nbytes = nbytes;
	msg.devCmd = iDevCmd;

	msgReply.type 	= _IO_DEVCTL_REPLY;
	msgReply.msglen	= sizeof(msgReply);
	msgReply.nbytes = 0;
	msgReply.return_value = 0;

	// Setup data to the device.
	SETIOV(iov + 0, &msg, sizeof(msg));
	SETIOV(iov + 1, data, (iDevCmd & _POSIX_DEVDIR_TO) ? nbytes : 0);

	// Setup data from the device.
	SETIOV(iov + 2, &msgReply, sizeof(msgReply));
	SETIOV(iov + 3, data, (iDevCmd & _POSIX_DEVDIR_FROM) ? nbytes : 0);

	if((status = MsgSendv(fd, iov + 0, GETIOVLEN(&iov[1]) ? 2 : 1, iov + 2, GETIOVLEN(&iov[3]) ? 2 : 1)) != EOK) {
		return status == -ENOSYS ? ENOTTY : -status;
	}

	if(info) {
		*info = msgReply.return_value;
	}

	return EOK;
}


int close(int fd) {
	io_close_t					msg;
	int							ret = 0;
	iov_t						iov_s;

	msg.type 	= _IO_CLOSE;
	msg.msglen 	= sizeof(msg);
	msg.nbytes 	= 0;
	SETIOV(&iov_s, &(msg), sizeof(msg) );

	if((ret = MsgSendv(fd, &iov_s, 1, 0, 0)) == EOK){
		if((ret = ConnectDetach(fd)) == EOK) {
			return EOK;	//ret;
		}else{
			return ret;
		}
	}else{
		return ERR_NOT_ACCEPTABLE;
	}
}

