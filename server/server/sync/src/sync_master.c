/*
 *
 * File: sync_master.c
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

struct ndir_name *glstDB = NULL;
struct nfil_name *glstDeleteFileList = NULL;
NAP_UINT32 guiDeletedListCount = 0;

NAP_UINT32 guiActiveClient = 0;

#define MAX_SYNC_SLAVES  50

NAP_BOOL sync_master_init_socket(NAP_CHAR *pcIpAddr/*NAP_UINT32  uiServIpAddr*/, 
		                 NAP_UINT16    sPort, 
				 NAP_UINT32  *piSockFd)
{
  
   NAP_UINT32    sockfd;
   NAP_UINT32    rc = -1;
   NAP_UINT32    flags = 0;
   struct sockaddr_in servaddr;
   printf("\n%s: Entering ", __FUNCTION__);
  
   sockfd = socket(AF_INET,SOCK_STREAM,0);

   if(sockfd < 0)
   {
      printf("\n%s: Master Socket creation failed", __FUNCTION__);
      return NAP_FAILURE;
   }

   /* Bind Server Port */
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr = inet_addr(pcIpAddr)/*uiServIpAddr*/;
   memset(&(servaddr.sin_zero),'\0', 8);
   servaddr.sin_port = htons(sPort);

   printf("\nsPort = %d htons(sPort) = %d",sPort, htons(sPort));
   printf("\nMaster Server is running on IP: %u, port : %u", servaddr.sin_addr.s_addr, 
				servaddr.sin_port);
   if( bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
      printf("\n%s: Bind failed ", __FUNCTION__);
      return NAP_FAILURE;
   }

   if( listen(sockfd, MAX_SYNC_SLAVES) < 0)
   {
      printf("\n%s: Listen failed ", __FUNCTION__);
      return NAP_FAILURE;
   }
    
   *piSockFd = sockfd;

   printf("\n%s: Exiting ", __FUNCTION__);
   return NAP_SUCCESS; 
}

extern NAP_VOID hex_dump(NAP_UCHAR *buff, NAP_UINT32 size);
/*
 * Sync thread function
 */
NAP_BOOL sync_send_reply(NAP_UINT32     uiSockDesc,
			SYNC_MESSAGE 	**psRetResp, 
			NAP_UINT32 	replyType, 
			NAP_UINT32 	replySubtype, 
			NAP_UINT32 	appData,
			NAP_UCHAR    	*DirName,
			NAP_UCHAR    	*FileName,
			time_t       	mtime)
{ 
   SYNC_MESSAGE 	*psResp = NAP_NULL;
   NAP_UINT32 		iResult = 0;
   
   psResp = (SYNC_MESSAGE*) malloc(sizeof(SYNC_MESSAGE));
   if (psResp == NAP_NULL)
   {
      printf("\n%s: Malloc failed at line : %d",__FUNCTION__,__LINE__);
   }
   memset(psResp, 0, sizeof(SYNC_MESSAGE));
   psResp->eMessageType = replyType;
   psResp->AppData      = appData;//guiActiveClient;
   psResp->eReplyType = replySubtype;
   if(DirName != NULL)
   	strcpy(psResp->DirName, DirName);
   if(FileName != NULL)
   	strcpy(psResp->FileName, FileName);
   psResp->mtime = mtime;
   
   printf("\n%s: \npsResp->eMessageType = %d, psResp->AppData = %d \npsResp->eReplyType = %d \npsResp->DirName = %s \npsResp->FileName = %s",__FUNCTION__, psResp->eMessageType, psResp->AppData, psResp->eReplyType, psResp->DirName, psResp->FileName);
   iResult = write(uiSockDesc, (NAP_VOID*)psResp, sizeof(SYNC_MESSAGE));
   if(iResult <= 0)
   {
      printf("\n%s Unable to send SYNC_CLIENT_LIST_REPLY", __FUNCTION__);
      return NAP_FAILURE;
   }
   else
   {
      printf("\n:%s: Sent Success", __FUNCTION__);
      *psRetResp = psResp;
      return NAP_SUCCESS;
   }
   return NAP_SUCCESS;
}

