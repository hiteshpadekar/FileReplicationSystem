#ifndef _FWK_QUEUE_H_
#define _FWK_QUEUE_H_

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "fwk_datatypes.h"

#define null ((NAP_VOID*)0)
#define QUEUE_SIZE 200

typedef struct _MSG_QUEUE{
    NAP_UINT32 send_count;
    NAP_UINT32 recv_count;
    NAP_VOID * messages[QUEUE_SIZE];
} MSG_QUEUE;


MSG_QUEUE * create_msg_queue(NAP_VOID);


NAP_VOID send_message(MSG_QUEUE * queue, NAP_VOID * message);


NAP_VOID * recv_message(MSG_QUEUE * queue);


NAP_VOID destroy(MSG_QUEUE * queue);

#endif
