#define MSRP_NS_INT16SZ       2
#define MSRP_NS_IN6ADDRSZ    16

#include <stdio.h>
#include "fwk_datatypes.h"
#include "fwk_ncm_net.h"
#include "fwk_string.h"
#include "fwk_error.h"
#include "fwk_memory.h"
#include <ifaddrs.h>
#include <netdb.h>
#include <errno.h>

#define _GNU_SOURCE     /* To get defns of NI_MAXSERV and NI_MAXHOST */

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
NAP_BOOL FWK_Lnx_ValidateIP(NAP_UCHAR *upcIpAddres, struct in_addr *pstAddr)
{
	NAP_INT32     iIndex       = 0;
    NAP_INT32     iNumber      = 0;
	NAP_UINT32   uiValue       = 0;
    NAP_UINT32	uiIpAddr      = 0;
    NAP_UINT32   iCheckValue = 0;
    NAP_UCHAR    *upcPresult     = (NAP_UCHAR *) &uiIpAddr;
	enum
	{
		BASE_OCT = 8,
		BASE_DEC = 10,
		BASE_HEX = 16
	} base   = BASE_DEC;

	/* Invalid function parameters.*/
	if( NULL == upcIpAddres || NULL == pstAddr || 0 == strlen(upcIpAddres))
         return NAP_FAILURE;

	for(iIndex = 0; iIndex < sizeof(struct in_addr); iIndex++)
	{
		base  = BASE_DEC;
		uiValue = 0;
		if( '0' == *upcIpAddres )
		{
			upcIpAddres++;
			base = ( 'X' == *upcIpAddres || 'x' == *upcIpAddres ) ? \
					BASE_HEX : BASE_OCT;
			if( BASE_HEX == base )
				upcIpAddres++;
		}
		while( '.' != *upcIpAddres && 0 != *upcIpAddres )
		{
			iNumber = *upcIpAddres - '0';
			switch(base)
			{
				case BASE_DEC:
				case BASE_OCT:
					if(iNumber < 0 || iNumber > ((NAP_INT32)base - 1))
					     return NAP_FAILURE;
				break;
		        case BASE_HEX:
					if(iNumber < 0 || iNumber > 9)
					{
						iNumber = *upcIpAddres - 'a' + 10;
						if (iNumber < 10 || iNumber > 15)
						{
							iNumber = *upcIpAddres - 'A' + 10;
							if(iNumber < 10 || iNumber > 15)
								return NAP_FAILURE;
						}
					}
				break;
		        default:
			        return NAP_FAILURE;
				break;
			} /* switch */
		    upcIpAddres++;
		   if( ((long) uiValue * base) + iNumber > 0xffffffff )
				return NAP_FAILURE;
		   uiValue = (uiValue * (NAP_INT32)base) + iNumber;
		}

     if(uiValue > 0xff && *upcIpAddres != 0)
		return NAP_FAILURE;
     else
     {
       if( 0 != *upcIpAddres )
			*upcPresult++ = uiValue;
       else
       {
          switch( iIndex )
          {
			case 0:
				 iCheckValue = 0xffffffff;
             break;
	        case 1:
			     iCheckValue = 0xffffff;
			break;
	        case 2:
		         iCheckValue = 0xffff;
            break;
	        case 3:
				iCheckValue = 0xff;
            break;
            default:
				 return 0;
         } /* switch */
		if( uiValue > iCheckValue )
	         return NAP_FAILURE;
        uiIpAddr |= htonl(uiValue);
      }
    }
    if(*upcIpAddres++ == 0)
       break;
  } /* for */
  pstAddr->s_addr = uiIpAddr;
 
  if(((signed)uiIpAddr >= 0x0000) && (uiIpAddr <= 0x08))
  {
	/*0.0.0.0/8 used for "This Network" as per RFC1790.*/			
	 return NAP_FAILURE;
  }
  else if( ((uiIpAddr >= 0xA000000) && (uiIpAddr <= 0xA000008)) ||
		 ((uiIpAddr >= 0xAC100000) && (uiIpAddr <= 0xAC10000C)) ||
		 ((uiIpAddr >= 0xC0A80000) && (uiIpAddr <= 0xC0A80010)) )
  {
	/*
	 * 10.0.0.0/8 used for "Private-Use Network" as per RFC1918.
	 * 172.16.0.0/12 used for "Private-used Networks" as per RFC1918.
	 * 192.168.0.0/16 used for "Private use networks" as per RFC1918.
	 */
 	 return NAP_FAILURE;
  }
  else if((uiIpAddr >= 0xE000000) && (uiIpAddr <= 0xE000008))
  {
	 /* 14.0.0.0/8 used for "Public-Data Networks" as per RFC1700*/
		return NAP_FAILURE;
  }
  else if((uiIpAddr >= 0x18000000) && (uiIpAddr <= 0x18000008))
  {
	/*
	 * 24.0.0.0/8 used for "Cable-Television Networks"
	 */
	 return NAP_FAILURE;
  }
  else if((uiIpAddr >= 0x7F000000) && (uiIpAddr <= 0x7F000008))
  {
	 /* ar$spa = 127.0.0.0/8 used for "Loopback"*/
		return NAP_FAILURE;
  }
  else if((uiIpAddr >= 0xA9FE0000) && (uiIpAddr <= 0xA9FE0010))
  {
		/* 169.254.0.0/16 used for "Link Local (APIPA) */
	return NAP_FAILURE;
  }
  else if((uiIpAddr >= 0xC0000200) && (uiIpAddr <= 0xC0000218))
  {
		/* 192.0.2.0/24 used for "Test Network"	 */
		return NAP_FAILURE;
  }
  else if((uiIpAddr >= 0xC0586300) && (uiIpAddr <= 0xC0586318))
  {
    	/* 192.88.99.0/24 used for "6to4Relay Anycast" as per RFC3068 */
		return NAP_FAILURE;
  }
  else if((uiIpAddr >= 0xC0120000) && (uiIpAddr <= 0xC012000F))
  {
		/* 192.18.0.0/15 used for "Network Interconnect Benchmack Tests"
		 * as per RFC2544*/
		return NAP_FAILURE;
  }
  else if((uiIpAddr >= 0xE0000000) && (uiIpAddr <= 0xE0000004))
  {
		/*224.0.0.0/4 used for "Multicast" as per RFC3171*/
		return NAP_FAILURE;
  }
  else if((uiIpAddr >= 0xF0000000) && (uiIpAddr <= 0xF0000004))
  {
		/*240.0.0.0/4 used for "Reserved for future use" as per RFC1700.*/
	return NAP_FAILURE;
  }
  return NAP_SUCCESS;
 }



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
NAP_BOOL FWK_Lnx_NetIsV4Address(NAP_CHAR*	pszAddress)
{
	struct in_addr stTempAddr;
	if (FWK_Lnx_ValidateIP(pszAddress, &stTempAddr) == NAP_FAILURE)
		return NAP_FAILURE;
	return NAP_SUCCESS;
}

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
						NAP_UCHAR **pszAddress,
						NAP_UINT16	usAddrLen)
{
   struct in_addr in;
   //char *some_addr = NULL;
   if (pszAddress == NAP_NULL || usAddrLen == 0)
	return NAP_FAILURE;
	in.s_addr = *((NAP_UINT32*)netAddress);
	//some_addr = inet_ntoa(in);
   sprintf(
	*pszAddress,
	"%d.%d.%d.%d",(in.s_addr ) & 0xff,
	(in.s_addr >> 8) & 0xff,
	(in.s_addr >> 16) & 0xff,
	(in.s_addr >> 24) & 0xff);
	//printf("%s: ipAddr = %s",__FUNCTION__,  inet_ntoa(in));
	if (*pszAddress == NAP_NULL)
		return NAP_FAILURE;
	return NAP_SUCCESS;
}

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
NAP_BOOL FWK_Lnx_NetV4AtoN(NAP_CHAR *ipv4Format, NAP_UINT32 *pstAddr)
{
    struct in_addr	IP_Addr;
    
    if (ipv4Format == NAP_NULL || pstAddr == NAP_NULL)
    {
	    return NAP_FAILURE;
    }

	IP_Addr.s_addr = inet_addr(ipv4Format);

    pstAddr[0] = (IP_Addr.s_addr >> 24) & 0xff;
    pstAddr[1] = (IP_Addr.s_addr >> 16) & 0xff;
    pstAddr[2] = (IP_Addr.s_addr >> 8) & 0xff;
    pstAddr[3] = (IP_Addr.s_addr) & 0xff;

	return NAP_SUCCESS;
}

