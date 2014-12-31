/*******************************************************************************
*******************************************************************************/

#ifndef FWK_NCM_LAYER_H
#define FWK_NCM_LAYER_H

#include "fwk_list.h"
#include "fwk_datatypes.h"
#include "fwk_error.h"
#include "fwk_file.h"
#include "fwk_memory.h"
#include "fwk_string.h"
#include "fwk_rtos.h"
#include "fwk_trace.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>

/****************************Macros******************************************/

#define MAX_ACCESS_NETWORK_SEND	8192		// Max data buffer.

#define MAX_ACCESS_NETWORK_RECV	8192		// Max data buffer.
#define MAX_CLIENTS_SUPPORTED	10			// MAx client supported/server socket.
#define IP_ADDR_LEN	16						// IP address length.
#define MAX_RETRAN  2						// Number of retransmission count.


extern NAP_UINT16 usMaxBufferLen;					// Max data buffer.
extern NAP_INT32 iMaxDatagramLen;					// Max datagram size.

// Module type.
typedef enum _E_NAP_MODULE
{
	eNapClient  =0,		//client module/udp module.
	eNapServer			//server module.
}E_NAP_MODULE;

// Transport protocol type.
typedef enum _E_NAP_PROTOCOL
{
	eNapTcp  =0,		//TCP
	eNapUdp				//UDP
}E_NAP_PROTOCOL;


//This is Protocol related information strcuture for each module.Depending on
//TCP or UDP,the union will be filled
typedef struct _S_MODULE_NODES
{
	NAP_BOOL	bModuleInitStatus;	/* Module init status */
	NAP_INT32	iModuleId;			/* Module id*/
	NAP_UINT32	uProtocolName;		/* Protocol assoicate with any module.*/
	NAP_UINT32	uIPAddress;			/* Destination IP Address */
	NAP_BYTE    bTransport;			/* TCP/UDP protocol*/
	NAP_INT32    iMaxMsgSize;		/* Max message size supported. This for
									   future use only.*/
	NAP_UINT32   uiThreadId;		/* Module thread Id.*/
	NAP_BYTE	 bModuleType;		/* Whether the module is TCP client/server
									   or UDP.*/
	NAP_VOID	*pvData;			/* Any additional data.*/

	union
	{
			// udp protocol
		struct
		{
			NAP_UINT16	usPort;		 /* UDP Port*/
			NAP_INT32	iDataSocket; /* UDP Socket*/
			S_FWK_LIST	*pstUdpList; /* Remote host info.*/
		}stUdp;

		// tcp protocol
		struct
		{
			NAP_UINT32	uiNumOfClients;			/*Number of Clients */
			NAP_UINT16	usPort;					/*TCP Port */
			NAP_INT32
			iDataSocket[MAX_CLIENTS_SUPPORTED];	/*List of Child Sockets*/
			NAP_CHAR
			cChildIpAddr[MAX_CLIENTS_SUPPORTED][16]; /*Child IP Address*/
			NAP_UINT32	uiTotActiveClients;		/*Num of clients connected*/
			NAP_UINT32	uiNumSockets;			/*Number of TCP Sockets*/
			NAP_INT32	iListenSocket;			/*Main Socket	*/
			NAP_UCHAR
			*pucData[MAX_CLIENTS_SUPPORTED];	/*Data buffer, used if send blocks
												when we do write operation on
												socket of specified module*/
			NAP_INT32   iDataLen[MAX_CLIENTS_SUPPORTED];	/* Data len*/
			NAP_INT32
			iReadRetranCount[MAX_CLIENTS_SUPPORTED]; /*Read retry count.*/
			NAP_INT32
			iWriteRetranCount[MAX_CLIENTS_SUPPORTED]; /*Write retry count.*/
			NAP_INT32  iModuleId[MAX_CLIENTS_SUPPORTED];
		}stTcp;
	}uProtocol;
}S_MODULE_NODES;

