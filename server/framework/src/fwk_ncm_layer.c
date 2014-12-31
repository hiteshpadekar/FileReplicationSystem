/*******************************************************************************
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef FD_SETSIZE
#undef FD_SETSIZE
#endif
#define FD_SETSIZE  40000

#include "fwk_ncm_net.h"
#include "fwk_ncm_layer.h"
#include "pthread.h"

/* Extern variables */
extern fd_set gstReadDesc;
extern fd_set gstWriteDesc;
//extern fd_set gstExceptDesc;
extern struct _S_NAP_ACCESS_NETWORK *pstAccessNw;


NAP_UINT16 usMaxBufferLen;					// Max data buffer.
NAP_INT32 iMaxDatagramLen;					// Max datagram size.

/******************************************************************************
* Function name	:	FWK_Ncm_AddModuleTo_AccessNetwork.
* Description	:	This routine is accessible only within access network.This
					routine creates a node for the module.And adds the node
					to the list after intializing the newly created node.
* Return type	:	NAP_VOID *
* Argument      : 	[IN_OUT] pstAccessNw : Access network structure.
					[IN] bIPAddress : module is TCP server: IP Address on which
					the module will listen for client connections.
									  module is TCP client: IP address of the
					server to which	the module gets connected.
									  module is UDP module: IP address used for
					data transmission.
					[IN] iModuleId : Module Id.
					[IN] iProtocol: Protocol type.
					[IN] bModuleType: Whether the module is TCP client/server or
					UDP module.
					[IN] bTransPort: Transport protocol TCP/UDP.
					[IN] iNumClients : Number of clients supported in case of
					the module is server.
					[OUT] piIndex : Node position.
					[IN] uiModuleThreadId : Thread id of the application.
					[IN] usPort : module is TCP server: port on which the module
					will listen for client connections.
									module is TCP client: port of the server to
					which	the module gets connected.
								  module is UDP module: port used for data
					transmission.
					[IN] pvData: If any additional data present.
					[IN] psError : Error type.
* Side Effects	: 	None
* NOTE 			: 	None
******************************************************************************/
NAP_BOOL FWK_Ncm_AddModuleTo_AccessNetwork(
								IN_OUT  S_NAP_ACCESS_NETWORK *pstAccessNw,
								IN  NAP_UBYTE *bIpAddress,
								IN  NAP_INT32 iModuleId,
								IN  NAP_UINT32 uProtocol,
								IN  NAP_BYTE bModuleType,
								IN  NAP_BYTE bTransPort,
								IN  NAP_INT32  iNumOfClients,
								OUT NAP_INT32 *piIndex,
								IN  NAP_UINT32 uiModuleThreadId,
								IN  NAP_UINT16 usPort,
								IN	NAP_VOID *pvData,
								OUT NAP_INT16 *psError)
{

	NAP_UINT16 usCount	= 0;
	NAP_INT32 iIndex = 0;			   /* Loop Index.*/
	NAP_UCHAR *pIPAddress  = NAP_NULL; /* IP Address. */
	NAP_UCHAR ucIPAddr[20] = {0};	/* IP Address. */
	NAP_BOOL bStatus = NAP_FALSE;	/* Status information. */
	S_MODULE_NODES *pstModule = NAP_NULL;	/* Module information structure.*/


	*psError = 0;
	 pIPAddress = ucIPAddr;

	// access network is not initialized or some error occured.
	if (pstAccessNw == NAP_NULL)
	{
		*psError = e_Err_AccessNw_NetworkInvalidParams;
		return NAP_FAILURE;
	}
	/* Update IP Address.*/
	FWK_Lnx_NetV4NtoA((NAP_UCHAR*)bIpAddress,&pIPAddress,4);

        printf("\n %s: IP Address: %s, bIpAddress = %d", __FUNCTION__, pIPAddress, *(NAP_UINT32 *)bIpAddress);

	/* Allocate the memory for module information structure.*/
	pstModule = (S_MODULE_NODES *) FWK_Malloc(sizeof(S_MODULE_NODES));
	if (pstModule == NAP_NULL)
	{
		/* Memory allocation failed.*/
		*psError = e_Err_Fwk_MallocFailed;
		return NAP_FAILURE;
	}

	/* SIM_PF_UT */
	FWK_Memset(pstModule,0,sizeof(S_MODULE_NODES));
	/* Module is yet to initialized.*/
	pstModule->bModuleInitStatus = NAP_FALSE;
	pstModule->bTransport  = bTransPort;		/* Protocol type.*/
	pstModule->bModuleType = bModuleType;		/* Server/client*/
	pstModule->iModuleId   = iModuleId;			/* Module id*/
	pstModule->iMaxMsgSize = 0;					/* future use.*/
	pstModule->uiThreadId  = uiModuleThreadId;	/* thread id.*/
	pstModule->uProtocolName = uProtocol;
	pstModule->uIPAddress = *(NAP_UINT32 *)bIpAddress;//inet_aton(pIPAddress);


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

			FWK_Memset(&pstModule->uProtocol.stTcp.cChildIpAddr[iIndex],0,IP_ADDR_LEN);
		}
		pstModule->uProtocol.stTcp.uiTotActiveClients = 0;
		pstModule->uProtocol.stTcp.uiNumSockets = 0;
		pstModule->uProtocol.stTcp.usPort = usPort;
	}
	else	/* Protocol is UDP. Initialize UDP parameters*/
	{
		pstModule->uProtocol.stUdp.iDataSocket = 0;
		pstModule->uProtocol.stUdp.usPort = usPort;
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
	*piIndex = usCount;
	pstModule->pvData = pvData;


	return bStatus;
}

