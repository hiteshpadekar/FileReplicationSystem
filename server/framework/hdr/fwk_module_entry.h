
#ifndef _FWK_MODULE_ENTRY_H_
#define _FWK_MODULE_ENTRY_H_

#include "fwk_datatypes.h"


typedef enum _E_NAP_UATKCBKS
{
	e_NAP_Uatk_Cbk_newCallRecv=0,
	e_NAP_Uatk_Cbk_newCallAckRecv,
	e_NAP_Uatk_Cbk_newFailCallAckRecv,
	e_NAP_Uatk_Cbk_cancelRecv,
	e_NAP_Uatk_Cbk_byeRecv,
	e_NAP_Uatk_Cbk_transactionRecv,
	e_NAP_Uatk_Cbk_reinviteRecv,
	e_NAP_Uatk_Cbk_reinviteAckRecv,
	e_NAP_Uatk_Cbk_unknownRecv,
	e_NAP_Uatk_Cbk_unexpRecv,
	e_NAP_Uatk_Cbk_overlapTransRecv,
	e_NAP_Uatk_Cbk_cancelComp,
	e_NAP_Uatk_Cbk_callFailed,
	e_NAP_Uatk_Cbk_callAccepted,
	e_NAP_Uatk_Cbk_byeRejected,
	e_NAP_Uatk_Cbk_callTerminated,
	e_NAP_Uatk_Cbk_callRedirected,
	e_NAP_Uatk_Cbk_transactionCompleted,
	e_NAP_Uatk_Cbk_inProgress,
	e_NAP_Uatk_Cbk_reinviteFailed,
	e_NAP_Uatk_Cbk_reinviteAccepted,
	e_NAP_Uatk_Cbk_rprRecv,
	e_NAP_Uatk_Cbk_overlapTransCompleted,
	e_NAP_Uatk_Cbk_noCallBack

}E_NAP_UATKCBKS;

/* This structure is filled by the decoder after decoding and processing 
   the message received	from network. */
typedef struct _S_AS_SIP_DECODE_DATA
{
	E_NAP_UATKCBKS		 enCbkType;
   	NAP_VOID			*pvCallObj;	
	NAP_VOID			*pvEventContext; //Can be deleted later-Appaji
	NAP_VOID			*pvOverlapTransInfo;
	//NAP_UINT16			 usDestModule;//Can be deleted later-Appaji
	
} S_AS_SIP_DECODE_DATA;

/* 
	This is used when we need to extract entire SIP message.
	This structure is filled by the decoder after decoding and processing 
	the message received	from network.
 */
typedef struct _S_AS_SIP_DECODE_DATA_WITH_BUFF
{
	S_AS_SIP_DECODE_DATA	*pstSipDecodeData;
	NAP_CHAR				*pucBuffer;
    NAP_UINT32				uiBufLen;
	
} S_AS_SIP_DECODE_DATA_WITH_BUFF;

/* this is the strcuture is used by AS modules and to be passed at the time of module specif
timers*/
typedef struct	_S_AS_TIMER_INFO
{
	NAP_UCHAR	  cModule;
	NAP_UCHAR	  cSubModule;
	NAP_UCHAR	  cEvent;//event on which timer is created*/
	NAP_VOID   	  *pvData;//module specific timer related data

} S_AS_TIMER_INFO;


/* XDMS::   This structure is filled by the decoder after decoding and processing 
   the message received	from network. */
typedef struct _S_XDMS_HTTP_DECODE_DATA
{ 
    NAP_UINT32			ussocketId;  // Socket descriptor	
   	NAP_VOID			*pvHttpTcb;  // HTTP TCB
	
} S_XDMS_HTTP_DECODE_DATA;