#if 0
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
#define FWK_NS_INT16SZ       2
#define FWK_NS_IN6ADDRSZ    16
NAP_BOOL FWK_Lnx_NetV6NtoA(NAP_UCHAR*	netAddress,
						NAP_CHAR*	pszAddress,
						NAP_UINT16	usAddrLen)
{

    /*
     * Note that int32_t and int16_t need only be "at least" large enough
     * to contain a uiValue of the specified size.  On some systems, like
     * Crays, there is no such thing as an integer variable with 16 bits.
     * Keep this in mind if you think this function should have been coded
     * to use pointer overlays.  All the world's not a VAX.
     */
    char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"], *tp;
    struct { int base, len; } best, cur;
    NAP_UINT32 words[MSRP_NS_IN6ADDRSZ / MSRP_NS_INT16SZ];
    int i;

	if (pszAddress == NAP_NULL || usAddrLen == 0)
		return NAP_FAILURE;
	/*
     * Preprocess:
     *      Copy the input (bytewise) array into a wordwise array.
     *      Find the longest run of 0x00's in src[] for :: shorthanding.
     */
    memset(words, '\0', sizeof words);
    for (i = 0; i < MSRP_NS_IN6ADDRSZ; i++)
            words[i / 2] |= (netAddress[i] << ((1 - (i % 2)) << 3));
    best.base = -1;
    cur.base = -1;
    for (i = 0; i < (MSRP_NS_IN6ADDRSZ / MSRP_NS_INT16SZ); i++) {
            if (words[i] == 0) {
                    if (cur.base == -1)
                            cur.base = i, cur.len = 1;
                    else
                            cur.len++;
            } else {
                    if (cur.base != -1) {
                            if (best.base == -1 || cur.len > best.len)
                                    best = cur;
                            cur.base = -1;
                    }
            }
    }
    if (cur.base != -1) {
            if (best.base == -1 || cur.len > best.len)
                    best = cur;
    }
    if (best.base != -1 && best.len < 2)
            best.base = -1;

    /*
     * Format the result.
     */
    tp = tmp;
    for (i = 0; i < (MSRP_NS_IN6ADDRSZ / MSRP_NS_INT16SZ); i++) {
            /* Are we inside the best run of 0x00's? */
            if (best.base != -1 && i >= best.base &&
                i < (best.base + best.len)) {
                    if (i == best.base)
                            *tp++ = ':';
                    continue;
            }
            /* Are we following an initial run of 0x00s or any real hex? */
            if (i != 0)
                    *tp++ = ':';
            /* Is this address an encapsulated IPv4? */
            if (i == 6 && best.base == 0 &&
                (best.len == 6 || (best.len == 5 && words[5] == 0xffff))) {
                    if (FWK_Lnx_NetV4NtoA(netAddress+12, tp,
                                    (NAP_UINT16)(sizeof tmp - (tp - tmp))) == NAP_FAILURE)
                            return NAP_FAILURE;
                    tp += strlen(tp);
                    break;
            }
            tp += FWK_Sprintf(tp, "%x", words[i]);
    }
    /* Was it a trailing run of 0x00's? */
    if (best.base != -1 && (best.base + best.len) ==
        (MSRP_NS_IN6ADDRSZ / MSRP_NS_INT16SZ))
            *tp++ = ':';
    *tp++ = '\0';

    /*
     * Check for overflow, copy, and we're done.
     */
    if ((size_t)(tp - tmp) > usAddrLen) {
            /*errno = ENOSPC;*/
            return NAP_FAILURE;
    }
    strcpy(pszAddress, tmp);
    return NAP_SUCCESS;
}
#endif

/* Functions below are added by Jayalakshmi on 07-Jun-2005*/
/******************************************************************************
* Function name : FWK_Ncm_GetSystemIpAddr
* Description   : This function gets the system ip address
* Return type   : NAP_BOOL
				  NAP_SUCCESS on successful retrieval of ip address.
				  NAP_ERROR on any error.
* Arguments     : [OUT] pucIpAddr sAppID	:ip address read.
* Side Effect   : None
*******************************************************************************/
NAP_BOOL FWK_Ncm_GetSystemIpAddr( NAP_CHAR *pucIpAddr)
{
	NAP_CHAR host[126] = {0};
	//LPHOSTENT lpHostEntry = NAP_NULL;
	struct sockaddr_in stIpAddr;
	struct ifaddrs *ifaddr = NULL;
	struct ifaddrs *ifa = NULL;
	int family, s, n;

	if (getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
		return NAP_FAILURE;
	}

	/* Walk through linked list, maintaining head pointer so we
	can free list later */

	for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
		if (ifa->ifa_addr == NULL)
		   continue;

		family = ifa->ifa_addr->sa_family;

		/* Display interface name and family (including symbolic
		  form of the latter for the common families) */

		printf("%-8s %s (%d)\n",
		      ifa->ifa_name,
		      (family == AF_PACKET) ? "AF_PACKET" :
		      (family == AF_INET) ? "AF_INET" :
		      (family == AF_INET6) ? "AF_INET6" : "???",
		      family);

		/* For an AF_INET* interface address, display the address */

		if (family == AF_INET || family == AF_INET6) {
		   s = getnameinfo(ifa->ifa_addr,
			   (family == AF_INET) ? sizeof(struct sockaddr_in) :
						 sizeof(struct sockaddr_in6),
			   host, NI_MAXHOST,
			   NULL, 0, NI_NUMERICHOST);
		   if (s != 0) {
		       printf("getnameinfo() failed: %s\n", gai_strerror(s));
		   }

		   printf("\t\taddress: <%s>\n", host);
		   stIpAddr.sin_addr.s_addr = inet_addr(host);
		   FWK_Sprintf(pucIpAddr,"%x",htonl(stIpAddr.sin_addr.s_addr));
		}
	}

	freeifaddrs(ifaddr);

	return NAP_SUCCESS;
}

/******************************************************************************
* Function name : FWK_Lnx_SockError
* Description   : This function logs the socket errors on windows
* Return type   : NAP_BOID
* Arguments     : [IN] sError: Error type.
				  [IN]: pcFunction:  function name returning error.
* Side Effect   : None
*******************************************************************************/
NAP_VOID FWK_Lnx_SockError(NAP_INT16 sError,NAP_CHAR *pcFunction,NAP_INT16 *psError)
{
	NAP_CHAR cBuffer[1000] = {0};
	switch(sError)
	{
		case EINTR:
			*psError = e_Err_AccessNw_OpnInterrupted;
		break;
		case EBADF:
			*psError = e_Err_AccessNw_BadFileDesc;	
		break;
		case EACCES:
			*psError = e_Err_AccessNw_PermissionDenied;
		break;
		case EPFNOSUPPORT:
			*psError = e_Err_AccessNw_ProtocolFamilyNotSupported;
		break;
		case ETOOMANYREFS:
			*psError = e_Err_AccessNw_TooManyReference;
		break;
		case ELOOP:
			*psError = e_Err_AccessNw_TooManyLevels;
		break;
		case ENAMETOOLONG:
			*psError = e_Err_AccessNw_NameTooLong;
		break;
		case EHOSTDOWN:
			*psError = e_Err_AccessNw_HostIsDown;
		break;
		case ENOTEMPTY:
			*psError = e_Err_AccessNw_NonEmptyDir;
		break;
		case EUSERS:
			*psError = e_Err_AccessNw_TooManyUsers;
		break;
		case EDQUOT:
			*psError = e_Err_AccessNw_DiscFull;
		break;
		case ESTALE:
			*psError = e_Err_AccessNw_WrongFileHandle;
		break;
		case EREMOTE:
			*psError = e_Err_AccessNw_TooManyLevelPath;
		break;
		case HOST_NOT_FOUND:
			*psError = e_Err_AccessNw_HostNotFound;
		break;
		case TRY_AGAIN:
			*psError = e_Err_AccessNw_ServerFailed;
		break;
		case NO_RECOVERY:
			*psError = e_Err_AccessNw_ServerError;
		break;
	//	case NO_ADDRESS:
		//	*psError = e_Err_AccessNw_NoAddressForLookUp;
	//	break;
		case EFAULT:	FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call\
		failed: The buf parameter is not completely contained in a valid part\
		of the user	address space.");
			*psError = e_Err_AccessNw_BadAddr;
		break;
		case EINVAL:	FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call failed:\
		The socket has not been bound with bind.");
			*psError = e_Err_AccessNw_InvalidArg;
		break;
		case EWOULDBLOCK: FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call \
		failed: The socket is nonblocking and the requested operation would block.");
			*psError = e_Err_AccessNw_OpnBlocked;
		break;
		case EINPROGRESS:
			*psError = e_Err_AccessNw_OpnInProgress;
		break;		
		case ENETDOWN: FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call \
		failed: The	network subsystem has failed.");
			*psError = e_Err_AccessNw_NetworkDown;
		break;		
		case ENETRESET: FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call\
		failed: The	connection has been broken due to the keep-alive activity \
		detecting a failure while the operation was in progress.");
			*psError = e_Err_AccessNw_NewworkReset;
		break;
		case ENOTCONN: FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call \
		failed: The	socket is not connected.");
			*psError = e_Err_AccessNw_SocketNotConnected;
		break;
		case ENOTSOCK: FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call \
		failed:  The descriptor is not a socket.");
			*psError = e_Err_AccessNw_InvalidSock;
		break;
		case EOPNOTSUPP:	FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call\
		failed: MSG_OOB	specified, but the socket is not of type SOCK_STREAM.");
			*psError = e_Err_AccessNw_OpNotSupported;
		break;
		case ESHUTDOWN: FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call\
		failed: The socket has been shut down; it is not possible to send on a \
		socket after shutdown has been invoked.");
			*psError = e_Err_AccessNw_CannotSendAfterShutdown;
		break;		
		case EMSGSIZE: FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call \
		failed: The socket is message oriented, and the message is larger than \
		the maximum supported by the underlying transport.");
			*psError = e_Err_AccessNw_MessageBig;
		break;
		case EHOSTUNREACH: FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call\
		failed: The remote host cannot be reached from this host at this time.");
			*psError = e_Err_AccessNw_HostNotReachable;
		break;
		case ECONNABORTED: FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call\
		failed: The	connection was terminated due to a time-out or other failure.");
			*psError = e_Err_AccessNw_ConnAborted;
		break;
		case ECONNRESET:	FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call\
		failed:  The connection was reset by the remote side.");
			*psError = e_Err_AccessNw_ConnReset;
		break;
		case ETIMEDOUT:	FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call\
		failed: The connection has been dropped, because of a network failure.");
			*psError = e_Err_AccessNw_TimeOut;
		break;
		case EADDRNOTAVAIL: FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call\
		failed: The	remote address is not a valid address.");
			*psError = e_Err_AccessNw_AddrNotAvailable;
		break;
		case EAFNOSUPPORT:	FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call \
		failed: Addresses in the specified family cannot be used with this socket.");
			*psError = e_Err_AccessNw_AddressFamilyNotSupported;
		break;
		case EDESTADDRREQ: FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call \
		failed: A destination address is required.",__FILE__,__LINE__);
			*psError = e_Err_AccessNw_DestAddrRequired;
		break;
		case ENETUNREACH: FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call \
		failed: The	network cannot be reached from this host at this time.");
			*psError = e_Err_AccessNw_NetWorkNotReachable;
		break;
		case ENOBUFS: FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call \
		failed: No buffer space is available. The socket cannot be connected.");
			*psError = e_Err_AccessNw_NoBufs;
		break;
		case EPROTOTYPE:	FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call\
		failed: The	network cannot be reached from this host at this time.");
			*psError = e_Err_AccessNw_WrongProtocol;
		break;
		case EMFILE:	FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call \
		failed: The	network cannot be reached from this host at this time.");
			*psError = e_Err_AccessNw_TooManyFilesOpen;
		break;
		case EPROTONOSUPPORT: FWK_Sprintf(cBuffer,"%s %s",pcFunction,
		"call failed: The network cannot be reached from this host at this time.");
			*psError = e_Err_AccessNw_PorotocolNotSupported;
		break;
		case ESOCKTNOSUPPORT: FWK_Sprintf(cBuffer,"%s %s",pcFunction,
		"call failed: The network cannot be reached from this host at this time.");
			*psError = e_Err_AccessNw_SocketNotSupported;
		break;
		case ENOPROTOOPT:
			FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call failed: The\
			option is unknown or unsupported by the indicated protocol\
			family.");
			*psError = e_Err_AccessNw_ProtocolNotAvailable;
		case EALREADY:
			FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call failed:");
			*psError = e_Err_AccessNw_OpnAlreadyInProgress;
		break;
		case ECONNREFUSED:
			FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call failed: The\
			attempt to connect was forcefully rejected.");
			*psError = e_Err_AccessNw_ConnRefused;
		break;
		case EISCONN:
			FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call failed: The\
			socket is already connected (connection-oriented sockets).");
			*psError = e_Err_AccessNw_SockAlreadyConnected;
		break;
		case EADDRINUSE:
			FWK_Sprintf(cBuffer,"%s %s",pcFunction,"call failed: The\
			socket's local address is already in use and the \
			socket was not marked to allow address reuse with SO_REUSEADDR.");
			*psError = e_Err_AccessNw_AddInUse;
		break;
		default:
		break;
	}
//	FWK_ERROR_LOG(e_Err_AccessNw_NetworkFail,cBuffer,__FILE__,__LINE__);
}