/******************************************************************************
* Function name	:	FWK_Ncm_Init_NetworkComponent.
* Description	:	This is a local routine.This routine is accessible only
					within access network module.This routine initializes
					the network component.
					Ex: If the module is TCP server parent socket will be
					created	and will start listening for connect request from
					the clients.
						If the module is TCP client  data socket is created
					and will be connected to the server.
						if the module is UDP module socket created and will be
					bound to accept the data.
					i.e, This invokes the routines
					1.	to create TCP/UDP sockets.
					2.	to bind the TCP/UDP sockets.
					3.	to listen on TCP socket.
					4.  to connect using TCP socket.
* Return type	:	<NAP_BOOL>
					NAP_SUCCESS on successfull initialization of network
					component of specified module.
					NAP_FAILURES on network initialization fails for a specified
					network module.
* Argument      : 	[IN_OUT] pstAccessNw : Contains IP address,Port information,
					Module information for initialization of network components.
					[OUT] psError : Error type.
* Side Effects	: 	None.
* NOTE 			: 	None.
******************************************************************************/
NAP_BOOL	FWK_Ncm_Init_NetworkComponent(
									IN_OUT  S_NAP_ACCESS_NETWORK *pstAccessNw,
									IN	NAP_INT32 iIndex,
									OUT	NAP_INT32 *piSocket,
									OUT	NAP_INT16 *psError)
{

	NAP_UINT32 uiIndex = 0;			/* Index used in loops.*/
	NAP_BOOL bStatus = NAP_FAILURE;	/* Network initialization status.*/
	S_MODULE_NODES *pstModule = NAP_NULL;

	/* Invalid function parameters.*/
	if (psError == NAP_NULL )
		return NAP_FAILURE;

	*psError = 0;

	/* Access the node.*/
	if ( FWK_LIST_AccessNode(pstAccessNw->pstModLst,(NAP_INT16)iIndex,\
		&pstModule,psError) == NAP_FAILURE)
	{
	   printf("\n%s: Can Not find node");
	   return NAP_FAILURE;
	}
	/*
	 * Module node is new module in Access network structure for which we need
	 * to do network initialization.Create the sockets for specified module.
	 */
	if (pstModule->bModuleInitStatus == NAP_FALSE)
	{
		/* Transport protocol selected is TCP.*/
		if (pstModule->bTransport == 0)
		{

			printf("\n%s: TCP Socket",__FUNCTION__);
			/* Create and bind,listen/connect the TCP sockets.*/
			bStatus = FWK_Ncm_Create_TcpSocket(pstModule,piSocket,psError);
			if (bStatus == NAP_SUCCESS)
			{
				printf("\n%s: Create socket successful",__FUNCTION__);
				if (pstModule->bModuleType == 1)
				{
					/* Module selected is server */
					printf("\n%s: go for bind n listen",__FUNCTION__);
					bStatus = FWK_Ncm_BindListen_TcpSocket(pstModule,psError);
				}
				else
					/* Module selected is client*/
				{
					printf("\n%s: TCP Client socket",__FUNCTION__);
					bStatus = FWK_Ncm_Connect_TcpSocket(pstModule,psError);
					if (bStatus == NAP_SUCCESS)
					{
						printf("\n%s: Connect success",__FUNCTION__);
						FD_SET ((NAP_UINT32)pstModule->uProtocol.stTcp.iDataSocket[0],
							&gstReadDesc);
						//FD_SET ((NAP_UINT32)pstModule->uProtocol.stTcp.iDataSocket[0],&gstExceptDesc);
					}
					printf("\n%s: Connect failed",__FUNCTION__);
				}
			}
		}
		else
		{
				/* Create and bind the UDP sockets.*/
			bStatus = FWK_Ncm_Create_UdpSocket(pstModule,piSocket,psError);
			if (bStatus == NAP_SUCCESS)
				bStatus = FWK_Ncm_Bind_UdpSocket(pstModule,psError);
		}
		/* Module specified is initialized successfully.*/
		if (bStatus == NAP_SUCCESS)
		{
			pstModule->bModuleInitStatus = NAP_TRUE;

			printf("\n%s: Init module successful",__FUNCTION__);
			/* Update the node.*/
			FWK_LIST_UpdateNode(pstAccessNw->pstModLst,(NAP_INT16)iIndex,
				(NAP_VOID*)pstModule,psError);
		}
	}
	printf("\n%s: Exiting",__FUNCTION__);
	return bStatus;
}

/******************************************************************************
* Function name	:	FWK_Ncm_Create_TcpSocket.
* Description	:	This is a local routine.This routine is accessible only
					within access network module.This routine creates TCP
					sockets for the specified module.
* Return type	:	<NAP_BOOL>
					NAP_SUCCESS on successful socket creation.
					NAP_FAILURE on failure of socket creation.
* Argument      : 	[IN_OUT] pstModule : Contains information about type of
					sockets to	be created.
					[OUT] psError : Error type.
* Side Effects	: 	None
* NOTE 			: 	None
******************************************************************************/
NAP_BOOL	FWK_Ncm_Create_TcpSocket(IN_OUT S_MODULE_NODES *pstModule,
							OUT	NAP_INT32 *piSocket,
							OUT	NAP_INT16 *psError)
{
	NAP_INT16 sAppId = 0;
	NAP_INT32 iSockId = -1;

	/* Invalid function parameters.*/
	if (psError == NAP_NULL )
		return NAP_FAILURE;

	*psError = 0;
	/*
	 * Create the socket using  which module can accept connection. or using
	 * which module can to connect to server.
	 */
	FWK_Lnx_Socket(&iSockId,AF_INET,SOCK_STREAM,IPPROTO_TCP,psError);

	/* Socket creation failed.*/
	if(iSockId < 0)
	{
		*psError = e_Err_AccessNw_NetworkFail;
		return NAP_FAILURE;
	}
	/* Socket created can accept the connection from client modules*/
	if (pstModule->bModuleType !=0)
		pstModule->uProtocol.stTcp.iListenSocket = iSockId;
	else
		/* Socket created can connect to server module.*/
		pstModule->uProtocol.stTcp.iDataSocket[0] = iSockId;

	/* Update the number of TCP sockets available */
	pstModule->uProtocol.stTcp.uiNumSockets = 1;
	*piSocket = iSockId;
	return NAP_SUCCESS;
}

