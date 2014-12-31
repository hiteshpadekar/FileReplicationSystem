#include "fwk_datatypes.h"
#include <sys/socket.h>
#include <netinet/ip.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "server_config.h"
#include "access_nw.h"

#define IP_ADDR_LEN     16                                              // IP address length.
#define  MAX_PORT               65535

extern SERVER_CONFIG g_server_config;

fd_set gstReadDesc;
fd_set gstWriteDesc;
NAP_UINT16 usMaxBufferLen;                                      // Max data buffer.
NAP_INT32 iMaxDatagramLen;                                      // Max datagram size.

static fd_set gstLocalReadDesc;
static fd_set gstLocalWriteDesc;

static pthread_t gstAcessNwTxThread;
static pthread_t gstAcessNwRxThread;

static pthread_mutex_t stConditionMutex;

S_NAP_ACCESS_NETWORK *pstAccessNw;


/*******************************************************************************
* Function name	:	access_nw_InitBuffer
* Description	:	This is local function which allocates memory for the buffer
					to read the data from the network and to the structures to
					send the data to corresponding modules
* Return type	:	<NAP_BOOL>
* Argument      : 	[IN] *puiThreadId:  Thread id
* Side Effects	: 	None.
* NOTE 			: 	None.
********************************************************************************/
NAP_BOOL	access_nw_InitBuffer(S_ACCESS_NETWORK_QUEUE **pstDataToStack,
									  S_ACCESS_NETWORK_PARAMS**pstMessage,
									  NAP_INT16 *psError)
{
	/* Invalid function parameters.*/
	if (psError == NAP_NULL)
		return NAP_FAILURE;
	if (pstDataToStack == NAP_NULL || pstMessage == NAP_NULL)
	{
		return NAP_FAILURE;
	}

	/* Allocate memory to read the data */
	*pstDataToStack = (S_ACCESS_NETWORK_QUEUE *) FWK_Malloc(sizeof(S_ACCESS_NETWORK_QUEUE));
	if (*pstDataToStack == NAP_NULL)
	{
		return NAP_FAILURE;
	}
	else
	{
	   FWK_Memset((*pstDataToStack),0,sizeof(S_ACCESS_NETWORK_QUEUE));
	   *pstMessage=  (S_ACCESS_NETWORK_PARAMS*)FWK_Malloc(sizeof(S_ACCESS_NETWORK_PARAMS));
		if (*pstMessage == NAP_NULL)
		{
			FWK_Free(pstDataToStack);
			return NAP_FAILURE;
		}
		else
		{
			FWK_Memset((*pstMessage),0,sizeof(S_ACCESS_NETWORK_PARAMS));
			(*pstMessage)->stTptInfo.pucIpaddr = (NAP_UCHAR*)FWK_Malloc(16*sizeof(NAP_UCHAR));
			if ((*pstMessage)->stTptInfo.pucIpaddr == NAP_NULL)
			{

				FWK_Free(pstDataToStack);
				FWK_Free(pstMessage);
				return NAP_FAILURE;
			}
			else
			{
				(*pstMessage)->stTptInfo.pucLocalIpaddr = (NAP_UCHAR*)FWK_Malloc(16*sizeof(NAP_UCHAR));
				if ((*pstMessage)->stTptInfo.pucLocalIpaddr == NAP_NULL)
				{
					FWK_Free(pstDataToStack);
					FWK_Free(pstMessage);
					FWK_Free(&((*pstMessage)->stTptInfo.pucIpaddr));
					return NAP_FAILURE;
				}
				else
				{
					FWK_Memset((*pstMessage)->stTptInfo.pucIpaddr,0,16);
					FWK_Memset((*pstMessage)->stTptInfo.pucLocalIpaddr,0,16);
					(*pstMessage)->stTptInfo.pvData = NAP_NULL;
					(*pstMessage)->stTptInfo.pucHost = NAP_NULL;
					(*pstMessage)->stTptInfo.eIptype = -1;
					(*pstMessage)->stTptInfo.eSoctype = -1;
					(*pstMessage)->stTptInfo.usPort = -1;
					(*pstMessage)->stTptInfo.usLocalPort = -1;
					(*pstMessage)->stTptInfo.ussocketId =-1;
					/* Allocate memory to read the data */
					(*pstMessage)->pucInBuffer = (NAP_UCHAR*)FWK_Malloc((NAP_UINT16)((usMaxBufferLen + 1) *sizeof(NAP_UCHAR)));
					if ((*pstMessage)->pucInBuffer == NAP_NULL)
					{
						FWK_Free(pstDataToStack);
						FWK_Free(pstMessage);
						FWK_Free(&((*pstMessage)->stTptInfo.pucIpaddr));
						FWK_Free(&((*pstMessage)->stTptInfo.pucLocalIpaddr));
						return NAP_FAILURE;
					}
					FWK_Memset((*pstMessage)->pucInBuffer,0,usMaxBufferLen);
					(*pstMessage)->usInBufLen = -1;
				}
			}
		}
	}
	return NAP_SUCCESS;
}




