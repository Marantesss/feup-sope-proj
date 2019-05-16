#include "utils.h"

typedef struct node {
	tlv_request_t req;
	node_t *next;
} node_t;
 
typedef struct queue {
	node_t *front;
	node_t *last;
	unsigned int size;
} queue_t;
 
void init(queue_t *q);

int front(queue_t *q);

void pop(queue_t *q);

void push(queue_t *q, tlv_request_t req);
