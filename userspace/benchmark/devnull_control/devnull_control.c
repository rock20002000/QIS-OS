/*
 * devnull_control.c
 *
 *  Created on: 2022-9-21
 *      Author: Test
 */

#include <platform/include/platform.h>
#include <libc/include/basic_file.h>
#include <platform/include/resmgr.h>


//dispatch_t   *dpp;
//
//static dev_t null_devno;

static int null_read(int iRcvID, io_read_t *msg){	//	resmgr_context_t *ctp, io_read_t *msg, void *ocb) {
	//unsigned		ioflag = (uintptr_t)ocb;
	//printf_s("null_read\n");
	//if(!(ioflag & _IO_FLAG_RD)) {
	//	return EBADF;
	//}
//	switch(msg->i.xtype & _IO_XTYPE_MASK) {	//	switch(msg->i.xtype & _IO_XTYPE_MASK) {
//	case _IO_XTYPE_NONE:
//	case _IO_XTYPE_OFFSET:
//		break;
//	default:
//		// We don't handle this xtype, return error
//		break;	//return ENOSYS;
//	}
	//_IO_SET_READ_NBYTES(ctp, 0);
	MsgReply(iRcvID, 0, NULL, 0);
	return EOK;
}

static int null_write(int iRcvID, io_write_t *msg){	//	resmgr_context_t *ctp, io_write_t *msg, void *ocb) {
	//unsigned		ioflag = (uintptr_t)ocb;
	//printf_s("null_write\n");
	//if(!(ioflag & _IO_FLAG_WR)) {
	//	return EBADF;
	//}
//	switch(msg->i.xtype & _IO_XTYPE_MASK) {
//	case _IO_XTYPE_NONE:
//	case _IO_XTYPE_OFFSET:
//		break;
//	default:
//		// We don't handle this xtype, return error
//		break;	//return ENOSYS;
//	}
//	_IO_SET_WRITE_NBYTES(ctp, msg->i.nbytes);
	MsgReply(iRcvID, 0, NULL, 0);
	return EOK;
}


int message_callback( int rcvid, void* msgRcv)
{
    char msg_reply[255];
    snprintf( msg_reply, 254, "Server Got Message Code");	//	"Server Got Message Code: _IO_MAX + %d", num );
    // Send a reply to the waiting (blocked) client
    MsgReply( rcvid, EOK, msg_reply, strlen( msg_reply ) + 1 );

    return 0;
}

//static int null_stat(resmgr_context_t *ctp, io_stat_t *msg, void *ocb) {
//	return 1;
//
//}


//static int null_devctl(resmgr_context_t *ctp, io_devctl_t *msg, void *ocb) {
//	union {
//		int					oflag;
//		int					mountflag;
//	}					*data = (void *)(msg + 1);
//	unsigned			ioflag = (uintptr_t)ocb;
//	int					nbytes = 0;
//
//	switch((unsigned)msg->i.dcmd) {
//	case DCMD_ALL_GETFLAGS:
//		data->oflag = (ioflag & ~O_ACCMODE) | ((ioflag - 1) & O_ACCMODE);
//		nbytes = sizeof data->oflag;
//		break;
//
//	case DCMD_ALL_SETFLAGS:
//		break;
//
//	default:
//		return ENOSYS;
//	}
//
//	if(nbytes) {
//		msg->o.ret_val = 0;
//		return _RESMGR_PTR(ctp, &msg->o, sizeof msg->o + nbytes);
//	}
//	return EOK;
//}

//static const resmgr_io_funcs_t null_io_funcs = {
//	_RESMGR_IO_NFUNCS,
//	null_read,
//	null_write,
//	0,
//	null_stat,
//	0,
//	null_devctl
//};

//
//static const resmgr_connect_funcs_t null_connect_funcs = {
//	_RESMGR_CONNECT_NFUNCS,
//	//	null_open
//};

//void devnull_init(void) {
//	resmgr_attr_t	rattr;
//	// allocate null directory entry
//	memset(&rattr, 0x00, sizeof(rattr));
//	rattr.flags = RESMGR_FLAG_CROSS_ENDIAN;
	//resmgr_attach(dpp, &rattr, "/dev/null", 0, 0, &null_connect_funcs, &null_io_funcs, 0);
	//	rsrcdbmgr_proc_devno("dev", &null_devno, -1, 0);
//}

int main(int argc, char **argv) {
	int 	iRParts		= MAX_MSG_SRPARTS;
	iov_t	iov[MAX_MSG_SRPARTS];

    for(int i=0; i<MAX_MSG_SRPARTS; i++){
    	iov[i].iov_base = malloc(1024);
    	iov[i].iov_len  = 1024;
    	memset(iov[i].iov_base, 0, 1024);
    }
	//resmgr_context_t	*ctp;	//dispatch_context_t   *ctp, *ctp_ret;
    //resmgr_attr_t        resmgr_attr;
    //message_attr_t       message_attr;
    //int                  resmgr_id, message_id;


	int iChid = ChannelCreateWithPath(0, "/dev/null");
	if(iChid<0){
		xprintf("Create Channel failed.\n");
		return -1;
	}

    screen_printf("devnull_control testing: resmgr_context_alloc \n");
//    if((dpp = dispatch_create()) == NULL) {
//    	printf_s("Unable to allocate dispatch handle.\n");
//        //	fprintf(stderr, "%s: Unable to allocate dispatch handle.\n", argv[0]);
//        return -1;	//EXIT_FAILURE;
//    }
//	devnull_init();
//
//    ctp = resmgr_context_alloc(dpp);
//    if (ctp == NULL)
//    {
//    	screen_printf("devnull_control: Unable to allocate resmgr context\n");//	: %s.", strerror(errno));
//    	return -1;
//    }

    while (1) {
        for(int i=0; i<MAX_MSG_SRPARTS; i++){
        	iov[i].iov_len  = 1024;
        	memset(iov[i].iov_base, 0, 1024);
        }
    	int iRcvid =  MsgReceivev( iChid, &iov, iRParts, NULL );	//iRcvid = MsgReceive(iChid, szBuf, sizeof(szBuf), NULL);
    	if (iRcvid < 0){
    		xprintf("Receive failed.\n");
    		return -1;
        }
    	//xprintf("Server receive msg.\n");
    	msg_header_t msgHeader;
    	memcpy(&msgHeader, iov[0].iov_base, sizeof(msg_header_t));
    	switch(msgHeader.type){
    	case _IO_READ:
    		null_read(iRcvid, (io_read_t *)iov[0].iov_base);	//, &iov);
    		break;
    	case _IO_WRITE:
    		null_write(iRcvid, (io_write_t *)iov[0].iov_base);	//,  &iov);
    		break;
    	default:
    		message_callback(iRcvid, (void*)iov[0].iov_base);

    	}
    	//msg_handler(iRcvid, szBuf);	//	dispatch_handler(ctp);		//
    }

//
//    while (1)
//    {
//    	//screen_printf("DevNow testing: resmgr_block \n");
//		ctp = resmgr_block(ctp);
//
//
//		if (ctp->rcvid != -1)
//		{
//			//screen_printf("devnull_control testing: resmgr_handler \n");
//
//			//char szTmp[1024] = {0};
//			//sprintf(szTmp, "devnull_control testing: resmgr_handler msg type:%d rcvid:0x%x\n", ctp->msg->type, ctp->rcvid);
//			//screen_printf(szTmp);
//
//			resmgr_handler(ctp);
//		}
//		//screen_printf("DevNow testing: resmgr_handler \n");
//    }


	return 0;
}


/*
int main(int argc, char **argv) {
	return 0;
}
*/