NAP_BOOL access_nw_add_module(
				IN_OUT  S_NAP_ACCESS_NETWORK *pstAccessNw,
				IN  NAP_INT32 iModuleId,
				IN  NAP_UINT32 uProtocol,
				IN  NAP_BYTE bModuleType,
				IN  NAP_BYTE bTransPort,
				IN  NAP_INT32  iNumOfClients,
				IN	NAP_VOID *pvData,
				OUT NAP_INT16 *psError)
{
	NAP_UINT16 usCount	= 0;
	NAP_INT32 iIndex = 0;			   /* Loop Index.*/
	NAP_BOOL bStatus = NAP_FALSE;	/* Status information. */
	S_MODULE_NODES *pstModule = NAP_NULL;	/* Module information structure.*/


	printf("\n%s:Entering",__FUNCTION__);
	*psError = 0;

	// access network is not initialized or some error occured.
	if (pstAccessNw == NAP_NULL)
	{
	        printf("\n%s:Error: Access NW not created",__FUNCTION__);
		return NAP_FAILURE;
	}

	/* Allocate the memory for module information structure.*/
	pstModule = (S_MODULE_NODES *) malloc(sizeof(S_MODULE_NODES));
	if (pstModule == NAP_NULL)
	{
		printf("\n%s:Memory allocation failed.",__FUNCTION__);
		return NAP_FAILURE;
	}

	memset(pstModule,0,sizeof(S_MODULE_NODES));
	/* Module is yet to initialized.*/
	pstModule->bModuleInitStatus = NAP_FALSE;
	pstModule->bTransport  = bTransPort;            /* Protocol type.*/
	pstModule->bModuleType = bModuleType;		/* Server/client*/
	pstModule->iModuleId   = iModuleId;			/* Module id*/
	pstModule->uProtocolName = uProtocol;


	printf("\n%s: handling for TCP",__FUNCTION__);
	/* Module is TCP client.*/
	if (bModuleType == 0)
	{
		if (bTransPort == 0)
		{
			pstModule->uProtocol.stTcp.uiNumOfClients =0;
		}
	}
	else	/* Module is TCP server.*/
	{
		if (bTransPort == 0)
		{
			if (iNumOfClients ==0)
				pstModule->uProtocol.stTcp.uiNumOfClients = \
									MAX_CLIENTS_SUPPORTED;
			else
			{
				if (iNumOfClients <= MAX_CLIENTS_SUPPORTED)
					pstModule->uProtocol.stTcp.uiNumOfClients = iNumOfClients;
				else
					pstModule->uProtocol.stTcp.uiNumOfClients = \
									MAX_CLIENTS_SUPPORTED;
			}
		}
	}

	/* Protocol is TCP. Initialize TCP parameters*/
	if (bTransPort == 0)
	{
		pstModule->uProtocol.stTcp.iListenSocket =0;
		for (iIndex = 0; iIndex<MAX_CLIENTS_SUPPORTED  ;iIndex++)
		{
			pstModule->uProtocol.stTcp.iDataSocket[iIndex] = 0;
			pstModule->uProtocol.stTcp.iDataLen[iIndex] = 0;
			pstModule->uProtocol.stTcp.pucData[iIndex] = NAP_NULL;
			pstModule->uProtocol.stTcp.iReadRetranCount[iIndex] = 0;
			pstModule->uProtocol.stTcp.iWriteRetranCount[iIndex] = 0;
			
			pstModule->uProtocol.stTcp.iModuleId[iIndex] = iModuleId;

			memset(&pstModule->uProtocol.stTcp.cChildIpAddr[iIndex],0,IP_ADDR_LEN);
		}
		pstModule->uProtocol.stTcp.uiTotActiveClients = 0;
		pstModule->uProtocol.stTcp.uiNumSockets = 0;
		//pstModule->uProtocol.stTcp.usPort = usPort;
	}
	else	/* Protocol is UDP. Initialize UDP parameters*/
	{
		pstModule->uProtocol.stUdp.iDataSocket = 0;
		//pstModule->uProtocol.stUdp.usPort = usPort;
		pstModule->uProtocol.stUdp.pstUdpList = NAP_NULL;

	}


	/* Add the module information structure to list.*/
	bStatus = FWK_LIST_AddNode(&pstAccessNw->pstModLst,(NAP_VOID *)pstModule,\
							psError);
	if (bStatus == NAP_FAILURE)
	{	/* Adding the node to the list is failed.*/
		return bStatus;
	}

	// Get the index of the newly added node.
	FWK_LIST_GetCount(pstAccessNw->pstModLst,&usCount,psError);
	pstModule->pvData = pvData;

	printf("\n%s:Exiting",__FUNCTION__);
	return bStatus;

}

