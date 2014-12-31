#ifndef FWK_NCM_NET_H
#define FWK_NCM_NET_H

#ifdef FD_SETSIZE 
#undef FD_SETSIZE
#endif
#define FD_SETSIZE  40000

#include "fwk_datatypes.h"
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 

/******************************************************************************
* Function name : FWK_Lnx_ValidateIP
* Description   : This validates the given ip address.
* Return type   : NAP_BOOL
*                 NAP_SUCCESS - if the string is IPv4 address
*				  NAP_FAILURE - if the string is not IPv4 address
* Arguments     : [IN]  upcIpAddres : IP address in string form.
				  [OUT] pstAddr
* Side Effect   : None
* NOTE          : None.
******************************************************************************/
NAP_BOOL FWK_Lnx_ValidateIP(NAP_UCHAR *upcIpAddres, struct in_addr *pstAddr);

/******************************************************************************
* Function name : FWK_Lnx_NetIsV4Address
* Description   : This is a utility function which determines whether given
*                 string is IPv4 address.
* Return type   : NAP_BOOL
*                 NAP_SUCCESS - if the string is IPv4 address
*				  NAP_FAILURE - if the string is not IPv4 address
* Arguments     : [IN]  pszAddress : IP address in string form.
* Side Effect   : None
* NOTE          : None.
******************************************************************************/
NAP_BOOL FWK_Lnx_NetIsV4Address(NAP_CHAR*	pszAddress);

/******************************************************************************
* Function name : FWK_Lnx_NetV4NtoA
* Description   : This is a utility function converts 4 byte address into Dot
				  address.
* Return type   : NAP_BOOL
*                 NAP_SUCCESS - if the string is IPv4 address
*				  NAP_FAILURE - if the string is not IPv4 address
* Arguments     : [IN]  netAddress    : IP address in network form( 4 bytes).
*			      [OUT] pszAddress    : IP address in string form(Dot format).
*				  [OUT] usAddrLen     : Length of the string(pszAddress Buffer
										length).
* Side Effect   : None
* NOTE          : None.
******************************************************************************/
NAP_BOOL FWK_Lnx_NetV4NtoA(NAP_UCHAR* netAddress,
						NAP_UCHAR**	pszAddress,
						NAP_UINT16	usAddrLen);

/******************************************************************************
* Function name : FWK_Lnx_NetV4AtoN
* Description   : This is a utility function converts Dot address into 4 byte address
* Return type   : NAP_BOOL
*                 NAP_SUCCESS - if the string is IPv4 address
*				  NAP_FAILURE - if the string is not IPv4 address
* Arguments     : [IN]  netAddress    : IP address in network form.
*				  [OUT] pszAddress    : IP address in string form.
*				  [OUT] usAddrLen     : Length of the string.
* Side Effect   : None
* NOTE          : None.
******************************************************************************/
NAP_BOOL FWK_Lnx_NetV4AtoN(NAP_CHAR *ipv4Format, NAP_UINT32 *addr);

/******************************************************************************
* Function name : FWK_Lnx_NetV6NtoA
* Description   : This is a utility function which converts given IPv4 network
*                 form address to string.
* Return type   : NAP_BOOL
*                 NAP_SUCCESS - if the string is IPv4 address
*				  NAP_FAILURE - if the string is not IPv4 address
* Arguments     : [IN]  netAddress    : IP address in network form.
*				  [OUT] pszAddress    : IP address in string form.
*				  [OUT] usAddrLen     : Length of the string.
* Side Effect   : None
* NOTE          : This code is copied from ISC source from
*                 http://samba.anu.edu.au/rsync/doxygen/head/inet__ntop_8c-source.html
*                 Therefore, as per the copyright statement issued in the ISC
*                 following contents must be present in this file.
* Copyright (C) 1996-2001  Internet Software Consortium.
* Permission to use, copy, modify, and distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM
* DISCLANAP ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
* INTERNET SOFTWARE CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT,
* INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
* FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
* NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
* WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*
*******************************************************************************
*/
NAP_BOOL FWK_Lnx_NetV6NtoA(NAP_UCHAR*	netAddress,
						NAP_CHAR*	pszAddress,
						NAP_UINT16	usAddrLen);

/******************************************************************************
* Function name : FWK_Ncm_GetSystemIpAddr
* Description   : This function gets the system ip address
* Return type   : NAP_BOOL
				  NAP_SUCCESS on successful retrieval of ip address.
				  NAP_ERROR on any error.
* Arguments     : [OUT] pucIpAddr sAppID	:ip address read.
* Side Effect   : None
*******************************************************************************/
NAP_BOOL FWK_Ncm_GetSystemIpAddr( NAP_CHAR *pucIpAddr);


