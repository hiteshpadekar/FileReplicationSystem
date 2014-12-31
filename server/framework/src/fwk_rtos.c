#include "fwk_rtos.h"
#include "fwk_trace.h"
#include "fwk_error.h"
#include "fwk_memory.h"
#include <time.h>

/*
 *	This function will get the current system time 
 *  with the current timezone adjustment. Eventually, it gives the
 *  current local time
 */
NAP_BOOL FWK_GetSystemTime
(
	S_NAP_FWK_TIMESTAMP *pstTime
)
{
	NAP_BOOL bRetVal = NAP_FAILURE;
        time_t raw_time;
        struct tm *lSystemTime;
 
        time(&raw_time);
        lSystemTime = localtime(&raw_time);


	pstTime->wYear			= lSystemTime->tm_year;
	pstTime->wMonth			= lSystemTime->tm_mon;
	pstTime->wDayOfWeek		= lSystemTime->tm_wday;
	pstTime->wDay			= lSystemTime->tm_mday;
	pstTime->wHour			= lSystemTime->tm_hour;
	pstTime->wMinute		= lSystemTime->tm_min;
	pstTime->wSecond		= lSystemTime->tm_sec;

	bRetVal = NAP_SUCCESS;

	return bRetVal;
}



/*
 *	This function will get the current system time 
 *  in UTC.
 */
NAP_BOOL FWK_GetSystemTimeInUTC
(
	S_NAP_FWK_TIMESTAMP *pstTime
)
{
	NAP_BOOL bRetVal = NAP_FAILURE;
        time_t raw_time;
        struct tm *lSystemTime;
 
        time(&raw_time);
        lSystemTime = localtime(&raw_time);


	pstTime->wYear			= lSystemTime->tm_year;
	pstTime->wMonth			= lSystemTime->tm_mon;
	pstTime->wDayOfWeek		= lSystemTime->tm_wday;
	pstTime->wDay			= lSystemTime->tm_mday;
	pstTime->wHour			= lSystemTime->tm_hour;
	pstTime->wMinute		= lSystemTime->tm_min;
	pstTime->wSecond		= lSystemTime->tm_sec;

	bRetVal = NAP_SUCCESS;


	return bRetVal;

}




/*
*******************************************************************************
* Function name : FWK_IPC_Initialize
* Description   : This function is called to initialize the IPC queue.

* Return type   :
* Arguments     :
* Side Effect   : None
* NOTE          : Queues for different tasks (NAP, NCM, UI, ..) should be
*				: initialized...
*******************************************************************************
*/
NAP_BOOL FWK_IPC_Initialize
(
	NAP_VOID	*queueId,
	NAP_INT16	*piError
)
{
	return NAP_SUCCESS;
}//End of FWK_IPC_Initialize()


/*
*******************************************************************************
* Function name : FWK_IPC_Put_Data
* Description   : This function is called to put the data into IPC queue.
* Return type   :
* Arguments     :
*	[IN]		: *queueId - One of the Values (Queue / Message ID / Signal/Thread-id)
*	[IN]		: *pvInData	- Pointer to Data passed by user in the format of
*				:		S_NAP_IMC_INFO.
*	[IN-OUT]	: *piError	- Error Code
* Side Effect   : None
* NOTE          : None
*******************************************************************************
*/

