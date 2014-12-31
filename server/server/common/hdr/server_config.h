/*
 * File: server_config.h
 * Author: Hitesh Padekar
 * Email id: hitesh.padekar@sjsu.edu
 */
#ifndef _SERVER_CONFIG_H_
#define _SERVER_CONFIG_H_

#include <stdlib.h>
#include <pthread.h>
#include "fwk_datatypes.h"
#include "fwk_queue.h"

typedef enum _E_NAP_MODULES
{
   e_INVALID,
   e_ACCESS_NW,
   e_SYNC_PROC_MASTER,
   e_SYNC_PROC_SLAVE,
   e_CLIENT_SERVING,
   e_DB_HANDLING
}E_NAP_MODULES;

typedef struct _SYNC_CONFIG
{
  NAP_UCHAR   sync_ip_addr[128];
  NAP_UINT16  sync_port;
  NAP_UINT32  socketfd;
}SYNC_CONFIG;

# if 0
typedef struct _SYNC_SLAVE_CONFIG
{
  NAP_UCHAR   peer_sync_ip_addr[128];
  NAP_UINT32  peer_sync_port;
  NAP_UINT32  slave_socketfd;
}SYNC_SLAVE_CONFIG;
#endif

typedef struct _SYNC_PARAM
{
  SYNC_CONFIG    master_cfg;
  SYNC_CONFIG    slave_cfg;
}SYNC_PARAM;

typedef struct _SERVER_CONFIG
{
  NAP_BOOL    bCompression; // to Enable Compression set this value to NAP_TRUE, else its NAP_FALSE
  NAP_BOOL    bEncryption;  // to Enable Encryption set this value to NAP_TRUE, else its NAP_FALSE
  NAP_UCHAR   ip_addr[128];
  NAP_UINT16  port;
  NAP_UINT32  client_serv_fd;
  NAP_UCHAR   db_dir[256];
  pthread_t   sync_master_thrd_id;
  pthread_t   sync_slave_thrd_id;
  pthread_t   sync_proc_thrd_id;
  pthread_t   client_thrd_id;  
  SYNC_PARAM  sync;
}SERVER_CONFIG;


#endif //_SERVER_CONFIG_H_