/*******************************************************************************
* Function name :       access_nw_Init.
* Description   :       This is an interface routine.This routine initializes the
                                        access network components.access network threads are created
                                        when this routine is invoked for the first time. This routines
                                        can be accessed by the user  modules for which network component is
                                        to be configured.
* Return type   :       <NAP_BOOL>
                                        NAP_SUCCESS on successfull initialization of access network
                                        component and thread creation.
                                        NAP_FAILURE on access network component initialization or
                                        thread creation failure.
* Argument      :       [IN] bIPAddress : IP Address on which the module invoking
                                                 this routine will listen for the connection requests
                                                 from the clients or IP Address using which module
                                                 invoking this routine will get connected to the server.
                                        [IN] bModuleId : Module Id of the module invoking this
                                                 routine.
                                        [IN] bTransPort: Transport protocol type (TCP/UDP).
                                        [IN] bModuleType: Whether the module is client or server.
                                        [IN] iNumClients : Number of clients supported by the module
                                                 in case if the module is server.
                                        [IN] uiModuleThreadId : Thread id of the module.
                                        [IN] usPort : Port on which the module invoking this routine
                                                 will listen for the connection requests from the
                                                 clients or Port using which module invoking this routine
                                                 will get connected to the server.
                                        [IN] pvData: Additional data.
                                        [OUT] piSocket: Socket descriptor.
                                        [OUT] psError : Error type.
* Side Effects  :       None.
* NOTE                  :       None.
********************************************************************************/