NAP_BOOL FWK_IPC_Put_Data
(
	NAP_UINT32	 ui_sig,
	NAP_VOID	*pvInData,
	NAP_INT16	*psError
)
{
	NAP_INT32 iResult = 0;
#if
	S_NAP_FWK_IPC_CMD_TYPE	*pstCmdType = NAP_NULL;
	S_NAP_IMC_INFO	*pstIMCData = NAP_NULL;

    //Birender : Nov20
	//ui_cmd_type  *pstUICmdType	= NAP_NULL;

	// @todo: Validate the input parameters.
	if (NAP_NULL == pvInData)
	{
		*piError = e_Err_IPC_InvalParams;
		return NAP_FAILURE;
	}

	FWK_TRACE_FXN(eTrace_Framework, "BEGIN :: FWK_IPC_Put_Data");

	// Store User Data (Reference) within IMC Data
	pstIMCData = (S_NAP_IMC_INFO *) pvInData;

	/* Shetti:	If the source module and destination module
				belongs to same task, then message must be
				put inside the main-task's internal QUEUE.
				Signal used shall be MAIN_SELF_CMD_Q_SIG.
				This message shall be processed inside the
				main task's next execution cycle.
	*/

	if ((pstIMCData->ucDest >= e_NAPModule) &&
		(pstIMCData->ucDest <= e_NAPModuleEnd))
	{
		/* This code handles messages destined to main task. */

		if ((pstIMCData->ucSource >= e_NAPModule) &&
			(pstIMCData->ucSource <= e_NAPModuleEnd))
		{
			/* Both source and destination modules belong to MAIN task. */
			/* Add this data to generic queue */
			NAP_FWK_AddToQueue_NAPManager(pstIMCData, piError);

			/* Passing NULL to the command type handler, which indicates that
			message is intra-module. */
			nap_cmd1 (NAP_NULL, MAIN_SELF_CMD_Q_SIG, piError);
			return (NAP_BOOL) NAP_SUCCESS;
		}
		else
		{
			/* Source module is from different task. */
			// Get Free Node from the Global Free Queue of the NAP task.
			pstCmdType = (S_NAP_FWK_IPC_CMD_TYPE*) q_get(&g_stNAPFreeQueue);
			if (NAP_NULL == pstCmdType)
			{
				*piError = e_Err_IPC_GlobalQueueEmpty;
				return (NAP_BOOL) NAP_FAILURE;
			}
			pstCmdType->pstIMCInfo = pstIMCData;

			switch(pstIMCData->ucSource)
			{
			case e_UIModule:
				nap_cmd1 (pstCmdType, MAIN_UI_CMD_Q_SIG, piError);
				break;
			case e_NetworkModule:
				nap_cmd1 (pstCmdType, NCM_CMD_Q_SIG, piError);
				break;
			case e_TimerModule:
				nap_cmd1 (pstCmdType, TIMER_CMD_Q_SIG, piError);
				break;
			case e_VocModule:

				break;
			default:
				FWK_ERROR_MINOR(e_Err_IPC_InvalParams, "FWK_IPC_Put_Data: invalid source module id.");
				return (NAP_BOOL) NAP_FAILURE;
			}
			return (NAP_BOOL) NAP_SUCCESS;
		}
	}
	else if (pstIMCData->ucDest == e_UIModule)
	{
		/* This block handles message destined to UI module. */

    	FWK_TRACE_DETAIL(eTrace_Framework, " FWK_IPC_Put_Data :: Signal To UI task.");
		if ((pstIMCData->ucSource >= e_NAPModule) &&
			(pstIMCData->ucSource <= e_NAPModuleEnd))
		{
			ui_cmd_type *pstUICmdType;

			/* If source module belong to MAIN task. */
			FWK_TRACE_DETAIL(eTrace_Framework, " FWK_IPC_Put_Data :: use UI_MAIN_CMD_Q_SIG signal.");
			pstUICmdType = (ui_cmd_type *) q_get( &ui_cmd_free_q);
			if (NAP_NULL == pstUICmdType )
			{
			    *piError = e_Err_IPC_GlobalQueueEmpty;
			    FWK_ERROR_MAJOR(e_Err_IPC_GlobalQueueEmpty, " FWK_IPC_Put_Data ::pstUICmdType is NULL ");
				return NAP_FAILURE;
			}

			// Update Command structure, as reqd by UI Task.
			pstUICmdType->ui_NAP_event.NAP_info.ucSource =
													pstIMCData->ucSource;
			pstUICmdType->ui_NAP_event.NAP_info.ucDest	=
													pstIMCData->ucDest;
			pstUICmdType->ui_NAP_event.NAP_info.ucProtocol =
													pstIMCData->ucProtocol;
			pstUICmdType->ui_NAP_event.NAP_info.ucEvent	=
													pstIMCData->ucEvent;
			pstUICmdType->ui_NAP_event.NAP_info.pvData =  pstIMCData;
            pstUICmdType->ui_NAP_event.hdr.cmd = UI_NAP_INCOMING_F;
			pstUICmdType->hdr.task_ptr		= NAP_NULL;
			pstUICmdType->hdr.done_q_ptr	= &ui_cmd_free_q;

		    FWK_TRACE_DETAIL(eTrace_Framework, " FWK_IPC_Put_Data ::Call ui_cmd ");
			ui_cmd(pstUICmdType);

			return (NAP_BOOL) NAP_SUCCESS;
		}
	}
	else if (pstIMCData->ucDest == e_VocModule)
	{
		/* This block handles message destined to UI module. */

	//	main_print(" FWK_IPC_Put_Data :: Signal To VOCODER (TOBE coded).");
		if ((pstIMCData->ucSource >= e_NAPModule) &&
			(pstIMCData->ucSource <= e_NAPModuleEnd))
		{
			/* If source module belong to MAIN task. */
			FWK_TRACE_DETAIL(eTrace_Framework, " FWK_IPC_Put_Data :: use VOC_MAIN_CMD_Q_SIG signal.");

			return (NAP_BOOL) NAP_SUCCESS;
		}
	}
	else if (pstIMCData->ucDest == e_NetworkModule)
	{
		/* This block handles message destined to NCM module. */
		FWK_TRACE_DETAIL(eTrace_Framework, " FWK_IPC_Put_Data :: Signal To NCM.");

		// Get Free Node from the Global Free Queue of the N/W task.
		pstCmdType = (S_NAP_FWK_IPC_CMD_TYPE*) q_get (&ui_cmd_free_q);
		if (NAP_NULL == pstCmdType)
		{
			*piError = e_Err_IPC_GlobalQueueEmpty;
			return (NAP_BOOL) NAP_FAILURE;
		}
		pstCmdType->pstIMCInfo = pstIMCData;
		ncm_cmd (pstCmdType, ui_sig, piError);
		return (NAP_BOOL) NAP_SUCCESS;
	}
	else
	{
		FWK_ERROR_MAJOR(e_Err_IPC_InvalParams, " FWK_IPC_Put_Data :: unexpected IMC dest id.");
		*piError = e_Err_IPC_InvalParams;
		return (NAP_BOOL) NAP_FAILURE;
	}

	FWK_TRACE_FXN(eTrace_Framework, "END :: FWK_IPC_Put_Data");
#endif
/* added by Jayalakshmi on  08-Jun-2005. Handles windows version of IPC*/
#ifdef NAP_ENV_WINDOWS
	if (psError == NAP_NULL)
		return NAP_FAILURE;
	iResult = PostThreadMessage(ui_sig,0,(WPARAM)pvInData,0);
	if (iResult <= 0)
	{
		NAP_INT32 iError = 0;
		FWK_Queue_Error(&iError);
		*psError = (NAP_INT16)iError;
		printf("\n FWK_IPC_Put_Data ::Failed! :ThreadId:%u :Error Code:%d ",ui_sig,iError);
		return NAP_FAILURE;
	}
	else
		*psError = iResult;
#endif

    return NAP_SUCCESS;
}// End of FWK_IPC_Put_Data()