typedef enum _E_NAP_NW_SIG_EVENT
{
	//Responses
	e_SIG100Trying,
	e_SIG180Ringing,
	e_SIG181CallForwarded,
	e_SIG182Queued,
	e_SIG183SessionProgress,
	e_SIG200OK,
	e_SIG202Accepted,
	e_SIG300MultipleChoices,
	e_SIG301MovedPermanently,
	e_SIG302MovedTemporarily,
	e_SIG305UseProxy,
	e_SIG380AlernativeService,
	e_SIG400BadRequest,
	e_SIG401Unauthorized,
	e_SIG402PaymentRequired,
	e_SIG403Forbidden,
	e_SIG404NotFound,
	e_SIG405Method_NotAllowed,
	e_SIG406NotAcceptable,
	e_SIG407ProxyAuthenticationReq,
	e_SIG408requestTimeout,
	e_SIG410Gone,
	e_SIG412CondReqFailed,
	e_SIG413RequestEntryLarge,
	e_SIG414RequestURILarge,
	e_SIG415UnsupportedMediaType,
	e_SIG416UnsupportedURIScheme,
	e_SIG420BadExtension,
	e_SIG421ExtensionRequired,
	e_SIG423InternvalTooBrief,
	e_SIG480TemorarilyUnavailable,
	e_SIG481CallDoesNotExist,
	e_SIG482LoopDetected,
	e_SIG483TooManyHops,
	e_SIG484AddressIncomplete,
	e_SIG485Ambiguous,
	e_SIG486BusyHere,
	e_SIG487RequestTerminated,
	e_SIG488NotAcceptable,
	e_SIG491RequestPending,
	e_SIG493Undecipherable,
	e_SIG500ServerInternalError,
	e_SIG501NotImplemented,
	e_SIG502Badgateway,
	e_SIG503ServiceUnavailable,
	e_SIG504ServerTimeout,
	e_SIG505VersionNotSupported,
	e_SIG513MsgLarge,
	e_SIG600BusyEverywhere,
	e_SIG603Decline,
	e_SIG604DoesNotExistAnywhere,
	e_SIG606NotAcceptable,      // Corrected by Sudha :: e_SIG606NotAccepttable
	e_SIGINVITE_Received,       // Corrected by Sudha :: e_SIGINVITE_Recieved
	e_SIGBYE_Received,       // Corrected by Sudha :: e_SIGBYE_SIGRecieved
	e_SIGCANCEL_Received,       // Corrected by Sudha :: e_SIGCANCEL_Recieved
	e_SIGNOTIFY_Received,       // Corrected by Sudha :: e_SIGNOTIFY_Recieved
	e_SIGACK_Received,          // Corrected by Sudha :: e_SIGACK_Recieved
	e_SIGUPDATE_Received,       // Corrected by Sudha :: e_SIGUPDATE_Recieved
	e_SIGPUBLISH_Received,      // Corrected by Sudha :: e_SIGPUBLISH_Recieved
	e_SIGSUBSCRIBE_Received,    // Corrected by Sudha :: e_SIGSUBSCRIBE_Recieved
	e_SIGREFER_Received,        // Corrected by Sudha :: e_SIGREFER_Recieved
	e_SIGMESSAGE_Received,      // Corrected by Sudha :: e_SIGMESSAGE_Recieved
    e_SIGPRACK_Received,        // Added by Sudha
    e_SIGOPTIONS_Received,      // Added by Sudha
    e_SIGUNPUBLISH_Received,    // Added by Sudha
    e_SIGUNSUBSCRIBE_Received,  // Added by Sudha
	e_SIGUNKNOWN_Received       // Corrected by Sudha :: e_SIGUNKNOWN_Recieved

}  E_NAP_NW_SIG_EVENT;


/**** START::  AS MOdule Entry functions ******************************/

NAP_BOOL	PS_Main
(
	NAP_UINT8	srcId,
	NAP_UINT8	eventId,
	NAP_VOID	*pvdata,
	NAP_INT16	*psError
);


