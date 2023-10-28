/*
 * msgAttach_client.c
 *
 *  Created by:
 *      Author: Test
 */


/*
 * Message Client Process
 */

#include <platform/include/platform.h>
#include <libc/include/basic_file.h>
#include <libc/include/stdio.h>
#include <platform/include/resmgr.h>


typedef struct
{
    uint16_t msg_no;
    char msg_data[255];
} client_msg_t;

int main( int argc, char **argv )
{
    int fd;
    int c;
    client_msg_t msg;
    int ret;
    int num;
    char msg_reply[255];

    num = 1;

    /* Process any command line arguments */
    while( ( c = getopt( argc, argv, "n:" ) ) != -1 )
    {
       if( c == 'n' )
       {
           num = atoi(optarg);
       }
    }
    /* Open a connection to the server (fd == coid) */
    fd = open( "/msgserver", "r" );	//	fd = open( "/msgserver", O_RDWR );
    if( fd == -1 )
    {
    	printf_s("Unable to open server connection\n" );	//	fprintf( stderr, "Unable to open server connection: %s\n", strerror( errno ) );
        return -1;	//	EXIT_FAILURE;
    }

    /* Clear the memory for the msg and the reply */
    memset( &msg, 0, sizeof( msg ) );
    memset( &msg_reply, 0, sizeof( msg_reply ) );

    /* Set up the message data to send to the server */
    msg.msg_no = _MSG_MAX + num;
    sprintf( msg.msg_data, "client %d requesting reply.", getpid() );	//	snprintf( msg.msg_data, 254, "client %d requesting reply.", getpid() );

    printf_s( "client: msg_no: _IO_MAX + %d\n", num );
    //	fflush( stdout );

    /* Send the data to the server and get a reply */
    ret = MsgSend( fd, &msg, sizeof( msg ), msg_reply, 255 );
    if( ret != EOK)	//	== -1 )
    {
    	printf_s("Unable to MsgSend() to server \n" );	//	printf_s("Unable to MsgSend() to server: %s\n", strerror( errno ) );	//	fprintf( stderr, "Unable to MsgSend() to server: %s\n", strerror( errno ) );
        return -1;	//	EXIT_FAILURE;
    }

    /* Print out the reply data */
    printf_s( "client: server replied: %s\n", msg_reply );

    close( fd );

    while(1);

    return 0;	//EXIT_SUCCESS;
}

