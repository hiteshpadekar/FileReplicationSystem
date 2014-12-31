
#ifndef _FWK_TRACE_H_
#define _FWK_TRACE_H_

/*
*******************************************************************************
*                               INCLUDE FILES
*******************************************************************************
*/
#include "fwk_datatypes.h"

/*
#ifndef FWK_TRACE_TO_FILESYSTEM
#define FWK_TRACE_TO_FILESYSTEM	
#endif	


#ifndef FWK_TRACE_LEVEL_ALL
#define FWK_TRACE_LEVEL_ALL	
#endif
*/


/*
 *Make sure that only one Logging mechanism is enabled.The MACROS for the type
 *of Logging must be mentioned in Project Settings.The MACRO definitions are 
 *explained in User Manual.
 */ 
#ifdef  FWK_TRACE_TO_TARGET
	#ifdef	FWK_TRACE_TO_FILESYSTEM
		#undef	FWK_TRACE_TO_FILESYSTEM
	#endif
	#ifdef FWK_TRACE_TO_CONSOLE
		#undef	FWK_TRACE_TO_CONSOLE
	#endif
#endif

#ifdef  FWK_TRACE_TO_FILESYSTEM
	#ifdef FWK_TRACE_TO_TARGET
		#undef	FWK_TRACE_TO_TARGET
	#endif
	#ifdef FWK_TRACE_TO_CONSOLE
		#undef	FWK_TRACE_TO_CONSOLE
	#endif
#endif

#ifdef  FWK_TRACE_TO_CONSOLE
	#ifdef FWK_TRACE_TO_TARGET
		#undef	FWK_TRACE_TO_TARGET
	#endif
	#ifdef	FWK_TRACE_TO_FILESYSTEM
		#undef	FWK_TRACE_TO_FILESYSTEM
	#endif
#endif

/*
 *Enabling all levels of Trace if macro FWK_LOG_LEVEL_ALL is mentioned in  
 *Project Settings
 */
#ifdef FWK_TRACE_LEVEL_ALL
#ifndef FWK_TRACE_LEVEL_FXNS
	#define FWK_TRACE_LEVEL_FXNS
#endif
#ifndef FWK_TRACE_LEVEL_BRIEF
	#define FWK_TRACE_LEVEL_BRIEF
#endif
#ifndef FWK_TRACE_LEVEL_DETAIL
	#define FWK_TRACE_LEVEL_DETAIL
#endif
#endif

#if 0 // For PF-UT : Trace capture
//Removed FWK_TRACE_LEVEL_FXNS for NAP Phase3 release
#ifdef FWK_TRACE_LEVEL_FXNS
#undef FWK_TRACE_LEVEL_FXNS
#endif
#endif

/* Maximum trace length allowed */
#define FWK_TRACE_MAX_TRACE_LEN 255
/*
 *Enabling Macros separately for logging in different levels.All of the these 
 *Macros will be mapped to FWK_TRACE_LOG macro
 */
void FWK_TRACE_VA_LOG(NAP_UINT32 module, NAP_CHAR *pFmtStr,...);
void FWK_TRACE_LOG(NAP_UINT32 module,NAP_CHAR *pcTrace,NAP_CHAR *pcFile,NAP_INT32 iLine);
NAP_VOID FWK_LOG_MESSAGE ( NAP_UINT32  module,NAP_CHAR	*pcMessage );
#ifdef FWK_TRACE_LEVEL_FXNS
#define	FWK_TRACE_FXN(a,b)              FWK_TRACE_LOG(a,b,__FILE__,__LINE__)
#else
#define	FWK_TRACE_FXN(a,b)	
#endif

#ifdef FWK_TRACE_LEVEL_BRIEF
#define 	FWK_TRACE_BRIEF(a,b)           FWK_TRACE_LOG(a,b,__FILE__,__LINE__)
#else
#define	FWK_TRACE_BRIEF(a,b)	
#endif