/*
*******************************************************************************
* Function name : FWK_IPC_Get_Data
* Description   : This function is called to read the data from IPC queue.
* Return type   :
* Arguments     :
* Side Effect   : None
* NOTE          : None
*******************************************************************************
*/

NAP_BOOL FWK_IPC_Get_Data
(
	NAP_VOID	*queueId,
	NAP_VOID	**pvOutData,
	NAP_INT16	*psError)
{
	NAP_INT32 iResult = 0;
    S_NAP_FWK_IPC_CMD_TYPE *pstCmdType = NAP_NULL;
	S_NAP_IMC_INFO *pstImcInfo = NAP_NULL;
	q_type	*stCmdQueue = NAP_NULL;

	// @todo: Validate the input parameters.

	stCmdQueue = (q_type *) queueId ;
	//pstCmdType = (S_NAP_FWK_IPC_CMD_TYPE *) q_get (&stCmdQueue);
	pstCmdType = (S_NAP_FWK_IPC_CMD_TYPE *) q_get (stCmdQueue); // Birender Dec 02
	if (NAP_NULL == pstCmdType)
	{
		FWK_ERROR_CRITICAL(e_Err_IPC_CmdQueueEmpty, "FWK_IPC_Get_Data :: Failed");
		*piError = e_Err_IPC_CmdQueueEmpty;
		*pvOutData	= NAP_NULL ;
		return NAP_FAILURE;
	}

	q_put (&g_stNAPFreeQueue, &pstCmdType->link);

	*piError = e_Err_IPC_Success;
	*pvOutData =  (NAP_VOID *)(pstCmdType->pstIMCInfo);

    return NAP_SUCCESS;
}// End of FWK_IPC_Get_Data()




