#ifndef _FWK_CONFIG_INTERFACE_H_
#define _FWK_CONFIG_INTERFACE_H_


#include "fwk_datatypes.h"

typedef struct _S_FWK_SERVER_CONFIG
{
	NAP_UINT32  uiServerId;
	NAP_UCHAR   *pucIpAddr;
	NAP_UINT16  uiPort;

}S_FWK_SERVER_CONFIG;

NAP_BOOL Fwk_Config_Main
(
	NAP_VOID    **ppProfileData,
	NAP_UINT32  uiModule,
	NAP_INT16   *psErrNo
);

NAP_BOOL Fwk_Config_GetSvrConfig
(
	S_FWK_SERVER_CONFIG	 **ppSrvrConfig,
	NAP_UINT32            uiServerID,
	NAP_INT16			*psErrNo
);
#endif