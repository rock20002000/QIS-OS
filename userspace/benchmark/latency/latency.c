/*
 *  latency -- jgarvey
 *  Attach to the internal clock/tick interrupt and time the latency
 *  between this and schedulling a user thread as a result of the
 *  interrupt event.  The latency is clock cycles between the last
 *  line of the interrupt handler and the first line of user notification.
 */
#include <platform/include/platform.h>
#include <libc/include/basic_file.h>
#include <platform/include/resmgr.h>

#define __PAGESIZE	4096
#define UCHAR_MAX 	255

#define YIELD yield

#define CYCLES2USEC(_c, _cps)	((double)(_c) * 1000.0) / ((double)(_cps) / 1000.0)
#define TAKETURNS(_a)			if ((_a)->load > 1) YIELD()

struct area {
	volatile int	state;
	int				count;
	int				overrun;
	uint64_t		cycles;
	pthread_mutex_t	active;
	volatile int	load;
}		comms;


struct timespec pstart;


int int_handler_interrupt(){
	if (!comms.state) {
		comms.state = !0;
		++comms.count;

		clock_gettime(0, &pstart);	//	stop
		comms.cycles = pstart.tv_sec*1000 + pstart.tv_nsec/1000000;
		//	ClockCycles();
	}
	else if (!comms.overrun) {
		comms.overrun = +1;
	}
	return 1;	//	(&event);

}

//const struct sigevent *handler(void *area, int id)
//{
//static const struct sigevent	event = { SIGEV_INTR };
//struct area						*ap = (struct area *)area;
//
//	if (!ap->state) {
//		ap->state = !0;
//		++ap->count;
//		ap->cycles = ClockCycles();
//	}
//	else if (!ap->overrun) {
//		ap->overrun = +1;
//	}
//	return(&event);
//}

void *load_mmap(void *arg)
{
struct area		*ap = (struct area *)arg;
void			*ptr;
size_t			size;
unsigned		pages;

	pthread_mutex_lock(&ap->active);
	pthread_mutex_unlock(&ap->active);
	for (pages = 0; ap->load > 0; ++pages) {
		if ((ptr = mmap(NULL, size = (pages % UCHAR_MAX) * __PAGESIZE, PROT_NONE, MAP_ANON | MAP_PRIVATE, NOFD, 0)) != MAP_FAILED)
			munmap(ptr, size);
		TAKETURNS(ap);
	}
	return(NULL);
}
void *load_msgpass(void *arg)
{
struct area		*ap = (struct area *)arg;
void			*ptr;
int				null;

	null = open("/dev/null", FILEOPEN_WRITEONLY), ptr = malloc(__PAGESIZE);
	//assert(null != -1 && ptr != NULL);
	pthread_mutex_lock(&ap->active);
	pthread_mutex_unlock(&ap->active);
	while (ap->load > 0) {
		write(null, ptr, __PAGESIZE), write(null, ptr, __PAGESIZE);
		write(null, ptr, __PAGESIZE), write(null, ptr, __PAGESIZE);
		write(null, ptr, __PAGESIZE), write(null, ptr, __PAGESIZE);
		write(null, ptr, __PAGESIZE), write(null, ptr, __PAGESIZE);
		TAKETURNS(ap);
	}
	free(ptr), close(null);
	return(NULL);
}
static void *_load_thread(void *arg)
{
	return(arg);
}
void *load_thread(void *arg)
{
struct area		*ap = (struct area *)arg;
pthread_t		tid;

	pthread_mutex_lock(&ap->active);
	pthread_mutex_unlock(&ap->active);
	while (ap->load > 0) {
		if (pthread_create(&tid, NULL, _load_thread, NULL) == EOK)
			pthread_join(tid, NULL);
		TAKETURNS(ap);
	}
	return(NULL);
}
int startload(char *mechanism, int pri)
{
	void				*(*func)(void *);
	pthread_attr_t		attr;
	//struct sched_param	param;

	if (!strcmp(mechanism, "mmap"))
		func = load_mmap;
	else if (!strcmp(mechanism, "msgpass"))
		func = load_msgpass;
	else if (!strcmp(mechanism, "thread"))
		func = load_thread;
	else
		return(0);
//	pthread_attr_init(&attr);
//	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
//	pthread_attr_setschedparam(&attr, (param.sched_priority = pri, &param));
//	pthread_attr_setschedpolicy(&attr, SCHED_RR);
//	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

	pthread_create(NULL, NULL, func, &comms);	//	pthread_create(NULL, &attr, func, &comms);
//	pthread_attr_destroy(&attr);
	return(1);
}

