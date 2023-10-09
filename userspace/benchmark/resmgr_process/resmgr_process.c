#include <platform/include/platform.h>
#include <libc/include/basic_file.h>
#include <platform/include/resmgr.h>

#include "circular_queue.h"

char buffer[1024];

//char 	szBuf0[1024] = {0};
//char 	szBuf1[1024] = {0};
//char 	szBuf2[1024] = {0};
//char 	szBuf3[1024] = {0};
//char 	szBuf4[1024] = {0};
//char 	szBuf5[1024] = {0};
//char 	szBuf6[1024] = {0};
int 	iRParts		= MAX_MSG_SRPARTS;
iov_t	iov[MAX_MSG_SRPARTS];

CirQueue dataQueue;
CirQueue * cq = &dataQueue;


typedef struct
{
    uint32_t msg_no;
    char     msg_data[256];
} server_msg_t;


int message_callback( int rcvid, void* msgRcv)
{
    server_msg_t *msg;
    int num;
    char msg_reply[255];

    // Cast a pointer to the message data
    msg = (server_msg_t *)msgRcv;

    // Print some useful information about the message
    printf_s( "\n\nServer Got Message:\n" );
    printf_s( "  type: %d\n" , msg->msg_no);//	type );
    printf_s( "  data: %s\n\n", msg->msg_data );

    // Build the reply message
    num = msg->msg_no;//type - _IO_MAX;
    snprintf( msg_reply, 254, "Server Got Message Code: %d", num );	//	"Server Got Message Code: _IO_MAX + %d", num );

    // Send a reply to the waiting (blocked) client
    MsgReply( rcvid, EOK, msg_reply, strlen( msg_reply ) + 1 );

    return 0;
}


int io_read(int iRcvid, io_read_t *msg)	//, iov_t *iov)
{
    int nleft;
    int nbytes;
    int nparts;
    int status;
    int index = 0;
    //iov_t						iov[MAX_MSG_SRPARTS];
    xprintf("io_read\n");
    //return 0;

    //if ((status = iofunc_read_default(ctp, msg, ocb)) != EOK)	//	if ((status = iofunc_read_verify(ctp, msg, ocb, NULL)) != EOK)
    //    return (status);
    
    //if ((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE)
    //    return (ENOSYS);


    // On all reads (first and subsequent), calculate
    // how many bytes we can return to the client,
    // based upon the number of bytes available (nleft)
    // and the client's buffer size


    for (index; index < msg->nbytes; index++) {
    	if (QUEUE_STAT_NOK == DeQueue(cq, &buffer[index]))
            break;
    }

    nleft = index;
    nbytes = MIN(msg->nbytes, nleft);

    if (nbytes > 0) {
        // set up the return data IOV
        SETIOV(&(iov[0]), buffer, nbytes);

        // set up the number of bytes (returned by client's read())
        //_IO_SET_READ_NBYTES (ctp, nbytes);


        // advance the offset by the number of bytes
        // returned to the client.


      //  ocb->offset += nbytes;

        nparts = 1;
    } else {

        // they've asked for zero bytes or they've already previously
        // read everything

        //_IO_SET_READ_NBYTES(ctp, 0);

        nparts = 0;
    }

    // mark the access time as invalid (we just accessed it)
    //	if (msg->i.nbytes > 0)  ocb->attr->flags |= IOFUNC_ATTR_ATIME;
    
    int iRslt = nparts;
//	if(_IO_READ!=ctp->msg->type){
//		MsgReply(ctp->rcvid, iRslt, NULL, 0);
//	}else
	{
		for(int i=nparts; i<MAX_MSG_SRPARTS; i++){
			iov[i].iov_len = 0;
		}
		MsgReplyv(iRcvid, iov[0].iov_len, iov, nparts);
	}

    return iRslt;	//nbytes;	//	(_RESMGR_NPARTS(nparts));
}