/******************************************************************************
* Function name : FWK_Lnx_Socket
* Description   : This function creates socket and returns socket id
* Return type   : NAP_INT32
* Arguments     : [OUT] NAP_INT32 *iScoketId: Socket id created.
				  [IN] NAP_INT32 iFamily	:Address family specification
				  [IN] NAP_INT32 iType		:Socket type
				  [IN] NAP_INT32 iProtocol	:Protocol used
				  [OUT]NAP_INT16 *psError	:Error type
* Side Effect   : None
*******************************************************************************/
NAP_BOOL  FWK_Lnx_Socket (OUT	NAP_INT32 *iScoketId,
						IN	NAP_INT32 iFamily,
						IN	NAP_INT32 iType,
						IN	NAP_INT32 iProtocol,
						OUT	NAP_INT16 *psError)
{

	NAP_INT32  iSocket =0;
	NAP_INT16 sError = 0;

	/* Invalid function parameters.*/
	if (iScoketId == NAP_NULL || psError ==NAP_NULL)
		return NAP_FAILURE;

	*iScoketId = -1;
	*psError = 0;

	iSocket = socket( iFamily,iType,iProtocol); /* Create socket.*/
	if (iSocket >0) /* Return the socket descriptor.*/
	{
		*iScoketId = iSocket;
		return NAP_SUCCESS;
	}
	else
	{
		*psError = errno;
		FWK_Lnx_SockError(*psError,"socket",&sError);		
		*psError = sError;
		*iScoketId = -1;
		return NAP_FAILURE;
	}
	return NAP_SUCCESS;
}

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
NAP_BOOL FWK_Lnx_Connect (IN	NAP_INT32 iSocket,
						 IN	const struct sockaddr* pstName,
						 IN	NAP_INT32 iNamelen,
						 OUT	NAP_INT16 *psError)
{

	NAP_INT32 iResult = 0;
	NAP_INT16 sError = 0;

	/* Invalid function parameters.*/
	if (psError == NAP_NULL)
		return NAP_FAILURE;
	*psError = 0;

	/* Connect to the specified address*/
	iResult  = connect (iSocket, (const struct sockaddr*)pstName,iNamelen);
	if (iResult != 0)
	{
		/* connect call failed.*/
		*psError = errno;
		FWK_Lnx_SockError(*psError,"connect",&sError);
		*psError = sError;
		return NAP_FAILURE;
	}
	return NAP_SUCCESS;
}

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
					OUT NAP_INT16 *psError)
{

	NAP_INT32 iResult =0;
	NAP_INT16 sError = 0;
	printf("\n%s: Entering iSocket = %u",__FUNCTION__,iSocket);
	/* Invalid function parameters.*/
	if (psError == NAP_NULL)
        {
 	 	printf("\n%s: psError == NULL",__FUNCTION__);
		return NAP_FAILURE;
	}
	*psError = 0;

	/* Bind the socket.*/
	iResult  = bind(iSocket,pstName,iNamelen);	
	if (iResult != 0)
	{
		/* Bind failed.*/
 	 	printf("\n%s: Bind Failed",__FUNCTION__);
		*psError = errno;
		FWK_Lnx_SockError(*psError,"bind",&sError);		
		*psError = sError;
		return NAP_FAILURE;
	}
	printf("\n%s: Exiting",__FUNCTION__);
	return NAP_SUCCESS;
}
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
						 NAP_INT16 *psError)
{

	NAP_INT32 iResult = 0;
	NAP_INT16 sError = 0;

	if (psError == NAP_NULL)
		return NAP_FAILURE;
	*psError = 0;
	iResult  = listen (iSocket, iQueue);	
	if (iResult != 0)
	{
		*psError = errno;
		FWK_Lnx_SockError(*psError,"listen",&sError);		
		*psError = sError;
		return NAP_FAILURE;
	}
	return NAP_SUCCESS;
}

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
NAP_BOOL FWK_Lnx_Accept (IN NAP_INT32 iSocket,
						OUT NAP_INT32	*iScoketId,
						IN struct sockaddr  *pstName,
						IN NAP_INT32  *piNamelen,
						OUT NAP_INT16  *psError)
{

	NAP_INT32 iResult =0;
	NAP_INT16 sError = 0;
	if (psError == NAP_NULL || iSocket <= 0)
		return NAP_FAILURE;
	*psError = 0;
	iResult  = accept(iSocket,pstName,piNamelen);		
	if (iResult >0)
	{	*iScoketId = iResult;
		return NAP_SUCCESS;
	}
	else
	{
		*psError = errno;
		FWK_Lnx_SockError(*psError,"accept",&sError);		
		*psError = sError;
		return NAP_FAILURE;
	}
	return NAP_SUCCESS;
}


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
						OUT NAP_INT32 *iSent,
						NAP_INT16 *psError)
{

	NAP_INT32 iResult = 0;
	NAP_INT16 sError = 0;
	if (psError == NAP_NULL || iSent == NAP_NULL)
		return NAP_FAILURE;
	*psError = 0;
	iResult  = send(iSocket,pBuf,iLen,iFlags);
	*iSent = iResult;
	if (iResult < 0)
	{
		*psError = errno;
		FWK_Lnx_SockError(*psError,"send",&sError);		
		*psError = sError;
		return NAP_FAILURE;
	}
	return NAP_SUCCESS;
}

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

NAP_BOOL FWK_Lnx_Recv (IN	NAP_INT32  iSocket,
					OUT	NAP_CHAR*  pBuf,
					IN	NAP_INT32  iLen,
					IN	NAP_INT32 iFlags,
					OUT  NAP_INT32 *iRecvd,
					OUT	NAP_INT16 *psError)
{

	NAP_INT32 iResult = 0;
	NAP_INT16 sError = 0;
	if (psError == NAP_NULL || iRecvd == NAP_NULL)
		return NAP_FAILURE;
	*psError = 0;
	iResult  = recv(iSocket,pBuf,iLen,iFlags);
	*iRecvd = iResult;
	if (iResult == -1)
	{
		*psError = errno;
		FWK_Lnx_SockError(*psError,"recv",&sError);		
		*psError = sError;
		return NAP_FAILURE;
	}
	else if(0 == iResult)
	{
		/* MSDN says : - If the connection has been 
		gracefully closed, the return value is zero.*/
		*psError = e_Err_AccessNw_ConnReset;
		return NAP_FAILURE;
	}

	return NAP_SUCCESS;
}
/*******************************************************************************
* Function name : FWK_Lnx_SocketSendTo
* Description   : This function sends data on the specified UDP socket

* Arguments     : [IN] NAP_INT32 iSockId	: Socket id
				  [IN] const NAP_VOID *pvSendBuffer	: buffer to send
				  [IN] NAP_UINT32 uiBytesToSend		: buffer length
				  [OUT]NAP_UINT32 *piBytesSent		: iNumber of bytes sent
				  [IN] NAP_UINT32 uiFlags :	Reserved.
				  [IN] struct sockaddr *pstDestAddr	 : destination address
				  [IN] NAP_UINT32 uiLen: Size of the address in to, in bytes.
				  [OUT]NAP_INT16 *psError			   : Error type
* Side Effect   : None
*******************************************************************************/