/*******************************************************************************
* Function name	:	FWK_Ncm_Create_UDPSocket
* Description	:	This is a local routine.This routine is accessible only
					within access network module.This routine creates UDP
					sockets for the module specified.
* Return type	:	<NAP_BOOL>
					NAP_SUCCESS on successfull socket creation.
					NAP_FAILURE on failure of socket creation.
* Argument      : 	[IN_OUT ] pstModule : module for which socket is to be
					created.
					[OUT] psError : Error type.
* Side Effects	: 	None
* NOTE 			: 	None
********************************************************************************/
NAP_BOOL	FWK_Ncm_Create_UdpSocket(IN_OUT S_MODULE_NODES *pstModule,
							OUT	NAP_INT32 *piSocket,
							OUT NAP_INT16 *psError)
{
	NAP_INT16 sAppId = 0;
	unsigned int uiMms = 0;
	int  iSize = sizeof (uiMms);

	*psError = 0;
	/* Create UDP socket.*/
	FWK_Lnx_Socket(&pstModule->uProtocol.stUdp.iDataSocket,AF_INET,\
				SOCK_DGRAM,IPPROTO_UDP,psError);

	getsockopt(pstModule->uProtocol.stUdp.iDataSocket,SOL_SOCKET,
				SO_SNDBUF, (char *)&uiMms, &iSize);

	iMaxDatagramLen = uiMms;

	/* Socket creation failed.*/
	if(pstModule->uProtocol.stUdp.iDataSocket <0)
	{
		*psError = e_Err_AccessNw_NetworkFail;
		return NAP_FAILURE;
	}
	*piSocket = pstModule->uProtocol.stUdp.iDataSocket;
	return NAP_SUCCESS;
}

/*******************************************************************************
* Function name	:	FWK_Ncm_BindListen_TcpSocket
* Description	:	This is a local routine.This routine is accessible only
					within access network module. This routine binds the
					specified address and port of the module. If the binding is
					successfull then socket waits for client connection requests.
* Return type	:	<NAP_BOOL>
					NAP_SUCCESS on successful bind and listen operation.
					NAP_FAILURE on failure of bind and listen operation.
* Argument      : 	[IN_OUT] pstAccessNw : module for which socket bind and listen
					needs to be done.
					[OUT] psError : Error type.
* Side Effects	: 	None
* NOTE 			: 	None
********************************************************************************/
NAP_BOOL	FWK_Ncm_BindListen_TcpSocket(IN_OUT S_MODULE_NODES *pstModule,
								OUT NAP_INT16 *psError)
{
	struct sockaddr_in  stServAddr;
	int opt =1;
	*psError = 0;
	printf("\n%s: Entering",__FUNCTION__);
	FWK_Memset(&stServAddr,0,sizeof(struct sockaddr_in));
	/* Prepare the address structure.*/
	stServAddr.sin_family = AF_INET;
	stServAddr.sin_addr.s_addr = pstModule->uIPAddress;
	stServAddr.sin_port = pstModule->uProtocol.stTcp.usPort;

	printf("\n%s:bind on: uIPAddres = %d, port = %d",__FUNCTION__,(pstModule->uIPAddress),\
		       				(pstModule->uProtocol.stTcp.usPort));

	//setsockopt(pstModule->uProtocol.stTcp.iListenSocket, SOL_SOCKET,SO_REUSEADDR,(char*)&opt,sizeof(int));

	/* Bind the socket.*/
	if(FWK_Lnx_Bind(pstModule->uProtocol.stTcp.iListenSocket,
									(struct sockaddr *)&stServAddr,
					sizeof(struct sockaddr_in),	psError	) == NAP_FAILURE)
	{
		/* Bind failed.*/
		*psError = e_Err_AccessNw_NetworkFail;
		printf("\n%s: BIND FAILED",__FUNCTION__);
		switch(*psError)
		{
			case EADDRNOTAVAIL:
			case EFAULT:
			case EINVAL:
				/* Close the socket.*/
				FWK_Lnx_Close(pstModule->uProtocol.stTcp.iListenSocket,
					psError);
				pstModule->uProtocol.stTcp.uiNumSockets--;
				break;
		}
		return NAP_FAILURE;
	}
	/* Listen on the socket.*/
	if (FWK_Lnx_Listen(pstModule->uProtocol.stTcp.iListenSocket,
				pstModule->uProtocol.stTcp.uiNumOfClients,psError)
				== NAP_FAILURE)
	{
		/* Listen failed.*/
		printf("\n%s: LISTEN FAILED",__FUNCTION__);
		*psError = e_Err_AccessNw_NetworkFail;
		switch(*psError)
		{
			case EINVAL:
			case EMFILE:
			case ENOBUFS:
			case ENOTSOCK:
			case EOPNOTSUPP:
				/* Close the socket.*/
				FWK_Lnx_Close(pstModule->uProtocol.stTcp.iListenSocket,
					psError);
				pstModule->uProtocol.stTcp.uiNumSockets--;
			break;
		}
		return NAP_FAILURE;
	}
	/* Set the socket descriptor in fd_set for read event*/
	FD_SET ((NAP_UINT32)pstModule->uProtocol.stTcp.iListenSocket,&gstReadDesc);
	//FD_SET ((NAP_UINT32)pstModule->uProtocol.stTcp.iListenSocket,&gstExceptDesc);
	printf("\n%s: EXITING",__FUNCTION__);
	return NAP_SUCCESS;
}

