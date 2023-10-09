/*
 * msgdeliverevent_client.c
 *
 *  Created on: 2022-9-21
 *      Author: Test
 */


#include <platform/include/platform.h>
#include <libc/include/basic_file.h>
#include <platform/include/resmgr.h>

// client.c

//#include "my_hdr.h"
struct my_msg
{
   short type;
   struct sigevent event;
};

#define MY_PULSE_CODE 5
#define MSG_GIVE_PULSE _MSG_MAX+4
#define MY_SERV "/dev/eventsvr"

int main( int argc, char **argv)
{
  int chid, coid, srv_coid, rcvid;
  struct my_msg msg;
  struct _pulse pulse;

  // we need a channel to receive the pulse notification on
  chid = ChannelCreate( 0 );

  // and we need a connection to that channel for the pulse to be delivered on
  coid = ConnectAttach( 0, 0, chid, 0, 0 );

  // fill in the event structure for a pulse
  SIGEV_PULSE_INIT( &msg.event, coid, 10,	//	SIGEV_PULSE_PRIO_INHERIT,
                    MY_PULSE_CODE, 0 );
  msg.type = MSG_GIVE_PULSE;

  // find the server
  if ( (srv_coid = open( MY_SERV, 0 )) == -1)	//	name_open( MY_SERV, 0 )) == -1)
  {
	  printf_s("failed to find server");	//	, errno %d\n", errno );
     exit(1);
  }

  //printf_s("Connect with server.\n");

//while(1){
  // give the pulse event we initialized above to the server for later delivery
  MsgSend( srv_coid, &msg, sizeof(msg), NULL, 0 );

  printf_s("Before receive pulse.\n");
  // wait for the pulse from the server
  rcvid = MsgReceivePulse( chid, &pulse, sizeof( pulse ), NULL );
  printf_s("After receive pulse: got pulse with code %d, waiting for %d\n", pulse.code,
         MY_PULSE_CODE );

  //sleep(2);
//}

  close(srv_coid);
  
  
  return 0;
}

/*
int main(int argc, char **argv) {
	return 0;
}
*/