NAP_BOOL	FWK_Lnx_SocketSendTo(IN NAP_INT32	iSockId,
				IN	const NAP_VOID  *pvSendBuffer,
				IN	NAP_UINT32 uiBytesToSend,
				OUT	NAP_UINT32 *puiBytesSent,
				IN	NAP_UINT32 uiFlags,
				IN	struct sockaddr *pstDestAddr,
				IN	NAP_UINT32 uiLen,
				OUT	NAP_INT16 *psError)
{
	NAP_INT32 iRetVal =0;
	NAP_INT16 sError = 0;
	if (psError == NAP_NULL || puiBytesSent == NAP_NULL)
		return NAP_FAILURE;
	*psError = 0;
	iRetVal =  sendto(iSockId,(const NAP_CHAR*)pvSendBuffer,uiBytesToSend,uiFlags,
				pstDestAddr,uiLen);
	*puiBytesSent = iRetVal;
	if (iRetVal == -1 )
	{
		*psError = errno;
		FWK_Lnx_SockError(*psError,"sendto",&sError);		
		*psError = sError;
		return NAP_FAILURE;
	}
	return NAP_SUCCESS;
}

/*******************************************************************************
* Function name : FWK_Lnx_SocketRecieveFrom
* Description   : This function read the data on the UDP socket
* Arguments     : [IN] NAP_INT32 iSockId				: Socket id
				  [IN] NAP_VOID  *pvReadBuffer	: buffer to read
				  [IN] NAP_UINT32  uiBytesToRead : buffer length to read
				  [OUT] NAP_UINT32 *puiBytesRead : iNumber of bytes read
				  [IN] NAP_UINT32 uiFlags :	Reserved.
  				  [IN] struct sockaddr *pstDestAddr : destination address
				  [IN] NAP_INT32 iLen: Size of the address in to, in bytes.
				  [OUT] NAP_INT16 *psError		 : Error type
* Side Effect   : None
*******************************************************************************/

NAP_BOOL	FWK_Lnx_SocketRecieveFrom(IN NAP_INT32	iSockId,
							IN	NAP_VOID  *pvReadBuffer,
							IN	NAP_UINT32 uiBytesToRead,
							OUT	NAP_UINT32 *puiBytesRead,
							IN	NAP_UINT32 uiFlags,
							IN	struct sockaddr *pstDestAddr,
							IN	NAP_INT32 *piLen,
							OUT	NAP_INT16 *psError)
{

	NAP_INT32 iRetVal =0;
	NAP_INT16 sError = 0;
	if (psError == NAP_NULL || puiBytesRead == NAP_NULL)
		return NAP_FAILURE;
	*psError = 0;
	iRetVal =  recvfrom(iSockId,(NAP_CHAR*)pvReadBuffer,uiBytesToRead,uiFlags,
				pstDestAddr,piLen);
	if (iRetVal == -1 )
	{
		*psError = errno;
		FWK_Lnx_SockError(*psError,"recvfrom",&sError);		
		*psError = sError;
		return NAP_FAILURE;
	}
	else
		*puiBytesRead = iRetVal;
	return NAP_SUCCESS;

}
/*******************************************************************************
* Function name : FWK_Lnx_Close
* Description   : This function closes the specified socket
* Return type   : NAP_INT32
* Arguments     : [IN]NAP_INT32 iSocket				: Socket id
				  [OUT]NAP_INT16 *psError		    : Error type
* Side Effect   : None
*******************************************************************************/
NAP_BOOL FWK_Lnx_Close(NAP_INT32 iSocket,NAP_INT16 *psError)
{

	NAP_INT32 iResult = 0;
	NAP_INT16 sError = 0;
	if (psError == NAP_NULL)
		return NAP_FAILURE;
	*psError = 0;
	iResult  = close(iSocket);
	if (iResult != 0)
	{
		*psError = errno;
		FWK_Lnx_SockError(*psError,"close",&sError);		
		*psError = sError;
		return NAP_FAILURE;
	}
	return NAP_SUCCESS;
}

/******************************************************************************
* Function name : FWK_Lnx_Select
* Description   : This function determines the status of one or more sockets,
				  waiting if necessary, to perform synchronous I/O.
* Return type   : NAP_INT32
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
******************************************************************************/

NAP_BOOL FWK_Lnx_Select(NAP_INT32 ifds,fd_set* readfds,
						 fd_set* writefds,fd_set* exceptfds,
						 const struct timeval* timeout,NAP_INT32 *piError)
{
	NAP_INT32 iStatus = 0;
	NAP_INT16 sError = 0;
	if (piError == NAP_NULL)
		return NAP_FAILURE;

	iStatus = select(ifds,readfds,writefds,exceptfds,timeout);
	if (iStatus < 0)
	{
		*piError = errno;
		FWK_Lnx_SockError((NAP_INT16)*piError,"select",&sError);		
		*piError = sError;
		return NAP_FAILURE;
	}
	else
	{
		*piError = iStatus;
		return NAP_SUCCESS;
	}
}


#if 0
/******************************************************************************
* Function name :	FWK_Lnx_DeInitSocketLibrary
* Side Effect   : None
******************************************************************************/
NAP_BOOL FWK_Lnx_DeInitSocketLibrary (OUT NAP_INT16 *psError)
{
	NAP_INT16 sError = 0;
	/* Invalid function parameters.*/
	if (psError == NAP_NULL)
		return NAP_FAILURE;
	*psError = 0;

	if (WSACleanup()!= 0)
	{
		*psError = errno;
		FWK_Lnx_SockError(*psError,"deinit network library",&sError);		
		*psError = sError;
		return NAP_FAILURE;
	}
	return NAP_SUCCESS;
}
#endif

/*******************************************************************************
* Function name : FWK_Lnx_Setsockopt
* Description   : This function sets a socket option.
* Arguments     : [IN]	NAP_INT32 iSocketId	: Descriptor identifying a socket.
				  [IN]	NAP_INT32  iLevel : Level at which the option is defined.
				  [IN]	NAP_INT32  iOptionName : Socket option for which the
												 uiValue is to be set.
				  [IN]	NAP_VOID   *iOptVal : Pointer to the buffer in which
							the uiValue for the requested option is specified.
				  [IN]	NAP_UINT32 *iOptLen : Size of the optval buffer,in bytes.
				  [IN]	NAP_INT16  *psError : Error type.
* Side Effect   : None
*******************************************************************************/
NAP_BOOL	FWK_Lnx_Setsockopt(IN	NAP_INT32 iSocketId,
							IN	NAP_INT32	iLevel,
							IN	NAP_INT32 iOptionName,
							IN	NAP_VOID *pvOptVal,
							IN	NAP_UINT32 *iOptLen,
							IN	NAP_INT16 *psError)
{


	NAP_INT32 iResult =0;
	NAP_INT16 sError = 0;
	if (psError == NAP_NULL)
		return NAP_FAILURE;
	*psError = 0;
	iResult = setsockopt(iSocketId,iLevel,iOptionName,(const NAP_CHAR*)pvOptVal,
							(NAP_INT32)(*iOptLen));
	if (iResult != 0)
	{
		*psError = errno;
		FWK_Lnx_SockError(*psError,"setsockopt",&sError);		
		*psError = sError;
		return NAP_FAILURE;
	}
	return NAP_SUCCESS;
}


/*******************************************************************************
* Function name : FWK_Lnx_Getsockopt
* Description   : This function retrieves a socket option.
* Arguments     : [IN] NAP_INT32 iSocketId	: Descriptor identifying a socket.
				  [IN] NAP_INT32  iLevel : Level at which the option is defined.
				  [IN] NAP_INT32  iOptionName : Socket option for which the
												uiValue is to be retrieved.
				  [OUT] NAP_VOID   *iOptVal : Pointer to the buffer in which
						the uiValue for the requested option is to be returned.
				  [IN_OUT] NAP_UINT32 *iOptLen : Pointer to the size of the
									optval buffer, in bytes.
				  [OUT] NAP_INT16  *psError     : Error type.
* Side Effect   : None
*******************************************************************************/
NAP_BOOL	FWK_Lnx_Getsockopt(IN	NAP_INT32 iSocketId,
							IN	NAP_INT32	iLevel,
							IN	NAP_INT32 iOptionName,
							OUT	NAP_VOID *pvOptVal,
							IN_OUT	NAP_UINT32 *iOptLen,
							OUT	NAP_INT16 *psError)
{

	NAP_INT32 iResult =0;
	NAP_INT16 sError = 0;
	if (psError == NAP_NULL || iOptLen == NAP_NULL)
		return NAP_FAILURE;
	*psError = 0;
	iResult = getsockopt(iSocketId,iLevel,iOptionName,(NAP_CHAR*)pvOptVal,
							iOptLen);
	if (iResult != 0)
	{
		*psError = errno;
		FWK_Lnx_SockError(*psError,"getsockopt",&sError);		
		*psError = sError;
		return NAP_FAILURE;
	}
	return NAP_SUCCESS;
}

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
NAP_BOOL	FWK_Lnx_Getsockname(IN	NAP_INT32 iSocket,
									OUT	struct sockaddr* pstAddr,
									IN_OUT	NAP_UINT32* iAddLen,
									IN	NAP_INT16 *psError)
{

	NAP_INT32 iResult =0;
	NAP_INT16 sError = 0;
	/* Invalid function parameters.*/
	if (psError == NAP_NULL || iAddLen == NAP_NULL)
		return NAP_FAILURE;
	*psError = 0;
	/* Get the local name for the socket specified.*/
	iResult = getsockname(iSocket,pstAddr,iAddLen);
	if (iResult != 0)
	{
		/* getsockname failed.*/
		*psError = errno;
		FWK_Lnx_SockError(*psError,"getsockname",&sError);		
		*psError = sError;
		return NAP_FAILURE;
	}
	return NAP_SUCCESS;
}


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
NAP_BOOL	FWK_Lnx_Getpeername(IN	NAP_INT32 iSocket,
							OUT	struct sockaddr* pstAddr,
							IN_OUT	NAP_UINT32* iAddLen,
							OUT	NAP_INT16 *psError)
{
	NAP_INT32 iResult =0;
	NAP_INT16 sError = 0;
	/* Invalid function parameters.*/
	if (psError == NAP_NULL || iAddLen == NAP_NULL)
		return NAP_FAILURE;

	*psError = 0;
	/* Get the name of the peer to which we wish to connect.*/
	iResult = getpeername(iSocket,pstAddr,iAddLen);
	if (iResult != 0)
	{
		/* getpeername function call failed.*/
		*psError = errno;
		FWK_Lnx_SockError(*psError,"getpeername",&sError);		
		*psError = sError;
		return NAP_FAILURE;
	}
	return NAP_SUCCESS;
}

