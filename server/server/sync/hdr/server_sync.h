/*
 * File: server_sync.h
 * Author: Hitesh Padekar
 * email id: hitesh.padekar@sjsu.edu
 */

#ifndef __SERVER_SYNC_H__
#define __SERVER_SYNC_H__

#include "fwk_datatypes.h"
#include "dir_oper.h"

#define MAX_FILE_SIZE  4096

typedef enum _SYNC_MSG_TYPE{
   eSYNC_INVALID,
   eSYNC_CLIENT_LIST_REQUEST = 1,
   eSYNC_CLIENT_LIST_REPLY,
   eSYNC_DEL_FILE_LIST_REQUEST,
   eSYNC_DEL_FILE_LIST_REPLY = 4,
   eSYNC_FILE_REQUEST,
   eSYNC_FILE_REPLY,
   eSYNC_FILE_CREATE,
   eSYNC_FILE_MODIFY,
   eSYNC_FILE_DELETE,
   eSYNC_MAX_TYPE
}SYNC_MSG_TYPE;

typedef enum _E_SYNC_REQUEST_TYPE
{
   eREQ_INVALID,
   eREQ_FILE_GET    = 150,
   eREQ_FILE_PUSH,
   eREQ_FILE_DELETE,
   eREQ_MAX
}E_SYNC_REQUEST_TYPE;

typedef enum _E_SYNC_REPLY_TYPE
{
   eINVALID,
   eSTART    = 50,
   eDIRECTORY,
   eFILE,
   eDONE
}E_REPLY_TYPE;

typedef struct _SYNC_CLIENT_LIST_REPLY
{
   E_REPLY_TYPE eReplyType;
   NAP_UCHAR    name[SIZE_FILE_NAME];
   time_t       mtime;
}SYNC_CLIENT_LIST_REPLY;


typedef struct _SYNC_MESSAGE{
   NAP_UINT32   eMessageType;
   NAP_UINT32   uiDataLen;
   NAP_UINT32	AppData;  
   NAP_UINT32   eRequestType;
   NAP_UINT32   eReplyType;
   NAP_UCHAR    DirName[SIZE_FILE_NAME];
   NAP_UCHAR    FileName[SIZE_FILE_NAME];
   time_t       mtime;
}SYNC_MESSAGE;

typedef enum _E_MSG_Q_MESSAGE_TYPE
{
   eINVALID_MSG,
   eFILE_CREATE_MSG,
   eFILE_MODIFY_MSG,
   eFILE_DELETE_MSG,
   eMAX_MSG
}E_MSG_Q_MESSAGE_TYPE;

typedef struct _MSG_Q_MESSAGE{
   NAP_UINT32   eMsgType;
   NAP_UINT32   FileSize;
   NAP_UCHAR    FileName[SIZE_FILE_NAME];
   NAP_UCHAR    DirName[SIZE_FILE_NAME];
   NAP_VOID     *pvData;
}MSG_Q_MESSAGE;

#endif // __SERVER_SYNC_H__