/*******************************************************************************
* Function name	:	FWK_Ncm_Connect_TcpSocket
* Description	:	This is a local routine.This routine is accessible only
					within access network module.This routine connect to
					specified server address and port.
* Return type	:	<NAP_BOOL>
					NAP_SUCCESS on successful connection to server.
					NAP_FAILURE on failure of connection to server.
* Argument      : 	[IN_OUT] pstModule : module which is trying to connect to
					server.
					[OUT] psError : Error type.
* Side Effects	: 	None
* NOTE 			: 	None
********************************************************************************/
NAP_BOOL	FWK_Ncm_Connect_TcpSocket(IN_OUT S_MODULE_NODES *pstModule,
								OUT NAP_INT16 *psError)
{

	struct sockaddr_in  stServAddr;
	//struct timespec stTime;

	*psError = 0;
	FWK_Memset(&stServAddr,0,sizeof(struct sockaddr_in));
	//FWK_Memset(&stTime,0,sizeof(struct timespec));

	//stTime.tv_sec = 0;
	//stTime.tv_nsec = 2;
	/* Prepare the address structure.*/
	stServAddr.sin_family = AF_INET;
	stServAddr.sin_addr.s_addr = pstModule->uIPAddress;
	stServAddr.sin_port = pstModule->uProtocol.stTcp.usPort;
	/* Connect the socket.*/
	printf("\n%s:connect on: uIPAddres = %d, port = %d",__FUNCTION__,(pstModule->uIPAddress),\
		       				(pstModule->uProtocol.stTcp.usPort));
	if(FWK_Lnx_Connect(pstModule->uProtocol.stTcp.iDataSocket[0],
				(const struct sockaddr*)&stServAddr,sizeof(struct sockaddr_in),
				psError) == NAP_FAILURE)
	{
		/* Connect failed.*/
		printf("%s: Connect Failed", __FUNCTION__);
		*psError = e_Err_AccessNw_NetworkFail;
		switch(*psError)
		{
			case EADDRNOTAVAIL:
			case EAFNOSUPPORT:
			case ECONNREFUSED:
			case EFAULT:
			case ENETUNREACH:
			case EHOSTUNREACH:
			case ENOBUFS:
			case ETIMEDOUT:
				/* Close the socket.*/
				if (pstModule->uProtocol.stTcp.iDataSocket[0])
					FWK_Lnx_Close(pstModule->uProtocol.stTcp.iDataSocket[0],
					psError);
				pstModule->uProtocol.stTcp.uiNumSockets--;
			break;
		}
		return NAP_FAILURE;
	}
	//pthread_delay_np(&stTime);
	return NAP_SUCCESS;
}

/*******************************************************************************
* Function name	:	FWK_Ncm_Bind_UdpSocket
* Description	:	This is a local routine.This routine is accessible only
					within access network module.This routine binds specified
					address and port.
* Return type	:	NAP_BOOL
					NAP_SUCCESS on successful binding.
					NAP_FAILURE on binding  failure.
* Argument      : 	[IN] pstModule : module for which bind operation needs to be
					done.
					[OUT] psError : Error type.
* Side Effects	: 	None
* NOTE 			: 	None
********************************************************************************/
NAP_BOOL	FWK_Ncm_Bind_UdpSocket(IN S_MODULE_NODES *pstModule,
							OUT NAP_INT16 *psError)
{

	struct sockaddr_in  stServAddr;
	int opt =1;
	FWK_Memset(&stServAddr,0,sizeof(struct sockaddr_in));
	/* Prepare the address structure.*/
	stServAddr.sin_family = AF_INET;
	stServAddr.sin_addr.s_addr = htonl(pstModule->uIPAddress);
	stServAddr.sin_port = htons(pstModule->uProtocol.stUdp.usPort);

	/* Set the socket descriptor in fd_set for read event*/
	//setsockopt(pstModule->uProtocol.stUdp.iDataSocket, SOL_SOCKET,SO_REUSEADDR,(char*)&opt,sizeof(int));

	/* Bind the socket.*/
	*psError = 0;
	if(FWK_Lnx_Bind(pstModule->uProtocol.stUdp.iDataSocket,
				(struct sockaddr *)&stServAddr,	sizeof(struct sockaddr_in),
				psError) == NAP_FAILURE)
	{
		/* Bind failed.*/
		*psError = e_Err_AccessNw_NetworkFail;
		switch(*psError)
		{
			case EADDRNOTAVAIL:
			case EFAULT:
			case EINVAL:
				/* Close the socket.*/
				FWK_Lnx_Close(pstModule->uProtocol.stUdp.iDataSocket,\
					psError);
			break;
		}
		return NAP_FAILURE;
	}
	FD_SET((NAP_UINT32)pstModule->uProtocol.stUdp.iDataSocket,&gstReadDesc);
	//FD_SET ((NAP_UINT32)pstModule->uProtocol.stUdp.iDataSocket,&gstExceptDesc);
	return NAP_SUCCESS;
}

