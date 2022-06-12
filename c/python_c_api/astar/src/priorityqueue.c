#include "astar/priorityqueue.h"

int PriorityQueue_new(PriorityQueue *queue) {
    PriorityQueue new_queue = {0, 1, malloc(1 * sizeof(AlgoQueueInfo))};

    if (!new_queue.queue) {
        return 1;
    }
    *queue = new_queue;

    return 0;
}

void PriorityQueue_destroy(PriorityQueue *queue) {
    free(queue->queue);
    queue->queue = NULL;
}

int PriorityQueue_empty(PriorityQueue queue) {
    if (queue.size == 0) {
        return 1;
    }
    return 0;
}

int PriorityQueue_put(PriorityQueue *queue, AlgoQueueInfo algo_queue_info) {
    if (!queue->queue) {
        return 1;
    }

    if (queue->size + 1 > queue->max_size) {
        queue->max_size *= 2;

        queue->queue = realloc(queue->queue, queue->max_size * sizeof(AlgoQueueInfo));
        if (!queue->queue) {
            return 2;
        }
    }
    
    int i = queue->size - 1;
    int f_score = algo_queue_info.f_score;
    int count = algo_queue_info.count;

    while (i >= 0 && (f_score >= queue->queue[i].f_score || f_score == -1)) {
        if (f_score == queue->queue[i].f_score && count < queue->queue[i].count) {
            break;
        }

        queue->queue[i + 1] = queue->queue[i];
        i--;
    }

    queue->queue[i + 1] = algo_queue_info;
    queue->size++;

    return 0;
}

int PriorityQueue_pop(PriorityQueue *queue, AlgoQueueInfo *algo_queue_info) {
    if (!queue->queue) {
        return 1;
    }

    if (queue->size == 0) {
        return 2;
    }

    *algo_queue_info = queue->queue[queue->size - 1];

    queue->size--;

    // Might remove this
    /*if (queue->size > 0 && queue->max_size / queue->size == 2) {
        queue->queue = realloc(queue->queue, queue->size * sizeof(AlgoQueueInfo));
        queue->max_size /= 2;
    }*/

    return 0;
}