/*
 * msgAttach_server.c
 *
 *  Created by:
 *      Author: Test
 */


#include <platform/include/platform.h>
#include <libc/include/basic_file.h>
#include <platform/include/resmgr.h>

typedef struct
{
    uint16_t msg_no;
    char     msg_data[255];
} server_msg_t;


int message_callback(int iRcvID, void* msgTmp)		// message_context_t *ctp, int type, unsigned flags,  void *handle )
{
    server_msg_t *msg;
    int num;
    char msg_reply[255];

    // Cast a pointer to the message data
    msg = (server_msg_t *)msgTmp;	//ctp->msg;

    int type = msg->msg_no;

    // Print some useful information about the message
    printf_s( "\n\nServer Got Message:\n" );
    printf_s( "  type: %d\n" , type );
    printf_s( "  data: %s\n\n", msg->msg_data );

    // Build the reply message
    num = type - _MSG_MAX;
    snprintf( msg_reply, 254, "Server Got Message Code: _IO_MAX + %d", num );

    // Send a reply to the waiting (blocked) client
    MsgReply( iRcvID, EOK, msg_reply, strlen( msg_reply ) + 1 );

    return 0;
}



int main( int argc, char **argv )
{
	int iChid 	= -1;

	int iRParts = MAX_MSG_SRPARTS;
	iov_t	iov[MAX_MSG_SRPARTS];

    for(int i=0; i<MAX_MSG_SRPARTS; i++){
    	iov[i].iov_base = malloc(1024);
    	iov[i].iov_len  = 1024;
    	memset(iov[i].iov_base, 0, 1024);
    }


//    resmgr_attr_t        resmgr_attr;
//    message_attr_t       message_attr;
//    dispatch_t           *dpp;
//    dispatch_context_t   *ctp, *ctp_ret;
//    int                  resmgr_id, message_id;
//
//    // Create the dispatch interface
//    dpp = dispatch_create();
//    if( dpp == NULL )
//    {
//    	printf_s("dispatch_create() failed \n");	//	fprintf( stderr, "dispatch_create() failed: %s\n", strerror( errno ) );
//        return -1;	//	EXIT_FAILURE;
//    }
//
//    memset( &resmgr_attr, 0, sizeof( resmgr_attr ) );
//    resmgr_attr.nparts_max = 1;
//    resmgr_attr.msg_max_size = 2048;
//
//    // Setup the default I/O functions to handle open/read/write/...
//    iofunc_func_init( _RESMGR_CONNECT_NFUNCS, &ConnectFuncs,
//                      _RESMGR_IO_NFUNCS, &IoFuncs );
//
//    // Setup the attribute for the entry in the filesystem
//    iofunc_attr_init( &IoFuncAttr, 0x5000 | 0666, 0, 0 );	//	S_IFNAM

	if ((iChid = ChannelCreateWithPath(0, "/msgserver")) == -1) {	//	if ((chid = ChannelCreate(0)) == -1) {
            perror("ChannelCreate");
            return -1;
    }

//    resmgr_id = resmgr_attach( dpp, &resmgr_attr, "/msgserver", _FTYPE_ANY,
//                               0, &ConnectFuncs, &IoFuncs, &IoFuncAttr );
//    if( resmgr_id == -1 )
//    {
//        printf_s("resmgr_attach() failed \n" );	//	fprintf( stderr, "resmgr_attach() failed: %s\n", strerror( errno ) );
//        return -1;	//	EXIT_FAILURE;
//    }
//
//    // Setup our message callback
//    memset( &message_attr, 0, sizeof( message_attr ) );
//    message_attr.nparts_max = 1;
//    message_attr.msg_max_size = 4096;
//
//    // Attach a callback (handler) for two message types
//    message_id = message_attach( dpp, &message_attr, _IO_MAX + 1,
//                                 _IO_MAX + 2, message_callback, NULL );
//    if( message_id == -1 )
//    {
//        printf_s( "message_attach() failed\n" );	//	fprintf( stderr, "message_attach() failed: %s\n", strerror( errno ) );
//        return -1;	//	EXIT_FAILURE;
//    }
//
//    // Setup a context for the dispatch layer to use
//    ctp = dispatch_context_alloc( dpp );
//    if( ctp == NULL )
//    {
//        printf_s("dispatch_context_alloc() failed\n" );	//	fprintf( stderr, "dispatch_context_alloc() failed: %s\n", strerror( errno ) );
//        return -1;	//	EXIT_FAILURE;
//    }


    while (1) {
        for(int i=0; i<MAX_MSG_SRPARTS; i++){
        	iov[i].iov_len  = 1024;
        	memset(iov[i].iov_base, 0, 1024);
        }
    	int iRcvid =  MsgReceivev( iChid, &iov, iRParts, NULL );	//iRcvid = MsgReceive(iChid, szBuf, sizeof(szBuf), NULL);
    	if (iRcvid < 0){
    		xprintf("Receive failed.\n");
    		continue;
        }
    	xprintf("Server receive msg.\n");

    	message_callback(iRcvid, (void*)iov[0].iov_base);
    	//msg_handler(iRcvid, szBuf);	//	dispatch_handler(ctp);		//
    }

    return 0;	//	EXIT_SUCCESS;
}

/*
int main(int argc, char **argv) {
	return 0;
}
*/
