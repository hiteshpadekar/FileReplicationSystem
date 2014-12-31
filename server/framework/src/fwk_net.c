
#include "fwk_net.h"
/* POC_IOT*/
#include "fwk_memory.h"
#include "fwk_string.h"
/*
*******************************************************************************
* Function name 	: FWK_NetIsV4Address
* Description   	: This is a utility function which determines whether given 
*                 			string is IPv4 address.
*                  
* Return type   	: NAP_BOOL
*                		 NAP_SUCCESS - if the string is IPv4 address
*
* Arguments     	: 
*    [IN]  			:pszAddress    : IP address in string form.
*
* Side Effect   : None 
* NOTE          : None.
*******************************************************************************
*/

NAP_BOOL FWK_NetIsV4Address
(
	const NAP_CHAR*				pszAddress
)
{
#if 0
	/*
	 * This function uses platform string to network format IP address
	 * conversion functions to validate the IP address format. These functions
	 * shall return failure in case of INVALID input.
	 */
	if (DSS_ERROR == dss_inet_aton(pszAddress, &stTempAddr))
	{
		return NAP_FAILURE;
	}
	else
	{
		return NAP_SUCCESS;
	}
#endif
    return NAP_SUCCESS;


}

/*
*******************************************************************************
* Function name : FWK_NetV4NtoA
* Description   : This is a utility function converts 4 byte address into Dot address
* 
* Return type   : NAP_BOOL
*                 NAP_SUCCESS - if the string is IPv4 address
*
* Arguments     : 
*    [IN]  netAddress    : IP address in network form( 4 bytes).
*    [OUT] pszAddress    : IP address in string form(Dot format).
*    [OUT] usAddrLen     : Length of the string(pszAddress Buffer length).
*
* Side Effect   : None 
* NOTE          : None.
*******************************************************************************
*/

NAP_BOOL FWK_NetV4NtoA
(
	const NAP_UINT8*			netAddress,
	NAP_CHAR*					pszAddress,
	NAP_UINT16					usAddrLen
)
{
#if 0
	if (DSS_ERROR == dss_inet_ntoa(*((const struct in_addr*)(netAddress)), 
		pszAddress, usAddrLen))
	{
		return NAP_FAILURE;
	}
	else
	{
		return NAP_SUCCESS;
	}
#endif
#ifdef NAP_ENV_WINDOWS
	struct in_addr in;
	NAP_UCHAR *pucAddr;
	in.S_un.S_addr = *((NAP_UINT32*)netAddress);

	pucAddr = inet_ntoa(in);
	if(NAP_NULL == pucAddr)
    {
        return NAP_FAILURE;
    }
     
	FWK_Strcpy(pszAddress,pucAddr);
    return NAP_SUCCESS;

#endif

}


/*
*******************************************************************************
* Function name : FWK_NetV4AtoN
* Description   : This is a utility function converts Dot address into 4 byte address
* 
* Return type   : NAP_BOOL
*                 NAP_SUCCESS - if the string is IPv4 address
*
* Arguments     : 
*    [IN]  netAddress    : IP address in network form.
*    [OUT] pszAddress    : IP address in string form.
*    [OUT] usAddrLen     : Length of the string.
*
* Side Effect   : None 
* NOTE          : None.
*******************************************************************************
*/
NAP_BOOL FWK_NetV4AtoN(NAP_CHAR *ipv4Format, NAP_UINT32 *addr)
{

#if 0
	/*
	 * This function uses platform string to network format IP address
	 * conversion functions to validate the IP address format. These functions
	 * shall return failure in case of INVALID input.
	 */
	 *addr = 0;
	if (DSS_ERROR == dss_inet_aton(ipv4Format, (struct in_addr*)addr))
	{
		return NAP_FAILURE;
	}
	else
	{
		return NAP_SUCCESS;
	}
#endif

#ifdef NAP_ENV_WINDOWS
	NAP_UINT32 ipAddr;
	ipAddr = inet_addr(ipv4Format);
	if(ipAddr == INADDR_NONE)
		return NAP_FAILURE;

	ipAddr = htonl(ipAddr);
	*addr = ipAddr;
	return NAP_SUCCESS;
#endif
	
}//End of FWK_NetV4AtoN


