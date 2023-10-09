/*
 * pulse.c
 *
 *  Created on: 2022-9-13
 *      Author: Test
 */


#include <platform/include/platform.h>

#define _PULSE_CODE_MINAVAIL	0
#define _PULSE_CODE_MAXAVAIL	127


enum { PULSE_SELF, PULSE_THREAD, PULSE_PROCESS };

int		chid, coid, limit, np, pri, compress, value, affinity;

static void *supplier(void *param)
{
	int		loop;

	coid = ConnectAttach(0, getpid(), chid, 0, 0);

	for (loop = limit; loop != 0; --loop) {
		MsgSendPulse(coid, pri, _PULSE_CODE_MINAVAIL, compress ? value : ++value);
	}
	ConnectDetach(coid);
	return(NULL);
}

void main(void)
{
	int					(*recv)(int, void *, int, struct _msg_info *);
	struct timespec 	pstart;

	clock_t				start, stop;
	pthread_t			thread;

	MSG_DATA			pulse;
	MSG_PAYLOAD_ITEM	info, *pMsginfo;
	int					loop, loop2, mode, opt, result;
	pid_t				child;
	char				szTestPassInfo[1024] = {0};


	compress = !0;
	np = 0;
	pri = 128;
	value = 0;
	affinity = 0;
	mode = PULSE_SELF;
	recv = MsgReceivePulse;
	pMsginfo = NULL;


	{
		if (!np && mode == PULSE_SELF) np = 254;
		limit = 1000;

		chid = ChannelCreate(0);

		switch (mode) {
		case PULSE_SELF:
			coid = ConnectAttach(0, getpid(), chid, 0, 0);

			limit /= np;

			clock_gettime(0, &pstart);
			start = pstart.tv_sec*1000 + pstart.tv_nsec/1000000;
			for (loop = limit; loop != 0; --loop) {
				for (loop2 = np; loop2 != 0; --loop2)
					MsgSendPulse(coid, pri, _PULSE_CODE_MINAVAIL, compress ? value : ++value);
				for (loop2 = np; loop2 != 0; --loop2)
					(*recv)(chid, &pulse, sizeof(pulse), pMsginfo);
			}
			clock_gettime(0, &pstart);
			stop = pstart.tv_sec*1000 + pstart.tv_nsec/1000000;
			limit *= np;
			ConnectDetach(coid);
			break;

		default:
			clock_gettime(0, &pstart);
			start = stop = pstart.tv_sec*1000 + pstart.tv_nsec/1000000;
			break;
		}

		ChannelDestroy(chid);

		sprintf(szTestPassInfo, "average %s pulse%s send/receive = %d usec\n",
					(mode == PULSE_SELF) ? "self" : (mode == PULSE_THREAD) ? "inter-thread" : "inter-process",
					(pMsginfo != NULL) ? " (with info)" : "", (stop - start));


		xprintf(szTestPassInfo);
		screen_printf(szTestPassInfo);
	}


	return;
}