/******************************************************************************
* Function name : FWK_Lnx_Socket
* Description   : This function creates socket and returns socket id
* Return type   : NAP_INT32
* Arguments     : [IN] NAP_INT16 sAppID	:Application Id
				  [OUT] NAP_INT32 *iScoketId: Socket id created.
				  [IN] NAP_INT32 iFamily	:Address family specification
				  [IN] NAP_INT32 iType		:Socket type
				  [IN] NAP_INT32 iProtocol	:Protocol used
				  [OUT]NAP_INT16 *psError	:Error type
* Side Effect   : None
*******************************************************************************/
NAP_BOOL  FWK_Lnx_Socket (NAP_INT32 *iScoketId,
						NAP_INT32 iFamily,
						NAP_INT32 iType,
						NAP_INT32 iProtocol,
						NAP_INT16 *psError);

/*****************************************************************************
* Function name : FWK_Lnx_Connect
* Description   : This function establishes a connection to a specified socket.
* Return type   : NAP_INT32
* Arguments     : [IN]NAP_INT32 iSocket			 : Socket id
				  [IN]struct sockaddr  *pstName	 : Destination host IP address
				  [IN]INT32 iNamelen			 : Length of name
				  [OUT]NAP_INT16 *psError	     : Error type
* Side Effect   : None
*************************************************************************/
NAP_BOOL FWK_Lnx_Connect ( NAP_INT32 iSocket,
						 	const struct sockaddr *stName,
						 	NAP_INT32 iNamelen,
						 	NAP_INT16 *psError);

/******************************************************************************
* Function name :	FWK_Lnx_Accept
* Description   :	This function permits an incoming connection attempt on
					a socket.
* Return type   :	NAP_BOOL
* Arguments     :	[IN]NAP_INT32 iSocket			: Socket id
					[IN]struct sockaddr  *pstName :Buffer to receive address
					[IN]INT32  *piNamelen	: Length of pstName
					[OUT]NAP_INT16 *psError	: Error type
* Side Effect   : None
******************************************************************************/
NAP_BOOL FWK_Lnx_Accept (NAP_INT32 iSocket,
						NAP_INT32	*iScoketId,
						struct sockaddr  *pstName,
						NAP_INT32  *piNamelen,
						NAP_INT16  *psError);

/******************************************************************************
* Function name :	FWK_Lnx_Listen
* Description   :	This function places a socket in a state in which it
					is listening for an incoming connection.
* Return type   :	NAP_BOOL
* Arguments     :	[IN]iSocket	: Socket id
					[IN]iQueue	: Number of connections supported.
					[OUT]*psError : Error type
* Side Effect   :	None
******************************************************************************/
NAP_INT32 FWK_Lnx_Listen(NAP_INT32 iSocket,
						 NAP_INT32 iQueue,
						 NAP_INT16 *psError);

/******************************************************************************
* Function name :	FWK_Lnx_Send
* Description   :	This function sends data on a connected socket.
* Return type   :	NAP_INT32
* Arguments     :	[IN]NAP_INT32  iSocket	: Socket id
					[IN]NAP_INT*  pBuf		: Buffer containing the data
												to be transmitted.
					[IN]NAP_INT32  iLen		: buffer length
					[IN]NAP_INT iFlags		: Indicator specifying the way
											  in which the call is made.
					[OUT]NAP_INT16 *psError	: Error type
* Side Effect   :	None
******************************************************************************/
NAP_BOOL FWK_Lnx_Send (NAP_INT32  iSocket,
						const NAP_INT8*  pBuf,
						NAP_INT32  iLen,
						NAP_INT32 iFlags,
						NAP_INT32 *iSent,
						NAP_INT16 *psError);

/******************************************************************************
* Function name : FWK_Lnx_Recv
* Description   : This function receives data on a connected socket.
* Return type   : NAP_INT32
* Arguments     : [IN]NAP_INT32  iSocket		: Socket id
				  [OUT]NAP_INT*  pBuf			: Buffer to receive data.
				  [IN]NAP_INT32  iLen			: buffer length
				  [IN]NAP_INT iFlags			: Indicator specifying the way
												  in which the call is made.
				  [OUT] NAP_INT32 *iRecvd,
				  [OUT]NAP_INT16 *psError	    : Error type
* Side Effect   : None
******************************************************************************/
NAP_BOOL FWK_Lnx_Recv (NAP_INT32  iSocket,
					NAP_CHAR*  pBuf,
					NAP_INT32  iLen,
					NAP_INT32 iFlags,
					NAP_INT32 *iRecvd,
					NAP_INT16 *psError);