int io_write (int iRcvId){	//, iov_t*	iov){	//io_write_t * msg) {
    int status;
    char * buf;
    unsigned int index;
    int nleft;
    char szTmp[1024] = {0};
    //iov_t						iov[MAX_MSG_SRPARTS];
    xprintf("io_write\n");


    //buf = (char *) malloc (msg->i.nbytes + 1);
    //if (buf == NULL)
    //    return (ERR_NO_MEMORY);	//	ENOMEM);


    //	Reread the data from the sender's message buffer.
    //	We're not assuming that all of the data fit into the
    //	resource manager library's receive buffer.

    //resmgr_msgread(ctp, buf, msg->i.nbytes, sizeof(msg->i));
    for(int i=1; i<MAX_MSG_SRPARTS; i++){
    	if(iov[i].iov_len>0){
    		((char*)iov[i].iov_base)[iov[i].iov_len] = '\0';
    		//	buf[msg->i.nbytes] = '\0'; // just in case the text is not NULL terminated
			printf_s("Received %d bytes = '%s'\n", iov[i].iov_len, iov[i].iov_base);
			for (index = 0; index < iov[i].iov_len; index++) {	//for (index = 0; index < msg->i.nbytes; index++) {
				if (QUEUE_STAT_NOK == EnQueue(cq, ((char*)iov[i].iov_base)[index])) {
					printf_s("Enqueue Failed!\n");
					//return (ENOMEM);
					break;
				}
			}
    	}
    }

    sprintf(szTmp, "io_write done with index:%d \n", index);
    printf_s(szTmp);
    //nleft = index;
    //free(buf);

    // set up the number of bytes (returned by client's write())

    //	_IO_SET_WRITE_NBYTES(ctp, MIN(msg->i.nbytes, nleft));

    //	if(msg->i.nbytes > 0) ocb->attr->flags |= IOFUNC_ATTR_MTIME | IOFUNC_ATTR_CTIME;


    int iRslt = index;

//	if(_IO_READ!=ctp->msg->type){
//		MsgReply(ctp->rcvid, iRslt, NULL, 0);
//	}else
	{
		for(int i=iRslt; i<MAX_MSG_SRPARTS; i++){
			iov[i].iov_len = 0;
		}
		MsgReplyv(iRcvId, index, iov, 0);	//	MsgReplyv(iRcvId, iov[0].iov_len, iov, index);
	}

    return iRslt;	//	(_RESMGR_NPARTS(0));
}

int main (int argc, char * argv[]) {

    // declare variables we'll be using
    int   	iChid = 0 ;
    int 	iRcvid = 0;

    for(int i=0; i<MAX_MSG_SRPARTS; i++){
    	iov[i].iov_base = malloc(1024);
    	iov[i].iov_len  = 1024;
    	memset(iov[i].iov_base, 0, 1024);
    }

//	iov[0].iov_base = szBuf0;
//	iov[0].iov_len  = 1024;
//
//	iov[1].iov_base = szBuf1;
//	iov[1].iov_len  = 1024;
//
//	iov[2].iov_base = szBuf2;
//	iov[2].iov_len  = 1024;
//
//	iov[3].iov_base = szBuf3;
//	iov[3].iov_len  = 1024;
//
//	iov[4].iov_base = szBuf4;
//	iov[4].iov_len  = 1024;
//
//	iov[5].iov_base = szBuf5;
//	iov[5].iov_len  = 1024;
//
//	iov[6].iov_base = szBuf6;
//	iov[6].iov_len  = 1024;

	iRParts		= MAX_MSG_SRPARTS;

    // initialize the circular queue
    InitQueue(cq);

    // initialize dispatch interface



    // attach our device name
    iChid = ChannelCreateWithPath(0, "/dev/hello");
    if(iChid<0){
    	xprintf("Create Channel failed.\n");
    	return -1;
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
    		return -1;
        }
    	//xprintf("Server receive msg.\n");
    	msg_header_t msgHeader;
    	memcpy(&msgHeader, iov[0].iov_base, sizeof(msg_header_t));
    	switch(msgHeader.type){
    	case _IO_READ:
    		io_read(iRcvid, (io_read_t *)iov[0].iov_base);	//, &iov);
    		break;
    	case _IO_WRITE:
    		io_write(iRcvid);	//,  &iov);
    		break;
    	default:
    		message_callback(iRcvid, (void*)iov[0].iov_base);

    	}
    	//msg_handler(iRcvid, szBuf);	//	dispatch_handler(ctp);		//
    }

    return 0;
}

/*
int main(int argc, char **argv) {
	return 0;
}

*/