/*
*******************************************************************************
* Function name : OS_GetSystemTime
* Description   : Gives system time

* Return type   : NAP_BOOL
* Arguments     :
*	[OUT]		: pstTimeStamp:Pointyer to the system date and time structure
* Side Effect   :
* NOTE          :
*******************************************************************************
*/
NAP_BOOL OS_GetSystemTime
(
	S_NAP_FWK_TIMESTAMP *pstTimeStamp
)
{
	NAP_UINT32	uiMilliSeconds = 0;
	struct timeval  tv;
	
	gettimeofday(&tv, NULL);

	uiMilliSeconds= 
         (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ;

	if (NAP_NULL == pstTimeStamp)
	{
		return NAP_FAILURE;
	}
	pstTimeStamp->wSecond = uiMilliSeconds / 1000 ;
	pstTimeStamp->wMilliseconds = uiMilliSeconds % 1000 ;
	pstTimeStamp->wMinute = pstTimeStamp->wSecond /60 ;
	pstTimeStamp->wHour = pstTimeStamp->wSecond % 60 ;

    return NAP_SUCCESS;
}

/* Functions below are added by Jayalakshmi on 08-Jun-2005*/
/*****************************************************************************
* Function name : FWK_Lock_Queue
* Description   : Blocks on a queue till the queue is released by invoking
				  FWK_Unlock_Queue

* Return type   : NAP_BOOL
* Arguments     : [IN] pQueueId : Queue id of the queue to be locked.
* Side Effect   : None
* NOTE          : None
*****************************************************************************/
NAP_BOOL	FWK_Lock_Queue(NAP_VOID *pQueueId)
{
	//sem_t pSemId = (sem_t)pQueueId;
	//sem_wait(&pSemId);
	return NAP_SUCCESS;
}

/*****************************************************************************
* Function name : FWK_Unlock_Queue
* Description   : Releases the queue.
* Return type   : NAP_BOOL
* Arguments     : [IN] pQueueId : Queue id of the queue to be released.
* Side Effect   : None
* NOTE          : None
*****************************************************************************/
NAP_BOOL	FWK_Unlock_Queue(NAP_VOID *pQueueId)
{
	//sem_t pSemId = (sem_t)pQueueId;
	//sem_post(&pSemId);
	return NAP_SUCCESS;

}

/*****************************************************************************
* Function name : FWK_Queue_Error
* Description   : Reads the error while reading from and writing to the queue.
* Return type   : NAP_BOOL
* Arguments     : [OUT] piError : ErrorType.
* Side Effect   : None
* NOTE          : None
*****************************************************************************/
NAP_BOOL  FWK_Queue_Error(NAP_INT32 *piError)
{
	*piError = GetLastError();
	return NAP_SUCCESS;
}

#if 0
/*****************************************************************************
* Function name : FWK_GetNTPTimestamp
* Description   : This function will get the 64 bit NTP timestamp
* Return type   : NAP_UINT64
* Arguments     : None
* Side Effect   : None
* NOTE          : NTP Sync is out of scope, underlying OS should 
				  be synchronized with NTP Server
*****************************************************************************/
NAP_UINT64 FWK_GetNTPTimestamp(NAP_VOID)
{
	S_FWK_TimeB stTimeBuff;

	NAP_UINT64 ui64Timestamp = 0;
	NAP_UINT32 ui64BitInt[2] = {0,0};
	NAP_UINT32 uiTimeInMicroSec = 0;
	/* get the current local time */
	FWK_FTime( &stTimeBuff );
	/* Convert the local time in 64 Bit NTP Time format */
	ui64BitInt[1]	 = stTimeBuff.time + FWK_SECS_BETWEEN_1900_1970;
	uiTimeInMicroSec = stTimeBuff.millitm * 1000;

	ui64BitInt[0] = (uiTimeInMicroSec << 12) 
			+ (uiTimeInMicroSec << 8) 
			- ((uiTimeInMicroSec * 3650) >> 6);

	FWK_Memcpy(&ui64Timestamp,ui64BitInt,sizeof(NAP_UINT64));

	return ui64Timestamp;
	
}

/*
 *	This function will get the 64 bit current System timestamp in UTC
 */
NAP_UINT64 FWK_Get64BitSysTimestamp(NAP_VOID)
{
	NAP_UINT64 ui64Timestamp = 0;

#ifdef NAP_ENV_WINDOWS

	NAP_UINT32			ui64BitInt[2]	= {0,0};
	FILETIME			stFiletime		= {0};
	S_NAP_FWK_TIMESTAMP stTimestamp		= {0};

	GetSystemTime((SYSTEMTIME*)&stTimestamp);

	if(SystemTimeToFileTime((SYSTEMTIME*)&stTimestamp,&stFiletime))
	{
		ui64BitInt[0] = stFiletime.dwLowDateTime;
		ui64BitInt[1] = stFiletime.dwHighDateTime;

		FWK_Memcpy(&ui64Timestamp,ui64BitInt,sizeof(NAP_UINT64));
	}


#endif // NAP_ENV_WINDOWS
	return ui64Timestamp;
}



/*
 *	This function will convert the 
 *  64 bit timestamp into S_NAP_FWK_TIMESTAMP
 */
NAP_BOOL FWK_ConvertFrom64BitTimestamp\
(\
	const NAP_UINT64 u64Timestamp,\
	S_NAP_FWK_TIMESTAMP *pstTime\
)
{
	NAP_BOOL bRetVal = NAP_FAILURE;

#ifdef NAP_ENV_WINDOWS
	if(NAP_NULL != pstTime)
	{
		NAP_UINT32 *pui32BitInt = (NAP_UINT32 *)&u64Timestamp;

		FILETIME stFiletime = {pui32BitInt[0],pui32BitInt[1]};

		if(FileTimeToSystemTime(&stFiletime,(SYSTEMTIME*)pstTime))
		{
			bRetVal = NAP_SUCCESS;
		}
	}
	
#endif // NAP_ENV_WINDOWS

	return bRetVal;
}

/*
 *	This function will convert the 
 *  S_NAP_FWK_TIMESTAMP into 64 bit timestamp
 */
NAP_UINT64 FWK_ConvertInto64BitTimestamp(S_NAP_FWK_TIMESTAMP *pstTime)
{
	NAP_UINT64 ui64Timestamp = 0;

#ifdef NAP_ENV_WINDOWS
	if(NAP_NULL != pstTime)
	{		
		NAP_UINT32 ui64BitInt[2] = {0,0};

		FILETIME stFiletime = {0};

		if(SystemTimeToFileTime((SYSTEMTIME*)pstTime,&stFiletime))
		{
			ui64BitInt[0] = stFiletime.dwLowDateTime;
			ui64BitInt[1] = stFiletime.dwHighDateTime;

			FWK_Memcpy(&ui64Timestamp,ui64BitInt,sizeof(NAP_UINT64));
		}
	}

#endif // NAP_ENV_WINDOWS
	return ui64Timestamp;
}


/*
 *	This will compare two 
 *  64 bit timestamps 
 *	-1 - First file time is earlier than second file time.
 *	 0 - First file time is equal to second file time.
 *	 1 - First file time is later than second file time.
 *
 */
NAP_INT32 FWK_Compare64BitTimestamp\
(\
	const NAP_UINT64 u64Timestamp1,\
	const NAP_UINT64 u64Timestamp2\
)
{
	NAP_INT32 iRes = -1;

#ifdef NAP_ENV_WINDOWS
	
	NAP_UINT32 *pui32 = (NAP_UINT32 *)&u64Timestamp1;

	FILETIME stFiletime1 = {pui32[0],pui32[1]};
	FILETIME stFiletime2 = {0};

	pui32 = (NAP_UINT32 *)&u64Timestamp2;

	stFiletime2.dwLowDateTime	= pui32[0];
	stFiletime2.dwHighDateTime	= pui32[1];

	iRes = (NAP_INT8)CompareFileTime(&stFiletime1,&stFiletime2);

#endif // NAP_ENV_WINDOWS

	return iRes;
}

#endif

