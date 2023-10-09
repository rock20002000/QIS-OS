#ifndef CIRCULAR_QUEUE_H_
#define CIRCULAR_QUEUE_H_

#include <platform/include/platform.h>
#include <libc/include/basic_file.h>
#include <platform/include/resmgr.h>

#define LENGTH 201
#define QUEUE_STAT_NOK 0
#define QUEUE_STAT_OK 1

typedef struct {
    char base[LENGTH];
    unsigned int front;
    unsigned int rear;
} CirQueue;

bool IsEmpty (CirQueue * cq);
bool IsFull (CirQueue * cq);
void InitQueue (CirQueue * cq);
unsigned int CalculateLength (CirQueue * cq);
int EnQueue (CirQueue * cq, char elem);
int DeQueue (CirQueue * cq, char * elem);

#endif /* CIRCULAR_QUEUE_H_ */
