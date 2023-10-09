/*
 * msgAttach.c
 *
 *  Created by:
 *      Author: Test
 */

#include <platform/include/platform.h>

#include <libc/include/basic_file.h>
#include <platform/include/resmgr.h>

//#define THREAD_POOL_PARAM_T     dispatch_context_t
//
//static resmgr_connect_funcs_t   connect_func;
//static resmgr_io_funcs_t        io_func;
//static iofunc_attr_t            attr;

struct sigevent       			event;

int 							iRepeatCount	= 0;

int timer_tick(struct _pulse* siginfo){	//, int code, unsigned flags, void *handle) {	//	int timer_tick(sigwait_context_t *ctp, int code, unsigned flags, void *handle) {
//	int timer_tick(message_context_t *ctp, int code, unsigned flags, void *handle) {
	struct sigevent       event;
	int value = siginfo->value;	//	int value = ctp->info.siginfo.value;
	//int value = ctp->msg->pulse.value;	//	union sigval       value = ctp->msg->pulse.value;
    // Do some useful work on every timer firing...
	//	printf_s("received timer event\n");		//
	printf_s("[pulseAttach]received timer event, value %d\n", value);		//	.sival_int);

	//if(iRepeatCount<5){
	//	TimerCreate(CLOCK_REALTIME, &event);
	//	iRepeatCount++;
	//}

    return 0;
}

int message_handler(int code){//, unsigned flags, void *handle ) {	//	int message_handler(message_context_t *ctp, int code, unsigned flags, void *handle ) {
	printf_s("received private message, type %d\n", code);
    return 0;
}

