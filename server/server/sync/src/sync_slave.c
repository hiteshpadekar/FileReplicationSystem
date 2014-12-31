/*
 * File: sync_slave.c
 * Author: Hitesh Padekar
 * Email: hitesh.padekar@sjsu.edu
 */

#include "fwk_datatypes.h"
#include "server_config.h"
#include <sys/socket.h>
#include <netinet/ip.h>
#include <fcntl.h>
#include "access_nw.h"
#include <signal.h>
#include <sys/time.h>
#include <netdb.h>
#include "server_sync.h"
#include "dir_oper.h"
extern int h_errno;

extern SERVER_CONFIG g_server_config;
extern struct ndir_name *glstDB;

extern struct nfil_name *glstDeleteFileList;
extern NAP_UINT32 guiDeletedListCount;


/*
 * Timer function handler
 */
NAP_VOID server_sync_slave_timer_handler(NAP_UINT32 signum)
{

   NAP_UINT16 sError = 0;
   NAP_BOOL bStatus = NAP_FAILURE;
   NAP_UINT32 iReturn = 0;
   struct in_addr sSlaveIP;
   printf("\n%s: Entering ", __FUNCTION__);
   printf("\n%s: Exiting ", __FUNCTION__);
}

/*
 * Connect with peer Server
 */