NAP_BOOL access_nw_init(IN NAP_UBYTE *bIpAddress, 
                        IN  NAP_INT32 iModuleId,
			IN  NAP_UINT32 uProtocol,
			IN  NAP_BYTE bModuleType,
			IN  NAP_BYTE bTransPort,
			IN  NAP_INT32  iNumOfClients,
			IN NAP_UINT16 usPort,
			IN	NAP_VOID *pvData,
			OUT NAP_INT16 *psError)
{
   NAP_BOOL  bStatus = 0;          /* Function return status.*/
   NAP_INT16 sError = 0;           /* Error type.*/
   NAP_INT32 iIndex = 0;           /* Loop Index.*/
   NAP_UINT32 uiMutex = 0;         /* Mutex return value.*/
   NAP_INT32 iSocket = 0;          /* Either listen /connect socket*/
   NAP_UCHAR *pIPAddress = NAP_NULL;       /* To validate IP Address.*/
   NAP_UCHAR ucIPAddr[20] = {0};           /* IP Address. */
   pthread_attr_t gAcessNwThreadAttr;      /* Thread attribute.*/
   struct timespec stTime;                         /* Time for thread synchronization.*/
   struct in_addr stAddr;                          /* To validate IP Address.*/
   S_MODULE_NODES *pstModule = NAP_NULL;


   /* First time invokation of the routine.Clear file descriptors.*/
   if (pstAccessNw == NAP_NULL)
   {
      /* stConditionMutex = PTHREAD_MUTEX_INITIALIZER; */
      if(pthread_mutex_init(&stConditionMutex,NAP_NULL))
      {
	 printf("\n\nACCESS NETWORK - FAILURE INITIALIZATION\n\n");
      }
      printf("\nACCESS NETWORK - MUTEX INITIALIZED");
      usMaxBufferLen = MAX_ACCESS_NETWORK_RECV;
   }

   /* Invalid parameters.*/
   if (bIpAddress == NAP_NULL || bTransPort > e_NAP_Udp || \
                bModuleType > eNapServer || usPort <= 0 || \
                usPort > MAX_PORT )
    {
       printf("\n ACCESS Network Invalid parameters");
        return NAP_FAILURE;
    }

    pIPAddress = ucIPAddr;
        /* Update IP Address.*/
    FWK_Lnx_NetV4NtoA((NAP_BYTE*)bIpAddress,&pIPAddress,4);

    printf("\n IP Address: %s, bIpAddress = %d", pIPAddress, *(NAP_UINT32 *)bIpAddress);
   /* Allocate memory for access network structure.*/
   pstAccessNw = (S_NAP_ACCESS_NETWORK*)malloc(sizeof(S_NAP_ACCESS_NETWORK));

   if (pstAccessNw == NAP_NULL)
   {	
      pthread_mutex_destroy(&stConditionMutex);
      printf("\nACCESS NETWORK - DESTROYED");
      return NAP_FAILURE;
   }
   memset(pstAccessNw,0,sizeof(S_NAP_ACCESS_NETWORK));
   pstAccessNw->pstModLst = NAP_NULL;
   /* Lock the mutex.*/
   uiMutex  = pthread_mutex_lock(&stConditionMutex);
   printf("\nACCESS NETWORK - LOCKED at LINE:%d",__LINE__);

   if (NAP_SUCCESS == FWK_Ncm_AddModuleTo_AccessNetwork(pstAccessNw,bIpAddress,\
							iModuleId,uProtocol,bModuleType,\
							bTransPort,iNumOfClients,&iIndex,\
							0,usPort,pvData,
							psError))

   /*
   if(NAP_SUCCESS == access_nw_add_module(pstAccessNw, iModuleId, uProtocol, \
						bModuleType, bTransPort, iNumOfClients, pvData, psError))
   */
   {
	pstAccessNw->iActiveModules++;
	/* Create,bind/listen/connect the sockets.*/
        printf("\nACCESS NETWORK - Module added to AN no of active modules = %d",pstAccessNw->iActiveModules);
	bStatus = FWK_Ncm_Init_NetworkComponent(pstAccessNw,iIndex,\
			&iSocket,psError);
	if (bStatus == NAP_FAILURE)
	{
	   printf("\nACCESS NETWORK Init network comonent failed - UNLOCKING");
	   /* Un-lock the mutex.*/
           uiMutex  = pthread_mutex_unlock(&stConditionMutex);

	   /* Invalid input or node does not exist.*/
           
	   //close(iSocket);
           
	   //FWK_LIST_DeleteNode(&pstAccessNw->pstModLst,(NAP_INT16)iIndex,
		//						&pstModule,psError);
	   return NAP_FAILURE;
	}

      /* Update the number of sockets available sockets in network
		and active modules.*/
      pstAccessNw->iNumOfSockets++;
      if (bTransPort == 0)
      {
	 if(bModuleType == 1)
		pstAccessNw->iNumOfTcpServers++;
	 else
		pstAccessNw->iNumOfTcpClients++;
      }
      else
      pstAccessNw->iNumOfUdpClientServer++;
      printf("\nACCESS NETWORK init success - UNLOCKING");
      /* Un-lock the mutex.*/
      uiMutex  = pthread_mutex_unlock(&stConditionMutex);
   }
   else
   {
      printf("\nACCESS NETWORK add module failed - UNLOCKING");
      /* Un-lock the mutex.*/
      uiMutex  = pthread_mutex_unlock(&stConditionMutex);
      /* Memory allocation failed.*/
      access_nw_DeInit(iModuleId,0,psError);
      return NAP_FAILURE;
   }
   
   /* Create the access network threads.*/
   if (pstAccessNw->iActiveModules <= 1)
   {
	pstAccessNw->bNwInitialized = NAP_TRUE;

	pthread_attr_init(&gAcessNwThreadAttr);
	pthread_attr_setdetachstate(&gAcessNwThreadAttr, PTHREAD_CREATE_JOINABLE);

	/* Create transmit thread.*/
	//if (pthread_create(&gstAcessNwTxThread,&gAcessNwThreadAttr,\
	//						access_nw_Transmit_Thread,NULL))
	//{
	//	access_nw_DeInit(0,0,psError);
	//	return NAP_FAILURE;
	//}

	/* Create receive thread.*/
	if (pthread_create(&gstAcessNwRxThread, &gAcessNwThreadAttr,\
		access_nw_receive_thread, NULL))
	{
		//access_nw_denit(0,0,psError);
		return NAP_FAILURE;
	}

   	printf("\n%s: Created AN Recv thread",__FUNCTION__);
	/* It has no effect on the threads created with this attribute.*/
	pthread_attr_destroy(&gAcessNwThreadAttr);
        //sleep(2);

   }
   printf("\n%s: Exiting",__FUNCTION__);
   return NAP_SUCCESS;
}