/*******************************************************************************
* Function name	:	FWK_Ncm_Send_Data
* Description	:	This is a local routine.This routine is accessible only
					within access network module.This routine is called when
					select system call is triggered by write event or when the
					application posts the data to transmit thread.This routine
					send the data on to network.
* Return type	:	<NAP_BOOL>
					NAP_SUCCESS on successfull data transfer on network.
					NAP_FAILURES on data transfer failure.
* Argument      :	[IN] pcData: Data to be sent on network.
					[IN] uLen: Data length
					[IN] pucIpAddress : Destination IP address,used only in case
						of UDP.
					[IN] usPort: Destination port used only in case of UDP.
					[IN] iFlags : Any instruction to be used with send socket call.
					[IN] uiDataSocketIndex: Socket desc postion in the list.
					[IN] uiModuleIndex:Module position in the list.
					[OUT] iNumOfBytesSent: Number of bytes sent on network.
					[OUT] psError: Error type.
* Side Effects	: 	None
* NOTE 			: 	None
********************************************************************************/
NAP_BOOL	FWK_Ncm_Send_Data(
					   IN  NAP_CHAR *pcData,
					   IN  NAP_UINT32 uLen,
					   IN  NAP_UCHAR *pucIpAddress,
					   IN  NAP_UINT16 usPort,
					   IN  NAP_INT32 iFlags,
					   IN  NAP_UINT32 uiDataSocketIndex,
					   IN  NAP_UINT32 uiModuleIndex,
					   OUT NAP_INT32 *iNumOfBytesSent,
					   OUT NAP_INT16 *psError)
{

	NAP_BOOL bNodeStatus = NAP_FAILURE;
	NAP_UINT32 uiDataIdx= uiDataSocketIndex;
	struct sockaddr_in stDestAddr;
	S_MODULE_NODES *pstModule = NAP_NULL;
	NAP_INT32 iSocketId = 0;
	S_FWK_LIST_NODE *pstList = NAP_NULL;
	S_NAP_UDP_INFO *pstUdpInfo = NAP_NULL;
	NAP_INT16	usCount = 0;
	NAP_INT16	usIndex = 1;



	/* Invalid function parameters*/
	if (psError == NAP_NULL)
		return NAP_FAILURE;

	if (iNumOfBytesSent == NAP_NULL)
	{
		*psError = e_Err_AccessNw_NetworkInvalidParams;
	
		return NAP_FAILURE;
	}
	*psError = 0;

	/* Access the node.*/
	if((!FWK_LIST_AccessNode(pstAccessNw->pstModLst,(NAP_INT16)uiModuleIndex,&pstModule,psError))
        || (NAP_NULL == pstModule))
    {
        *psError = e_Err_AccessNw_ConnAborted;
        return NAP_FAILURE;
    }

	if (pstModule->bTransport == 0)
	{	/* Data to be sent on TCP connection*/
		iSocketId = pstModule->uProtocol.stTcp.iDataSocket[uiDataSocketIndex];		
		FWK_Lnx_Send(pstModule->uProtocol.stTcp.iDataSocket[uiDataSocketIndex],\
							pcData,uLen,iFlags,iNumOfBytesSent,\
							psError);
	}
	else
	{
		/* Data to be sent on UDP connection*/
		FWK_Memset(&stDestAddr,0,sizeof(struct sockaddr_in));
		stDestAddr.sin_family = AF_INET;
		stDestAddr.sin_addr.s_addr =  inet_addr(pucIpAddress);
		stDestAddr.sin_port = htons(usPort);
		iSocketId = pstModule->uProtocol.stUdp.iDataSocket;
		FWK_Lnx_SocketSendTo(pstModule->uProtocol.stUdp.iDataSocket\
									,pcData,uLen,iNumOfBytesSent,iFlags,
									(struct sockaddr *)&stDestAddr,\
									sizeof(struct sockaddr_in),psError);
	}
		
	/* UDP socket error.*/
	if (*psError >0 && pstModule->bTransport == 1)
	{
		switch(*psError)
		{
			case	e_Err_AccessNw_WinsockNotInitialized :
			case	e_Err_AccessNw_PermissionDenied:
			case	e_Err_AccessNw_InvalidArg:
			case	e_Err_AccessNw_OpnInterrupted:
			case	e_Err_AccessNw_OpnInProgress:
			case	e_Err_AccessNw_BadAddr:
			case	e_Err_AccessNw_NoBufs:
			case	e_Err_AccessNw_SocketNotConnected:
			case	e_Err_AccessNw_InvalidSock:
			case	e_Err_AccessNw_OpNotSupported:
			case	e_Err_AccessNw_CannotSendAfterShutdown:
			case	e_Err_AccessNw_MessageBig:
			case	e_Err_AccessNw_HostNotReachable:
			case	e_Err_AccessNw_AddrNotAvailable:
			case	e_Err_AccessNw_AddressFamilyNotSupported:
			case	e_Err_AccessNw_DestAddrRequired:
			case	e_Err_AccessNw_NetWorkNotReachable:
					FWK_Printf("\nFWK_Ncm_Send_Data(UDP) :: Failure-1 \n");
					return NAP_SUCCESS;
			break;			
			case	e_Err_AccessNw_NetworkDown:
			case	e_Err_AccessNw_NewworkReset:
			case	e_Err_AccessNw_ConnAborted:
			case	e_Err_AccessNw_ConnReset:
			case	e_Err_AccessNw_TimeOut:
				/* Close the socket and update the network structure.*/
					FWK_Printf("\nFWK_Ncm_Send_Data(UDP) :: Failure-2 \n");
					return NAP_FAILURE;
			break;
			case	e_Err_AccessNw_OpnBlocked:
				// Data should be sent in chunks.
#if 0 // SIM_PF_UT
				if ((unsigned int)*iNumOfBytesSent != uLen && uLen>0 )
				{
					FWK_Printf("\nNCM Send :: Pending \n");
 
					// find the node
					FWK_LIST_GetCount(pstModule->uProtocol.stUdp.pstUdpList,&usCount,psError);
					if (usCount >=1)
						pstList = pstModule->uProtocol.stUdp.pstUdpList->pstHead;
					while (usIndex <= usCount)
					{
						pstUdpInfo = (S_NAP_UDP_INFO *)pstList->pvData ;
						if (pstUdpInfo->usDestPort == usPort &&
							FWK_Strcmp(pucIpAddress,pstUdpInfo->cDestIpAddr)==0)
						{
							// data cannot be sent.
							if (pstUdpInfo->iWriteRetranCount > MAX_RETRAN)
							{
								*psError = e_Err_AccessNw_NetworkMaxRetrans;
								FD_CLR((NAP_UINT32)pstModule->uProtocol.stUdp.iDataSocket,\
											&gstWriteDesc);
								FWK_LIST_DeleteNode(
									&pstModule->uProtocol.stUdp.pstUdpList,
									usIndex,(NAP_VOID **)pstUdpInfo,psError);
								return NAP_FAILURE;

							}							
							break;
						}
						usIndex++;
					}
					// node is available or created successfully.
					if (pstUdpInfo != NAP_NULL)
					{
						if (pstUdpInfo->iWriteRetranCount ==0)
							pstUdpInfo->pucData = (NAP_UCHAR*)\
							FWK_Malloc((NAP_UINT16)((uLen-*iNumOfBytesSent) +1));
						if (pstUdpInfo->pucData == NAP_NULL)
						{	// Memory allocation failed.
							if (bNodeStatus == NAP_TRUE)
								FWK_LIST_DeleteNode(&pstModule->uProtocol.stUdp.pstUdpList,
								usIndex,(NAP_VOID **)pstUdpInfo,psError);
							else
								FWK_Free(&pstUdpInfo);
							*psError = e_Err_Fwk_MallocFailed;
							return NAP_FAILURE;
						}
						else
						{
							// Copy the data remaining and should be sent
							FWK_Memset(pstUdpInfo->pucData,
								0,(NAP_INT16)(uLen-*iNumOfBytesSent));
							FWK_Memcpy(pstUdpInfo->pucData,\
								&pcData[*iNumOfBytesSent],\
								(NAP_UINT16)(uLen-*iNumOfBytesSent));
							pstUdpInfo->iDataLen = uLen-*iNumOfBytesSent;
							pstUdpInfo->pucData[pstUdpInfo->iDataLen] = '\0';
							FD_SET((NAP_UINT32)iSocketId,&gstWriteDesc);
							// Increment the retransmission count.
							pstUdpInfo->iWriteRetranCount++;
						}						
						FWK_LIST_UpdateNode(pstModule->uProtocol.stUdp.pstUdpList,
							usIndex,(NAP_VOID*)pstUdpInfo,psError);						
					}
				}
#endif // SIM_PF_UT 
				
				FWK_Printf("\nFWK_Ncm_Send_Data(UDP) :: OpnBlocked \n");

				if((unsigned int)*iNumOfBytesSent != uLen && uLen>0 )
				{
					FWK_Printf("\nFWK_Ncm_Send_Data(UDP) :: Pending\n");

					// find the node
					FWK_LIST_GetCount(pstModule->uProtocol.stUdp.pstUdpList,&usCount,psError);
					for(usIndex=1; usIndex <= usCount; usIndex++)
					{
						pstUdpInfo = NAP_NULL;

						FWK_LIST_AccessNode(pstModule->uProtocol.stUdp.pstUdpList,
												usIndex,&pstUdpInfo,psError);

						if (pstUdpInfo->usDestPort == usPort &&
							FWK_Strcmp(pucIpAddress,pstUdpInfo->cDestIpAddr)==0)
						{
							// data cannot be sent.
							if (pstUdpInfo->iWriteRetranCount > MAX_RETRAN)
							{
								*psError = e_Err_AccessNw_NetworkMaxRetrans;

								FD_CLR((NAP_UINT32)pstModule->uProtocol.stUdp.iDataSocket,&gstWriteDesc);


								FWK_LIST_DeleteNode(
									&pstModule->uProtocol.stUdp.pstUdpList,
									usIndex,&pstUdpInfo,psError);

								FWK_Free(&pstUdpInfo->pucData);
								FWK_Free(&pstUdpInfo);

								return NAP_FAILURE;
							}							
							break;
						}
					}// For loop

					// node is available or created successfully.
					if (pstUdpInfo)
					{
						NAP_UINT32	ulTotalLen;

						ulTotalLen = pstUdpInfo->iDataLen + (uLen-*iNumOfBytesSent);

						if(pstUdpInfo->iWriteRetranCount ==0)
						{
							pstUdpInfo->pucData = (NAP_UCHAR*)FWK_Malloc((NAP_UINT16)(MAX_ACCESS_NETWORK_SEND+1));
							FWK_Memset(pstUdpInfo->pucData,0,(NAP_UINT16)(MAX_ACCESS_NETWORK_SEND+1));
						}

						if( (pstUdpInfo->pucData == NAP_NULL) || (ulTotalLen < MAX_ACCESS_NETWORK_SEND))
						{

							FD_CLR((NAP_UINT32)pstModule->uProtocol.stUdp.iDataSocket,
											&gstWriteDesc);

							/* Either Malloc failed or pending data exceeded the LIMIT*/
							FWK_LIST_DeleteNode(&pstModule->uProtocol.stUdp.pstUdpList,
											usIndex,(NAP_VOID **)&pstUdpInfo,psError);

							FWK_Free(&pstUdpInfo->pucData);
							FWK_Free(&pstUdpInfo);
							*psError = e_Err_Fwk_MallocFailed;
							return NAP_FAILURE;
						}
						else
						{
							// Copy or append the remaining data
							FWK_Memcpy(
									&pstUdpInfo->pucData[pstUdpInfo->iDataLen],
									&pcData[*iNumOfBytesSent],
									(NAP_UINT16)(uLen-*iNumOfBytesSent));

							pstUdpInfo->iDataLen = ulTotalLen;
							pstUdpInfo->pucData[ulTotalLen] = '\0';

							FD_SET((NAP_UINT32)pstModule->uProtocol.stUdp.iDataSocket,&gstWriteDesc);

							pstUdpInfo->iWriteRetranCount++;
						}	
					}

				}// end of IF
			break;
		}	// switch
	 }// End of UDP

	/* TCP socket error.*/
	 if (*psError >0 && pstModule->bTransport == 0)
	 {
		switch(*psError)
		{
			case	e_Err_AccessNw_WinsockNotInitialized :
			case	e_Err_AccessNw_PermissionDenied:
			case	e_Err_AccessNw_InvalidArg:
			case	e_Err_AccessNw_OpnInterrupted:
			case	e_Err_AccessNw_OpnInProgress:
			case	e_Err_AccessNw_BadAddr:
			case	e_Err_AccessNw_NoBufs:
			case	e_Err_AccessNw_SocketNotConnected:
			case	e_Err_AccessNw_InvalidSock:
			case	e_Err_AccessNw_OpNotSupported:
			case	e_Err_AccessNw_CannotSendAfterShutdown:
			case	e_Err_AccessNw_MessageBig:
			case	e_Err_AccessNw_HostNotReachable:
			case	e_Err_AccessNw_AddrNotAvailable:
			case	e_Err_AccessNw_AddressFamilyNotSupported:
			case	e_Err_AccessNw_DestAddrRequired:
			case	e_Err_AccessNw_NetWorkNotReachable:
					return NAP_SUCCESS;
					break;			
			case	e_Err_AccessNw_NetworkDown:
			case	e_Err_AccessNw_NewworkReset:
			case	e_Err_AccessNw_ConnAborted:
			case	e_Err_AccessNw_ConnReset:
			case	e_Err_AccessNw_TimeOut:
				/* Close the socket and update the network structure.*/				
					FWK_Printf("\nFWK_Ncm_Send_Data(TCP) :: Failure \n");
				return NAP_FAILURE;
			break;
			case	e_Err_AccessNw_OpnBlocked:
					FWK_Printf("\nFWK_Ncm_Send_Data(TCP) :: OpnBlocked \n");
				if ((unsigned int)*iNumOfBytesSent != uLen && uLen>0)
				{
					FWK_Printf("\nFWK_Ncm_Send_Data(TCP) :: Pending\n");
					if (pstModule->uProtocol.stTcp.iWriteRetranCount[uiDataIdx] > MAX_RETRAN)
					{
						*psError = e_Err_AccessNw_NetworkMaxRetrans;
						return NAP_FAILURE;
					}
					if (pstModule->uProtocol.stTcp.iWriteRetranCount[uiDataIdx] ==0)
						pstModule->uProtocol.stTcp.pucData [uiDataIdx]=	\
							(NAP_UCHAR*)FWK_Malloc((NAP_UINT16)((uLen-*iNumOfBytesSent)+1));

					if (pstModule->uProtocol.stTcp.pucData [uiDataIdx] == NAP_NULL)
					{
						FD_CLR((NAP_UINT32)pstModule->uProtocol.stTcp.pucData [uiDataIdx],\
											&gstWriteDesc);
						pstModule->uProtocol.stTcp.iWriteRetranCount[uiDataIdx] = 0;
						*psError = e_Err_Fwk_MallocFailed;
						return NAP_FAILURE;
					}
					else
					{
						pstModule->uProtocol.stTcp.iWriteRetranCount[uiDataIdx]++;
						FWK_Memset(pstModule->uProtocol.stTcp.pucData[uiDataIdx],
							0,(NAP_INT16)(uLen-*iNumOfBytesSent));
						/* Copy the data remaining and should be sent */
						FWK_Memcpy(pstModule->uProtocol.stTcp.pucData [uiDataIdx],\
							&pcData[*iNumOfBytesSent],\
							(NAP_UINT16)(uLen-*iNumOfBytesSent));
						pstModule->uProtocol.stTcp.iDataLen[uiDataIdx] = \
							uLen-*iNumOfBytesSent;
						pstModule->uProtocol.stTcp.pucData [uiDataIdx][pstModule->uProtocol.stTcp.iDataLen[uiDataIdx]] = '\0';
						FD_SET((NAP_UINT32)iSocketId,&gstWriteDesc);
					}
			}
			break;
		}	// switch
	}
	return NAP_SUCCESS;
}

