#ifndef _ACCESS_NW_H_
#define _ACCESS_NW_H_

#include "fwk_datatypes.h"
#include "fwk_list.h"
#include "fwk_queue.h"
#include "server_config.h"
#include <sys/socket.h>
#include <netinet/ip.h>
#include <fcntl.h>
#include "fwk_ncm_layer.h"


#define MAX_ACCESS_NETWORK_SEND 8192            // Max data buffer. 
 
#define MAX_ACCESS_NETWORK_RECV 8192            // Max data buffer. 
#define MAX_CLIENTS_SUPPORTED   10                      // MAx client supported/server socket. 

typedef struct _S_ACCESS_NETWORK_QUEUE
{
        NAP_UCHAR ucSource;
        NAP_UCHAR ucDest;
        NAP_UCHAR ucProtocol;
        NAP_UCHAR ucEvent;
        NAP_VOID *pvData;

}S_ACCESS_NETWORK_QUEUE;

typedef enum _E_ACCESS_NETWORK_IPADDR_TYPE
{
    e_nap_ipv4=0,
    e_nap_ipv6
}E_ACCESS_NETWORK_IPADDR_TYPE;

typedef enum _E_ACCESS_NETWORK_SOC_TYPE
{
    e_NAP_Tcp=0,
    e_NAP_Udp
}E_ACCESS_NETWORK_SOC_TYPE;



/*This structure holds the transport related information received from the
network.*/
typedef struct _S_ACCESS_NETWORK_TRANSPORT
{
    E_ACCESS_NETWORK_IPADDR_TYPE   eIptype;
    E_ACCESS_NETWORK_SOC_TYPE      eSoctype;
    NAP_UCHAR   *pucHost;
    NAP_UCHAR   *pucIpaddr;             // Remote IP addr (Dot Format)
        NAP_UINT16      usPort;                 // Remote Port
        NAP_UCHAR       *pucLocalIpaddr;// Local IP Addr (Dot Format)
        NAP_UINT16      usLocalPort;    // Local Port
    NAP_UINT32  ussocketId;
    /* For any contextual data to be passed */
    NAP_VOID                *pvData;
}S_ACCESS_NETWORK_TRANSPORT;


/*The messages received by from network will be of this format.*/
typedef struct _S_ACCESS_NETWORK_PARAMS
{
    NAP_UCHAR   *pucInBuffer;   /* Network Buffer */
    NAP_UINT16  usInBufLen;  /* Network Buffer Length */
    S_ACCESS_NETWORK_TRANSPORT   stTptInfo;  /* Transport Info. */
}S_ACCESS_NETWORK_PARAMS;


#if 0
                                                    
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

//This structure contains generic network Information and the list of  all
//individual modules.
typedef struct _S_NAP_ACCESS_NETWORK
{
        NAP_INT32   iActiveModules;             /* Total number of modules.*/
        NAP_INT32   iNumOfSockets;              /* Total Number of sockets.*/
        NAP_INT32   iNumOfTcpServers;  // Number of TCP servers.
        NAP_INT32   iNumOfTcpClients;  // Number of TCP clients.
        NAP_INT32   iNumOfUdpClientServer; // Number of UDP modules.
        NAP_UINT32      uiNwCompTxId;           /* Transmit thread id.*/
        NAP_UINT32      uiNwCompRxId;           /* Receive thread id.*/
        NAP_BOOL    bNwInitialized;         /* Access network init status.*/
        S_FWK_LIST      *pstModLst;                     /* Array of Modules*/
}S_NAP_ACCESS_NETWORK;
#endif

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
				  NAP_INT16 *psError);


NAP_BOOL access_nw_add_module(
				IN_OUT  S_NAP_ACCESS_NETWORK *pstAccessNw,
				IN  NAP_INT32 iModuleId,
				IN  NAP_UINT32 uProtocol,
				IN  NAP_BYTE bModuleType,
				IN  NAP_BYTE bTransPort,
				IN  NAP_INT32  iNumOfClients,
				IN	NAP_VOID *pvData,
				OUT NAP_INT16 *psError);

NAP_BOOL access_nw_init(IN NAP_UBYTE *bIpAddress,
                        IN  NAP_INT32 iModuleId,
                        IN  NAP_UINT32 uProtocol,
                        IN  NAP_BYTE bModuleType,
                        IN  NAP_BYTE bTransPort,
                        IN  NAP_INT32  iNumOfClients,
                        IN NAP_UINT16 usPort,
                        IN      NAP_VOID *pvData,
                        OUT NAP_INT16 *psError);

/*******************************************************************************
* Function name	:	access_nw_DeInit.
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
				NAP_INT16 *psError);


NAP_BOOL access_nw_receive_thread(NAP_VOID);

#endif // _ACCESS_NW_H_