/*******************************************************************************
* Function name	:	ACCESS_Network_DeInit.
* Description	:	This is an interface routine. This routine updates the
					structures if any module is removed from the network or if
					any critical error occurs. 	This routinr also terminates
					network threads gracefully if access network component is 
					not required.This routine is accessable to other modules also.
* Return type	:	<NAP_BOOL>
					NAP_SUCCESS on successfull deinitialization of network
					component.
					NAP_FAILURES on network deinitialization failure.
* Argument      : 	[IN] iModuleId: Module id.
					[IN] uiThreadId: ThreadId,
					[OUT] psError : Error type.
* Side Effects	: 	None.
* NOTE 			: 	None.
********************************************************************************/
NAP_BOOL access_nw_DeInit(NAP_INT32 iModuleId,
							NAP_UINT32 uiThreadId,
							NAP_INT16 *psError)
{
 return NAP_SUCCESS;
}	

NAP_BOOL access_nw_receive_thread(NAP_VOID)
{
   NAP_INT32 iCount = 0;	/* Total number of data socket which are active*/
   NAP_INT32 iCountLoop = 1;
   NAP_INT32 iDesc  = 0;	/* Socket desc created using accept socket call*/
   NAP_INT32 iFlags = 0;	/* If any flags to be used in recv socket call*/
   NAP_INT32 iIndex = 0;	/* Index used in loops*/
   NAP_INT32 iIdx = 0;		/* Index used in loops*/
   NAP_INT32 iLen = sizeof(struct sockaddr_in);
   NAP_INT32 iLoopId = 0;
   NAP_INT32 iModuleId = -1;
   NAP_INT32 iMutex = 0;
   NAP_INT32 iNumOfBytesRcvd = 0; /* Num of bytes successfully read in recv*/
   NAP_INT32 iNumOfBytesSent = 0;	/* Number of bytes successfully sent on the network.*/
   NAP_INT32 iStatus = 0;	/* Variable to store the select sys. call result*/
   NAP_INT16 sError = 0; /* Any error */
   NAP_BOOL bStatus;
   NAP_UINT16 usNwError = 0;

   /* structures.*/
   S_ACCESS_NETWORK_QUEUE *pstDataToStack = NAP_NULL;
   S_ACCESS_NETWORK_PARAMS *pstMessage = NAP_NULL;
   S_MODULE_NODES *pstModule = NAP_NULL;
   struct sockaddr_in stAddr;
   struct timeval timeout;
 
   memset(&timeout,0,sizeof(struct timeval));
   timeout.tv_sec = 0;
   timeout.tv_usec = 100;

   /*Get the current thread's Id*/
   if (pstAccessNw != NAP_NULL)
   {
#ifdef SYS_gettid
	pstAccessNw->uiNwCompRxId = syscall(SYS_gettid);
#else
#error "SYS_gettid unavailable on this system"
#endif
	//pstAccessNw->uiNwCompRxId = gettid();
   }
   FD_ZERO(&gstReadDesc);
   FD_ZERO(&gstWriteDesc);
   
   while(NAP_TRUE)
   {
      iCount = 0,iDesc  = 0,iFlags = 0,iIndex = 0;
      iIdx = 0,iLen = sizeof(struct sockaddr_in),iNumOfBytesRcvd = 0;
      sError = 0;
      iLoopId = 0,iModuleId = -1,iNumOfBytesSent = 0;
      gstLocalReadDesc = gstReadDesc;
      gstLocalWriteDesc = gstWriteDesc;
		
      if (pstAccessNw != NAP_NULL)
      {

	iStatus = select(0, &gstLocalReadDesc, &gstLocalWriteDesc, NAP_NULL, NAP_NULL/*&timeout*/);
        iMutex = pthread_mutex_lock(&stConditionMutex);
	switch(iStatus)
	{
	    case -1:

		    printf("\nAN Recv Thread :: Select description exception has occured\n");
		    break;
            case 0:
		    printf("\nAN Recv Thread :: Select error has occured\n");
		    break;

            default:
		    if(pstAccessNw == NAP_NULL)
		    {
			printf("\nAN Recv Thread :: AccessNW is NULL\n");
			break;
		    }
	            if (pstAccessNw->pstModLst == NAP_NULL)
		    {
			printf("\nAN Recv Thread :: AccessNW->pstModList is NULL (default)\n");
			break;
		    }
		    /* Get the number of nodes.*/
		    FWK_LIST_GetCount(pstAccessNw->pstModLst, (NAP_UINT16 *)&iIndex, &sError);
		    printf("\nACCESS NETWORK - iIndex:%d, Line:%d",iIndex,__LINE__);

		    /* Traverse all the nodes.*/
		    iCountLoop = 1;
		    while (iCountLoop <= iIndex)
		    {
			printf("\nAN Recv Thread :: traversing all nodes\n");
			pstModule = NAP_NULL;
			FWK_LIST_AccessNode(pstAccessNw->pstModLst,(NAP_UINT16)iCountLoop,&pstModule,&sError);

			if (pstModule == NAP_NULL || pstAccessNw == NAP_NULL )
				break;
			if (pstModule->bTransport == 0) //TCP.
			{
			   if (pstModule->bModuleType == 1)	//server
			   {
				iCount = pstModule->uProtocol.stTcp.uiNumOfClients;
				if (FD_ISSET(pstModule->uProtocol.stTcp.iListenSocket,
						&gstLocalReadDesc))
				{
				   if (pstModule->uProtocol.stTcp.uiTotActiveClients 
								> MAX_CLIENTS_SUPPORTED)
				   {
				       printf("\n ACCESS NETWORK :: Max clients reached\n");
				   }
				   else
				   {
					if (pstModule->bModuleInitStatus == NAP_TRUE)
					{
					   FWK_Memset (&stAddr,0,sizeof(struct sockaddr_in));
					   /* Accept the connection*/
					   iDesc = accept(pstModule->uProtocol.stTcp.iListenSocket,
								(struct sockaddr*)&stAddr, &iLen);
					   if (iDesc <= 0)	/* Accept failed*/
					   {
						printf("\nACCESS NETWORK :: TCP Accept Failed");
						FD_CLR((NAP_UINT32)pstModule->uProtocol.stTcp.iListenSocket,
								&gstReadDesc);
						pstModule->uProtocol.stTcp.uiNumSockets--;
						pstAccessNw->iNumOfSockets --;
					   }  // accept failed
					   else
					   {
						printf("\nACCESS NETWORK - iLoopId:%d, Line:%d",iLoopId,__LINE__);
						/* Update the new socket in array*/
						for (iLoopId = 0;
								iLoopId <MAX_CLIENTS_SUPPORTED;
								iLoopId++)
						{
						   if (pstModule->uProtocol.stTcp.iDataSocket[iLoopId] == 0)
						   {
							pstModule->uProtocol.stTcp.uiTotActiveClients++;
							pstModule->uProtocol.stTcp.iDataSocket[iLoopId] = iDesc;
							/* Update the Module Id  */
							pstModule->uProtocol.stTcp.iModuleId[iLoopId] = pstModule->iModuleId;
							FD_SET((NAP_UINT32)pstModule->uProtocol.stTcp.iDataSocket[iLoopId],&gstReadDesc);
							pstModule->uProtocol.stTcp.uiNumSockets++;
							pstAccessNw->iNumOfSockets++;
							sprintf(
								pstModule->uProtocol.stTcp.cChildIpAddr[iLoopId],
								"%d.%d.%d.%d",(stAddr.sin_addr.s_addr) & 0xff,
								(stAddr.sin_addr.s_addr >> 8) & 0xff,
								(stAddr.sin_addr.s_addr >> 16) & 0xff,
								(stAddr.sin_addr.s_addr >> 24)& 0xff);
							break;
						   }
						}
					   }// successful accept
					}	// module is active.
					else
					{
					
					}	// accept critical error.
				    } // accepted request handle.
				} //Accept
			   } // Server
			   else // Client module
				   iCount = 1;
			   
			   /***********************************
			     Now Check for all TCP sockets
			    **********************************/

			   {
				printf("\nACCESS NETWORK - iLoopId:%d, Line:%d",iLoopId,__LINE__);
				for (iLoopId = 0; iLoopId<iCount; iLoopId++)
				{
				   if (pstModule != NAP_NULL)
				   {
					/* Check if we have to read the data*/
					if (pstModule->uProtocol.stTcp.iDataSocket[iLoopId] >0)
					{
					   /* Read the data.*/
					   if (FD_ISSET(pstModule->uProtocol.stTcp.iDataSocket[iLoopId],&gstLocalReadDesc))
					   {
						if (pstModule->bTransport == 1 &&
							iMaxDatagramLen <MAX_ACCESS_NETWORK_RECV)
							usMaxBufferLen = iMaxDatagramLen;
						else
							usMaxBufferLen = MAX_ACCESS_NETWORK_RECV;
						if (access_nw_InitBuffer(&pstDataToStack,&pstMessage,&sError) == NAP_SUCCESS)
						{
						   /* Read the data*/
						   FWK_Memset(pstMessage->pucInBuffer,0,usMaxBufferLen);
									
						   iNumOfBytesRcvd = recv(pstModule->uProtocol.stTcp.iDataSocket[iLoopId],
								               (NAP_INT8*)pstMessage->pucInBuffer,
									       usMaxBufferLen,
									       iFlags);
						   if(iNumOfBytesRcvd == -1)
						   {
						      printf("\n ACCESS NETWORK :: recv failed \n");
						   }
						   if(iNumOfBytesRcvd == 0)
						   {
						        printf("\n ACCESS NETWORK :: Connection gracefully closed \n");
						        if (bStatus == NAP_SUCCESS && sError>0)
							{
							   if(FWK_Strlen(pstMessage->pucInBuffer) >0)
							   {	
								goto send_tcp;
							   }
							}
							else 
							{
							   FD_CLR((NAP_UINT32)pstModule->uProtocol.stTcp.iDataSocket[iLoopId],
										&gstReadDesc);

							   FD_CLR((NAP_UINT32)pstModule->uProtocol.stTcp.iDataSocket[iLoopId],
										&gstWriteDesc);

							   printf("\nAN RX Thread :: Received Failed(1) - B4 access_nw_UpdateModule \n");
							   /*
							      access_nw_UpdateModule(
										pstModule->uProtocol.stTcp.iDataSocket[iLoopId],
							  			iLoopId,iCountLoop,&sError);

							   */
							   if (pstModule->bModuleType == 0)
							   {
								iIndex--;
							   }

							}

							if (pstMessage->stTptInfo.pucHost == NAP_NULL)
									FWK_Free(&pstMessage->stTptInfo.pucHost);
							if (pstMessage->stTptInfo.pucIpaddr != NAP_NULL)
									FWK_Free(&pstMessage->stTptInfo.pucIpaddr);
							if (pstMessage->stTptInfo.pucLocalIpaddr != NAP_NULL)
									FWK_Free(&pstMessage->stTptInfo.pucLocalIpaddr);
							if (pstMessage->pucInBuffer != NAP_NULL)
									FWK_Free(&pstMessage->pucInBuffer);
							if (pstMessage != NAP_NULL)
									FWK_Free(&pstMessage);
							if (pstDataToStack != NAP_NULL)
									FWK_Free(&pstDataToStack);
						   }
						   else if (iNumOfBytesRcvd>0)
						   {
send_tcp:	
						      if (FWK_Strlen(pstMessage->pucInBuffer) >0)
							   iNumOfBytesRcvd = MAX_ACCESS_NETWORK_RECV;
						      usNwError = sError;
						      pstMessage->pucInBuffer[iNumOfBytesRcvd] = '\0';
						      pstMessage->usInBufLen = iNumOfBytesRcvd;
						      pstMessage->stTptInfo.pvData = pstModule->pvData;
						      pstMessage->stTptInfo.eIptype = e_nap_ipv4;
						      pstMessage->stTptInfo.eSoctype = e_NAP_Tcp;
						      pstMessage->stTptInfo.pucHost = NAP_NULL;
						      //FWK_Ncm_GetSystemIpAddr(pstMessage->stTptInfo.pucLocalIpaddr);
						      if (pstModule->bModuleType == 0)
						      {
							  sprintf(
								pstMessage->stTptInfo.pucIpaddr,
								"%d.%d.%d.%d",(pstModule->uIPAddress) & 0xff,
								(pstModule->uIPAddress >> 8) & 0xff,
								(pstModule->uIPAddress >> 16) & 0xff,
								(pstModule->uIPAddress >> 24) & 0xff);

							/* 
							   FWK_Ncm_ByteToDotFormat(pstModule->uIPAddress,
											pstMessage->stTptInfo.pucIpaddr,
											&sError);
							*/

						      }
						      else
						  	 FWK_Strcpy(pstMessage->stTptInfo.pucIpaddr,pstModule->uProtocol.stTcp.cChildIpAddr[iLoopId]);

						      if (pstModule->bModuleType ==1)
						      {
						 	 pstMessage->stTptInfo.usLocalPort = \
										pstModule->uProtocol.stTcp.usPort;
							 pstMessage->stTptInfo.usPort = 0;
						      }
						      else
							  pstMessage->stTptInfo.usPort = \
										pstModule->uProtocol.stTcp.usPort;

							  pstMessage->stTptInfo.ussocketId =
									pstModule->uProtocol.stTcp.iDataSocket[iLoopId];
							  pstDataToStack->ucSource = e_ACCESS_NW;
							  pstDataToStack->ucProtocol = pstModule->uProtocolName;
							  //pstDataToStack->ucEvent = e_AccessNwSuccess;
							  pstDataToStack->ucDest = pstModule->iModuleId;//e_FwkModuleEnd
							  pstDataToStack->pvData = (NAP_VOID*)pstMessage;
							  pstModule->uProtocol.stTcp.iReadRetranCount[iLoopId] = 0;											
							  /* Prepare the queue and post the data*/

							  send_message((MSG_QUEUE *)pstModule->pvData, (NAP_VOID*)pstDataToStack);

							  /*FWK_IPC_Put_Data (pstModule->uiThreadId,\
										(NAP_VOID*)pstDataToStack,&sError);
							  */
							  break;
							}//else
						    }
						}
						else
						{
						    /* Check if we have to send some pending data*/
						    if (FD_ISSET(pstModule->uProtocol.stTcp.iDataSocket[iLoopId],\
									&gstLocalWriteDesc))
						    {
							/* Send the pending data*/
							iNumOfBytesSent = send(pstModule->uProtocol.stTcp.iDataSocket[iLoopId],
									(NAP_CHAR*)pstModule->uProtocol.stTcp.pucData[iLoopId],
									(NAP_UINT32)pstModule->uProtocol.stTcp.iDataLen[iLoopId],
									iFlags);
							/* 
							   bStatus = FWK_Ncm_Send_Data((NAP_CHAR*)\
										pstModule->uProtocol.stTcp.pucData[iLoopId],
										(NAP_UINT32)pstModule->uProtocol.stTcp.iDataLen[iLoopId],
										NULL,0,iFlags,(NAP_UINT32)iLoopId,(NAP_UINT32)iCountLoop,\
										&iNumOfBytesSent,&sError);
							*/
							if (iNumOfBytesSent == (NAP_UINT32)pstModule->uProtocol.stTcp.iDataLen[iLoopId])
							{

							   pstModule->uProtocol.stTcp.iWriteRetranCount[iLoopId] = 0;
							   FWK_Free(&(pstModule->uProtocol.stTcp.pucData[iLoopId]));
							   pstModule->uProtocol.stTcp.iDataLen[iLoopId] =0;
							   FD_CLR((NAP_UINT32)pstModule->uProtocol.stTcp.iDataSocket[iLoopId],\
											&gstWriteDesc);
							}
							else
							{
							    
							   FD_CLR((NAP_UINT32)\
								pstModule->uProtocol.stTcp.iDataSocket[iLoopId],\
								&gstWriteDesc);								
							   FWK_Free(&(pstModule->uProtocol.stTcp.pucData[iLoopId]));
							   pstModule->uProtocol.stTcp.iDataLen[iLoopId] =0;


							   pstModule->uProtocol.stTcp.iWriteRetranCount[iLoopId] = 0;

							   printf("\nAN RX Thread(2) :: Received Failed - B4 access_nw_UpdateModule \n");
							   /*
							      access_nw_UpdateModule(\
									pstModule->uProtocol.stTcp.iDataSocket[iLoopId],\
									iLoopId,iCountLoop,&sError);
							   */
							   iIndex--;
							}

						    } // write data
					        }
					  } // valid socket id
				     }
				}//for
			   } // data check
			} // TCP
			if ((pstModule->bTransport == 0 && pstModule->bModuleInitStatus == NAP_FALSE &&
						pstModule->uProtocol.stTcp.uiNumSockets ==0) ||
						(pstModule->bTransport == 1 && pstModule->bModuleInitStatus == NAP_FALSE))
			{
			   printf("\n%s:%d Deleting access nw module node no of active modules = %d", __FUNCTION__,\
						 __LINE__,pstAccessNw->iActiveModules);
			   FWK_LIST_DeleteNode(&pstAccessNw->pstModLst,(NAP_INT16)iCountLoop,
								&pstModule,&sError);

			   FWK_Free (&pstModule);
			   /* Update the number of active modules.*/
			   pstAccessNw->iActiveModules--;
			   iIndex--;
			}
			else
			{
			   iCountLoop++;
			}

		    }//while
		    break;
		}//switch

	        //printf("\nACCESS NETWORK - END OF SWITCH , Line:%d",__LINE__);

	        if (pstAccessNw != NAP_NULL)
	        {
		   if (pstAccessNw->iActiveModules ==0)
		   {
			printf("\nACCESS NETWORK - UNLOCKING");

			printf("\nAN Recv Thread :: Num of ActiveModule is 0\n");
			iMutex = pthread_mutex_unlock(&stConditionMutex);
			access_nw_DeInit(0,0,&sError);
			printf("\nACCESS_Network_Receive_Thread is killed [on pstAccessNw->iActiveModules ==0]");
			pthread_exit(NULL);
		   }
		}
		//printf("\nACCESS NETWORK - UNLOCKING");
		iMutex = pthread_mutex_unlock(&stConditionMutex);
	     }
	     else // pstAccessNW is NULL
	     {
		 printf("\nACCESS NETWORK - UNLOCKING");
		 iMutex = pthread_mutex_unlock(&stConditionMutex);
		 pthread_exit(NULL);
	     }

	}// while

	printf("END::Receive thread\n");
	return NAP_SUCCESS;

}
