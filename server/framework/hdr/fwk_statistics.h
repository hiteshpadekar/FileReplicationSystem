
#ifndef _NAP_STATISTICS_H_
#define _NAP_STATISTICS_H_

#include "fwk_datatypes.h"
#include "fwk_trace.h" //for including FWK_TRACE_DETAIL Macro definition
#include "fwk_error.h"
#include "fwk_string.h"
/*
*******************************************************************************
*                               MACRO DEFINITIONS
*******************************************************************************
*/

#define NO_STAT_OBJ 67


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
typedef enum    _E_STATS_OBJ_ID
{
	/* 7 Stats for HTTP */
	eStat_Http_Num_Msg_Rcvd	= 0x0000,		
	eStat_Http_Num_Msg_Send,
	eStat_Http_Num_Msg_Decoded,
	eStat_Http_Num_Msg_DecFail,
	eStat_Http_Total_Mem,
	eStat_Http_Curr_Free_Mem,
	eStat_Http_Max_Used_Mem,

	/* 9 Stats for RTP & RTCP */
	eStat_Rtp_Num_Pkts_Send,
	eStat_Rtp_Num_Pkts_Rcvd,
	eStat_Rtcp_Num_Pkts_Send,
	eStat_Rtcp_Num_Pkts_Rcvd,
	eStat_Rtcp_App_Pkts_Send,
	eStat_Rtcp_App_Pkts_Rcvd,
	eStat_Rtp_Total_Mem,
	eStat_Rtp_Curr_Free_Mem,
	eStat_Rtp_Max_Used_Mem,

	/* 15 Stats for Sip Signaling */
	eStat_Sip_Num_Call_SetUp,
	eStat_Sip_Num_Call_Fail_UserBusy,
	eStat_Sip_Num_Call_Fail_NetwkFail,
	eStat_Sip_Num_Call_Fail_SrvrUnavail,
	eStat_Sip_Num_Reg_Success,
	eStat_Sip_Num_Reg_Fail_Auth,
	eStat_Sip_Num_Reg_Fail_UnknwnReason,
	eStat_Sip_Num_Publish_Req_Accp,
	eStat_Sip_Num_Publish_Fail_RsrcUnavil, // Resource Unavailable 
	eStat_Sip_Num_Publish_Fail_Auth,
	eStat_Sip_Num_IM_Send_Success,
	eStat_Sip_Num_IM_Fail,
	eStat_Sip_Total_Mem,
	eStat_Sip_Curr_Free_Mem,
	eStat_Sip_Max_Used_Mem,

	/* 8 Stats for Look up */
	eStat_Lkup_Num_DnsQry_Exec,
	eStat_Lkup_Num_DnsQry_Fail_Naptr,
	eStat_Lkup_Num_DnsQry_Fail_Srv,
	eStat_Lkup_Num_DnsQry_Fail_A,
	eStat_Lkup_Num_DnsQry_Fail_Aaaa,
	eStat_Lkup_Total_Mem,
	eStat_Lkup_Curr_Free_Mem,
	eStat_Lkup_Max_Used_Mem,

	/* 8 Stats for Message Body Handler */
	eStat_Msg_Num_Handled_Successfully,
	eStat_Msg_Num_Handled_Fail_Sdp,
	eStat_Msg_Num_Handled_Fail_Xml,
	eStat_Msg_Num_Handled_Fail_Sipfrag,
	eStat_Msg_Num_Handled_Fail_Refer,
	eStat_Msg_Total_Mem,
	eStat_Msg_Curr_Free_Mem,
	eStat_Msg_Max_Used_Mem,

	/* 15 Stats for SigComp*/
	eStat_Sgcmp_Num_Msg_Comp_Lzss_Algo,	
	eStat_Sgcmp_Num_Msg_Comp_Deflate_Algo,	
	eStat_Sgcmp_Num_Msg_Comp_Success,
	eStat_Sgcmp_Num_Msg_Comp_Fail,
	eStat_Sgcmp_Num_Msg_Decomp_Success,
	eStat_Sgcmp_Num_Msg_Decomp_Fail,
	eStat_Sgcmp_Num_Msg_Decomp_Fail_StateAccess,
	eStat_Sgcmp_Num_Msg_Decomp_Fail_MsgCorrupt,
	eStat_Sgcmp_Num_Msg_Decomp_Fail_BcodeCorrupt,
	eStat_Sgcmp_Num_Msg_Decomp_Fail_InsuffMemory,
	eStat_Sgcmp_Num_Msg_Decomp_Fail_MsgIndefinite,
	eStat_Sgcmp_Num_Msg_Decomp_Fail_IncompVers,
	eStat_Sgcmp_Total_Mem,
	eStat_Sgcmp_Curr_Free_Mem,
	eStat_Sgcmp_Max_Used_Mem,

	/* 8 Stats for XML */
	eStat_Xml_Num_Msg_Parsed_Success,
	eStat_Xml_Num_Msg_Parse_Fail_Syntax,
	eStat_Xml_Num_Msg_Parse_Fail_Schema,
	eStat_Xml_Num_Msg_Packed_Success,
	eStat_Xml_Num_Msg_Packed_Fail,
	eStat_Xml_Total_Mem,
	eStat_Xml_Curr_Free_Mem,
	eStat_Xml_Max_Used_Mem

}E_STATS_OBJ_ID;

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

// Initialize Statistics Module
NAP_BOOL	NAP_STATS_INIT 
(
	NAP_INT16		*psiError
);

// Update Statistics of any particular item.
NAP_BOOL 	NAP_STATS_UPDATE 
(
	E_STATS_OBJ_ID 	eStatId,
	NAP_INT16		siUpdateIncrmnt, // Increase or Decr stat by this value.
	NAP_INT16		*psiError
);

// Get Statistics of any particular item
NAP_BOOL	NAP_STATS_GET 
(
	E_STATS_OBJ_ID 	eStatId,
	NAP_INT16		*psiStatValue,
	NAP_INT16		*psiError
);

// Display all statistics
NAP_BOOL	NAP_STATS_DISPLAY 
(
	NAP_INT16		*psiError
);


#endif // End of _NAP_STATISTICS_H_