int main(int argc, char *argv[])
{
	//	struct sched_param	param;
	//	struct _clockperiod	period;
	uint64_t			cps, cycles, snap, total, latency[2];
	int					iid, limit, loop, intnum, pri, alg, opt, result;
	char				msg[128];

	//teststart(argv[0]);
	comms.state = 0, comms.count = 0, comms.overrun = 0;
	pthread_mutex_init(&comms.active, NULL), pthread_mutex_lock(&comms.active);
	comms.load = 0;

	//ClockPeriod(CLOCK_REALTIME, NULL, &period, 0);
	cps = 8000000;	//	SYSPAGE_ENTRY(qtime)->cycles_per_sec >> CLOCKCYCLES_INCR_BIT;
	intnum = 0;//87;	//SYSPAGE_ENTRY(qtime)->intr;
//	pri = sched_get_priority_max(alg = SCHED_FIFO);
//	while ((opt = getopt(argc, argv, "i:L:P:")) != -1) {
//		if (opt == 'i')
//			intnum = atoi(optarg);
//		else if (opt == 'L')
//			comms.load += startload(optarg, pri - 1);
//		else if (opt == 'P')
//			pri = atoi(optarg);
//	}
	limit = 1000;	//calibrate_loop() / 100;

	//SchedSet(0, 1, alg, (param.sched_priority = pri, &param));
//	result = mlock(&comms, sizeof(struct area));
//	assert(result != -1 || errno == ENOSYS);
//	result = ThreadCtl(_NTO_TCTL_IO, 0);
//	assert(result != -1);
//	result = ThreadCtl(_NTO_TCTL_RUNMASK, (void *)0x1);
//	assert(result != -1);

	//sprintf(msg, "interrupt/schedulling latency, %3.3f Hz, %lld cycles/sec", (double)period.nsec / 1000.0, cps);
	//testpntbegin(msg);
	total = 0, latency[0] = -1, latency[1] = 0;		//	total = 0, latency[0] = ULLONG_MAX, latency[1] = 0;
	sleep(1);	//	delay(1);
	pthread_mutex_unlock(&comms.active);
#if defined(__ARM__)
	/*
	 * FIXME: ClockCycles() is emulated on ARM with the value being updated
	 *        by the clock handler, so if we use ClockCycles() before we
	 *        invoke the clock handler we can get occasional bogus values.
	 *        As a workaround, add our handler _after_ the clock handler.
	 *        However, this means our latency measurments will not include
	 *        any scheduling latency due to time spent in the clock handler
	 */
	iid = AttachInterruptC(intnum, int_handler_interrupt);//, &comms, sizeof(struct area), _NTO_INTR_FLAGS_END);
#else
	iid = AttachInterruptC(intnum, int_handler_interrupt);//, &comms, sizeof(struct area), 0);
#endif
	//	assert(iid != -1);
	for (loop = limit; loop != 0; --loop) {
		//result = InterruptWait(0, NULL);	//	87
		clock_gettime(0, &pstart);	//	stop
		cycles = pstart.tv_sec*1000 + pstart.tv_nsec/1000000;	//	ClockCycles();
		snap = comms.cycles;
		//assert(result != -1);
		if (comms.state) {
			comms.state = 0;
		}
		else if (!comms.overrun) {
			comms.overrun = -1;
		}
		cycles -= snap;//, cycles >>= CLOCKCYCLES_INCR_BIT;
		total += cycles;
		if (cycles < latency[0])
			latency[0] = cycles;
		if (cycles > latency[1])
			latency[1] = cycles;
		if (comms.overrun != 0)
			break;
	}
	InterruptDetach(iid);
	comms.load = -comms.load;

	if (!comms.overrun) {
		sprintf(msg, "latency%s min = %3.3f usec, avg = %3.3f usec, max = %3.3f usec",
				(comms.load != 0) ? " (loaded)" : "", CYCLES2USEC(latency[0], cps), CYCLES2USEC(total / limit, cps), CYCLES2USEC(latency[1], cps));
		xprintf(msg);	//testpntpass(msg);
	}
	else {
		sprintf(msg, "latency = %d", (int)(CYCLES2USEC(latency[1], cps)));	//	sprintf(msg, "latency = %3.3f", CYCLES2USEC(latency[1], cps));
		xprintf(msg);	//testnote(msg);
		//testpntunres((comms.overrun > 0) ? "interrupt overrun occurrred (user handler incomplete before next tick?)" : "interrupt underrun occurred (spurious interrupt?)");
	}

	//testpntend();
	//teststop(argv[0]);
	return(0);
}
