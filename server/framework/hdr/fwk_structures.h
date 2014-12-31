
#ifndef _FWK_STRUCTURES_H_
#define _FWK_STRUCTURES_H_

#include "fwk_datatypes.h"

#define FWK_IP_ADDR_LEN		64
#define FWK_ID				NAP_UINT64

#define FWK_INVALID_PORT	65530
typedef struct _S_FWK_IMC_INFO
{
	NAP_UINT8	srcId;
	NAP_UINT8	destId;
	NAP_UINT8	protocolId;
	NAP_UINT8	actionEventId;
	NAP_VOID	*pvData;
} S_FWK_IMC_INFO;

typedef  enum _E_FWK_MODULE
{
	
	
	
	
	
	
		
	
	e_UeModule=0,
	e_AccessNwModule,
	e_NapCoreModule,
	e_PcscfModule, // P-CSCF
	e_ScscfModule, // S-CSCF
	e_MrfcModule,  // MGC
	e_MrfpModule,  // MG
	e_VtModule,
	e_VscModule,
	e_TimerModule,
	e_HssModuleAS,		//HSS-AS			/* 10 */
	e_HssModuleS_CSCF,  // HSS-NAP
	e_GuiModule,
	e_XdmsModuleHTTP,  // XDMS-HTTP
	e_XdmsModuleSIP,   // XDMS-SIP
	e_AsModuleHTTP,    // AS-HTTP
	e_AsModuleSIP,     // AS-SIP
	e_PsModule,
	e_RlsModule,
	e_PoCModule,
	e_ImModule,								/* 20 */
	e_RtftModule,
	e_MsrpModule,
	e_AggregationProxy,  // Aggregation-Proxy
	e_PresenceXDMS,      // Presence XDMS
	e_RlsXDMS,			 // RLS XDMS
	e_PoCXDMS,			 // PoC XDMS
	e_SharedXDMS,		 // Shared XDMS
	e_ContentServer,	 // Content Server
	e_PTDXDMS,			 // PTD XDMS
	e_IMXDMS,			 // IM XDMS			/* 30 */
	e_MsrpAppModule,
	e_MsrpDecoderModule,
	e_Reserved1,
	e_PcscfMtModule,	// Added for VSC support(For SIP Txn Seperation)    =34
	e_AsMtModuleSIP,	// Added for As2As support(For SIP Txn Seperation)  =35
	e_RTPStackModule,	 // RTP Stack Module
	e_PoCMediaMainModule,
	e_CapabilityServer, //namita 	
	e_CapabilityXDMS,	// namita 39
	e_StorageServer,	// namita			/* 40 */
	e_PtdModule,          // PTD Modules (excluding media)
    e_PtdMediaMainModule, // PTD Media(Data) Module
	e_Ptd_Module_Sub2Capability,
    e_HssClientModule,
    e_HssServerModule,	// Value = 45
	e_CttModule,
	e_IsModule,      // NAP_PHASE3_TEST : Add new services
	e_VonapModule,   // NAP_PHASE3_TEST : Add new services
	e_IPSecModule,
	e_NwIfPcapModule,		/* Network Interface WinPcap Module */	/* 50 */
	e_TCPIPStackModule,		/* TCP/IP Stack Module */
	e_SimpleIM_corModule,	/* Coordinator Module IM */
	e_reserver53,
	e_reserver54,
	e_reserver55,
	e_SimSigPFModule=56,		/* SIMPLE IM Signalling Participating Module */
	e_SimSigCFModule,		/* SIMPLE IM Signalling Controlling Module */
	e_SimSigDMFModule,		/* SIMPLE IM Signalling Deferred Module */
	e_SimSigCHFModule,		/* SIMPLE IM Signalling History Module */
	e_SimMediaPFModule,		/* SIMPLE IM Media Participating Module */
	e_SimMediaCFModule,		/* SIMPLE IM Media Controlling Module */
	e_SimCordnatorModule,	/* SIMPLE_IM CoOrdinator Module 62 */
	e_SimMediaCHFModule,	/* SIMPLE IM Media Deferred Module */
	e_SimMediaDMFModule,	/* SIMPLE IM Media History Module */
	e_SimXdmcModule,		/* SIMPLE IM XDMC Module (for both signalling and media) */  /* 60 */
	e_SimXdmsModule,		/* SIMPLE IM XDMS Module (for both signalling and media) */  /* 60 */
	e_Reserved70,
	e_QPF_Log=71,				//Logger Client Module
	e_Logger_ServerModule=72,	//Logger Server Module
	e_LoggerSettings,		//Logger Settings Module

	/* Added by Syed Malgimani for Integration with XDMS */
	e_SharedXdms_eas = 103,		/* Module for Listening Application Server Request */
	e_ImXdms_eas	=  108,
	e_FwkModuleEnd
} E_FWK_MODULE;

typedef  enum _E_FWK_PROTO
{
	e_SipProto=0,
	e_HssProto,
	e_MrfpProto,
	e_HttpProto,
	e_FwkProtoEnd
} E_FWK_PROTO;

