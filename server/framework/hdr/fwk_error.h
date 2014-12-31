#ifndef _FWK_ERROR_H_
#define _FWK_ERROR_H_

#include "fwk_datatypes.h"

/*
#ifndef FWK_ERROR_TO_FILESYSTEM
#define FWK_ERROR_TO_FILESYSTEM
#endif
*/

/*
#ifndef FWK_ERROR_LEVEL_ALL
#define FWK_ERROR_LEVEL_ALL
#endif
*/

/*
 *Make sure that only one Logging mechanism is enabled.The MACROS for the type
 *of Logging must be mentioned in Project Settings.The MACRO definitions are
 *explained in User Manual.
 */
#ifdef  FWK_ERROR_TO_TARGET
	#ifdef	FWK_ERROR_TO_FILESYSTEM
		#undef	FWK_ERROR_TO_FILESYSTEM
	#endif
	#ifdef FWK_ERROR_TO_CONSOLE
		#undef	FWK_ERROR_TO_CONSOLE
	#endif
#endif

#ifdef  FWK_ERROR_TO_FILESYSTEM
	#ifdef FWK_ERROR_TO_TARGET
		#undef	FWK_ERROR_TO_TARGET
	#endif
	#ifdef FWK_ERROR_TO_CONSOLE
		#undef	FWK_ERROR_TO_CONSOLE
	#endif
#endif

#ifdef  FWK_ERROR_TO_CONSOLE
	#ifdef FWK_ERROR_TO_TARGET
		#undef	FWK_ERROR_TO_TARGET
	#endif
	#ifdef	FWK_ERROR_TO_FILESYSTEM
		#undef	FWK_ERROR_TO_FILESYSTEM
	#endif
#endif

/*
 *Enabling all levels of Trace if macro FWK_LOG_LEVEL_ALL is mentioned in
 *Project Settings
 */

#ifdef FWK_ERROR_LEVEL_ALL
#ifndef FWK_ERROR_LEVEL_MINOR
#define FWK_ERROR_LEVEL_MINOR
#endif
#ifndef FWK_ERROR_LEVEL_MAJOR
#define FWK_ERROR_LEVEL_MAJOR
#endif
#ifndef FWK_ERROR_LEVEL_CRITICAL
#define FWK_ERROR_LEVEL_CRITICAL
#endif
#endif


/****************************  Macros ****************************************/

void FWK_ERROR_LOG(NAP_INT16 errorCode,NAP_CHAR *pcTrace,NAP_CHAR *pFile, NAP_INT32 iLine);

/*
 *Enabling Macros separately for logging in different levels.All of the these
 *Macros will be mapped to FWK_ERROR_LOG macro
 */

#ifdef FWK_ERROR_LEVEL_MINOR
#define	FWK_ERROR_MINOR(a,b)	FWK_ERROR_LOG(a,b,__FILE__,__LINE__)
#else
#define	FWK_ERROR_MINOR(a,b)
#endif

#ifdef FWK_ERROR_LEVEL_MAJOR
#define FWK_ERROR_MAJOR(a,b)    FWK_ERROR_LOG(a,b,__FILE__,__LINE__)
#else
#define	FWK_ERROR_MAJOR(a,b)
#endif