/*******************************************************************************
* Function name : FWK_Lnx_Close
* Description   : This function closes the specified socket
* Return type   : NAP_INT32
* Arguments     : [IN]NAP_INT32 iSocket				: Socket id
				  [OUT]NAP_INT16 *psError		    : Error type
* Side Effect   : None
*******************************************************************************/
NAP_BOOL FWK_Lnx_Close(NAP_INT32 iSocket,NAP_INT16 *psError);

/******************************************************************************
* Function name : FWK_Lnx_Select
* Description   : This function determines the status of one or more sockets,
				  waiting if necessary, to perform synchronous I/O.
* Return type   : NAP_BOOL
* Arguments     : [IN]NAP_INT32 ifds			:
				  [IN,OUT]fd_set *readfds		: Optional pointer to a set
												  of sockets to be checked
												  for readability.
				  [IN,OUT]fd_set *writefds		: Optional pointer to a set
												  of sockets to be checked
												  for writability.
				  [IN,OUT]fd_set *exceptfds		: Optional pointer to a set
												  of sockets to be checked
												  for errors.
				  [IN,OUT]const struct timeval *timeout	: Maximum time for select
														to wait

* Side Effect   : None
*****************************************************************************/
NAP_BOOL FWK_Lnx_Select(NAP_INT32 ifds,fd_set* readfds,
						 fd_set* writefds,fd_set* exceptfds,
						 const struct timeval* timeout,NAP_INT32 *piError);

/******************************************************************************
* Function name :	FWK_Lnx_Bind
* Description   :
* Return type   :	NAP_BOOL
* Arguments     :	[IN]NAP_INT32 iSocket			: Socket id
					[IN]struct sockaddr  *pstName :Buffer to receive address
					[IN]INT32  *piNamelen	: Length of pstName
					[OUT]NAP_INT16 *psError	: Error type
* Side Effect   : None
******************************************************************************/
NAP_BOOL FWK_Lnx_Bind (IN NAP_INT32 iSocket,
					IN struct sockaddr  *pstName,
					IN NAP_INT32  iNamelen,
					OUT NAP_INT16 *psError);
#if 0
/******************************************************************************
* Function name :	FWK_Lnx_DeInitSocketLibrary
* Side Effect   : None
******************************************************************************/
NAP_BOOL FWK_Lnx_DeInitSocketLibrary (NAP_INT16 *psError);
#endif
/******************************************************************************
* Function name : FWK_Lnx_OpenPPPSession
* Description   : This function opens PPP session
* Return type   : NAP_BOOL
* Arguments     : [IN] NAP_INT16 *sAppID	:Application Id
				  [OUT] NAP_INT16 *psError	:error type
* Side Effect   : None
******************************************************************************/
NAP_BOOL	FWK_Lnx_OpenPPPSession (NAP_INT16 sAppID,NAP_INT16 *psError);

/******************************************************************************
* Function name : FWK_Lnx_ClosePPPSession
* Description   : This function closes PPP session

* Return type   : NAP_BOOL
* Arguments     : [IN] NAP_INT16 *AppID	:Application Id
				  [OUT] NAP_INT16 *psError	:error type
* Side Effect   : None
******************************************************************************/
NAP_BOOL	FWK_Lnx_ClosePPPSession (NAP_INT16 sAppID,NAP_INT16 *psError);

/*******************************************************************************
* Function name : FWK_Lnx_SocketSendTo
* Description   : This function sends data on the specified UDP socket

* Arguments     : [IN] NAP_INT32 iSockId	: Socket id
				  [IN] const NAP_VOID *pvSendBuffer	: buffer to send
				  [IN] NAP_UINT32 uiBytesToSend		: buffer length
				  [OUT]NAP_UINT32 *piBytesSent		: number of bytes sent
				  [IN] NAP_UINT32 uiFlags :	Reserved.
				  [IN] struct sockaddr *pstDestAddr	 : destination address
				  [IN] NAP_UINT32 uiLen: Size of the address in to, in bytes.
				  [OUT]NAP_INT16 *psError			   : Error type
* Side Effect   : None
*******************************************************************************/