/******************************************************************************
* Function name : FWK_Lnx_Shutdown
* Description   : This function disables sends or receives on a socket.
* Arguments     : [IN] NAP_INT32 iSocketId	: Descriptor identifying a socket.
				  [IN] NAP_UINT32 iHow	: Flag that describes what types
					   of operation will no longer be allowed.
				  [OUT]	NAP_INT16 *psError  :	Error Type
* Side Effect   : None
*******************************************************************************/
NAP_BOOL	FWK_Lnx_Shutdown(IN NAP_INT32 iSocket,
								IN	NAP_UINT32 iHow,
								OUT	NAP_INT16 *psError)
{

	NAP_INT32 iResult =0;
	NAP_INT16 sError = 0;
	/* Invalid function parameter.*/
	if (psError == NAP_NULL)
		return NAP_FAILURE;
	*psError = 0;

	/* Disables sends or receives on the socket specified.*/
	iResult = shutdown(iSocket,iHow);
	if (!iResult)
	{
		/* shutdown call failed.*/
		*psError = errno;
		FWK_Lnx_SockError(*psError,"shutdown",&sError);		
		*psError = sError;
		return NAP_FAILURE;
	}
	return NAP_SUCCESS;
}

#if 0
/********************************************************************************
* Function name : FWK_Ncm_Rex_Initialize
* Description   : This function initializes the network library
* Return type   : NAP_BOOL
* Arguments     : [OUT] sAppID	:Application Id
				  [IN]  net_callback_fcn		:network callback function
				  [IN]  socket_callback_fcn	:socket callback function
				  [OUT] *psError	:error type
* Side Effect   : None
*******************************************************************************/

NAP_BOOL	FWK_Ncm_Rex_Initialize (NAP_INT16 *sAppID,
								NAP_VOID  (*net_callback_fcn)(NAP_VOID *),
								NAP_VOID  (*socket_callback_fcn)(NAP_VOID *),
								NAP_INT16 *psError)
{

	if (psError == NAP_NULL)
		return NAP_FAILURE;
	*psError = 0;
	/* call rex net library*/
	 *sAppID = dss_open_netlib(net_callback_fcn, socket_callback_fcn, psError);

	 /* validate the application id*/
	if (*sAppID == DSS_ERROR)
	{
        *sAppID = 0;
		FWK_ERROR_LOG(e_Err_Fwk_NetworkFail,
			"No more applications available - max apps exceeded.");
		return NAP_FAILURE;
	}
	else
		return  NAP_SUCCESS;
}

/********************************************************************************
* Function name : FWK_Ncm_Rex_DeInitialize
* Description   : This function deinitializes the network library
* Return type   : NAP_BOOL
* Arguments     : [IN] sAppID	:Application Id
				  [OUT] *psError	:error type
* Side Effect   : None
*******************************************************************************/
NAP_BOOL	FWK_Ncm_Rex_DeInitialize  (NAP_INT16 sAppID,
									NAP_INT16 *psError)
{

	NAP_INT16 sRetVal;

	if (psError == NAP_NULL)
		return NAP_FAILURE;

	/* closes  the net library*/
	sRetVal = dss_close_netlib(AppID, piError);

	/* validate the return vlaue of library close function */
   	if(DSS_ERROR == sRetVal)
	{
		switch(*psError)
		{
			case DS_SOCKEXIST:
				 FWK_ERROR_LOG(e_Err_Fwk_NetworkFail,"dss_close_netlib failed:\
					Invalid application ID.",__FILE__,__LINE__);
			break;
			case DS_EBADAPP:
				FWK_ERROR_LOG(e_Err_Fwk_NetworkFail,"dss_close_netlib failed:\
					there are existing sockets.",__FILE__,__LINE__);
			break;
			case DS_ENETEXIST:
				FWK_ERROR_LOG(e_Err_Fwk_NetworkFail,"dss_close_netlib failed:\
					the network subsystem exists.",__FILE__,__LINE__);
			break;
			default :
			break;
		}
		return NAP_FAILURE;
	}
	else
		return NAP_SUCCESS;
}

/********************************************************************************
* Function name : NAP_REX_FWK_TC_OpenPPPSession
* Description   : This function opens PPP session

* Return type   : NAP_BOOL
* Arguments     : [IN] NAP_APP_ID *AppID	:Application Id
				  [OUT] NAP_INT16 *piError	:error type
* Side Effect   : None
*******************************************************************************/

NAP_BOOL	NAP_REX_FWK_TC_OpenPPPSession (
				NAP_APP_ID AppID,
				NAP_INT16 *piError
				)

{
	NAP_INT16 iRetVal;

   /* call rex ppp session open fucntion and return the status */
	iRetVal = dss_pppopen(AppID,piError);


	if (iRetVal == DSS_SUCCESS)
	{
	    return (NAP_BOOL) NAP_SUCCESS;
	}
    else
	{
	    return (NAP_BOOL) NAP_FAILURE;
	}
}


/********************************************************************************
* Function name : NAP_REX_FWK_TC_ClosePPPSession
* Description   : This function closes PPP session

* Return type   : NAP_BOOL
* Arguments     : [IN] NAP_APP_ID *AppID	:Application Id
				  [OUT] NAP_INT16 *piError	:error type
* Side Effect   : None
*******************************************************************************/

NAP_BOOL	NAP_REX_FWK_TC_ClosePPPSession (
				NAP_APP_ID AppID,
				NAP_INT16 *piError
				)

{

	NAP_INT16 iRetVal;

   /* call rex ppp session close fucntion and return the status */
	iRetVal =  dss_pppclose(AppID, piError);

  	 if(DSS_ERROR == iRetVal)
	{
		switch(*piError)
			{
			case DS_EWOULDBLOCK:
				*piError = TC_EWOULDBLOCK;
				NAP_ERROR_MAJOR(*piError, "dss_pppclose : DS_EWOULDBLOCK");
				break;
			case DS_EBADAPP:
				*piError = TC_EBADAPP;
				NAP_ERROR_MAJOR(*piError, "dss_pppclose : DS_EBADAPP");
				break;
			case DS_ENETCLOSEINPROGRESS:
				*piError = TC_ENETCLOSEINPROGRESS;
				NAP_ERROR_MAJOR(*piError, "dss_pppclose : DS_ECONNREFUSED");
				break;
			default :
				NAP_ERROR_MAJOR(*piError, "dss_pppclose : default");
				break;

			}
		return (NAP_BOOL) NAP_FAILURE;
	}
	else
	{
		return (NAP_BOOL) NAP_SUCCESS;
	}
}

/*******************************************************************************
* Function name : NAP_REX_FWK_TC_CreateSocket
* Description   : This function creates socket and returns socket id

* Arguments     : [IN] NAP_APP_ID *AppID				:Application Id
				  [OUT]NAP_SOCKET_ID *SockId			:Created socket id
				  [IN] E_TC_SOCKET_TYPE  eSocketType	:Socket type
				  [OUT]NAP_INT16 *piError				:error type
* Side Effect   : None
*******************************************************************************/

NAP_BOOL	NAP_REX_FWK_TC_CreateSocket(
					NAP_SOCKET_ID *SockId,
					NAP_APP_ID	AppID,
					E_TC_SOCKET_TYPE  eSocketType,
					NAP_INT16 *piError
					)
{
	sint15 socID;

    /* validate the socket type if not proper return error*/
	if ((eSocketType != eSock_Stream) && (eSocketType != eSock_Datagram))
    {
        *piError = TC_EOPNOTSUPP;
        return (NAP_BOOL) NAP_FAILURE;
    }

	/* call rex socket create fucntion based on stream type and return the status*/
    if (eSocketType == eSock_Stream)
    {
        socID  = dss_socket(AppID,AF_INET,SOCK_STREAM,IPPROTO_TCP,piError);
    }
	/* if socket type is datagram*/
    else
    {
         socID = dss_socket(AppID,AF_INET,SOCK_DGRAM,IPPROTO_UDP,piError);
    }

    if (socID == DSS_ERROR)
	{
	    *SockId = TC_INVALID_ID;
		return (NAP_BOOL) NAP_FAILURE;
	}
    else
	{
		*SockId = socID;
		return (NAP_BOOL) NAP_SUCCESS;
	}
}


/*******************************************************************************
* Function name : NAP_REX_FWK_TC_Connect
* Description   : This function establishes the connection for TCP socket

* Arguments     : [IN]NAP_SOCKET_ID *SockId	 : Socket id
				  [IN]NAP_INT32 iDestAddr	 : Destination host IP address
				  [IN]NAP_INT32 iDestPort	 : Desintation host port no
				  [OUT]NAP_INT16 *piError	 : Error type
* Side Effect   : None
*******************************************************************************/