#if 1
//This structure contains generic network Information and the list of  all
//individual modules.
typedef struct _S_NAP_ACCESS_NETWORK
{
	NAP_INT32   iActiveModules;		/* Total number of modules.*/
	NAP_INT32   iNumOfSockets;		/* Total Number of sockets.*/
	NAP_INT32   iNumOfTcpServers;  // Number of TCP servers.
	NAP_INT32   iNumOfTcpClients;  // Number of TCP clients.
	NAP_INT32   iNumOfUdpClientServer; // Number of UDP modules.
	NAP_UINT32	uiNwCompTxId;		/* Transmit thread id.*/
	NAP_UINT32	uiNwCompRxId;		/* Receive thread id.*/
	NAP_BOOL    bNwInitialized;	    /* Access network init status.*/
	S_FWK_LIST	*pstModLst;			/* Array of Modules*/
}S_NAP_ACCESS_NETWORK;
#endif
// UDP protocol information. Remote host information.
typedef struct _S_NAP_UDP_INFO
{
	NAP_UCHAR	*pucData;    /*Data buffer, used if send blocks
							when we do write operation on
							socket of specified module*/
	NAP_INT32	iDataLen;   /* Data len*/
	NAP_CHAR	cDestIpAddr[16]; /*Dest IP Address*/
	NAP_UINT16  usDestPort; /*Dest Port*/
	NAP_INT32   iReadRetranCount; /*Read retry count.*/
    NAP_INT32   iWriteRetranCount; /*Write retry count.*/
	NAP_INT32   iModuleId;
}S_NAP_UDP_INFO;


//Access network data transmission status. (Used in case of Errors.)
typedef struct _S_NAP_ACCESS_NW_ACK
{
	NAP_INT32	iSocketId;
	NAP_INT32	iNumberOfBytesHandled;   //Number of bytes transmitted.
	NAP_INT16	sError;	//Type of errors.
	NAP_CHAR	cMessage[125];
}S_NAP_ACCESS_NW_ACK;