#ifdef FWK_TRACE_LEVEL_DETAIL
#define 	FWK_TRACE_DETAIL(a,b)         FWK_TRACE_LOG(a,b,__FILE__,__LINE__)
#else
#define	FWK_TRACE_DETAIL(a,b)	
#endif

#define FWK_TRACE_MESSAGE(a,b) FWK_LOG_MESSAGE(a,b)
/*
*******************************************************************************
*                               TYPEDEF DEFINITIONS
*******************************************************************************
*/
/*
*******************************************************************************
*                               ENUM DEFINITIONS
*******************************************************************************
*/
// Enum of Trace Module Ids
#define	eTrace_Framework			((NAP_UINT32)0x00000001)
#define	eTrace_Pcscf				((NAP_UINT32)0x00000002)
#define	eTrace_Scscf				((NAP_UINT32)0x00000004)
#define	eTrace_P1Fwk				((NAP_UINT32)0x00000008)
#define	eTrace_Mrfc					((NAP_UINT32)0x00000010)
#define	eTrace_Mrfp					((NAP_UINT32)0x00000020)
#define	eTrace_Vt_As				((NAP_UINT32)0x00000040)
#define	eTrace_Hss					((NAP_UINT32)0x00000080)
#define	eTrace_P2Fwk				((NAP_UINT32)0x00000100)
#define	eTrace_AccessNw				((NAP_UINT32)0x00000200)
#define	eTrace_NAPerv				((NAP_UINT32)0x00000400)
#define	eTrace_PoCServer			((NAP_UINT32)0x00000800)
#define	eTrace_As					((NAP_UINT32)0x00001000)
#define	eTrace_PoCMedia				((NAP_UINT32)0x00002000)
#define	eTrace_Pxdms				((NAP_UINT32)0x00004000)
#define	eTrace_Rxdms				((NAP_UINT32)0x00008000)
#define	eTrace_Sxdms				((NAP_UINT32)0x00010000)
#define	eTrace_PoCXdms				((NAP_UINT32)0x00020000)
#define	eTrace_AggProxy				((NAP_UINT32)0x00040000)
#define	eTrace_Ps					((NAP_UINT32)0x00080000)
#define	eTrace_Rls					((NAP_UINT32)0x00100000)
#define	eTrace_ContentServ			((NAP_UINT32)0x00200000)
#define	eTrace_RtpStackAbs			((NAP_UINT32)0x00400000)
#define	eTrace_Http					((NAP_UINT32)0x00800000)
#define	eTrace_SipAbstraction		((NAP_UINT32)0x01000000)
#define	eTrace_PTDXdms				((NAP_UINT32)0x02000000)
#define	eTrace_PTDMedia				((NAP_UINT32)0x04000000)
#define	eTrace_PtdServer			((NAP_UINT32)0x08000000)
#define	eTrace_Media				((NAP_UINT32)0x10000000)
#define	eTrace_SimPfSigServer		((NAP_UINT32)0x20000000)
#define	eTrace_SimCfSigServer		((NAP_UINT32)0x40000000)
#define	eTrace_SimDmfSigServer		((NAP_UINT32)0x80000000)
#define	eTrace_SimChfSigServer		((NAP_UINT32)0x00000002)
#define eTrace_SimXdmc			    ((NAP_UINT32)0x00000008)//Unique TraceId
#define	eTrace_All					((NAP_UINT32)0xFFFFFFFF)

/*
*******************************************************************************
*                               STRUCTURE DEFINITIONS
*******************************************************************************
*/

/*
*******************************************************************************
*                          INTERFACE FUNCTION PROTOTYPES
*******************************************************************************
*/

/*Initialize function will set the trace module for tracing logs.In case of 
tracing into file system, file will be created & closed. */
NAP_BOOL 	FWK_TRACE_Init 
(
	NAP_UINT32 	ulModuleTrace,
	NAP_INT16	*piError
);

NAP_VOID FWK_TRACE_DeInit(NAP_VOID);

#endif // End of  _FWK_TRACE_H_
