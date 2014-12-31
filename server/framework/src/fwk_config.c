
#include "fwk_trace.h"
#include "fwk_config.h"
#include "fwk_string.h"
#include "fwk_memory.h"
/* POC_IOT*/
#include "fwk_net.h"



NAP_BOOL (*gpfn_FwkConfig_FSMHdlr[e_FwkModuleEnd+1])
	(NAP_VOID *,NAP_VOID *,NAP_INT16 *)=
{
	FwkConfig_NullFunc,			// e_UeModule
	FwkConfig_NullFunc,			// e_AccessNwModule,
	FwkConfig_NullFunc,			// e_NapCoreModule,
	FwkConfig_NullFunc			// e_FwkModuleEnd
};
	
NAP_BOOL FwkConfig_NullFunc
(
	NAP_VOID *pvProfileData	,
	NAP_VOID *pvServerConfig,
	NAP_INT16 *psErrNo
)
{
	return NAP_FAILURE;
}




