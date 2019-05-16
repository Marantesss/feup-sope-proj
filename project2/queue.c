#include "queue.h"

void init(queue_t *q) {
	q->front = NULL;
	q->last = NULL;
	q->size = 0;
}
 
tlv_request_t front(queue_t *q) {
	return q->front->req;
}
 
void pop(queue_t *q) {
	q->size--;
 
	node_t *tmp = q->front;
	q->front = q->front->next;
	free(tmp);
}
 
void push(queue_t *q, tlv_request_t req) {
	q->size++;
 
	if (q->front == NULL) {
		q->front = (struct Node *) malloc(sizeof(node_t));
		q->front->req = req;
		q->front->next = NULL;
		q->last = q->front;
	} else {
		q->last->next = (struct Node *) malloc(sizeof(node_t));
		q->last->next->req = req;
		q->last->next->next = NULL;
		q->last = q->last->next;
	}
}

int empty(queue_t *q) {
    return q->size == 0;
}
