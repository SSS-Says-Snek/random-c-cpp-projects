#include "astar/utils.h"

typedef struct {
    int size;
    int max_size;
    AlgoQueueInfo *queue;
} PriorityQueue;

int PriorityQueue_new(PriorityQueue *queue);
void PriorityQueue_destroy(PriorityQueue *queue);

int PriorityQueue_empty(PriorityQueue queue);

int PriorityQueue_put(PriorityQueue *queue, AlgoQueueInfo algo_queue_info);
int PriorityQueue_pop(PriorityQueue *queue, AlgoQueueInfo *algo_queue_info);
