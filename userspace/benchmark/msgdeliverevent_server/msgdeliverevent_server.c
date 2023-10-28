/*
 * msgdeliverevent_server.c
 *
 *  Created on: 2022-9-21
 *      Author: Test
 */


#include <platform/include/platform.h>
#include <libc/include/basic_file.h>
#include <platform/include/resmgr.h>

struct my_msg
{
   short type;
   struct sigevent event;
};

#define MY_PULSE_CODE 5
#define MSG_GIVE_PULSE _MSG_MAX+4
#define MY_SERV "/dev/eventsvr"

int main(int argc, char **argv) {
	int		iChid;
	int     rcvid;
	union {
	  struct my_msg   mine;
	  struct _pulse   pulse;
	}           msg;


//  name_attach_t  *attach;
//  // Attach the name the client will use to find us.
//  // Our channel will be in the attach structure
//  if ((attach = name_attach(NULL, "/dev/eventsvr", 0)) == NULL) {
//	  printf_s("server:failed to attach name");	//, errno %d\n", errno);
//     exit(1);
//  }

	if ((iChid = ChannelCreateWithPath(0, "/dev/eventsvr")) == -1) {	//	if ((chid = ChannelCreate(0)) == -1) {
          perror("ChannelCreate");
          return -1;
	}

  printf_s("server: start running...");

  // Wait for the message from the client.
  for( ;; ) {
     rcvid = MsgReceive(iChid, &msg, sizeof(msg), NULL);

     //printf_s("server: receive msg ...\n");

     switch(msg.mine.type) {
//        case _PULSE_CODE_DISCONNECT:
//           ConnectDetach(msg.pulse.scoid);
//           break;
//        case _IO_CONNECT:
        case _IO_CLOSE:
           MsgReply(rcvid, 0, NULL, 0);
           break;
        case MSG_GIVE_PULSE:
        	//printf_s("server: receive msg for pulse...\n");

           // Wait until it is time to notify the client
           //sleep(2);


           // Deliver notification to client that client
           // requested

           MsgDeliverEvent(rcvid, &msg.mine.event);
           printf_s("server:delivered event\n");
           //	return 0;

           MsgReply(rcvid, 0, NULL, 0);
           break;
        default:
        	printf_s("server: unexpected message %d\n", msg.mine.type);
        	//return 0;

        	MsgReply(rcvid, 0, NULL, 0);
        	break;
     }
  }

  return 0;
}

/*
int main(int argc, char **argv) {
	return 0;
}

*/