NAP_VOID sync_master_thread_handler(NAP_VOID *thread_hdl)
{
   NAP_UINT16 		sError = 0;
   struct in_addr 	sMasterIP;
   NAP_BOOL 		bStatus = NAP_FAILURE;
   NAP_UINT32 		iReturn = 0;
   NAP_UINT32 		iResult = 0;
   NAP_UINT32 		iSelect = 0;
   NAP_INT32 		iDesc  = 0;        /* Socket desc created using accept socket call*/
   NAP_INT32 		iParent  = 0;        /* Parent socket desc*/
   NAP_INT32 		iLen = sizeof(struct sockaddr_in);
   NAP_VOID 		*pvMessage = NAP_NULL;
   NAP_CHAR  		*buff;
   NAP_CHAR  		recv_len = 0;

   struct sockaddr_in 	stAddr;
   struct timeval 	timeout;
   SYNC_MESSAGE       	psRequest;// = NULL;
   SYNC_MESSAGE       	*psResp;// = NULL;

   DIR_NAME     *pTempDB = NULL;
   struct nfil_name 	*pTempFile = NULL;

   NAP_UINT32		msgSize = 0;
   NAP_UINT32		counter = 0;
   NAP_UINT32		DirCount = 0;
   NAP_UINT32		index = 0;
   NAP_CHAR		ch = 0;
   NAP_CHAR		file_data[MAX_FILE_SIZE];
   FILE			*filed;
   char temp_filepath[MAX_FILE_SIZE];

   struct nfil_name     *DelList = NULL;

   NAP_UINT32		len = 0;
   NAP_UINT32		ret = 0;
   memset(&timeout,0,sizeof(struct timeval));
   timeout.tv_sec = 0;
   timeout.tv_usec = 100;

   
   printf("\n%s: Entering ", __FUNCTION__);
  
   /*
    * Create Directory list
    */
   //print_ndir_name(&glstDB);

   /*
    * Create, bind, and listen Sync Master TCP Server socket
    */

   
   printf("\n%s: Calling Master init with ip = %d, port = %hu",__FUNCTION__, sMasterIP.s_addr,
                         g_server_config.sync.master_cfg.sync_port);

   bStatus = sync_master_init_socket(&g_server_config.sync.master_cfg.sync_ip_addr/*sMasterIP.s_addr*/, 
		         g_server_config.sync.master_cfg.sync_port,
		   	 &g_server_config.sync.master_cfg.socketfd);
  
   if(bStatus == NAP_SUCCESS)
   {
       printf("\n%s: Master Sock FD = %d",__FUNCTION__,g_server_config.sync.master_cfg.socketfd);
       printf("\n%s: Master Init Success",__FUNCTION__);
   }
   else
   {
       printf("\n%s: Master Init failed",__FUNCTION__);
       //return;
   }

NewClient:
   delete_list(&glstDB);
   directorylisting2levels(&glstDB);
   iDesc = accept(g_server_config.sync.master_cfg.socketfd, (struct sockaddr*)&stAddr, &iLen);
   if (iDesc <= 0)
   {
      printf("\n%s: Accept error",__FUNCTION__);
   }
   else
   {
      printf("\n%s: Accept success",__FUNCTION__);
   }

   while(1)
   {
       memset(&psRequest, 0, sizeof(SYNC_MESSAGE));
       recv_len = recv(iDesc, &psRequest, sizeof(SYNC_MESSAGE), MSG_WAITALL);
       if (recv_len <= 0)
       {
       	   printf("\n%s: Recv len = %d", __FUNCTION__,recv_len);
       	   printf("\n%s: Closing Descriptor = %d", __FUNCTION__,iDesc);
	   close(iDesc);
	   iDesc = 0;
	   goto NewClient;

       }
       else 
       {
	  printf("\n%s: Message type = %d",__FUNCTION__, psRequest.eMessageType);
	  switch (psRequest.eMessageType)
	  {
	     case eSYNC_CLIENT_LIST_REQUEST:
		/*
		 * Send Start of sync Dir message
		 */
#if 0
		printf("\n%s Sending eSYNC_CLIENT_LIST_REPLY, eSTART", __FUNCTION__);
		iResult = sync_send_reply(iDesc, &psResp, eSYNC_CLIENT_LIST_REPLY, eSTART, 1, NULL, NULL, 0);
		if(iResult == NAP_SUCCESS)
		{
		   printf("\n%s SENT eSYNC_CLIENT_LIST_REPLY, eSTART", __FUNCTION__);
		   free(psResp);
		}
		else
		   printf("\n%s ERROR:: eSYNC_CLIENT_LIST_REPLY, eSTART", __FUNCTION__);
     
#endif
		pTempDB = glstDB;
		while(pTempDB != NULL)
		{
		   /*
		    * Send Dir message
		    */
		   iResult = sync_send_reply(iDesc, 
					    &psResp, 
					    eSYNC_CLIENT_LIST_REPLY, 
					    eDIRECTORY, 
					    pTempDB->FileCount, 
					    pTempDB->dir_name, 
					    NULL,
					    0);
		   if(iResult == NAP_SUCCESS)
		   {
		      printf("\n%s SENT eSYNC_CLIENT_LIST_REPLY, eDIRECTORY", __FUNCTION__);
		      free(psResp);
		   }

		   pTempFile = pTempDB->filenext;

		   for (counter = 0; (counter < pTempDB->FileCount) && 
				   (pTempFile != NULL); counter++)
		   {
		      /*
		       * Send File message
		       */
		      iResult = sync_send_reply(iDesc, 
					    &psResp, 
					    eSYNC_CLIENT_LIST_REPLY, 
					    eFILE, 
					    0, 
					    NULL,
					    pTempFile->fil_name, 
					    pTempFile->mtime);
		      if(iResult == NAP_SUCCESS)
		      {
			 printf("\n%s SENT eSYNC_CLIENT_LIST_REPLY, eFILE", __FUNCTION__);
			 free(psResp);
		      }

		      pTempFile = pTempFile->next;
		   }

		   pTempDB = pTempDB->next;
		   DirCount++;
		}
#if 0		
		//hex_dump(&buff, psResp->uiDataLen);
		/*
		 * Send Done message
		 */
		iResult = sync_send_reply(iDesc, 
					    &psResp, 
					    eSYNC_CLIENT_LIST_REPLY, 
					    eSTART, 
					    index, 
					    NULL, 
					    NULL,
					    0);
		if(iResult == NAP_SUCCESS)
		{
		   printf("\n%s SENT eSYNC_CLIENT_LIST_REPLY, eSTART", __FUNCTION__);
		   free(psResp);
		}
#endif
		printf("\n%s Sent Dir List", __FUNCTION__);
		
		break;
	     case eSYNC_DEL_FILE_LIST_REQUEST:
		   /*
		    * Send deleted File List reply
		    */
		  printf("\n%s: Received DELETED FILE LIST request",__FUNCTION__);
		  iResult = sync_send_reply(iDesc, 
					&psResp, 
					eSYNC_DEL_FILE_LIST_REPLY, 
					eSTART, 
					guiDeletedListCount, 
					"DB", 
					NULL,
					0);
		   if(iResult == NAP_SUCCESS)
		   {
		      printf("\n%s SENT eSYNC_DEL_FILE_LIST_REPLY, eFILE", __FUNCTION__);
		      free(psResp);
		   }

		  DelList = glstDeleteFileList;

		  while(DelList)
		  {
		      iResult = sync_send_reply(iDesc, 
					&psResp, 
					eSYNC_DEL_FILE_LIST_REPLY, 
					eFILE, 
					0, 
					"DB", 
					DelList->fil_name,
					0);
		       if(iResult == NAP_SUCCESS)
		       {
			  printf("\n%s SENT eSYNC_DEL_FILE_LIST_REPLY, eFILE", __FUNCTION__);
			  free(psResp);
		       }
			DelList = DelList->next;
		   }
                   delete_list(&glstDeleteFileList);
		   guiDeletedListCount = 0;

		break;
	     case eSYNC_FILE_REQUEST:
		  printf("\n%s: Received FILE request",__FUNCTION__);
		  switch(psRequest.eRequestType)
		  {
		      case eREQ_FILE_GET:
			   printf("\n%s: Received FILE GET request",__FUNCTION__);
			   memset(temp_filepath, 0, sizeof(temp_filepath));
			   strcpy(temp_filepath, g_server_config.db_dir);//DIR_SERVER_PATH);
                           strcat(temp_filepath, psRequest.DirName);
                           strcat(temp_filepath, "/");
                           strcat(temp_filepath, psRequest.FileName);
			   
			   memset(file_data, 0, sizeof(file_data));
			   index =0;
			   printf("\n%s: File PAth = %s", __FUNCTION__,temp_filepath);
			   /*
			   filed = fopen(temp_filepath, "r");
			   if(filed ==NULL)
			       printf("\n%s: ERROR in file open for File PAth = %s", __FUNCTION__, temp_filepath);
			       
			   ch = getc(filed);///reading char by char from a file to buff
			   while(ch != EOF)
			   {	
				file_data[index] = ch;
				index++;
				if(index >MAX_FILE_SIZE)
				{
				    printf("increase the size of buffer for file copy /n");
				    break;
				}
			        ch = getc(filed);
			   } 
			   fclose(filed);
				*/
			   filed = fopen(temp_filepath, "r");
			   /* calculate file size */
			   if(filed ==NULL)
			       printf("\n%s: ERROR in file open for File PAth = %s", __FUNCTION__, temp_filepath);
			   else {
				fseek(filed, 0, SEEK_END);
			   	index = ftell(filed);
			   	fseek(filed, 0, SEEK_SET);
			   }
			   iResult = sync_send_reply(iDesc, 
						    &psResp, 
						    eSYNC_FILE_REPLY, 
						    eSTART, 
						    index, 
						    psRequest.DirName, 
						    psRequest.FileName,
						    0);
			   if(iResult == NAP_SUCCESS)
			   {
			       printf("\n%s SENT eSYNC_FILE_REPLY, eDONE", __FUNCTION__);
			       free(psResp);
		 	   }

			   printf("\n%s Sent File size %d", __FUNCTION__, index);
					       
			   do {
				ret = fread(file_data, sizeof(char), sizeof(file_data), filed);
				len += ret;
				if(ret > 0) {
				    ret = write(iDesc, file_data, ret);
				}
			    } while(len < index);
			   fclose(filed);
			   /*iResult = write(iDesc, (NAP_VOID*)&file_data, index);
			   if(iResult <= 0)
			   {
			      printf("\n%s Unable to send SYNC_CLIENT_LIST_REPLY", __FUNCTION__);
			   }
			   else
			   {
			      printf("\n:%s: Sent Success ", __FUNCTION__);
			   }
		     */
			  break;
		      case eREQ_FILE_PUSH:

			   printf("\n%s: Received FILE PUSH request",__FUNCTION__);
			   /*
			    * TO DO Receive read and store file
			    */
			   memset(file_data, 0, sizeof(file_data));
			   memset(temp_filepath, 0, sizeof(temp_filepath));
			   strcpy(temp_filepath, g_server_config.db_dir);//DIR_SERVER_PATH);
                           strcat(temp_filepath, psRequest.DirName);
                           strcat(temp_filepath, "/");
                           strcat(temp_filepath, psRequest.FileName);

			   printf("\n%s: File PAth = %s", __FUNCTION__,temp_filepath);
			   printf("\n%s Waiting for reply Line:", __FUNCTION__, __LINE__);
			   recv_len = read(iDesc, &file_data, psRequest.AppData);
			   if (recv_len <= 0)
			   {
			       printf("\n%s: Recv len = %d", __FUNCTION__,recv_len);
			       printf("\n%s: Closing Descriptor = %d", __FUNCTION__,iDesc);
			       //close(iDesc);
			       //iDesc = 0;
			       //goto NewClient;

			   }
			   filed = fopen(temp_filepath, "w");// O_WRONLY);
			   if (filed !=NULL){
				fwrite(file_data, sizeof(char), recv_len, filed);
			   	//fputs(file_data, filed);
			   	fclose(filed);
			   }
			  break;
		      default:
			  printf("\n%s: ERROR :: Invalid FILE REQUEST message received",__FUNCTION__);
			  break;
		  }
		break;

	     default:
		  printf("\n%s: ERROR :: Invalid message received",__FUNCTION__);
		break;
		   
	  }
       }
     /*
      * For UT
      */
   //insert_nfil_name(&(glstDeleteFileList), "hitesh.txt", 0);
   //guiDeletedListCount++;
   }


   printf("\n%s: Exiting ", __FUNCTION__);
  return;
}

