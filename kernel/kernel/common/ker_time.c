/*
 * time.c
 *
 *  Created on: 2022-9-5
 *      Author: Test
 */
#include <types.h>
#include "../include/ker_time.h"
#include "kernel.h"

extern volatile size_t gl_timer_tick;
extern struct tm g_initBiosTime;

size_t get_systime(systime_t* p_systime)
{
  size_t cycles = 0;
  size_t ticks = getTicks();

  if (g_syspage_ptr->systime.get_cycles)
  {
	cycles = g_syspage_ptr->systime.get_cycles(&g_syspage_ptr->systime);
	if (getTicks() > ticks)
		cycles = g_syspage_ptr->systime.get_cycles(&g_syspage_ptr->systime);
  }
  if (p_systime)
  {
	p_systime->cycles = cycles;
	p_systime->ticks = ticks;
  }
  return ticks;
}

size_t add_systime(const systime_t* p_systime1, const systime_t* p_systime2, systime_t* p_sum)
{
	systime_t systime1;

	if (!p_systime1)
	{
		get_systime(&systime1);
		p_systime1 = &systime1;
	}

	size_t cycles = p_systime1->cycles;
	size_t ticks = p_systime1->ticks;

	if (p_systime2)
	{
		size_t cycles_per_time = g_syspage_ptr->systime.cycles_per_sec / g_syspage_ptr->systime.timer_freq;

		cycles += p_systime2->cycles;
		ticks += p_systime2->ticks;
		if (cycles >= cycles_per_time)
		{
			ticks++;
			cycles -= cycles_per_time;
		}
	}

	if (p_sum)
	{
		p_sum->ticks = ticks;
		p_sum->cycles = cycles;
	}

	return ticks;
}

size_t sub_systime(const systime_t* p_systime1, const systime_t* p_systime2, systime_t* p_diff)
{
	systime_t systime1;

	if (!p_systime1)
	{
		get_systime(&systime1);
		p_systime1 = &systime1;
	}

	size_t cycles = p_systime1->cycles;
	size_t ticks = p_systime1->ticks;

	if (p_systime2)
	{
		size_t cycles_per_time = g_syspage_ptr->systime.cycles_per_sec / g_syspage_ptr->systime.timer_freq;

		cycles -= p_systime2->cycles;
		ticks -= p_systime2->ticks;
		if (cycles >= cycles_per_time)
		{
			ticks--;
			cycles += cycles_per_time;
		}
	}

	if (p_diff)
	{
		p_diff->ticks = ticks;
		p_diff->cycles = cycles;
	}

	return ticks;
}

size_t systime_to_us(const systime_t* p_systime)
{
	if (!p_systime)	
		return 0;

	size_t us = p_systime->ticks * (1000000 / g_syspage_ptr->systime.timer_freq);

	if (p_systime->cycles)
	{
		size_t cycles_per_time = g_syspage_ptr->systime.cycles_per_sec / g_syspage_ptr->systime.timer_freq;

		us += p_systime->cycles * (1000000 / g_syspage_ptr->systime.timer_freq) / cycles_per_time;
	}

	return us;
}

size_t systime_to_timespec(const systime_t* p_systime, struct timespec* ts)
{
	if (!p_systime)	
		return 0;

	size_t tv_sec = p_systime->ticks / g_syspage_ptr->systime.timer_freq;
	size_t tv_nsec = (p_systime->ticks % g_syspage_ptr->systime.timer_freq) * (1000000000 / g_syspage_ptr->systime.timer_freq);

	if (p_systime->cycles)
	{
		size_t cycles_per_time = g_syspage_ptr->systime.cycles_per_sec / g_syspage_ptr->systime.timer_freq;

		tv_nsec += p_systime->cycles * (1000000000 / g_syspage_ptr->systime.timer_freq) / cycles_per_time;
		if (tv_nsec >= 1000000000)
		{
			tv_nsec -= 1000000000;
			tv_sec++;
		}
	}

	if (ts)
	{
		ts->tv_sec = tv_sec;
		ts->tv_nsec = tv_nsec;
	}

	return tv_sec;
}

size_t get_sys_timespec(struct timespec* ts)
{
	systime_t systime;

	get_systime(&systime);

	return systime_to_timespec(&systime, ts);
}

int ker_clock_gettime( clockid_t clock_id, struct timespec* tp, struct tm* pInitBiosTime ){
	get_sys_timespec(tp);
	if(pInitBiosTime!=NULL) {
		pInitBiosTime->tm_sec	= g_initBiosTime.tm_sec;
		pInitBiosTime->tm_min	= g_initBiosTime.tm_min;
		pInitBiosTime->tm_hour	= g_initBiosTime.tm_hour;
		pInitBiosTime->tm_mday	= g_initBiosTime.tm_mday;
		pInitBiosTime->tm_mon	= g_initBiosTime.tm_mon;
		pInitBiosTime->tm_year	= g_initBiosTime.tm_year;
		pInitBiosTime->tm_wday	= g_initBiosTime.tm_wday;
		pInitBiosTime->tm_yday	= g_initBiosTime.tm_yday;
		pInitBiosTime->tm_isdst	= g_initBiosTime.tm_isdst;
		pInitBiosTime->tm_gmtoff= g_initBiosTime.tm_gmtoff;
		pInitBiosTime->tm_zone	= g_initBiosTime.tm_zone;
	}
	return EOK;
}

int ker_vdso_gettime(clockid_t clock_id, struct timespec * tp)
{
	get_sys_timespec(tp);
	return EOK;
}
