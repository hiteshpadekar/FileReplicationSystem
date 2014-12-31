
#ifndef __FWK_NET_H__
#define __FWK_NET_H__

#include "fwk_datatypes.h"

#ifdef  __cplusplus
extern "C" {
#endif


#ifdef NAP_ENV_WINDOWS
#define FWK_Ntohl(x)	ntohl((x))
#define FWK_Htonl(x)	htonl((x))
#define FWK_Ntohs(x)	ntohs((x))
#define FWK_Htons(x)	htons((x))
#endif /* NAP_ENV_WINDOWS */

/* 64 Bit Network to Host byte conversion */
#define FWK_Ntoh64(x)	FWK_Swap64((x))

/* 64 Bit Host to Network byte conversion */
#define FWK_Hton64(x)	FWK_Swap64((x))

/*===========================================================================*/

/*
 * This function determines whether the given string IPv4 address or not.
 */
NAP_BOOL FWK_NetIsV4Address
(
	const NAP_CHAR*				pszAddress
);

/*
 * This function converts the given IPv4 network form address to string.
 */
NAP_BOOL FWK_NetV4NtoA
(
	const NAP_UINT8*			netAddress,
	NAP_CHAR*					pszAddress,
	NAP_UINT16					usAddrLen
);

NAP_BOOL FWK_NetV4AtoN(NAP_CHAR *ipv4Format, NAP_UINT32 *addr);


/*
 * This function converts the given IPv6 network form address to string.
 */
NAP_BOOL FWK_NetV6NtoA
(
	const NAP_UINT8*			netAddress,
	NAP_CHAR*					pszAddress,
	NAP_UINT16					usAddrLen
);

/*
 * This function converts the given IPv6 network form address to string.
 */
NAP_BOOL FWK_GetSystemIpAddr( NAP_CHAR *pucIpAddr);


/*
 * This function gets the system host name
 */
NAP_BOOL FWK_GetSystemHostName( NAP_CHAR *pcHostname);


/*
 * This function swaps 64 bit Network to Host Byte Order for 
 * LITTLE ENDIAN machines
 */
NAP_UINT64 FWK_Swap64(NAP_UINT64 ui64Nw);


/*===========================================================================*/
#ifdef __cplusplus
}
#endif

#endif // __FWK_NET_H__