NAP_BOOL	NAP_REX_FWK_TC_Connect(
				NAP_SOCKET_ID SocketId,
				NAP_UINT32 iDestAddr,
				NAP_UINT16  iDestPort,
				NAP_INT16 *piError
				)
{
	S_NAP_TC_SOCK_ADDR stServAddr;
    NAP_INT16 iRetVal;

    	FWK_Ncm_OS_Memset(&stServAddr,0,sizeof(stServAddr));
	/* fill the serveraddress structure*/
	stServAddr.sin_family = AF_INET;
	stServAddr.sin_addr.s_addr = dss_htonl(iDestAddr);
	stServAddr.sin_port = dss_htons(iDestPort);

	/* call rex socket connect function for TCP  and return the status*/
    	iRetVal = dss_connect(SocketId,(struct sockaddr *)&stServAddr,
										sizeof(stServAddr),piError);

    	if(DSS_ERROR == iRetVal)
	{
		switch(*piError)
			{
			case DS_EWOULDBLOCK:
				*piError = TC_EWOULDBLOCK;
				NAP_ERROR_MAJOR(*piError, "dss_connect : DS_EWOULDBLOCK");
				break;
			case DS_EBADF:
				*piError = TC_EBADF;
				NAP_ERROR_MAJOR(*piError, "dss_connect : DS_EBADF");
				break;
			case DS_ECONNREFUSED:
				*piError = TC_ECONNREFUSED;
				NAP_ERROR_MAJOR(*piError, "dss_connect : DS_ECONNREFUSED");
				break;
			case DS_ETIMEDOUT:
				*piError = TC_ETIMEDOUT;
				NAP_ERROR_MAJOR(*piError, "dss_connect : DS_ETIMEDOUT");
				break;
			case DS_EFAULT:
				*piError = TC_EFAULT;
				NAP_ERROR_MAJOR(*piError, "dss_connect : DS_EFAULT");
				break;
			case DS_EIPADDRCHANGED:
				*piError = TC_EIPADDRCHANGED;
				NAP_ERROR_MAJOR(*piError, "dss_connect : DS_EIPADDRCHANGED");
				break;
			case DS_EINPROGRESS:
				*piError = TC_EINPROGRESS;
				NAP_ERROR_MAJOR(*piError, "dss_connect : DS_EINPROGRESS");
				break;
			case DS_EISCONN:
				*piError = TC_EISCONN;
				NAP_ERROR_MAJOR(*piError, "dss_connect : DS_EISCONN :: Already Connected.");
				return (NAP_BOOL)NAP_SUCCESS;

				break;
			case DS_ENETDOWN:
				*piError = TC_ENETDOWN;
				NAP_ERROR_MAJOR(*piError, "dss_connect : DS_ENETDOWN");
				break;
			case DS_EOPNOTSUPP:
				*piError = TC_EOPNOTSUPP;
				NAP_ERROR_MAJOR(*piError, "dss_connect : DS_EOPNOTSUPP");
				break;
			case DS_EINVAL:
				*piError = TC_EINVAL;
				NAP_ERROR_MAJOR(*piError, "dss_connect : DS_EINVAL");
				break;
			case DS_ENOROUTE:
				*piError = TC_ENOROUTE;
				NAP_ERROR_MAJOR(*piError, "dss_connect : DS_ENOROUTE");
				break;
			case DS_EADDRREQ:
				*piError = TC_EADDRREQ;
				NAP_ERROR_MAJOR(*piError, "dss_connect : DS_EADDRREQ");
				break;
			default :
				NAP_ERROR_MAJOR(*piError, "dss_connect : default");
				break;
			}
		return (NAP_BOOL) NAP_FAILURE;
	}
	else
	{
		return (NAP_BOOL) NAP_SUCCESS;
	}
}


/*******************************************************************************
* Function name : NAP_REX_FWK_TC_SocketSend
* Description   : This function sends the data on the specified socket id


* Arguments     : [IN]NAP_SOCKET_ID *SockId				: Socket id
				  [IN]const NAP_VOID  *pvSendBuffer		: buffer to send
				  [IN]NAP_UINT32 uiBytesToSend			: buffer length
				  [OUT]NAP_UINT32 *piBytesSent			: number of bytes sent
				  [OUT]NAP_INT16 *piError			    : Error type
* Side Effect   : None
*******************************************************************************/

NAP_BOOL	NAP_REX_FWK_TC_SocketSend (
				NAP_SOCKET_ID	SocketId,
				 const NAP_VOID  *pvSendBuffer,
				 NAP_UINT16 uiBytesToSend,
				 NAP_UINT16 *piBytesSent,
				 NAP_INT16 *piError
				 )

{
	sint31 retVal;

	retVal = dss_write(SocketId, pvSendBuffer, uiBytesToSend,piError);

      	if(DSS_ERROR == retVal)
	{
		*piBytesSent = 0;

		switch(*piError)
			{
			case DS_EBADF:
				*piError = TC_EBADF;
				NAP_ERROR_MAJOR(*piError, "dss_write : DS_EBADF");
				break;
			case DS_ENOTCONN:
				*piError = TC_ENOTCONN;
				NAP_ERROR_MAJOR(*piError, "dss_write : DS_ENOTCONN");
				break;
			case DS_ECONNRESET:
				*piError = TC_ECONNRESET;
				NAP_ERROR_MAJOR(*piError, "dss_write : DS_ENOTCONN");
				break;
			case DS_ECONNABORTED:
				*piError = TC_ECONNABORTED;
				NAP_ERROR_MAJOR(*piError, "dss_write : DS_ECONNABORTED");
				break;
			case DS_EIPADDRCHANGED:
				*piError = TC_EIPADDRCHANGED;
				NAP_ERROR_MAJOR(*piError, "dss_write : DS_EIPADDRCHANGED");
				break;
			case DS_EPIPE:
				*piError = TC_EPIPE;
				NAP_ERROR_MAJOR(*piError, "dss_write : DS_EPIPE");
				break;
			case DS_ESHUTDOWN:
				*piError = TC_ESHUTDOWN;
				NAP_ERROR_MAJOR(*piError, "dss_write : DS_ESHUTDOWN :: Already Connected.");
				break;
			case DS_ENETDOWN:
				*piError = TC_ENETDOWN;
				NAP_ERROR_MAJOR(*piError, "dss_write : DS_ENETDOWN");
				break;
			case DS_EFAULT:
				*piError = TC_EFAULT;
				NAP_ERROR_MAJOR(*piError, "dss_write : DS_EFAULT");
				break;
			case DS_EMSGSIZE:
				*piError = TC_EMSGSIZE;
				NAP_ERROR_MAJOR(*piError, "dss_write : DS_EMSGSIZE");
				break;
			case DS_EWOULDBLOCK:
				*piError = TC_EWOULDBLOCK;
				NAP_ERROR_MAJOR(*piError, "dss_write : DS_EWOULDBLOCK");
				break;
			default :
				NAP_ERROR_MAJOR(*piError, "dss_write : default");
				break;
			}
		return (NAP_BOOL) NAP_FAILURE;
	}
	else
	{
		*piBytesSent = retVal;
		return (NAP_BOOL) NAP_SUCCESS;
	}

}

/*******************************************************************************
* Function name : NAP_REX_FWK_TC_SocketSendTo
* Description   : This function sends data on the specified UDP socket

* Arguments     : [IN]NAP_SOCKET_ID *SockId				: Socket id
				  [IN]const NAP_VOID  *pvSendBuffer		: buffer to send
				  [IN]NAP_UINT32 uiBytesToSend			: buffer length
				  [OUT]NAP_UINT32 *piBytesSent			: number of bytes sent
				  [IN]NAP_INT32 iDestAddr				: destination IP address
				  [IN]NAP_INT32 iDestPort				: destination port number
				  [OUT]NAP_INT16 *piError			    : Error type
* Side Effect   : None
*******************************************************************************/

NAP_BOOL	NAP_REX_FWK_TC_SocketSendTo(
				NAP_SOCKET_ID	SockId,
				const NAP_VOID  *pvSendBuffer,
				NAP_UINT16 uiBytesToSend,
				NAP_UINT16 *piBytesSent,
				NAP_UINT32 iDestAddr,
				NAP_UINT16 iDestPort,
				NAP_INT16 *piError
				)
{

	S_NAP_TC_SOCK_ADDR stServAddr;
	sint15 retVal;

	FWK_Ncm_OS_Memset(&stServAddr,0,sizeof(stServAddr));

	/* fill the destination address structure */
	stServAddr.sin_family = AF_INET;
	stServAddr.sin_addr.s_addr = dss_htonl(iDestAddr);
	stServAddr.sin_port = dss_htons(iDestPort);

	/* call rex sent function  and return the status*/
	retVal  = dss_sendto(SockId,pvSendBuffer,uiBytesToSend,0,
					(struct sockaddr *)&stServAddr,sizeof(stServAddr),piError);

	if(DSS_ERROR == retVal)
	{
		*piBytesSent = 0;

		switch(*piError)
		{
			case DS_EBADF:
				NAP_ERROR_MAJOR(*piError, "dss_sendto : DS_EBADF");
				break;
			case DS_EAFNOSUPPORT:
				NAP_ERROR_MAJOR(*piError, "dss_sendto : DS_EAFNOSUPPORT");
				break;
			case DS_EWOULDBLOCK:
				NAP_ERROR_MAJOR(*piError, "dss_sendto : DS_EWOULDBLOCK");
				break;
			case DS_ENETDOWN:
				NAP_ERROR_MAJOR(*piError, "dss_sendto : DS_ENETDOWN");
				break;
			case DS_EFAULT:
				NAP_ERROR_MAJOR(*piError, "dss_sendto : DS_EFAULT");
				break;
	#if 0
			case DS_EOPNOSUPPORT:
				ncm_print("dss_sendto : TC_EOPNOSUPPORT");
				break;
	#endif
			case DS_ENOROUTE:
				NAP_ERROR_MAJOR(*piError, "dss_sendto : DS_ENOROUTE");
				break;
			case DS_EADDRREQ:
				NAP_ERROR_MAJOR(*piError, "dss_sendto : DS_EADDRREQ");
				break;
			case DS_EMSGSIZE:
				NAP_ERROR_MAJOR(*piError, "dss_sendto : DS_EMSGSIZE");
				break;
			default :
				NAP_ERROR_MAJOR(*piError, "dss_sendto : default");
				break;
		}
		return (NAP_BOOL) NAP_FAILURE;

	}
	else
	{
		*piBytesSent = retVal;
		return (NAP_BOOL) NAP_SUCCESS;
	}

}