NAP_BOOL	RLS_Main
(
	NAP_UINT8	srcId,
	NAP_UINT8	eventId,
	NAP_VOID	*pvdata,
	NAP_INT16	*psError
);



NAP_BOOL	IM_Main
(	
	NAP_UINT8	srcId,
	NAP_UINT8	eventId,
	NAP_VOID	*pvdata,
	NAP_INT16	*psError
);

NAP_BOOL	PoC_Main
(
	NAP_UINT8	srcId,
	NAP_UINT8	eventId,
	NAP_VOID	*pvdata,
	NAP_INT16	*psError
);
NAP_BOOL CS_Main
(
	NAP_UINT8	srcId,
	NAP_UINT8	eventId,
	NAP_VOID	*pvdata,
	NAP_INT16	*psError
);

NAP_BOOL	VSC_Main
(
	NAP_UINT8	srcId,
	NAP_UINT8	eventId,
	NAP_VOID	*pvdata,
	NAP_INT16	*psError
);

/// added on 20-Jul-05 
NAP_BOOL  VSC_Init
(
	NAP_VOID
);


NAP_BOOL	VSC_DeInit
(
	NAP_VOID
);

// Added by BSP :: 17-Oct-05 for Poc Server
NAP_BOOL PoC_Server_Init
(
    NAP_INT16	*piError 
);

NAP_BOOL PoC_Server_DeInit
(
    NAP_INT16	*piError 
);

NAP_BOOL	VT_Main
(
	NAP_UINT8	srcId,
	NAP_UINT8	eventId,
	NAP_VOID	*pvdata,
	NAP_INT16	*psError
);

NAP_BOOL	RTFT_Main
(
	NAP_UINT8	srcId,
	NAP_UINT8	eventId,
	NAP_VOID	*pvdata,
	NAP_INT16	*psError
);


/**** END::  AS MOdule Entry functions ******************************/

/**** START:: XDMS Module Entry functions ******************************/

NAP_BOOL	Agg_Proxy_Main
(
	NAP_UINT8	srcId,
	NAP_UINT8	eventId,
	NAP_VOID	*pvdata,
	NAP_INT16	*psError
);

NAP_BOOL	XDMS_PoC_Main
(
	NAP_UINT8	srcId,
	NAP_UINT8	eventId,
	NAP_VOID	*pvdata,
	NAP_INT16	*psError
);

NAP_BOOL	XDMS_PTD_Main
(
	NAP_UINT8	srcId,
	NAP_UINT8	eventId,
	NAP_VOID	*pvdata,
	NAP_INT16	*psError
);

NAP_BOOL	ContentServer_Main
(
	NAP_UINT8	srcId,
	NAP_UINT8	eventId,
	NAP_VOID	*pvdata,
	NAP_INT16	*psError
);



NAP_BOOL	XDMS_Presence_Main
(	
	NAP_UINT8	srcId,
	NAP_UINT8	eventId,
	NAP_VOID	*pvdata,
	NAP_INT16	*psError
);

NAP_BOOL	XDMS_RLS_Main
(
	NAP_UINT8	srcId,
	NAP_UINT8	eventId,
	NAP_VOID	*pvdata,
	NAP_INT16	*psError
);


NAP_BOOL	XDMS_Shared_Main
(
	NAP_UINT8	srcId,
	NAP_UINT8	eventId,
	NAP_VOID	*pvdata,
	NAP_INT16	*psError
);

NAP_BOOL	XDMS_IM_Main
(
	NAP_UINT8	srcId,
	NAP_UINT8	eventId,
	NAP_VOID	*pvdata,
	NAP_INT16	*psError
);

NAP_BOOL CXDMS_Main
(
	NAP_UINT8	srcId,
	NAP_UINT8	eventId,
	NAP_VOID	*pvdata,
	NAP_INT16	*psError
);



/**** END::  XDMS Module Entry functions ******************************/
#endif //_FWK_MODULE_ENTRY_H_