typedef  enum _E_FWK_EVENT
{
	e_AccessNwFailure = 0,
	e_AccessNwSuccess = 1,
	/*Logger support*/
	e_LoggerTraceEvent,
	e_LoggerMessageEvent
}E_FWK_EVENT;

typedef enum _E_FWK_IPADDR_TYPE
{
	e_FWK_Ipv4=0,
	e_FWK_Ipv6
}E_FWK_IPADDR_TYPE;

typedef enum _E_FWK_SOC_TYPE
{
	e_FWK_Tcp=0,
	e_FWK_Udp
}E_FWK_SOC_TYPE;

/*This structure holds the transport related information received from the network.*/
typedef struct _S_FWK_TRANSPORT
{
	E_FWK_IPADDR_TYPE	eIptype;
	E_FWK_SOC_TYPE		eSoctype;
	NAP_UCHAR			*pucHost;
	NAP_UCHAR			*pucIpaddr;		// Remote IP addr (Dot Format)
	NAP_UINT16			usPort;			// Remote Port
	NAP_UCHAR			*pucLocalIpaddr;// Local IP Addr (Dot Format)
	NAP_UINT16			usLocalPort;	// Local Port
	NAP_UINT32			ussocketId;
	/* For any contextual data to be passed */
	NAP_VOID			*pvData;
}S_FWK_TRANSPORT;

/*The messages received by from network will be of this format.*/
typedef struct _S_FWK_NW_PARAMS
{
	NAP_UCHAR 			*pucInBuffer;
	NAP_UINT16			usInBufLen;
	S_FWK_TRANSPORT		stTptInfo;
}S_FWK_NW_PARAMS;

/* Prototype for Regitered modules Entry Function*/
typedef 	NAP_BOOL (*CB_MODULE_HANDLER)
					(NAP_UINT8	srcId,
					NAP_UINT8	eventId,
					NAP_VOID	*pvdata,
					NAP_INT16	*psError);

typedef struct _S_FWK_MODULE_INFO
{
	NAP_UINT8				srcId;
	CB_MODULE_HANDLER		cbModuleHandler;

} S_FWK_MODULE_INFO;


typedef struct _S_FWK_MODULE_LIST
{
	S_FWK_MODULE_INFO			stModuleInfo;
	struct _S_FWK_MODULE_LIST	*next;

} S_FWK_MODULE_LIST;


typedef struct _S_FWK_QUEUE_ELEM
{
	NAP_VOID * pvData;
	//struct _S_FWK_QUEUE *pstQNext;
	struct _S_FWK_QUEUE_ELEM *pstQNext;

}S_FWK_QUEUE_ELEM;

typedef struct _S_FWK_QUEUE
{
	S_FWK_QUEUE_ELEM	*pstQHead;
	S_FWK_QUEUE_ELEM	*pstQTail;

}S_FWK_QUEUE;

typedef struct _S_FWK_CB
{
	NAP_BOOL			bInUse;
	FWK_ID			ulFwkId;
	S_FWK_QUEUE			stFwkQueue;
	S_FWK_MODULE_LIST  *pstModuleList;
}S_FWK_CB;

NAP_BOOL	FWK_MNG_InitFramework
(
	NAP_INT16 *psError
);

NAP_BOOL	FWK_MNG_DeInitFramework
(
	NAP_INT16 *psError
);

/* This function creates an Framework instance and returns the unique framework ID.
All modules belonging to this framework has to be registered using the corresponding
framework Id. This function helps in using multiple instance of framework in single
application. */
NAP_BOOL	FWK_MNG_CreateFramework
(
	FWK_ID *pusFrameworkId,
	NAP_INT16 *psError
);

NAP_BOOL	FWK_MNG_DeleteFramework
(
	FWK_ID usFrameworkId,
	NAP_INT16 *psError
);

/*This function is used for registering the module to the frame work. The module
entry function pfxnModule is registered to the framework. Hereafter moduleID can
be used as a reference to the module. */
NAP_BOOL	FWK_MNG_Registration
(
	FWK_ID			usFrameworkId,
	NAP_UINT8			srcModuleID,
	CB_MODULE_HANDLER	CbModuleHandler,
	NAP_INT16			*psError
);

/*This function is used to de-register the module from the frame work. */
NAP_BOOL	FWK_MNG_Deregistration
(
	FWK_ID usFrameworkId,
	NAP_UINT8 moduleID,
	NAP_INT16 *psError
);

/*This function invokes the corresponding module based on the information in
S_FWK_IMC_INFO structure. */
NAP_BOOL	FWK_MNG_AddToQueue
(
	FWK_ID usFrameworkId,
	S_FWK_IMC_INFO *pstImcInfo,
	NAP_INT16 *psError
);

/* process the generic queue*/
NAP_BOOL	FWK_MNG_ProcessQueue
(
	FWK_ID usFrameworkId,
	NAP_INT16 *psError
);

NAP_BOOL FWK_FreeNwParam
(
	S_FWK_NW_PARAMS **ppstNw
);

#endif //_FWK_STRUCTURES_H_

