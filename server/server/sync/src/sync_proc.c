/*
 *
 * File: sync_proc.c
 * Author: Hitesh Padekar
 * Email id: hitesh.padekar@sjsu.edu
 */

#include "fwk_datatypes.h"
#include "server_config.h"
#include <sys/socket.h>
#include <netinet/ip.h>
#include <fcntl.h>
#include "access_nw.h"
#include <signal.h>
#include <sys/time.h>

#include "server_sync.h"
#include "dir_oper.h"

extern SERVER_CONFIG g_server_config;
MSG_QUEUE *sync_proc_msg_q = NAP_NULL;

extern struct ndir_name *glstDB;
//NAP_UINT32 giMasterSocket = 0;

extern NAP_UINT32 guiActiveClient ;

NAP_BOOL get_filename_from_file_path(NAP_CHAR *pcFilePath, 
					NAP_CHAR *pcFileName, 
					NAP_CHAR *pcClientID)
{
    NAP_UINT32   uiDBpathLen = 0;
    NAP_CHAR     *ClientID = NULL;
    NAP_CHAR     *FileName = NULL;

    if((pcFilePath == NULL))
	return NAP_FAILURE;
    uiDBpathLen = strlen(g_server_config.db_dir);

    FileName = strstr((pcFilePath + uiDBpathLen), "/");
    strcpy(pcFileName, FileName + 1);
    memcpy(pcClientID, (pcFilePath + uiDBpathLen), FileName - (pcFilePath + uiDBpathLen));
    pcClientID[FileName - (pcFilePath + uiDBpathLen)] = '\0';

    return NAP_SUCCESS;
    
}


NAP_VOID sync_proc_thread_handler(NAP_VOID *thread_hdl)
{
    MSG_Q_MESSAGE *pMessage = NAP_NULL;
    SYNC_MESSAGE       psRequest;
    NAP_UINT32 iResult = 0;

    /*
    * INIT Sync thread
    */
    // create message queue
    sync_proc_msg_q = create_msg_queue();
    if (sync_proc_msg_q == NAP_NULL)
    {
	printf("\n%s: Message queue creation failed");
    }
    printf("\n%s: going for message read",__FUNCTION__);
    while(1)
    {
	pMessage = (MSG_Q_MESSAGE *)recv_message(sync_proc_msg_q);  
	printf("\n%s: Message received",__FUNCTION__);
	if (pMessage == NAP_NULL)
	{
	    printf("\n%s: Invalid message",__FUNCTION__);
	    pthread_yield();
	    continue;
	}
	switch(pMessage->eMsgType)
	{
	    case eFILE_CREATE_MSG:

		printf("\n%s received SYNC_FILE_REQUEST", __FUNCTION__);
		memset(&psRequest, 0, sizeof(SYNC_MESSAGE));
		psRequest.eMessageType      = eSYNC_FILE_CREATE;
		psRequest.AppData           = pMessage->FileSize;
		psRequest.eRequestType      = eREQ_FILE_PUSH;
		get_filename_from_file_path(pMessage->FileName, &psRequest.FileName, &psRequest.DirName);
		printf("\n FileName: %s  ClientID: %s", psRequest.FileName, psRequest.DirName);
		//strcpy(psRequest.FileName, pMessage->FileName);
		//strcpy(psRequest.DirName, pMessage->DirName);

		iResult = write(g_server_config.sync.slave_cfg.socketfd, 
				&psRequest, 
				sizeof(SYNC_MESSAGE));
		if(iResult <= 0)
		{
		  printf("\n%s: ERROR Unable to send SYNC_FILE_REQUEST", __FUNCTION__);
		}
		printf("\n%s received SYNC_FILE_REQUEST", __FUNCTION__);

		break;
	    case eFILE_MODIFY_MSG:

		printf("\n%s received FILE_MODIFY_MSG", __FUNCTION__);
		break;
	    case eFILE_DELETE_MSG:
		
		printf("\n%s received FILE_DELETE_MSG", __FUNCTION__);
		break;
	    default:

		printf("\n%s INVALID Message", __FUNCTION__);
		break;
	}
	/* Do processing */
	pthread_yield();
    }
}
