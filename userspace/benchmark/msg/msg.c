/*
 * msg.c
 *
 *  Created on: 2022-9-21
 *      Author: Test
 */

#include <platform/include/platform.h>
#include <libc/include/basic_file.h>
#include <platform/include/resmgr.h>

struct timespec pstart;

char *buf1, *buf2;
unsigned flag = 0;

void *
rcv(void *data) {
	int             chid = (int) data;

	if((buf2 = mmap(NULL, 64*4096, PROT_READ | PROT_WRITE, MAP_ANON  | MAP_PRIVATE, NOFD, 0)) == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}

	while(flag) {
		int rcvid;

		if ((rcvid = MsgReceive(chid, buf2, 64*4096, 0)) == -1)
			perror("MsgReceive");
		// buf2[0] is the # of IOV's to reply with, buf2[1] is length of each.
		if (MsgReply(rcvid, 0, NULL, 0) == -1) {
			perror("MsgReply");
		}
	}
	return 0;
}

int testloop(int coid, int size, int iovs, int limit) {
	int i, loop = limit;
	clock_t start, stop;
	iov_t		iov[6];
	char		szTmp[1024] = {0};
	if (iovs > 6) {
		perror("Too many IOV's\n");
		exit(1);
	}


	for(i=0; i < iovs; i++) {
		SETIOV(&iov[i], buf1 + i * size, size);
	}


	clock_gettime(0, &pstart);
	start =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//start = myclock();
	while (loop--) {
		if (MsgSendv(coid, iovs ? iov : NULL, iovs, NULL, 0) != EOK){	//	== -1) {
			perror("MsgSend");
			exit(1);
		}
	}
	clock_gettime(0, &pstart);
	stop =  pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//stop = myclock();
	sprintf(szTmp, "PASS: Time for Size:%d iovs:%d = %d usec\n", (size *iovs), iovs, (int)(1000.0 * (double)(stop - start) / (double) limit));	//sprintf(szTmp, "PASS: Time for Size:%10d iovs:%4d = %f usec\n", (size *iovs), iovs, (1000.0 * (double)(stop - start) / (double) limit));
	printf_s(szTmp);
	return 0;
}

int
main(int argc, char *argv[]) {
	int             pid, chid, coid;
	int				limit;
	char 			pid_str[64], chid_str[64];
	char			name[256];
	char			buf[80];


	// for automation
	//	teststart(argv[0]);
	//	testpntbegin(argv[0]);

	limit = 100;	//	calibrate_loop() / 2;

	//if(argc > 2) {
	//	pid = atoi(argv[2]);
	//	chid = atoi(argv[3]);
	//	coid = ConnectAttach(0, pid, chid, 0, 0);
	//	//printf("Client, pid of receiver = %d chid %d coid %x errno %d\n", pid, chid, coid, errno);
	//} else
	{
		chid = ChannelCreate(0);
		coid = ConnectAttach(0, 0, chid, 0, 0);
	}

	sprintf(buf, "Number of iterations is %d", limit);	//	SPRNT(buf, sizeof(buf), "Number of iterations is %d", limit);
	printf_s(buf);	//	testnote(buf);
	sprintf(buf, "**** Times include a zero-length reply ****");	//	SPRNT(buf, sizeof(buf), "**** Times include a zero-length reply ****");
	printf_s(buf);	//	testnote(buf);

	if((buf1 = mmap(NULL, 64*4096, PROT_READ | PROT_WRITE, MAP_ANON  | MAP_PRIVATE, NOFD, 0)) == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}

	clock_gettime(CLOCK_REALTIME, &pstart);
	setprio(0, 29);


	flag = 1;

	if(argc <= 2) {
		pthread_create(0, 0, &rcv, (void*) chid);
		sleep(3);
	}

	testloop(coid, 0, 0, limit);
	testloop(coid, 16, 1, limit);
	testloop(coid, 8, 2, limit);
	testloop(coid, 64, 1, limit);
	testloop(coid, 128, 1, limit);
	testloop(coid, 512, 1, limit);
	testloop(coid, 32, 2, limit);
	testloop(coid, 16, 4, limit);
	testloop(coid, 4096, 1, limit);
	testloop(coid, 4097, 1, limit);
	testloop(coid, 4096, 2, limit);

	//if(argc > 2) exit(0);

	rcv((void *)pid);
	return 0;
}

/*
int main(int argc, char **argv) {
	return 0;
}
*/