NAP_BOOL	FWK_Lnx_SocketSendTo(NAP_INT32	iSockId,
				const NAP_VOID  *pvSendBuffer,
				NAP_UINT32 uiBytesToSend,
				NAP_UINT32 *puiBytesSent,
				NAP_UINT32 uiFlags,
				struct sockaddr *pstDestAddr,
				NAP_UINT32 uiLen,
				NAP_INT16 *psError);

/*******************************************************************************
* Function name : FWK_Lnx_SocketRecieveFrom
* Description   : This function read the data on the UDP socket
* Arguments     : [IN] NAP_INT32 iSockId				: Socket id
				  [IN] NAP_VOID  *pvReadBuffer	: buffer to read
				  [IN] NAP_UINT32  uiBytesToRead : buffer length to read
				  [OUT] NAP_UINT32 *puiBytesRead : number of bytes read
				  [IN] NAP_UINT32 uiFlags :	Reserved.
  				  [IN] struct sockaddr *pstDestAddr : destination address
				  [IN] NAP_INT32 iLen: Size of the address in to, in bytes.
				  [OUT] NAP_INT16 *psError		 : Error type
* Side Effect   : None
*******************************************************************************/
NAP_BOOL	FWK_Lnx_SocketRecieveFrom(NAP_INT32	iSockId,
							NAP_VOID  *pvReadBuffer,
							NAP_UINT32 uiBytesToRead,
							NAP_UINT32 *puiBytesRead,
							NAP_UINT32 uiFlags,
							struct sockaddr *pstDestAddr,
							NAP_INT32 *piLen,
							NAP_INT16 *psError);

/*******************************************************************************
* Function name : FWK_Lnx_NetworkStatus
* Description   : This function provides status of network subsystem.  Called in
				 response to DS_ENETDOWN errors.  Note that origination status
				 is based on the last attempted origination.
* Return type   : NAP_BOOL
* Arguments     : [IN] NAP_INT16 *AppID	:Application Id
				  [OUT] NAP_INT16 *psStatus	:
				  [OUT] NAP_INT16 *psError : Error type
* Side Effect   : None
*******************************************************************************/
NAP_BOOL	FWK_Lnx_NetworkStatus(NAP_INT16 sAppID,
								NAP_INT16 *psStatus,
								NAP_INT16 *psError);

/*******************************************************************************
* Function name : FWK_Lnx_SocketAsyncSelect
* Description   : This function enables the events to be notified about through
				the asynchronous notification mechanism.  Application specifies
				a bitmask of events that it is interested in, for which it will
				receive asynchronous notification via its application callback
				function.  This function also performs a real-time  check to
				determine if any of the events have already occurred, and if so
				invokes the application callback.
* Arguments     : [IN] NAP_INT32 *SocketId	: Socket id
				  [IN] NAP_INT32 iEventMask		: event types
				  [OUT] NAP_INT16 *psError		: Error Type
* Side Effect   : None
*******************************************************************************/
NAP_BOOL	FWK_Lnx_SocketAsyncSelect (NAP_INT16 pSocketId,
									NAP_INT32 iEventMask,
									NAP_UINT16 *psError);
/*******************************************************************************
* Function name : FWK_Lnx_SocketAsyncDeselect
* Description   : This function clears events of interest in the socket control
				  block interest mask.  The application specifies a bitmask of
				  events that it wishes to clear; events for which it will no
				  longer receive notification.
* Arguments     : [IN] NAP_INT32 *SocketId	: Socket id.
				  [IN] NAP_INT32 iEventMask	: event types
				  [OUT] NAP_INT16 *psError	: Error Type
* Side Effect   : None
*******************************************************************************/
NAP_BOOL	FWK_Lnx_SocketAsyncDeselect (NAP_INT32 SocketId,
								NAP_INT32 iEventMask,
								NAP_UINT16 *psError);

/*******************************************************************************
* Function name : FWK_Lnx_GetNextEvent
* Description   : This function performs a real-time check to determine if any
				  of the events of interest specified in the socket control
				  block's event mask have occurred.  It also clears any bits
				  in the event mask that have occurred.
* Arguments     : [IN]	NAP_INT32 iAppId	: Application id
				  [IN]	NAP_INT32 *psSocketId : Socket id
				  [OUT] NAP_INT32 *piEvent : Events ouccurred.
				  [OUT] NAP_INT16 *psError	: Error Type
* Side Effect   : None
*******************************************************************************/
NAP_BOOL	FWK_Lnx_GetNextEevent(NAP_INT16 sAppId,
								 NAP_INT16 *psSocketId,
								 NAP_INT32 *piEvent,
								 NAP_INT16 *psError);