/*******************************************************************************
* Function name : NAP_REX_FWK_TC_SocketReceive
* Description   : This function read the data on the TCP socket

* Arguments     : [IN]NAP_SOCKET_ID *SockId				: Socket id
				  [OUT]NAP_UCHAR  *pcReadBuffer			: buffer to read
				  [IN]NAP_UINT16	uiBytesToRead		: buffer length to read
				  [OUT]NAP_UINT16	*piBytesRead		: number of bytes read
				  [OUT]NAP_INT16 *piError			    : Error type
* Side Effect   : None
*******************************************************************************/

NAP_BOOL	NAP_REX_FWK_TC_SocketReceive (
				NAP_SOCKET_ID	SockId,
				NAP_UCHAR  *pcReadBuffer,
				NAP_UINT16	uiBytesToRead,
				NAP_UINT16	*piBytesRead,
				NAP_INT16 *piError
				)

{
	sint15 retVal;

	/* call rex read function on the TCP socket and return the status */
	retVal = dss_read(SockId,pcReadBuffer,uiBytesToRead,piError);
	if (retVal == DSS_ERROR)
	{
		 *piBytesRead = 0;
		 return (NAP_BOOL) NAP_FAILURE;
	}
   	else
	{
		*piBytesRead = retVal;
		return (NAP_BOOL) NAP_SUCCESS;
	}
}

/*******************************************************************************
* Function name : NAP_REX_FWK_TC_SocketRecieveFrom
* Description   : This function read the data on the UDP socket

* Arguments     : [IN]NAP_SOCKET_ID *SockId				: Socket id
				  [OUT]NAP_UCHAR  *pcReadBuffer			: buffer to read
				  [IN]NAP_UINT16	uiBytesToRead		: buffer length to read
				  [OUT]NAP_UINT16	*piBytesRead		: number of bytes read
  				  [IN]NAP_INT32 iDestAddr				: destination IP address
				  [IN]NAP_INT32 iDestPort				: destination port number
				  [OUT]NAP_INT16 *piError			    : Error type
* Side Effect   : None
*******************************************************************************/

NAP_BOOL	NAP_REX_FWK_TC_SocketRecieveFrom(
				NAP_SOCKET_ID	SockId,
				const NAP_UCHAR  *pcReadBuffer,
				NAP_UINT16 uiBytesToRead,
				NAP_UINT16 *piBytesRead,
				NAP_UINT32 *piDestAddr,
				NAP_UINT16 *psDestPort,
				NAP_INT16 *piError
				)
{

	S_NAP_TC_SOCK_ADDR stServAddr;
	sint15 retVal;

	FWK_Ncm_OS_Memset(&stServAddr,0,sizeof(stServAddr));

	/* fill the destination address */
	/*
	stServAddr.sin_family = AF_INET;
	stServAddr.sin_addr.s_addr = dss_htonl(iDestAddr);
	stServAddr.sin_port = dss_htons(iDestPort);
	*/

	/* call rex read function on the UDP socket and return the status */
	retVal = dss_recvfrom(SockId,pcReadBuffer,uiBytesToRead,0,
					(struct sockaddr *)&stServAddr,sizeof(stServAddr),piError);

	if (retVal == DSS_ERROR)
	{
		 *piBytesRead = 0;
		 return (NAP_BOOL) NAP_FAILURE;
	}
    else
	{
		*piBytesRead = retVal;
		*piDestAddr = stServAddr.sin_addr.s_addr;
		*psDestPort = stServAddr.sin_port;
		return (NAP_BOOL) NAP_SUCCESS;
	}

}

/*******************************************************************************
* Function name : NAP_REX_FWK_TC_CloseSocket
* Description   : This function closes the specified socket

* Return type   : NAP_BOOL
* Arguments     : [IN]NAP_SOCKET_ID SockId	: Socket id
				  [OUT] NAP_INT16 *piError	: Error type
* Side Effect   : None
*******************************************************************************/

NAP_BOOL	NAP_REX_FWK_TC_CloseSocket (
				NAP_SOCKET_ID SocketId,
                NAP_INT16 *piError
                )
{

    NAP_INT16 iRetVal;

    iRetVal = dss_close(SocketId, piError);

	if(iRetVal == DSS_ERROR)
	{
		switch(*piError)
		{
			case DS_EBADF:
				*piError = TC_EBADF;
				NAP_ERROR_MAJOR(*piError, "dss_close : DS_EBADF");
				break;
			case DS_EWOULDBLOCK:
				*piError = TC_EWOULDBLOCK;
				NAP_ERROR_MAJOR(*piError, "dss_close : DS_EWOULDBLOCK");
				break;
			default :
				NAP_ERROR_MAJOR(*piError, "dss_close : default");
				break;
		}
		return (NAP_BOOL) NAP_FAILURE;
	}
	else
	{
		return (NAP_BOOL) NAP_SUCCESS;
	}
}


/*******************************************************************************
* Function name : NAP_REX_FWK_TC_NetworkStatus
* Description   : This function returns the network status about the PPP session

* Return type   : NAP_BOOL
* Arguments     : [IN] NAP_APP_ID *AppID	:Application Id
				  [OUT] NAP_INT16 *piStatus	: status of PPP session
* Side Effect   : None
*******************************************************************************/


NAP_BOOL	NAP_REX_FWK_TC_NetworkStatus(
				NAP_APP_ID AppID,
                NAP_INT16 *piStatus
                )

{
	/* call rex network status function */
    dss_netstatus(AppID, piStatus);

    return (NAP_BOOL) NAP_SUCCESS;
}

/*******************************************************************************
* Function name : NAP_REX_FWK_TC_GetNextSocketEvent
* Description   : This function returns the event and socket id for which event
				  occured

* Arguments     : [IN] NAP_APP_ID *AppID		: Application Id
				  [OUT] NAP_SOCKET_ID *SocketId	: returns the Socket id for which
												  event occur
				  [OUT] NAP_INT32 *piEvent		: event type occurs
				  [OUT] NAP_INT16 *piError		: Error Type
* Side Effect   : None
*******************************************************************************/
NAP_BOOL	NAP_REX_FWK_TC_GetNextSocketEvent(
				NAP_APP_ID AppID,
				NAP_SOCKET_ID *SocketId,
				NAP_INT32 *piEvent,
                NAP_INT16 *piError
                )
{
    /* call rex next evnet function and return the status */
    *piEvent = 0; // Clear the Value

    *piEvent = dss_getnextevent(AppID,SocketId,piError);

    if( (*piEvent == DSS_ERROR) ||( *piEvent == 0))
    {
    		NAP_ERROR_MAJOR(*piError, "NAP_REX_FWK_TC_GetNextSocketEvent :: Failed");
	    return (NAP_BOOL) NAP_FAILURE;
    }
    else
	{
	    return (NAP_BOOL) NAP_SUCCESS;
	}

}

/*******************************************************************************
* Function name : NAP_REX_FWK_TC_SocketAsyncSelect
* Description   : This function is used to register events for the socket id so
				  application will be intimated when that event occurs for that
				  socket id

* Arguments     : [IN] NAP_SOCKET_ID *SocketId	: Socket id
												  event occur
				  [IN] NAP_INT32 iEventMask		: event types
				  [OUT] NAP_INT16 *piError		: Error Type
* Side Effect   : None
*******************************************************************************/

NAP_BOOL	NAP_REX_FWK_TC_SocketAsyncSelect (
				NAP_SOCKET_ID SocketId,
				NAP_INT32 iEventMask,
				NAP_UINT16 *piError
				)

{
	NAP_INT32 iRetVal;


    /* call rex event register function and return the status */
	iRetVal = dss_async_select(SocketId,iEventMask,piError);

    if (iRetVal == DSS_SUCCESS)
	{
	    return (NAP_BOOL) NAP_SUCCESS;
	}
    else
	{
	    return (NAP_BOOL) NAP_FAILURE;
	}

}


/*******************************************************************************
* Function name : NAP_REX_FWK_TC_SocketAsyncDeselect
* Description   : This function is used to deregister events for the socket id so
				  application will be intimated when that event occurs for that
				  socket id

* Arguments     : [IN] NAP_SOCKET_ID *SocketId	: Socket id
												  event occur
				  [IN] NAP_INT32 iEventMask		: event types
				  [OUT] NAP_INT16 *piError		: Error Type
* Side Effect   : None
*******************************************************************************/

NAP_BOOL	NAP_REX_FWK_TC_SocketAsyncDeselect (
				NAP_SOCKET_ID SocketId,
				NAP_INT32 iEventMask,
				NAP_UINT16 *piError
				)

