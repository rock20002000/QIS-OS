#include "circular_queue.h"

void InitQueue (CirQueue * cq) {
    cq->front = 0;
    cq->rear = 0;
}

bool IsEmpty (CirQueue * cq){
    if (cq->front == cq->rear)
        return true;
    else
        return false;
}

bool IsFull (CirQueue * cq){
    if ((cq->rear + 1)%LENGTH == (cq->front)%LENGTH)
        return true;
    else
        return false;
}

unsigned int CalculateLength (CirQueue * cq){
    return (LENGTH + cq->rear - cq->front)%LENGTH;
}

int EnQueue (CirQueue * cq, char elem){
    int stat;
	if (IsFull(cq)) {
		perror("The circular queue is full. Cannot enqueue anymore!\n"); // fprintf(stderr, "The circular queue is full. " "Cannot enqueue anymore!\n");
		stat = QUEUE_STAT_NOK;
	} else {
	cq->base[cq->rear] = elem;
	cq->rear = (cq->rear + 1) % LENGTH;
    stat = QUEUE_STAT_OK;
	}
    return stat;
}

int DeQueue (CirQueue * cq, char * ptrElem){
    int stat;
	if (IsEmpty(cq)) {
		perror("The circular queue is empty. Cannot dequeue anymore!\n");	//	fprintf(stderr, "The circular queue is empty. "	"Cannot dequeue anymore!\n");
		stat = QUEUE_STAT_NOK;
	} else {
		printf_s("Dequeue the front element: %c\n", cq->base[cq->front]);
        *ptrElem = cq->base[cq->front];
        cq->front = (cq->front + 1) % LENGTH;
        stat = QUEUE_STAT_OK;
	}
    return stat;
}