/*******************************************************************************
* Function name : FWK_Lnx_Setsockopt
* Description   : This function sets a socket option.
* Arguments     : [IN]	NAP_INT32 iSocketId	: Descriptor identifying a socket.
				  [IN]	NAP_INT32  iLevel : Level at which the option is defined.
				  [IN]	NAP_INT32  iOptionName : Socket option for which the
												 value is to be set.
				  [IN]	NAP_VOID   *iOptVal : Pointer to the buffer in which
							the value for the requested option is specified.
				  [IN]	NAP_UINT32 *iOptLen : Size of the optval buffer,in bytes.
				  [IN]	NAP_INT16  *psError : Error type.
* Side Effect   : None
*******************************************************************************/
NAP_BOOL	FWK_Lnx_Setsockopt(NAP_INT32 iSocketId,
							NAP_INT32	iLevel,
							NAP_INT32 iOptionName,
							NAP_VOID *pvOptVal,
							NAP_UINT32 *iOptLen,
							NAP_INT16 *psError);

/*******************************************************************************
* Function name : FWK_Lnx_Getsockopt
* Description   : This function retrieves a socket option.
* Arguments     : [IN] NAP_INT32 iSocketId	: Descriptor identifying a socket.
				  [IN] NAP_INT32  iLevel : Level at which the option is defined.
				  [IN] NAP_INT32  iOptionName : Socket option for which the
												value is to be retrieved.
				  [OUT] NAP_VOID   *iOptVal : Pointer to the buffer in which
						the value for the requested option is to be returned.
				  [IN_OUT] NAP_UINT32 *iOptLen : Pointer to the size of the
									optval buffer, in bytes.
				  [OUT] NAP_INT16  *psError     : Error type.
* Side Effect   : None
*******************************************************************************/
NAP_BOOL	FWK_Lnx_Getsockopt(NAP_INT32 iSocketId,
							NAP_INT32	iLevel,
							NAP_INT32 iOptionName,
							NAP_VOID *pvOptVal,
							NAP_UINT32 *iOptLen,
							NAP_INT16 *psError);

/*******************************************************************************
* Function name : FWK_Lnx_Getsockname
* Description   : This function retrieves the local name for a socket.
* Arguments     : [IN]	NAP_INT32 iSocketId	: Socket id
				 [OUT]	struct sockaddr* pstAaddr	:Pointer to a SOCKADDR
					structure that receives the address (name) of the socket.
				 [IN_OUT] NAP_UINT32* iAddLen :Size of the name buffer,in bytes.
				  [OUT]	MS_INT16 *psError :  Error type.
* Side Effect   : None
*******************************************************************************/
NAP_BOOL	FWK_Lnx_Getsockname(NAP_INT32 iSocketId,
							struct sockaddr* pstAddr,
							NAP_UINT32* iAddLen,
							NAP_INT16 *psError);

/******************************************************************************
* Function name : FWK_Lnx_Getpeername
* Description   : This function retrieves the name of the peer to which a
					socket is connected.
* Arguments     : [IN] NAP_INT32 iSocketId : Descriptor identifying a
						connected socket.
				  [OUT] struct sockaddr* pstAaddr : The SOCKADDR structure
						that receives the name of the peer.
				  [IN_OUT] NAP_UINT32* iAddLen : Pointer to the size of the
						   name structure, in bytes.
				  [OUT]	MS_INT16 *psError :	Error type.
* Side Effect   : None
******************************************************************************/
NAP_BOOL	FWK_Lnx_Getpeername(NAP_INT32 iSocketId,
							struct sockaddr* pstAddr,
							NAP_UINT32* iAddLen,
							NAP_INT16 *psError);

/******************************************************************************
* Function name : FWK_Lnx_Shutdown
* Description   : This function disables sends or receives on a socket.
* Arguments     : [IN] NAP_INT32 iSocketId	: Descriptor identifying a socket.
				  [IN] NAP_UINT32 iHow	: Flag that describes what types
					   of operation will no longer be allowed.
				  [OUT]	NAP_INT16 *psError  :	Error Type
* Side Effect   : None
*******************************************************************************/
NAP_BOOL	FWK_Lnx_Shutdown(NAP_INT32 iSocketId,
							NAP_UINT32 iHow,
							NAP_INT16 *psError);

NAP_BOOL FWK_Ncm_ByteToDotFormat(NAP_INT32  uiSrcIpAddress,
								  NAP_UCHAR *pDstIpAddress,
								  NAP_INT16 *psError);
#endif