{
	NAP_INT32 iRetVal;


    /* call rex event deregister function and return the status */
    iRetVal = dss_async_deselect(SocketId,iEventMask,piError);

    if (iRetVal == DSS_SUCCESS)
	{
	    return (NAP_BOOL) NAP_SUCCESS;
	}
    else
	{
	    return (NAP_BOOL) NAP_FAILURE;
	}

}


/*******************************************************************************
* Function name : NAP_REX_FWK_TC_accept
* Description   :

* return type   :
* Arguments     :
* Side Effect   : None
* NOTE          : None
*******************************************************************************/

NAP_SOCKET_ID	NAP_REX_FWK_TC_accept(
    				NAP_SOCKET_ID	SockId,
					S_NAP_TC_SOCK_ADDR *pstRemoteAddr,
					NAP_INT16 *piError
					)
{
	return (NAP_BOOL) 0;

}

/*******************************************************************************
* Function name : NAP_REX_FWK_TC_listen
* Description   :

* return type   :
* Arguments     :
* Side Effect   : None
* NOTE          : None
*******************************************************************************/

NAP_SOCKET_ID	NAP_REX_FWK_TC_listen(
    				NAP_SOCKET_ID	SockId,
					NAP_CHAR cMaxConnection,
					NAP_INT16 *piError
					)

{
	return (NAP_BOOL) 0;

}



/*******************************************************************************
* Function name : NAP_REX_FWK_TC_bindSocket
* Description   :

* return type   :
* Arguments     :
* Side Effect   : None
* NOTE          : None
*******************************************************************************/
NAP_BOOL	NAP_REX_FWK_TC_bindSocket(
								NAP_SOCKET_ID SockId,
								 NAP_UINT32 iLocalAddr,
								 NAP_UINT16 iLocalPort,
								 NAP_INT16 *piError)

{
	sint15 status;
	struct sockaddr_in localAddr;

	FWK_Ncm_OS_Memset(&localAddr,0,sizeof(localAddr));

	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = INADDR_ANY; //dss_htonl(iLocalAddr);
	localAddr.sin_port = FWK_Ncm_OS_HTONS(iLocalPort);

	status = dss_bind((sint15)SockId,(struct sockaddr *)&localAddr,sizeof (localAddr),piError);
	if(status == DSS_ERROR)
	{
		switch(*piError)
			{
			case DS_EBADF:
				NAP_ERROR_MAJOR(*piError, "dss_bind : DS_EBADF");
				break;
			case DS_EOPNOTSUPP:
				NAP_ERROR_MAJOR(*piError, "dss_bind : DS_EOPNOTSUPP");
				break;
			case DS_EADDRINUSE:
				NAP_ERROR_MAJOR(*piError, "dss_bind : DS_EADDRINUSE");
				break;
			case DS_EINVAL:
				NAP_ERROR_MAJOR(*piError, "dss_bind : DS_EINVAL");
				break;
			case DS_EFAULT:
				NAP_ERROR_MAJOR(*piError, "dss_bind : DS_EFAULT");
				break;
			default :
				NAP_ERROR_MAJOR(*piError, "dss_bind : default");
				break;
			}
		return (NAP_BOOL) NAP_FAILURE;
	}
	else
	{
		return (NAP_BOOL) NAP_SUCCESS;
	}
}




/**********************************************************************
**********************************************************************/

/******************************************************************************
* Function name : FWK_Ncm_RexOpenPPPSession
* Description   : This function opens PPP session
* Return type   : NAP_BOOL
* Arguments     : [IN] NAP_INT16 *sAppID	:Application Id
				  [OUT] NAP_INT16 *psError	:error type
* Side Effect   : None
******************************************************************************/
NAP_BOOL	FWK_Ncm_Rex_OpenPPPSession (NAP_INT16 sAppID,NAP_INT16 *psError)
{
#ifdef NAP_ENV_REX
	NAP_INT16 sRetVal =0;
	if (psError == NAP_NULL)
		return NAP_FAILURE;
	*psError =0;
	sRetVal = dss_pppopen(sAppID,psError);
	if(DSS_ERROR == sRetVal)
		return NAP_FAILURE;
#endif
	return NAP_SUCCESS;
}

/******************************************************************************
* Function name : FWK_Ncm_Rex_ClosePPPSession
* Description   : This function closes PPP session

* Return type   : NAP_BOOL
* Arguments     : [IN] NAP_INT16 *AppID	:Application Id
				  [OUT] NAP_INT16 *psError	:error type
* Side Effect   : None
******************************************************************************/
NAP_BOOL	FWK_Ncm_Rex_ClosePPPSession (NAP_INT16 sAppID,NAP_INT16 *psError)
{
#ifdef NAP_ENV_REX
	NAP_INT16 sRetVal =0;
	if (psError == NAP_NULL)
		return NAP_FAILURE;
	*psError =0;
    sRetVal =  dss_pppclose(sAppID, psError);
    if(DSS_ERROR == sRetVal)
		return NAP_FAILURE;
#endif
	return NAP_SUCCESS;
}



/*******************************************************************************
* Function name : FWK_Ncm_Rex_NetworkStatus
* Description   : This function provides status of network subsystem.  Called in
				 response to DS_ENETDOWN errors.  Note that origination status
				 is based on the last attempted origination.
* Return type   : NAP_BOOL
* Arguments     : [IN] NAP_INT16 *AppID	:Application Id
				  [OUT] NAP_INT16 *psStatus	:
				  [OUT] NAP_INT16 *psError : Error type
* Side Effect   : None
*******************************************************************************/
NAP_BOOL	FWK_Ncm_Rex_NetworkStatus(IN	NAP_INT16 sAppID,
								OUT	NAP_INT16 *psStatus,
								OUT	NAP_INT16 *psError)

{
#ifdef NAP_ENV_REX
	NAP_INT16 sRetVal;
	if (psError == NAP_NULL)
		return NAP_FAILURE;
	*psError = 0;
	/* call rex network status function */
    dss_netstatus(AppID, piStatus);
    if (sRetVal == DSS_ERROR)
	    return NAP_FAILURE;
	*psStatus = sRetVal;
#endif
	return NAP_SUCCESS;
}

/*******************************************************************************
* Function name : FWK_Ncm_Rex_SocketAsyncSelect
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
NAP_BOOL	FWK_Ncm_Rex_SocketAsyncSelect (IN	NAP_INT16 pSocketId,
									IN	NAP_INT32 iEventMask,
									OUT	NAP_UINT16 *psError)
{
#ifdef NAP_ENV_REX
	NAP_INT32 iRetVal;
	if (psError == NAP_NULL)
		return NAP_FAILURE;
	*psError = 0;
	iRetVal = dss_async_select(pSocketId,iEventMask,psError);
    if (iRetVal == DSS_ERROR)
	    return NAP_FAILURE;
#endif
	return NAP_SUCCESS;
}


/*******************************************************************************
* Function name : FWK_Ncm_Rex_SocketAsyncDeselect
* Description   : This function clears events of interest in the socket control
				  block interest mask.  The application specifies a bitmask of
				  events that it wishes to clear; events for which it will no
				  longer receive notification.
* Arguments     : [IN] NAP_INT32 *SocketId	: Socket id.
				  [IN] NAP_INT32 iEventMask	: event types
				  [OUT] NAP_INT16 *psError	: Error Type
* Side Effect   : None
*******************************************************************************/

NAP_BOOL	FWK_Ncm_Rex_SocketAsyncDeselect (IN	NAP_INT32 SocketId,
								IN	NAP_INT32 iEventMask,
								OUT	NAP_UINT16 *psError)

{
#ifdef NAP_ENV_REX
	NAP_INT32 iRetVal;
	if (psError == NAP_NULL)
		return NAP_FAILURE;
	*psError = 0;
    iRetVal = dss_async_deselect(SocketId,iEventMask,psError);
    if (iRetVal == DSS_ERROR)
	    return NAP_FAILURE;
#endif
	return NAP_SUCCESS;

}

/*******************************************************************************
* Function name : FWK_Ncm_Rex_GetNextEvent
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
NAP_BOOL	FWK_Ncm_Rex_GetNextEevent(NAP_INT16 sAppId,
								 NAP_INT16 *psSocketId,
								 NAP_INT32 *piEvent,
								 NAP_INT16 *psError)
{
#ifdef NAP_ENV_REX
	NAP_INT32 iResult =0;
	if (psError == NAP_NULL)
		return NAP_FAILURE;
	*psError = 0;
	iResult = dss_getnextevent(sAppId,psSocketId,psError);
	if (iResult = DSS_ERROR)
		return NAP_FAILURE;
	*piEvent = iResult;
#endif
	return NAP_SUCCESS;
}
#endif



NAP_BOOL FWK_Ncm_ByteToDotFormat(NAP_INT32  uiSrcIpAddress,
								  NAP_UCHAR *pDstIpAddress,
								  NAP_INT16 *psError)
{
	NAP_INT32 iTemp = 0,iTemp1 = 0,iTemp2 = 0,iTemp3 = 0;

	iTemp = uiSrcIpAddress  &  0xFF;
	uiSrcIpAddress = uiSrcIpAddress >> 8;

	iTemp1 = uiSrcIpAddress & 0xFF;
	uiSrcIpAddress = uiSrcIpAddress >> 8;

	iTemp2 = uiSrcIpAddress & 0xFF;
	uiSrcIpAddress = uiSrcIpAddress >> 8;

	iTemp3 = uiSrcIpAddress & 0xFF;
	FWK_Sprintf(pDstIpAddress,"%u.%u.%u.%u",iTemp,iTemp1,iTemp2,iTemp3);
	
	return NAP_SUCCESS;

}