/*******************************************************************************
* Function name	:	FWK_Ncm_Receive_Data.
* Description	:	This is a local routine.This routine is accessible only
					within access network module.This routine is called when
					select system call is triggered by read event.This routine
					receive the data from network.
* Return type	:	<NAP_BOOL>
					NAP_SUCCESS on successfull data receive operation from
					network.
					NAP_FAILURES on data receive operation failure.
* Argument      :   [IN] iSockDesc: Socket on which data should be read.
					[OUT] *pcData: Buffer where the data read will be stored.
					[IN] iLen: Max data len.
					[IN] bProtocol: Protocol type (TCP/UDP)
					[IN] iFlags : Flags for recv socket call.
					[OUT] *piNumOfBytesRcvd: Number of bytes which is read,
					[OUT] *pstDestAddr:	Destination from where data is read,
							used in case of UDP.
					[OUT] *piLen : struct sockaddr_in len.
					[OUT] *psError: Error type.
* Side Effects	: 	None
* NOTE 			: 	None
********************************************************************************/
NAP_BOOL FWK_Ncm_Receive_Data( NAP_INT32 iSocketId,
						NAP_CHAR *pcData,
						NAP_INT32 iLen,
						NAP_BYTE  bProtocol,
						NAP_INT32 iFlags,
						NAP_INT32 *piNumOfBytesRcvd,
						struct sockaddr *pstDestAddr,
						NAP_INT32 *piLen,
						NAP_INT32 iIndex,
						NAP_INT16 *psError)
{

	S_MODULE_NODES *pstModule = NAP_NULL;
	NAP_INT32 iIdx = 0;

	/* Invalid function parameters*/
	if (psError == NAP_NULL)
		return NAP_FAILURE;

	if (pcData == NAP_NULL || piNumOfBytesRcvd == NAP_NULL)
	{
		*psError = e_Err_AccessNw_NetworkInvalidParams;
		return NAP_FAILURE;
	}


	*psError = 0;
	/* Get the module node from the list*/
	FWK_LIST_AccessNode(pstAccessNw->pstModLst,(NAP_INT16)iIndex,&pstModule,psError);
	/* TCP protocol.*/
	if (bProtocol == 0)
	{
		FWK_Lnx_Recv(iSocketId,pcData,iLen,iFlags,piNumOfBytesRcvd,psError);	
	}
	else
	{	/* Invalid function parameters*/
		if (pstDestAddr == NAP_NULL || piLen == NAP_NULL)
		{
			*psError = e_Err_AccessNw_NetworkInvalidParams;
			return NAP_FAILURE;
		}
		FWK_Lnx_SocketRecieveFrom(iSocketId,pcData,iLen,piNumOfBytesRcvd,
			iFlags,pstDestAddr,piLen,psError);
	}

	if (*psError >0  && bProtocol == 1)
	{

		switch(*psError)
		{	
			case	e_Err_AccessNw_WinsockNotInitialized :
			case	e_Err_AccessNw_PermissionDenied:
			case	e_Err_AccessNw_InvalidArg:
			case	e_Err_AccessNw_OpnInterrupted:
			case	e_Err_AccessNw_OpnInProgress:
			case	e_Err_AccessNw_BadAddr:
			case	e_Err_AccessNw_NoBufs:
			case	e_Err_AccessNw_SocketNotConnected:
			case	e_Err_AccessNw_InvalidSock:
			case	e_Err_AccessNw_OpNotSupported:
			case	e_Err_AccessNw_CannotSendAfterShutdown:			
			case	e_Err_AccessNw_HostNotReachable:
			case	e_Err_AccessNw_AddrNotAvailable:
			case	e_Err_AccessNw_AddressFamilyNotSupported:
			case	e_Err_AccessNw_DestAddrRequired:
			case	e_Err_AccessNw_NetWorkNotReachable:
			case	e_Err_AccessNw_SockAlreadyConnected:
				return NAP_SUCCESS;
			break;			
			case	e_Err_AccessNw_NetworkDown:
			case	e_Err_AccessNw_NewworkReset:
			case	e_Err_AccessNw_TimeOut:
				/* Close the socket and update the network structure.*/
				return NAP_FAILURE;
			break;
			case	e_Err_AccessNw_OpnBlocked:
			case	e_Err_AccessNw_MessageBig:
					return NAP_SUCCESS;
			break;
		}
	}

	if (*psError >0 && bProtocol ==0)
	{
		switch(*psError)
		{
			case	e_Err_AccessNw_WinsockNotInitialized :
			case	e_Err_AccessNw_PermissionDenied:
			case	e_Err_AccessNw_InvalidArg:
			case	e_Err_AccessNw_OpnInterrupted:
			case	e_Err_AccessNw_OpnInProgress:
			case	e_Err_AccessNw_BadAddr:
			case	e_Err_AccessNw_NoBufs:
			case	e_Err_AccessNw_SocketNotConnected:
			case	e_Err_AccessNw_InvalidSock:
			case	e_Err_AccessNw_OpNotSupported:
			case	e_Err_AccessNw_CannotSendAfterShutdown:			
			case	e_Err_AccessNw_HostNotReachable:
			case	e_Err_AccessNw_AddrNotAvailable:
			case	e_Err_AccessNw_AddressFamilyNotSupported:
			case	e_Err_AccessNw_DestAddrRequired:
			case	e_Err_AccessNw_NetWorkNotReachable:			
				return NAP_SUCCESS;
			break;			
			case	e_Err_AccessNw_NetworkDown:
			case	e_Err_AccessNw_NewworkReset:
			case	e_Err_AccessNw_ConnAborted:
			case	e_Err_AccessNw_ConnReset:
			case	e_Err_AccessNw_TimeOut:
			case	e_Err_AccessNw_SockAlreadyConnected:
				/* Close the socket and update the network structure.*/
				return NAP_FAILURE;
			break;
			case	e_Err_AccessNw_OpnBlocked:
			case	e_Err_AccessNw_MessageBig:
				for (iIdx = 0 ; iIdx < MAX_CLIENTS_SUPPORTED ; iIdx++)
				{
					if (pstModule->uProtocol.stTcp.iDataSocket[iIdx] == iSocketId)
					{
						if (pstModule->uProtocol.stTcp.iReadRetranCount[iIdx] > MAX_RETRAN)
						{
							pstModule->uProtocol.stTcp.iReadRetranCount[iIdx] = 0;
							return NAP_FAILURE;
						}
						else
						{
							pstModule->uProtocol.stTcp.iReadRetranCount[iIdx]++;
							FD_SET((NAP_UINT32)iSocketId,&gstReadDesc);
						}
					}
				}
			break;
		}
	}
	return NAP_SUCCESS;
}


