/** \file vdso.h
  *
  * Copyright 2022, KunyuOS Software Systems. All Rights Reserved.
  *
  * @author   	WangYang
  * @email   	yang.wang@kunyuos.com
  * @version	V1.0
  * @date    	2022-09-20
  * @brief   	vdso define & interface
  *
  * Version|Auther|Date|Describe
  * ------|----|------|--------
  * V1.0|WangYang|2022-09-28|Create File
  */
#ifndef __U_RESMGR_H__
#define __U_RESMGR_H__

#include <platform/include/msg.h>

#define _PULSE_TYPE				0
#define _PULSE_SUBTYPE			0


 struct _pulse {
 	int16_t						code;
 	int16_t					subtype;	//	Whether in need
 	int32_t						value;
 	int32_t						scoid;
 };

 struct sigevent{
	 uint16_t			notify;
	 uint16_t			priority;	//	uint16_t
	 int32_t			coid;
	 uint16_t			type;
	 int16_t			code;
	 int32_t			value;
 };
#define SIGEV_NONE				0	/* notify */
#define SIGEV_SIGNAL			1	/* notify, signo, value */
#define SIGEV_SIGNAL_CODE		2	/* notify, signo, value, code */
#define SIGEV_SIGNAL_THREAD		3	/* notify, signo, value, code */
#define SIGEV_PULSE				4	/* notify, coid, priority, code, value */
#define SIGEV_UNBLOCK			5	/* notify */
#define SIGEV_INTR				6	/* notify */
#define SIGEV_THREAD			7	/* notify, notify_function, notify_attributes */

#define SIGEV_PULSE_INIT(__e, __f, __p, __c, __v)	(\
 	(__e)->notify = SIGEV_PULSE, \
 	(__e)->coid = (__f), \
 	(__e)->priority = (__p), \
	(__e)->value = (__v), \
	(__e)->code = (__c))

#define SIGEV_INTR_INIT(__e) (	\
	(__e)->notify = SIGEV_INTR)

 typedef struct _msg_payload_item
 {
 	union{
 		struct{
 			uint32_t nPulseCode;
 			uint32_t nPulseValue;
 			uint32_t nPulsePriority;
 		}pulse;
 		struct{
 			iov_t* siov;	//	const iov_t* siov;
 			int sparts;
 			iov_t* riov;	//	const iov_t* riov;
 			int rparts;
 		}msg;
 	}payload;
 } MSG_PAYLOAD_ITEM;

#endif