int
main(int argc, char **argv) {
    int                   	timer_id;
    struct _itimer        	itime;

    int						iTimerCount = 0;

    char 					szTmp[1024] = {0};
    //thread_pool_attr_t    pool_attr;
    //thread_pool_t         *tpp;
   /* dispatch_t            	*dpp;
    resmgr_attr_t         	resmgr_attr;
    int                   	id;


    dispatch_context_t   	*ctp, *ctp_ret;


    printf_s("Test pulse attach\n" );

    if((dpp = dispatch_create()) == NULL) {
    	printf_s("Unable to allocate dispatch handle.\n");
        //	fprintf(stderr, "%s: Unable to allocate dispatch handle.\n", argv[0]);
        return -1;	//EXIT_FAILURE;
    }


    iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_func,
                     _RESMGR_IO_NFUNCS, &io_func);
    iofunc_attr_init(&attr, 0x5000 | 0666, 0, 0);	//	S_IFNAM

    memset(&resmgr_attr, 0, sizeof resmgr_attr);
    resmgr_attr.nparts_max = 1;
    resmgr_attr.msg_max_size = 2048;

    if((id = resmgr_attach(dpp, &resmgr_attr, "/dev/mynull",
                 _FTYPE_ANY, 0,
                 &connect_func, &io_func, &attr)) == -1) {
        printf_s("%s: Unable to attach name.\n");	//	fprintf(stderr, "%s: Unable to attach name.\n", argv[0]);
        return -1;	//	EXIT_FAILURE;
    }
	*/
    int iChid = ChannelCreateWithPath(0, "/dev/mynull");
    if(iChid<0){
    	xprintf("Create Channel failed.\n");
    	return -1;
    }

    //	We want to handle our own private messages, of type
    //	0x5000 to 0x5fff
    /*printf_s("Before message_attach()  \n" );
    if(message_attach(dpp, NULL, 0x5000, 0x5fff,
                     &message_handler, NULL) == -1) {
       printf_s("Unable to attach to private message range.\n");	//	fprintf(stderr, "Unable to attach to private message range.\n");
       return -1;	//	EXIT_FAILURE;
    }
    printf_s("Before pulse_attach()  \n" );
    // Initialize an event structure, and attach a pulse to it
    if((event.code = pulse_attach(dpp,
                              MSG_FLAG_ALLOC_PULSE, 0,
                              &timer_tick, NULL)) == -1) {
        printf_s("Unable to attach timer pulse.\n");	//	fprintf(stderr, "Unable to attach timer pulse.\n");
        return -1;	//	EXIT_FAILURE;
    }
    */
    event.code = 123;
    // Connect to our channel
    printf_s("Before message_connect()  \n" );
    if((event.coid = ConnectAttach(0, 0, iChid, 0, 0)) == -1) {	//	message_connect(dpp, MSG_FLAG_SIDE_CHANNEL)) == -1) {
        printf_s("Unable to attach to channel.\n");	//	fprintf(stderr, "Unable to attach to channel.\n");
        return -1;	//EXIT_FAILURE;
    }
    sprintf(szTmp, "After message_connect() :0x%x for channel:0x%x \n", event.coid, iChid);
    printf_s(szTmp);
    event.notify = SIGEV_PULSE;
    event.priority = 10;	//-1;

    // We could create several timers and use different
    // sigev values for each
    event.value = 12345;	//	0;	//	.sival_int
    printf_s("Before TimerCreate()  \n" );
    if((timer_id = TimerCreate(CLOCK_REALTIME, &event)) == -1) {
        printf_s("Unable to attach channel and connection.\n");	//	fprintf(stderr, "Unable to attach channel and connection.\n");
        return -1;	//	EXIT_FAILURE;
    }

    // And now setup our timer to fire every second
    itime.it_value.tv_sec = 0;
    itime.it_value.tv_nsec = 1000000;
    itime.it_interval.tv_sec = 0;
    itime.it_interval.tv_nsec = 0;

    printf_s("Before TimerSettime()  \n" );
    TimerSettime(timer_id, 0, &itime, NULL);

    struct _pulse pulseTmp;

    while (1) {
    	memset(&pulseTmp, 0, sizeof(struct _pulse));
    	int iRet = MsgReceivePulse(iChid, &pulseTmp, sizeof(pulseTmp), NULL);	//	 MsgReceivePulse( iChid, &iov, iRParts, NULL );	//iRcvid = MsgReceive(iChid, szBuf, sizeof(szBuf), NULL);
    	if (iRet < 0){
    		xprintf("Receive failed.\n");
    		continue;
        }
    	//xprintf("Receive pulse.\n");
    	if(pulseTmp.code==123){
    		timer_tick(&pulseTmp);
            if(iTimerCount++>3){
            	TimerDestroy(timer_id);
            }
    	}else{
    		message_handler(pulseTmp.code);
    	}
    }
    /*
    // Never returns
    printf_s("Before dispatch_context_alloc()  \n" );
    dpp->block_type = _DISPATCH_BLOCK_SIGWAIT;
    ctp = malloc(sizeof(sigwait_context_t));	//_DPP(dpp)->context_size);dispatch_context_alloc( dpp );
    if( ctp == NULL )
    {
        printf_s("dispatch_context_alloc() failed\n" );	//	fprintf( stderr, "dispatch_context_alloc() failed: %s\n", strerror( errno ) );
        return -1;	//	EXIT_FAILURE;
    }
    ctp->sigwait_context.dpp = dpp;
    while( 1 )
    {
    	//printf_s("Before dispatch_block()  \n" );
        ctp_ret = dispatch_block( ctp );
        if( ctp_ret )
        {
        	//printf_s("Before dispatch_handler()  \n" );
            dispatch_handler( ctp );

            if(iTimerCount++>3){
            	TimerDestroy(timer_id);
            }
            //break;
        }
        else
        {
        	;//printf_s("dispatch_block() failed\n" );	//	fprintf( stderr, "dispatch_block() failed: %s\n", strerror( errno ) );
            //break;	//return -1;	//	EXIT_FAILURE;
        }
    }
    */
    //thread_pool_start(tpp);
    return 0;	//	EXIT_SUCCESS;
}


/*
int main(int argc, char **argv) {
	return 0;
}
*/
