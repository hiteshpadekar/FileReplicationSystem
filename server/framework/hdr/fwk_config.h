#ifndef _FWK_CONFIG_H_
#define _FWK_CONFIG_H_


/* fwk include files */
#include "fwk_datatypes.h"
#include "fwk_structures.h"

#include "fwk_ConfigInterface.h"



typedef enum _E_FWK_CONFIG_ERR
{
	e_FwkConfig_InvConfigData,
	e_FwkConfig_InvModule,
	e_FwkConfig_MemFail,
	e_FwkConfig_InvInput

}E_FWK_CONFIG_ERR;


NAP_BOOL FwkConfig_NullFunc
(
	NAP_VOID *pvProfileData	,
	NAP_VOID *pvServerConfig,
	NAP_INT16 *psErrNo
);


#endif
