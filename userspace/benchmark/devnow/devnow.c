/*
 * devnow.c
 *
 *  Created on: 2022-9-21
 *      Author: Test
 */

#include <platform/include/platform.h>
#include <platform/include/time.h>
#include <libc/include/basic_file.h>

#define _S_IFMT     0xF000              //  Type of file
#define _S_IFIFO    0x1000              //  FIFO
#define _S_IFCHR    0x2000              //  Character special
#define _S_IFDIR    0x4000              //  Directory
#define _S_IFNAM    0x5000              //  Special named file
#define _S_IFBLK    0x6000              //  Block special
#define _S_IFREG    0x8000              //  Regular
#define _S_IFLNK    0xA000              //  Symbolic link
#define _S_IFSOCK   0xC000              //  Socket


static int now_read(int iRcvID, io_read_t *msg)	//	resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb)
{
	char nowstr[128];
	time_t t;
	int n;

	struct tm * tmTmp = localtime();
	sprintf(nowstr, "%d-%d-%d %d:%d:%d", tmTmp->tm_year, tmTmp->tm_mon, tmTmp->tm_mday, tmTmp->tm_hour, tmTmp->tm_min, tmTmp->tm_sec);
	n = strlen(nowstr); //	1;

	MsgReply(iRcvID, n, nowstr, n);	//	MsgReply(ctp->rcvid, n, nowstr, n);

	return 1;	//	_RESMGR_NOREPLY;
}

int message_callback( int rcvid, void* msgRcv)
{
    char msg_reply[255];

    snprintf( msg_reply, 254, "Server Got Message Code" );	//	"Server Got Message Code: _IO_MAX + %d", num );

    // Send a reply to the waiting (blocked) client
    MsgReply( rcvid, EOK, msg_reply, strlen( msg_reply ) + 1 );

    return 0;
}


void main(void) {

    int   	iChid = 0 ;
    int 	iRcvid = 0;

    iov_t	iov[MAX_MSG_SRPARTS];
    int 	iRParts		= MAX_MSG_SRPARTS;

    for(int i=0; i<MAX_MSG_SRPARTS; i++){
    	iov[i].iov_base = malloc(1024);
    	iov[i].iov_len  = 1024;
    	memset(iov[i].iov_base, 0, 1024);
    }

    screen_printf("DevNow testing...\n");

    iChid = ChannelCreateWithPath(0, "/dev/now");
     if(iChid<0){
     	xprintf("Create Channel failed.\n");
     	return ;
     }


     // start the resource manager message loop
     while (1) {
         for(int i=0; i<MAX_MSG_SRPARTS; i++){
         	iov[i].iov_len  = 1024;
         	memset(iov[i].iov_base, 0, 1024);
         }
     	iRcvid =  MsgReceivev( iChid, &iov, iRParts, NULL );	//iRcvid = MsgReceive(iChid, szBuf, sizeof(szBuf), NULL);
     	if (iRcvid < 0){
     		xprintf("Receive failed.\n");
     		return ;
         }
     	xprintf("Server receive msg.\n");
     	msg_header_t msgHeader;
     	memcpy(&msgHeader, iov[0].iov_base, sizeof(msg_header_t));
     	switch(msgHeader.type){
     	case _IO_READ:
     		now_read(iRcvid, (io_read_t *)iov[0].iov_base);	//, &iov);
     		break;
//     	case _IO_WRITE:
//     		io_write(iRcvid);	//,  &iov);
//     		break;
     	default:
     		message_callback(iRcvid, (void*)iov[0].iov_base);

     	}
     }
//
//    resmgr_context_t         *ctp;
//    resmgr_connect_funcs_t   connect_funcs;
//    resmgr_io_funcs_t        io_funcs;
//    iofunc_attr_t          	 io_attr;
//    resmgr_attr_t            res_attr;
//    dispatch_t               *dispatch;
//    int                      rmgid;
//
//    screen_printf("DevNow testing...\n");
//
//    dispatch = dispatch_create();
//    if( dispatch == NULL )
//    {
//        screen_printf("dev-now: Unable to create dispatch context\n");//	: %s.", strerror(errno));
//        return ;
//    }
//
//    screen_printf("DevNow testing: res_attr \n");
//
//    memset( &res_attr, 0, sizeof( res_attr ) );
//    res_attr.nparts_max = 10;
//    res_attr.msg_max_size = 0;
//
//    screen_printf("DevNow testing: iofunc_attr \n");
//
//    iofunc_attr_init(&io_attr, 0666 | 0x2000 , 0, 0);	//	S_IFCHR
//    iofunc_func_init( _RESMGR_CONNECT_NFUNCS, &connect_funcs,
//                      _RESMGR_IO_NFUNCS, &io_funcs );
//
//    io_funcs.read = now_read;
//
//    screen_printf("DevNow testing: resmgr_attach \n");
//
//    rmgid = resmgr_attach(dispatch, &res_attr, "/dev/now", _FTYPE_ANY, 0, &connect_funcs, &io_funcs, &io_attr);
//    if (rmgid == -1)
//    {
//    	screen_printf("dev-now: Unable to attach resmgr\n");//	: %s", strerror(errno));
//    	return ;
//    }
//
//    screen_printf("DevNow testing: resmgr_context_alloc \n");
//    ctp = resmgr_context_alloc(dispatch);
//    if (ctp == NULL)
//    {
//    	screen_printf("dev-now: Unable to allocate resmgr context\n");//	: %s.", strerror(errno));
//    	return ;
//    }
//
//    while (1)
//    {
//    	ctp->rcvid 		= -1;
//    	ctp->msg->type 	= 0;
//    	//screen_printf("DevNow testing: resmgr_block \n");
//		ctp = resmgr_block(ctp);
//
//
//		if (ctp->rcvid != -1)
//		{
//			//screen_printf("DevNow testing: resmgr_handler \n");
//
//			//char szTmp[1024] = {0};
//			//sprintf(szTmp, "DevNow testing: resmgr_handler msg type:%d with rcvid:0x%x \n", ctp->msg->type, ctp->rcvid);
//			//screen_printf(szTmp);
//			//	screen_printf("DevNow testing: resmgr_handler \n");
//			resmgr_handler(ctp);
//		}
//
//    }

	return ;//EXIT_SUCCESS;
}


/*
int main(int argc, char **argv) {
	return 0;
}
*/