static struct S_ACCESS_NW_ERRORS
{
	NAP_INT32 iError;
	const NAP_CHAR *pMsg;
}stAccessNwError[] =
{
	{e_Err_AccessNw_OpnInterrupted, "Interrupted system call"},
	{e_Err_AccessNw_BadFileDesc, "Bad file descriptor"},
	{e_Err_AccessNw_PermissionDenied,"Permission denied"},
	{e_Err_AccessNw_BadAddr, "Bad address"},
	{e_Err_AccessNw_InvalidArg, "Invalid argument"},
	{e_Err_AccessNw_TooManyFilesOpen, "Too many open files"},
	{e_Err_AccessNw_OpnBlocked, "The socket operation could not complete without blocking"},
	{e_Err_AccessNw_OpnInProgress, "Operation now in progress"},
	{e_Err_AccessNw_OpnAlreadyInProgress, "Operation already in progress"},
	{e_Err_AccessNw_InvalidSock, "Socket operation on non-socket"},
	{e_Err_AccessNw_DestAddrRequired, "Destination address required"},
	{e_Err_AccessNw_MessageBig, "Message too long you might have not received it properly."},
	{e_Err_AccessNw_WrongProtocol, "Protocol wrong type for socket"},
	{e_Err_AccessNw_ProtocolNotAvailable,"Protocol not available"},
	{e_Err_AccessNw_PorotocolNotSupported, "Protocol not supported"},
	{e_Err_AccessNw_SocketNotSupported, "Socket type not supported"},
	{e_Err_AccessNw_OpNotSupported, "Operation not supported"},
	{e_Err_AccessNw_ProtocolFamilyNotSupported, "Protocol family not supported"},
	{e_Err_AccessNw_AddressFamilyNotSupported, "Address family not supported"},
	{e_Err_AccessNw_AddInUse, "Address already in use"},
	{e_Err_AccessNw_AddrNotAvailable, "Can't assign requested address"},
	{e_Err_AccessNw_NetworkDown, "Network is down"},
	{e_Err_AccessNw_NetWorkNotReachable, "Network is unreachable"},
	{e_Err_AccessNw_NewworkReset,"Network dropped connection on reset"},
	{e_Err_AccessNw_ConnAborted, "Software caused connection abort"},
	{e_Err_AccessNw_ConnReset, "Connection reset by peer"},
	{e_Err_AccessNw_NoBufs, "No buffer space available"},
	{e_Err_AccessNw_SockAlreadyConnected, "Socket is already connected"},
	{e_Err_AccessNw_SocketNotConnected, "Socket is not connected"},
	{e_Err_AccessNw_CannotSendAfterShutdown, "Can't send after socket shutdown"},
	{e_Err_AccessNw_TooManyReference,"Too many references: can't splice"},
	{e_Err_AccessNw_TimeOut, "Operation timed out"},
	{e_Err_AccessNw_ConnRefused,"Connection refused"},
	{e_Err_AccessNw_TooManyLevels, "Too many levels of symbolic links"},
	{e_Err_AccessNw_NameTooLong, "File name too long"},
	{e_Err_AccessNw_HostIsDown, "Host is down"},
	{e_Err_AccessNw_HostNotReachable, "No route to host"},
	{e_Err_AccessNw_NonEmptyDir, "Directory not empty"},
	{e_Err_AccessNw_TooManyProcess, "Too many processes"},
	{e_Err_AccessNw_TooManyUsers,"Too many users"},
	{e_Err_AccessNw_DiscFull,"Disc quota exceeded"},
	{e_Err_AccessNw_WrongFileHandle, "Stale NFS file handle"},
	{e_Err_AccessNw_TooManyLevelPath,"Too many levels of remote in path"},
	{e_Err_AccessNw_NwSubSystemNotAvailable, "Network subsystem is unvailable"},
	{e_Err_AccessNw_WrongWinsockVesrion, "WinSock version is not supported"},
	{e_Err_AccessNw_WinsockNotInitialized,  "Successful WSAStartup not yet performed."},
	{e_Err_AccessNw_ShutDownInProgress, "Graceful shutdown in progress"},
	/* Resolver errors */
	{e_Err_AccessNw_HostNotFound, "No such host is known"},
	{e_Err_AccessNw_ServerFailed, "Host not found, or server failed"},
	{e_Err_AccessNw_ServerError, "Unexpected server error encountered"},
	{e_Err_AccessNw_ServerData, "Valid name without requested data"},
	{e_Err_AccessNw_NoAddressForLookUp, "No address, look for MX record"},
	{0, NULL}
};




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
						OUT NAP_INT16 *psError);

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
						OUT	NAP_INT16 *psError);

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
NAP_BOOL	FWK_Ncm_Create_TcpSocket(
						 IN_OUT S_MODULE_NODES *pstModule,
						 OUT	NAP_INT32 *piSocket,
						OUT	NAP_INT16 *psError);

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
NAP_BOOL	FWK_Ncm_Create_UdpSocket(
						 IN_OUT S_MODULE_NODES *pstModule,
						 OUT	NAP_INT32 *piSocket,
							OUT NAP_INT16 *psError);

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
NAP_BOOL	FWK_Ncm_BindListen_TcpSocket(
							IN_OUT  S_MODULE_NODES *pstModule,
								OUT NAP_INT16 *psError);

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
NAP_BOOL	FWK_Ncm_Connect_TcpSocket(
						 	 IN_OUT S_MODULE_NODES *pstModule,
								OUT NAP_INT16 *psError);

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
NAP_BOOL	FWK_Ncm_Bind_UdpSocket(
						    IN  S_MODULE_NODES *pstModule,
							OUT NAP_INT16 *psError);

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
					   OUT NAP_INT16 *psError);

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
NAP_BOOL FWK_Ncm_Receive_Data(
					    IN  NAP_INT32 iSockDesc,
						OUT NAP_CHAR *pcData,
						IN  NAP_INT32 iLen,
						IN  NAP_BYTE  bProtocol,
						IN  NAP_INT32 iFlags,
						OUT NAP_INT32 *piNumOfBytesRcvd,
						OUT struct sockaddr *pstDestAddr,
						OUT NAP_INT32 *piLen,
						IN  NAP_INT32 iIndex,
						OUT NAP_INT16 *psError);

#endif