#ifdef FWK_ERROR_LEVEL_CRITICAL
#define FWK_ERROR_CRITICAL(a,b) FWK_ERROR_LOG(a,b,__FILE__,__LINE__)
#else
#define	FWK_ERROR_CRITICAL(a,b)
#endif

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
typedef enum _E_NAP_ERROR_
{
	/* Error Codes for FWK Module */
	e_Err_Fwk_Start = 1,

	// All error codes for FWK must go here.

       /* Hash Erroors*/
       e_Err_Fwk_Hash_InvalidParam,
       e_Err_Fwk_HashElementsNotFound,
       e_Err_Fwk_HashElementsExceeded,/*renamed e_Err_MsrpFwk_HashElementsExceeded
							to remove redefinition between msrp and framework*/
       
    
	// Memory Management, Timer, Link-List, IPC,
	e_Err_Timer_NotCreated,
	e_Err_Timer_ResetSuccess,
	e_Err_Timer_ResetFail,
	// String Manipulations.

	e_Err_Fwk_MallocFailed,
    e_Err_Fwk_FailedtoSendMsg,

	/* Error Codes for Network Module */
	e_Err_AccessNw_NetworkTempError,
	e_Err_AccessNw_NetworkFail,
	e_Err_AccessNw_NetworkInvalidParams,
	e_Err_AccessNw_NetworkMaxClientLimit,
	e_Err_AccessNw_NetworkMaxRetrans,
	e_Err_AccessNw_NetworkConnrefused,
	e_Err_AccessNw_NetworkTimeOut,
	e_Err_AccessNw_OpnInterrupted,
	e_Err_AccessNw_BadFileDesc,
	e_Err_AccessNw_PermissionDenied,
	e_Err_AccessNw_BadAddr,
	e_Err_AccessNw_InvalidArg,
	e_Err_AccessNw_TooManyFilesOpen,
	e_Err_AccessNw_OpnBlocked,
	e_Err_AccessNw_OpnInProgress,
	e_Err_AccessNw_OpnAlreadyInProgress,
	e_Err_AccessNw_InvalidSock,
	e_Err_AccessNw_DestAddrRequired,
	e_Err_AccessNw_MessageBig,
	e_Err_AccessNw_WrongProtocol,
	e_Err_AccessNw_ProtocolNotAvailable,
	e_Err_AccessNw_PorotocolNotSupported,
	e_Err_AccessNw_SocketNotSupported,
	e_Err_AccessNw_OpNotSupported,
	e_Err_AccessNw_ProtocolFamilyNotSupported,
	e_Err_AccessNw_AddressFamilyNotSupported,
	e_Err_AccessNw_AddInUse,
	e_Err_AccessNw_AddrNotAvailable,
	e_Err_AccessNw_NetworkDown,
	e_Err_AccessNw_NetWorkNotReachable,
	e_Err_AccessNw_NewworkReset,
	e_Err_AccessNw_ConnAborted,
	e_Err_AccessNw_ConnReset,
	e_Err_AccessNw_NoBufs,
	e_Err_AccessNw_SockAlreadyConnected,
	e_Err_AccessNw_SocketNotConnected,
	e_Err_AccessNw_CannotSendAfterShutdown,
	e_Err_AccessNw_TooManyReference,
	e_Err_AccessNw_TimeOut,
	e_Err_AccessNw_ConnRefused,
	e_Err_AccessNw_TooManyLevels,
	e_Err_AccessNw_NameTooLong,
	e_Err_AccessNw_HostIsDown,
	e_Err_AccessNw_HostNotReachable,
	e_Err_AccessNw_NonEmptyDir,
	e_Err_AccessNw_TooManyProcess,
	e_Err_AccessNw_TooManyUsers,
	e_Err_AccessNw_DiscFull,
	e_Err_AccessNw_WrongFileHandle,
	e_Err_AccessNw_TooManyLevelPath,
	e_Err_AccessNw_NwSubSystemNotAvailable,
	e_Err_AccessNw_WrongWinsockVesrion,
	e_Err_AccessNw_WinsockNotInitialized,
	e_Err_AccessNw_ShutDownInProgress,
	e_Err_AccessNw_HostNotFound,
	e_Err_AccessNw_ServerFailed,
	e_Err_AccessNw_ServerError,
	e_Err_AccessNw_ServerData,
	e_Err_AccessNw_NoAddressForLookUp,

	


	// Framework Queue
	e_Err_FWK_InvalidQueue,
	e_Err_Sip_MallocFailed,

	// IPC Success
    e_Err_IPC_FailedtoSendMsg,
    e_Err_IPC_FailedtoRecvMsg,
	e_Err_IPC_Success,
	e_Err_IPC_InvalParams,
	e_Err_IPC_GlobalQueueEmpty,
	e_Err_IPC_CmdQueueEmpty,
	e_Err_IPC_Failure,
	e_Err_Fwk_End,


	/* Error codes for List Module */
	e_Err_List_Start = 1301,

	// All Error codes for List goes here
	e_err_list_no_mem,
	e_err_list_inv_input,
	e_err_list_not_exist,
	e_err_list_node_not_exist,
	e_Err_List_End,

	/* Error codes for P-CSCF */
	e_Err_Pcscf_Start = 1351,

	// All PCSCF error codes goes here
	e_Err_Pcscf_Success,
	e_Err_Pcscf_InvalidParam,
	e_Err_Pcscf_UnrecognisedModule,
	e_Err_Pcscf_UnexpectedModule,
	e_Err_Pcscf_MallocFailed,
	e_Err_Pcscf_MsgDecompressionFailed,
	e_Err_Pcscf_DecodeMsgFailed,
	e_Err_Pcscf_StackFailure,
	e_Err_Pcscf_FailedToFormResponse,
	e_Err_Pcscf_FailedtoSendMsg,
	e_Err_Pcscf_ValidationFailed,
	e_Err_Pcscf_FailedTosetData,
	e_Err_Pcscf_FWKfailure,

	//PCSCF-Reg Error Codes goes here
	e_Err_Pcscf_Reg_Begin = 1401,
	e_Err_Pcscf_Reg_NoCB,
	e_Err_Pcscf_Reg_UnexpectedEvt,
	e_Err_Pcscf_Reg_End,

	e_Err_Pcscf_Sub_Begin = 1451,
	e_Err_Pcscf_Sub_RogueEvt,
	e_Err_Pcscf_Sub_NoCB,
	e_Err_Pcscf_Sub_UnexpectedEvt,
	e_Err_Pcscf_Sub_End,

	e_Err_Pcscf_CH_Begin = 1501,
	e_Err_Pcscf_CH_No_CB,
	e_Err_Pcscf_CH_InvalidEvent,
	e_Err_Pcscf_CH_End,

	e_Err_Pcscf_Pxy_Begin = 1551,
	e_Err_Pcscf_Pxy_InvalidParam,
	e_Err_Pcscf_Pxy_End,

	e_Err_Pcscf_End,

	e_Err_SCSCF_Begin = 1600,
	e_Err_SCSCF_Success,
	e_Err_SCSCF_InvalidParam,
	e_Err_SCSCF_UnrecognisedModule,
	e_Err_SCSCF_UnexpectedState,
	e_Err_SCSCF_UnexpectedModule,
	e_Err_SCSCF_UnexpectedProtoType,
	e_Err_SCSCF_MemAllocationFailure,
	e_Err_SCSCF_HssResultCodeError,
	e_Err_SCSCF_TimerCreationFailure,
	e_Err_SCSCF_StartTimerFailure,
	e_Err_SCSCF_StopTimerFailure,
	e_Err_SCSCF_GetMethodFailure,
	e_Err_SCSCF_GetHeaderFailure,
	e_Err_SCSCF_FormResponseFailure,
	e_Err_SCSCF_SendCall2PeerFailure,
	e_Err_SCSCF_MakeNotifyFailure,
	e_Err_SCSCF_HeaderManipulationError,
	e_Err_SCSCF_MemFreeFailure,
	e_Err_SCSCF_StartRegularTransactionFailure,
	e_Err_SCSCF_HashFailure,
	e_Err_SCSCF_HashInitFailure,
	e_Err_SCSCF_HashAddFailure,
	e_Err_SCSCF_RegCbFreeFailure,
	e_Err_SCSCF_UeSubCbFreeFailure,
	e_Err_SCSCF_PcscfSubCbFreeFailure,
	e_Err_SCSCF_PostEventFailure,
	e_Err_SCSCF_ChCbFreeFailure,
	e_Err_SCSCF_UserBarredForNAPServices,
	e_Err_SCSCF_FailedToFormHeader,
	e_Err_SCSCF_FailedToSetHeader,
	e_Err_SCSCF_FailedToGetRespCode,
	e_Err_SCSCF_FailedToSetSeconds,
	e_Err_NAP_SCSCF_StackFailure,
	e_Err_NAP_SCSCF_CH_No_CB,
	e_Err_NAP_SCSCF_CH_InvalidEvent,
	e_Err_NAP_SCSCF_FailedtoSendMsg,
	e_Err_NAP_SCSCF_FailedToFormResponse,
	e_Err_NAP_SCSCF_StrayResponse,
	e_Err_SCSCF_End,

	e_Err_MRFP_Begin = 1700,
	e_Err_MRFP_InvalidParam,
	e_Err_MRFP_MemoryAllocFailed,
	e_Err_MRFP_InvalidCmd,
	e_Err_MRFP_HandlerFailed,
	e_Err_MRFP_InvTerminationID,
	e_Err_MRFP_InvalidContextID,
	e_Err_MRFP_InvalidAddMode,
	e_Err_MRFP_CmdMsgDecoderFailed,
	e_Err_MRFP_CmdMsgEncoderFailed,
	e_Err_MRFP_RtpFwdFailed,
	e_Err_MRFP_DecoderFailed,
	e_Err_MRFP_EncoderFailed,
	e_Err_MRFP_InvVersioinNum,
	e_Err_MRFP_InvDestination,
	e_Err_MRFP_rcBuffNotAllocated,
	/*RTCP Handling Errors*/
	e_Err_MRFP_SrHndFailed,
	e_Err_MRFP_RrHndFailed,
	e_Err_MRFP_SdesHndFailed,
	e_Err_MRFP_ByeHndFailed,
	e_Err_MRFP_AppHndFailed,
	e_Err_MRFP_RtcpPktDropped,
	e_Err_MRFP_End,

  	e_Err_AS_Sip_Begin = 1900,
	e_Err_AS_Sip_Success,
	e_Err_AS_Sip_InvalidParam,
	e_Err_AS_Sip_UnrecognisedModule,
	e_Err_AS_Sip_UnexpectedState,
	e_Err_AS_Sip_UnexpectedModule,
	e_Err_AS_Sip_UnexpectedProtoType,
	e_Err_AS_Sip_MemAllocationFailure,
	e_Err_AS_Sip_HssResultCodeError,
	e_Err_AS_Sip_TimerCreationFailure,
	e_Err_AS_Sip_StartTimerFailure,
	e_Err_AS_Sip_StopTimerFailure,
	e_Err_AS_Sip_GetMethodFailure,
	e_Err_AS_Sip_GetHeaderFailure,
	e_Err_AS_Sip_FormResponseFailure,
	e_Err_AS_Sip_SendCall2PeerFailure,
	e_Err_AS_Sip_MakeNotifyFailure,
	e_Err_AS_Sip_HeaderManipulationError,
	e_Err_AS_Sip_MemFreeFailure,
	e_Err_AS_Sip_StartRegularTransactionFailure,
	e_Err_AS_Sip_HashFailure,
	e_Err_AS_Sip_HashInitFailure,
	e_Err_AS_Sip_HashAddFailure,
	e_Err_AS_Sip_RegCbFreeFailure,
	e_Err_AS_Sip_UeSubCbFreeFailure,
	e_Err_AS_Sip_PcscfSubCbFreeFailure,
	e_Err_AS_Sip_PostEventFailure,
	e_Err_AS_Sip_ChCbFreeFailure,
	e_Err_AS_Sip_UserBarredForNAPServices,
	e_Err_AS_Sip_FailedToFormHeader,
	e_Err_AS_Sip_FailedToSetHeader,
	e_Err_AS_Sip_FailedToGetRespCode,
	e_Err_AS_Sip_FailedToSetSeconds,
	e_Err_AS_Sip_HeaderListAccessFailure,
	e_Err_AS_Sip_BasicHeaderError,
    e_Err_AS_Sip_StackFailure,
	e_Err_AS_Sip_End,

    /* Error Codes for VSC Module */
	e_Err_Vsc_InvalidInputParams,
	e_Err_Vsc_UnexpectedSource,
	e_Err_Vsc_UE_Info_DoesNotExist,
	e_Err_Vsc_FailedToFormResponse,
	e_Err_Vsc_UnexpectedEvent,
	e_Err_Vsc_FailedtoSendMsg,
	e_Err_Vsc_StackFailure,
	e_Err_Vsc_FWKfailure,
    e_Err_Vsc_MallocFailed,
	e_Err_Vsc_End

}E_NAP_ERROR;

/*
*******************************************************************************
*                          INTERFACE FUNCTION PROTOTYPES
*******************************************************************************
*/
//Initialize function will set the error module for tracing log.
NAP_BOOL 	FWK_ERROR_Init
(
	NAP_INT16		*piError
);
NAP_VOID 	FWK_ERROR_DeInit
(
	NAP_VOID
);
#endif // End of _FWK_ERROR_H_