NAP_BOOL sync_slave_connect_peer(NAP_CHAR *pcIpAddr, 
		                 NAP_UINT16    sPort, 
				 NAP_UINT32  *piSockFd)
{
  NAP_UINT32    sockfd;
  NAP_UINT32    rc = -1;
  NAP_UINT32    flags = 0;
  struct sockaddr_in servaddr;
  struct sockaddr_in localAddr;
  struct hostent *he;

  printf("\n%s: Entering ", __FUNCTION__);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  bzero(&servaddr,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(pcIpAddr);
  memset(&(servaddr.sin_zero),'\0',8);
  servaddr.sin_port = htons(sPort);

  /* bind any port number */
  localAddr.sin_family = AF_INET;
  localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  localAddr.sin_port = htons(0);

  rc = bind(sockfd, (struct sockaddr *) &localAddr, sizeof(localAddr));
  if(rc<0) {
    printf("%s: cannot bind port TCP %u\n",__FUNCTION__,sPort);
  }

  printf("\nConnect with peer ip %d port %hu", servaddr.sin_addr.s_addr, servaddr.sin_port);
  rc = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
  if(rc == -1)
  {
     printf("\nConnect with peer failed");
     close(sockfd);
     return NAP_FAILURE;
  }
  g_server_config.sync.slave_cfg.socketfd = sockfd;
  printf("\nConnect with peer Success");
  printf("\n%s: Exiting ", __FUNCTION__);

  return NAP_SUCCESS;

}

NAP_VOID hex_dump(NAP_UCHAR *buff, NAP_UINT32 size)
{
   NAP_UINT32 counter = 0;
   printf("\n HEX dump:\n***********************************\n");
   for(counter = 0; counter < size; counter++)
      printf("%c", buff[counter]);
   printf("\n***********************************");

   return;
}

NAP_BOOL sync_recv_message(NAP_UINT32     uiSockDesc,
		           SYNC_MESSAGE    **psRetResp)
{
   SYNC_MESSAGE         *psResp = NAP_NULL;
   NAP_UINT32           recv_len = 0;

   psResp = (SYNC_MESSAGE*) malloc(sizeof(SYNC_MESSAGE));
   if (psResp == NAP_NULL)
   {
       printf("\n%s: Malloc failed at line : %d",__FUNCTION__,__LINE__);
   }
   memset(psResp, 0, sizeof(SYNC_MESSAGE));

   recv_len = read(uiSockDesc, psResp, sizeof(SYNC_MESSAGE));
   if (recv_len <= 0)
   {
      printf("\n%s: Read failed:", __FUNCTION__);
      close(uiSockDesc);
      return NAP_FAILURE;
   }
   else
   {
      printf("\n%s: Read Sucess recv_len = %d",__FUNCTION__, recv_len);
      *psRetResp = psResp;
      return NAP_SUCCESS;
   }
   return NAP_SUCCESS;
}

NAP_VOID sync_slave_thread_handler(NAP_VOID *pvData)
{
   NAP_BOOL bStatus = NAP_FAILURE;
   NAP_UINT32 iReturn = 0;
   NAP_UINT32 iResult = 0;
   NAP_UINT16 iPort = 0;
 
   struct in_addr     sSlaveIP;
   struct sigaction   sa;
   struct itimerval   timer;
   NAP_VOID           *pvMessage = NAP_NULL;

   NAP_VOID*          pvChar = NAP_NULL;
   SYNC_MESSAGE       psRequest;
   SYNC_MESSAGE       *psResp = NAP_NULL;
   NAP_UINT32	      uiSockId = 0;
   NAP_UINT32	      recv_len = 0;

   NAP_UINT32		msgSize = 0;
   NAP_CHAR  		*buff;
   NAP_UINT32           index = 0;
   NAP_CHAR             ch = 0;

   NAP_UINT32		counter = 0;
   NAP_UINT32		DirCount = 0;
   NAP_UINT32		FileSize = 0;
   NAP_UINT32		FileCnt = 0;
   NAP_UINT32		FileCount = 0;
   char 		dir_name[SIZE_FILE_NAME];
   char 		file_name[SIZE_FILE_NAME];
   struct ndir_name     *pLstPeerDir = NULL;
   struct ndir_name     *MasterDirList = NULL;
   struct nfil_name     *MasterFileList = NULL;
   struct ndir_name     *SlaveDirNode = NULL;
   struct nfil_name     *SlaveFileNode = NULL;
   struct ndir_name     *GetList = NULL;
   struct ndir_name     *PushList = NULL;
   struct ndir_name     *DeleteList = NULL;
   struct ndir_name     *PushListDirPtr = NULL;
   struct ndir_name     *GetListDirPtr = NULL;
   struct nfil_name     *DelFileNode = NULL;
   NAP_CHAR		file_data[MAX_FILE_SIZE];
   FILE			*filed;
   NAP_CHAR 		temp_filepath[MAX_FILE_SIZE];
   NAP_UINT32           len = 0;
   NAP_UINT32           ret = 0;

   printf("\n%s: Entering ", __FUNCTION__);
     
   /*
    * INIT Sync thread
    */
   /*
    * Connect with socket
    */
    int s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
     if (s != 0)
	printf("pthread_setcancelstate");

RECONNECT:

   bStatus = sync_slave_connect_peer(&g_server_config.sync.slave_cfg.sync_ip_addr , 
		         g_server_config.sync.slave_cfg.sync_port,
		   	 &g_server_config.sync.slave_cfg.socketfd);
  
   if(bStatus == NAP_SUCCESS)
   {
       printf("\n%s: Slave Init Success",__FUNCTION__);
       uiSockId = g_server_config.sync.slave_cfg.socketfd;
   }
   else
   {
       printf("\n%s: Slave Init failed",__FUNCTION__);
       goto EXIT_DELAY;
   }
   
   delete_list(&GetList);
   delete_list(&PushList);
   delete_list(&glstDB);
   delete_list(&pLstPeerDir);
   directorylisting2levels(&glstDB);

   /*
    * Send Get request for the list of files that are deleted when this server was down
    */
   memset(&psRequest, 0, sizeof(SYNC_MESSAGE));
   psRequest.eMessageType = eSYNC_DEL_FILE_LIST_REQUEST;
   psRequest.AppData    = 0;

   iResult = write(uiSockId, &psRequest, sizeof(SYNC_MESSAGE));
   if(iResult <= 0)
   {
      printf("\n%s: ERROR Unable to send eSYNC_DEL_FILE_LIST_REQUEST", __FUNCTION__);
      close(uiSockId); 
      goto EXIT_DELAY;
   
   }
   printf("\n%s Sent eSYNC_DEL_FILE_LIST_REQUEST", __FUNCTION__);
   
   if(NAP_SUCCESS == sync_recv_message(uiSockId, &psResp))
   { 
       if ((psResp->eMessageType == eSYNC_DEL_FILE_LIST_REPLY) &&
	   (psResp->eReplyType == eSTART))
       {
	   FileCount = psResp->AppData;
	   printf("\n%s File Count = %d", __FUNCTION__, FileCount);
	   
	   free(psResp);

	   for(FileCnt = 0; FileCnt < FileCount; FileCnt++)
	   {
	      if(NAP_SUCCESS == sync_recv_message(uiSockId, &psResp))
	      {
		 if ((psResp->eMessageType == eSYNC_DEL_FILE_LIST_REPLY) &&
		     (psResp->eReplyType == eFILE))
		 {
		    memset(&temp_filepath, 0, sizeof(temp_filepath));
		    strcpy(temp_filepath, g_server_config.db_dir);//DIR_SERVER_PATH);
		    strcat(temp_filepath, psResp->DirName);
		    strcat(temp_filepath, "/");
		    strcat(temp_filepath, psResp->FileName);
		    printf("\n%s: removing file  = %s", __FUNCTION__, temp_filepath);
		    remove(temp_filepath);
		    free(psResp);
		 }
	      }    
	      else
	      {
		 close(uiSockId); 
		 goto EXIT_DELAY;
	      }

	   }
	}

    }
    else
    {
      close(uiSockId); 
      goto EXIT_DELAY;
    }
 
   /*
    * Send Get request for the list of clients and there corresponding files
    */
   memset(&psRequest, 0, sizeof(SYNC_MESSAGE));
   psRequest.eMessageType = eSYNC_CLIENT_LIST_REQUEST;
   psRequest.AppData    = 0;

   iResult = write(uiSockId, &psRequest, sizeof(SYNC_MESSAGE));
   if(iResult <= 0)
   {
      printf("\n%s: ERROR Unable to send SYNC_CLIENT_LIST_REQUEST", __FUNCTION__);
      close(uiSockId); 
      goto EXIT_DELAY;
   
   }
   printf("\n%s Sent SYNC_CLIENT_LIST_REQUEST", __FUNCTION__);
   
   if(NAP_SUCCESS == sync_recv_message(uiSockId, &psResp))
   { 

       if ((psResp->eMessageType == eSYNC_CLIENT_LIST_REPLY) &&
	   (psResp->eReplyType == eDIRECTORY))
       {
	   insert_ndir_name(&pLstPeerDir, psResp->DirName);
	   pLstPeerDir->FileCount = psResp->AppData;
	   FileCount = psResp->AppData;
	   printf("\n%s File Count = %d", __FUNCTION__, FileCount);
	   
	   free(psResp);

	   for(FileCnt = 0; FileCnt < FileCount; FileCnt++)
	   {

	      if(NAP_SUCCESS == sync_recv_message(uiSockId, &psResp))
	      {
		 if ((psResp->eMessageType == eSYNC_CLIENT_LIST_REPLY) &&
		     (psResp->eReplyType == eFILE))
		 {
                    if(!file_lookup(&glstDeleteFileList, &DelFileNode, "DB", psResp->FileName)){
		    	insert_nfil_name(&(pLstPeerDir->filenext),
					       psResp->FileName, 
					       psResp->mtime);
		    }
		    free(psResp);
		 }
	      }    
	      else
	      {

		 close(uiSockId); 
		 goto EXIT_DELAY;
	      }

	   }
	}
    }
    else
    {
      close(uiSockId); 
      goto EXIT_DELAY;
    }
    printf("\n%s: Master folder structure:");
    print_ndir_name(&pLstPeerDir);
    MasterDirList = pLstPeerDir;

    while(MasterDirList) 
    {
       if(dir_lookup(&glstDB, NULL, &SlaveDirNode, MasterDirList->dir_name)) 
       {
          MasterFileList = MasterDirList->filenext;
          while(MasterFileList)
          {
              if(file_lookup(&glstDB, &SlaveFileNode, MasterDirList->dir_name, MasterFileList->fil_name))
              {
		 /*both has a file so find the latest copy */
		 if(SlaveFileNode->mtime < MasterFileList->mtime) 
		 {
		     /* Master has latest copy add to get list */
		     if(GetList == NULL)
		     {
                     	insert_ndir_name(&GetList, SlaveDirNode->dir_name);
			GetListDirPtr = GetList;
		     }
		     else /* 
			 if (strcpy(GetList->dir_name, MasterDirList->dir_name) != 0)
                     	insert_ndir_name(&GetList, SlaveDirNode->dir_name);
                     insert_nfil_name(&GetList->filenext, SlaveFileNode->fil_name, MasterFileList->mtime);
		     */
		     {
			 if(!dir_lookup(&GetList, NULL, &GetListDirPtr, MasterDirList->dir_name))
			 {
			     insert_ndir_name(&GetList, SlaveDirNode->dir_name);
			     GetListDirPtr = GetList;
			 }
		     }
                     insert_nfil_name(&GetListDirPtr->filenext, SlaveFileNode->fil_name, MasterFileList->mtime);
		     GetListDirPtr->FileCount++;
		 }
		 else if(SlaveFileNode->mtime > MasterFileList->mtime) 
		 {
		     /* Slave has the latest copy add to push list */
		     if(PushList == NULL){
                     	insert_ndir_name(&PushList, SlaveDirNode->dir_name);
			PushListDirPtr = PushList;
		     }
		     else /*if (strcpy(PushList->dir_name, MasterDirList->dir_name) != 0)
                     	insert_ndir_name(&PushList, SlaveDirNode->dir_name);
                     insert_nfil_name(&PushList->filenext, SlaveFileNode->fil_name, SlaveFileNode->mtime);
                     */
		     {
			 if(!dir_lookup(&PushList, NULL, &PushListDirPtr, MasterDirList->dir_name))
			 {
			     insert_ndir_name(&PushList, SlaveDirNode->dir_name);
			     PushListDirPtr = PushList;
			 }
		     }
                     insert_nfil_name(&PushListDirPtr->filenext, SlaveFileNode->fil_name, SlaveFileNode->mtime);
		     PushListDirPtr->FileCount++;
		 }
		 else if(SlaveFileNode->mtime == MasterFileList->mtime) 
		 {
			 /* both files are same do nothing */
		 }
              }
              else
              {
                    /*slave does not have this file so add to get list */
		  if(GetList == NULL){
                     	insert_ndir_name(&GetList, SlaveDirNode->dir_name);
			GetListDirPtr = GetList;
		  }
		  else /*if (strcpy(GetList->dir_name, MasterDirList->dir_name) != 0)
                     	insert_ndir_name(&GetList, SlaveDirNode->dir_name);
                  insert_nfil_name(&GetList->filenext, MasterFileList->fil_name, MasterFileList->mtime);
		  */
		  {
		     if(!dir_lookup(&GetList, NULL, &GetListDirPtr, MasterDirList->dir_name))
		     {
			 insert_ndir_name(&GetList, SlaveDirNode->dir_name);
			 GetListDirPtr = GetList;
		     }
		  }
                  insert_nfil_name(&GetListDirPtr->filenext, MasterFileList->fil_name, MasterFileList->mtime);
		  GetListDirPtr->FileCount++;
              }

	      MasterFileList = MasterFileList->next;
          }
       }
       	MasterDirList = MasterDirList->next;
    }
 
    printf("\n%s: Get folder structure:");
    print_ndir_name(&GetList);
    SlaveDirNode = GetList;
    while(SlaveDirNode)
    {
	SlaveFileNode = SlaveDirNode->filenext;
	while(SlaveFileNode)
	{
	    memset(&psRequest, 0, sizeof(SYNC_MESSAGE));
	    psRequest.eMessageType 	= eSYNC_FILE_REQUEST;
	    psRequest.AppData    	= 0;
	    psRequest.eRequestType 	= eREQ_FILE_GET;
	    strcpy(psRequest.FileName, SlaveFileNode->fil_name);
	    strcpy(psRequest.DirName, SlaveDirNode->dir_name);

	    iResult = write(uiSockId, &psRequest, sizeof(SYNC_MESSAGE));
	    if(iResult <= 0)
	    {
	      printf("\n%s: ERROR Unable to send SYNC_FILE_REQUEST", __FUNCTION__);
	    }
	    printf("\n%s Sent SYNC_FILE_REQUEST for dir %s & file %s", __FUNCTION__,psRequest.DirName, psRequest.FileName);
	    if(NAP_SUCCESS == sync_recv_message(uiSockId, &psResp))
	    {
	      if ((psResp->eMessageType == eSYNC_FILE_REPLY) &&
		      (psResp->eReplyType == eSTART))
	      {
		 FileSize = psResp->AppData;
		 printf("\n%s FileSize = %d", __FUNCTION__, FileSize);
	 
	         recv_len = read(uiSockId, file_data, FileSize);
	         if (recv_len <= 0)
	         { 
		     printf("\n%s: Read failed:", __FUNCTION__);
	         }

		 memset(&temp_filepath, 0, sizeof(temp_filepath));

		 strcpy(temp_filepath, g_server_config.db_dir);//DIR_SERVER_PATH);
                 strcat(temp_filepath, psResp->DirName);
                 strcat(temp_filepath, "/");
                 strcat(temp_filepath, psResp->FileName);
		 printf("\n%s: File PAth = %s", __FUNCTION__,temp_filepath);

		 filed = fopen(temp_filepath, "w");
		 if(filed == NULL)
		     printf("\n\n ********* Error in file open for filepath : %s", temp_filepath);
		 else
		 {
                     fwrite(file_data, sizeof(char), recv_len, filed);
		    /*fputs(file_data, filed);*/
                     fclose(filed);
		 }
	      }
	      free(psResp);
	    }
	    SlaveFileNode = SlaveFileNode->next;
	}
	SlaveDirNode = SlaveDirNode->next;
    }
    
    printf("\n%s: Push folder structure:");
    print_ndir_name(&PushList);
     SlaveDirNode = PushList;
    while(SlaveDirNode)
    {
	SlaveFileNode = SlaveDirNode->filenext;
	while(SlaveFileNode)
	{
	   strcpy(temp_filepath, g_server_config.db_dir);//DIR_SERVER_PATH);
	   strcat(temp_filepath, SlaveDirNode->dir_name);
	   strcat(temp_filepath, "/");
	   strcat(temp_filepath, SlaveFileNode->fil_name);
	   
	   index =0;
	   printf("\n%s: File PAth = %s", __FUNCTION__,temp_filepath);
	   memset(file_data, 0, sizeof(file_data));
	   filed = fopen(temp_filepath, "r");
	   if(filed ==NULL)
	       printf("\n%s: ERROR in file open for File PAth = %s", __FUNCTION__, temp_filepath);
	   else 
	   {    /*
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
		   /* calculate file size */
		   fseek(filed, 0, SEEK_END);
		   index = ftell(filed);
		   fseek(filed, 0, SEEK_SET);

		
		memset(&psRequest, 0, sizeof(SYNC_MESSAGE));
		psRequest.eMessageType 	= eSYNC_FILE_REQUEST;
		psRequest.AppData    	= index;
		psRequest.eRequestType 	= eREQ_FILE_PUSH;
		strcpy(psRequest.FileName, SlaveFileNode->fil_name);
		strcpy(psRequest.DirName, SlaveDirNode->dir_name);

		iResult = write(uiSockId, &psRequest, sizeof(SYNC_MESSAGE));
		if(iResult <= 0)
		{
		   printf("\n%s: ERROR Unable to send SYNC_FILE_REQUEST", __FUNCTION__);
		}
		printf("\n%s Sent PUSH SYNC_FILE_REQUEST for dir %s & file %s", __FUNCTION__,psRequest.DirName, psRequest.FileName);
		
		do {
		    ret = fread(file_data, sizeof(char), sizeof(file_data), filed);
		    len += ret;
		    if(ret > 0) {
			ret = write(uiSockId, file_data, ret);
		    }
		} while(len < index);

	 
		/*
		iResult = write(uiSockId, file_data, index);
		if(iResult <= 0)
		{
		   printf("\n%s Unable to send PUSH SYNC_FILE_REPLY", __FUNCTION__);
		}
		else
		{
		   printf("\n:%s: PUSH Success ", __FUNCTION__);
	        }
		*/
	    }
	    SlaveFileNode = SlaveFileNode->next;
	}
	SlaveDirNode = SlaveDirNode->next;
    }
   /*
    * Form list of mismatch clients and files
    */

   /*
    * Form a list of 
    */


#if 0
   while(1)
   {
      pvMessage = recv_message(sync_slave_thr_msg_q);
      printf("\n%s: Message received",__FUNCTION__);
      if (pvMessage == NAP_NULL)
      {
         printf("\n%s: Invalid message",__FUNCTION__);
	 pthread_yield();
         continue;
      }
      pthread_yield();
   }
#endif
EXIT_DELAY:
   close(uiSockId);
   sleep(5);
   goto RECONNECT;
   //while(1);
   printf("\n%s: Exiting ", __FUNCTION__);

   return;
}


