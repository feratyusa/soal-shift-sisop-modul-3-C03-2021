#include<stdio.h>
#include <stdlib.h>
#include<stdbool.h>

// Queue Implementation
typedef struct QueueNode{
	int data;
	struct QueueNode *next;
}Qnode;

typedef struct Queue{
	Qnode *front, *rear;
	unsigned size;
}Queue;

void qinit(Queue *q);
bool q_isEmpty(Queue *q);
void qpush(Queue *q, int value);
void qpop(Queue *q);
int qfront(Queue *q);

void qinit(Queue *q){
	q->size = 0;
	q->front = NULL;
	q->rear = NULL;
}

bool q_isEmpty(Queue *q){
	return (q->front == NULL && q->rear == NULL);
}

void qpush(Queue *q, int value){
	Qnode *newNode = (Qnode*) malloc(sizeof(Qnode));
	if(newNode){
		q->size++;
		newNode->data = value;
		newNode->next = NULL;
		if(q_isEmpty(q)){
			q->front = newNode;
			q->rear = newNode;
		}
		else{
			q->rear->next = newNode;
			q->rear = newNode;
		}
	}
}

void qpop(Queue *q){
	if(!q_isEmpty(q)){
		Qnode *temp = q->front;
		q->front = q->front->next;
		free(temp);

		if(q->front == NULL){
			q->rear = NULL;
		}
		q->size--;
	}
}

int qfront(Queue *q){
	if(!q_isEmpty(q)){
		return (q->front->data);
	}
	return (int)0;
}
// End of Queue Implementation

int main(){
	Queue myQ;
	qinit(&myQ);

	qpush(&myQ, 7);
	qpush(&myQ, 5);
	printf("%d\n", qfront(&myQ));
	qpop(&myQ);
	qpush(&myQ, 7);
	printf("%d\n", qfront(&myQ));
	qpop(&myQ);
	qpush(&myQ, 5);
	printf("%d\n", qfront(&myQ));
}