/*
*******************************************************************************
* Function name : FWK_NetV6NtoA
* Description   : This is a utility function which converts given IPv4 network
*                 form address to string.
* 
* Return type   : NAP_BOOL
*                 NAP_SUCCESS - if the string is IPv4 address
*
* Arguments     : 
*    [IN]  netAddress    : IP address in network form.
*    [OUT] pszAddress    : IP address in string form.
*    [OUT] usAddrLen     : Length of the string.
*
* Side Effect   : None 
* NOTE          : This code is copied from ISC source from 
*                 http://samba.anu.edu.au/rsync/doxygen/head/inet__ntop_8c-source.html
*                 Therefore, as per the copyright statement issued in the ISC
*                 following contents must be present in this file.
*
*
* Copyright (C) 1996-2001  Internet Software Consortium.
*
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
#define NAP_NS_INT16SZ       2
#define NAP_NS_IN6ADDRSZ    16

NAP_BOOL FWK_NetV6NtoA
(
	const NAP_UINT8*			netAddress,
	NAP_CHAR*					pszAddress,
	NAP_UINT16					usAddrLen
)

{
    /*
     * Note that int32_t and int16_t need only be "at least" large enough
     * to contain a value of the specified size.  On some systems, like
     * Crays, there is no such thing as an integer variable with 16 bits.
     * Keep this in mind if you think this function should have been coded
     * to use pointer overlays.  All the world's not a VAX.
     */
    char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"], *tp;
    struct { int base, len; } best, cur;
    unsigned int words[NAP_NS_IN6ADDRSZ / NAP_NS_INT16SZ];
    int i;

    /*
     * Preprocess:
     *      Copy the input (bytewise) array into a wordwise array.
     *      Find the longest run of 0x00's in src[] for :: shorthanding.
     */
    memset(words, '\0', sizeof words);
    for (i = 0; i < NAP_NS_IN6ADDRSZ; i++)
            words[i / 2] |= (netAddress[i] << ((1 - (i % 2)) << 3));
    best.base = -1;
    cur.base = -1;
    for (i = 0; i < (NAP_NS_IN6ADDRSZ / NAP_NS_INT16SZ); i++) {
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
    for (i = 0; i < (NAP_NS_IN6ADDRSZ / NAP_NS_INT16SZ); i++) {
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
                    if (FWK_NetV4NtoA(netAddress+12, tp,
                                    (NAP_UINT16)(sizeof tmp - (tp - tmp))) == NAP_FAILURE)
                            return NAP_FAILURE;
                    tp += strlen(tp);
                    break;
            }
            tp += sprintf(tp, "%x", words[i]);
    }
    /* Was it a trailing run of 0x00's? */
    if (best.base != -1 && (best.base + best.len) ==
        (NAP_NS_IN6ADDRSZ / NAP_NS_INT16SZ))
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


NAP_BOOL FWK_GetSystemIpAddr( NAP_CHAR *pucIpAddr)
{
#if 0
	Get_Ipaddr_Inppp(pucIpAddr);
#endif

	/* POC_IOT*/

#ifdef NAP_ENV_WINDOWS
	NAP_CHAR cHostName[126] = {0},aucBuf[32];
	LPHOSTENT lpHostEntry = NAP_NULL;
	struct sockaddr_in stIpAddr;

	if (pucIpAddr == NAP_NULL)
		return NAP_FAILURE;

	 FWK_Memset(&stIpAddr,0,sizeof(struct sockaddr_in));
	 gethostname(cHostName,126);
	 lpHostEntry = gethostbyname(cHostName);
	 if (lpHostEntry == NAP_NULL)
		 return NAP_FAILURE;
	 stIpAddr.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);
	 FWK_Sprintf(pucIpAddr,"%x",htonl(stIpAddr.sin_addr.s_addr));

	 FWK_NetV4NtoA
		(
			(NAP_UINT8*)&stIpAddr.sin_addr.s_addr,
			aucBuf,
			32
		);
	 FWK_Strcpy(pucIpAddr,aucBuf);

#endif
	return NAP_SUCCESS;
}



/*
 * This function gets the system host name
 */
NAP_BOOL FWK_GetSystemHostName( NAP_CHAR *pcHostname)
{
#ifdef NAP_ENV_WINDOWS
	NAP_CHAR cHostName[128] = "";

	gethostname(cHostName,128);

	if(NAP_NULL != pcHostname)
	{
		FWK_Strcpy(pcHostname,cHostName);	
	}

#endif

	return NAP_SUCCESS;
}



NAP_UINT64 FWK_Swap64(NAP_UINT64 ui64Nw)
{
	NAP_UINT64 ui64Host;
	NAP_UCHAR *pucTempA = (NAP_UCHAR*) &ui64Host;
	NAP_UCHAR *pucTempB = (NAP_UCHAR*) &ui64Nw;
	
	pucTempA[0] = pucTempB[7]; 
	pucTempA[1] = pucTempB[6]; 
	pucTempA[2] = pucTempB[5]; 
	pucTempA[3] = pucTempB[4];
	pucTempA[4] = pucTempB[3]; 
	pucTempA[5] = pucTempB[2]; 
	pucTempA[6] = pucTempB[1]; 
	pucTempA[7] = pucTempB[0];

	return ui64Host;
}

