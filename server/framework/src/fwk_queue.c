#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

#include "fwk_queue.h"

inline NAP_UINT32 isfull(MSG_QUEUE * queue) {
    return (queue->send_count+1)%QUEUE_SIZE == queue->recv_count;
}

inline NAP_UINT32 isempty(MSG_QUEUE * queue) {
    return queue->send_count == queue->recv_count;
}

MSG_QUEUE * create_msg_queue(NAP_VOID) {
    MSG_QUEUE * queue = (MSG_QUEUE*)malloc(sizeof(MSG_QUEUE));
    queue->send_count = 0;
    queue->recv_count = 0;
}

NAP_VOID send_message(MSG_QUEUE * queue, NAP_VOID * message) {
    while (isfull(queue));
    NAP_UINT32 next = (queue->send_count+1) % QUEUE_SIZE;
    queue->messages[next] = message;
    queue->send_count = next;
}

NAP_VOID * recv_message(MSG_QUEUE * queue) {
    while (isempty(queue));
    NAP_VOID * message = queue->messages[queue->send_count];
    queue->recv_count = (queue->recv_count+1) % QUEUE_SIZE;
    return message;
}

NAP_VOID destroy(MSG_QUEUE * queue) {
    while (!isempty(queue));
    free(queue);
}
