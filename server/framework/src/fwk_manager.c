
#include "fwk_structures.h"
#include "fwk_memory.h"
#include "fwk_trace.h"
#include "fwk_error.h"
#include "fwk_rtos.h"
#include "fwk_string.h"

#define MAX_FWK_ID	6

#ifdef NAP_ENV_WINDOWS
HANDLE	gFwkMutex;
#endif

S_FWK_CB	gstFwkCb[MAX_FWK_ID];

NAP_BOOL	FWK_ProcessQueue (
	S_FWK_MODULE_LIST	*pstModuleList,
	S_FWK_IMC_INFO		*pstIMCData,
	NAP_INT16			*piError
);

NAP_BOOL	FWK_MNG_InitFramework
(
	NAP_INT16 *psError
)
{
	NAP_UINT16 i;

	for(i=0;i<MAX_FWK_ID;i++)
	{
		gstFwkCb[i].bInUse = 0;
		gstFwkCb[i].pstModuleList = NAP_NULL;
		gstFwkCb[i].stFwkQueue.pstQHead = NAP_NULL;
		gstFwkCb[i].stFwkQueue.pstQTail = NAP_NULL;		
	}

#ifdef NAP_ENV_WINDOWS
	gFwkMutex = CreateMutex(
								NULL, // No Secrity attributes
								FALSE,//Initially Not Owned
								"MutexToProtectFwk" // Name of the Mutex
							 );

	if(gFwkMutex==NULL)
	{
		FWK_TRACE_DETAIL(eTrace_Framework,"NAP_MNG_InitFramework :: Not able to create Fwk Mutex");
	}
#endif
/* Enable memory corruption and leak detection in the framework under debug mode */
#ifdef NAP_ENV_WINDOWS
#ifdef _DEBUG
#ifdef FWK_DETECT_MEMORY_ERRORS
	{
		NAP_UINT32 uiDbgFlag;

		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
		_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
		_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);

		/* Get the current bits */
		uiDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

        uiDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
        uiDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
        uiDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF; 

		/* Set the new bits */
		_CrtSetDbgFlag(uiDbgFlag);
	}
#endif /* FWK_DETECT_MEMORY_ERRORS */
#endif /* _DEBUG */
#endif /* NAP_ENV_WINDOWS */

    /* Initialize the random number generator seed here */
    {
        FWK_Srand(1);
       /* Seed the random-number generator with current time so that
        * the numbers will be different every time we run.
        */
        FWK_Srand((NAP_UINT32)FWK_TimeElapsed(NAP_NULL));
    }


	return NAP_SUCCESS;
}// End of FWK_MNG_InitFramework

NAP_BOOL	FWK_MNG_DeInitFramework
(
	NAP_INT16 *psError
)
{
#ifdef NAP_ENV_WINDOWS
	CloseHandle(gFwkMutex);
#endif
	return NAP_SUCCESS;
}// End of FWK_MNG_DeInitFramework

/* This function creates an Framework instance and returns the unique framework ID. 
All modules belonging to this framework has to be registered using the corresponding 
framework Id. This function helps in using multiple instance of framework in single 
application. */
NAP_BOOL	FWK_MNG_CreateFramework
(
	FWK_ID *pusFrameworkId,
	NAP_INT16 *psError
)
{
	NAP_UINT16 i;

#ifdef NAP_ENV_WINDOWS
   	WaitForSingleObject(gFwkMutex,INFINITE);    
#endif

	for(i=1;i<MAX_FWK_ID;i++)
	{
		if(gstFwkCb[i].bInUse == 0)
			break;
	}

	if(i == MAX_FWK_ID)
	{
#ifdef NAP_ENV_WINDOWS
   		ReleaseMutex(gFwkMutex);    
#endif	
		return NAP_FAILURE;
	}

	//gstFwkCb[i].ulFwk = Get Task ID;
#if 0
	{
		DWORD threadId;
		threadId = GetCurrentThreadId();
		gstFwkCb[i].ulFwkId = threadId;
		*pusFrameworkId = threadId;
	}
#endif

	{
		gstFwkCb[i].ulFwkId = i;
		*pusFrameworkId = i;
	}

	gstFwkCb[i].bInUse = 1;

	/* Initialize FWK related Queue and Module List*/
	gstFwkCb[i].stFwkQueue.pstQHead = NAP_NULL;
	gstFwkCb[i].stFwkQueue.pstQTail = NAP_NULL;

	gstFwkCb[i].pstModuleList = NAP_NULL;

	#ifdef NAP_ENV_WINDOWS
   	ReleaseMutex(gFwkMutex);    
#endif

	return NAP_SUCCESS;
}// End of FWK_MNG_CreateFramework

NAP_BOOL	FWK_MNG_DeleteFramework
(
	FWK_ID usFrameworkId,
	NAP_INT16 *psError
)
{
	NAP_UINT16			usFwkIndex;
	S_FWK_MODULE_LIST	*pstModuleList;

#ifdef NAP_ENV_WINDOWS
   	WaitForSingleObject(gFwkMutex,INFINITE);    
#endif
	/* Check usFrameworkId and get corresponding Framework Index*/
	for(usFwkIndex=1;usFwkIndex<MAX_FWK_ID;usFwkIndex++)
	{
		if(gstFwkCb[usFwkIndex].bInUse == 1)
		{
			if(gstFwkCb[usFwkIndex].ulFwkId == usFrameworkId)
				break;
		}
	}

	if(usFwkIndex == MAX_FWK_ID)
	{
#ifdef NAP_ENV_WINDOWS
   		ReleaseMutex(gFwkMutex);    
#endif
		return NAP_FAILURE;
	}

	gstFwkCb[usFwkIndex].bInUse = 0;

	/* Free All Queue Element if existing */
	while(gstFwkCb[usFwkIndex].stFwkQueue.pstQHead)
	{
		S_FWK_IMC_INFO *pstIMCData;	
		S_FWK_QUEUE_ELEM *pstQueue;
		pstIMCData = (S_FWK_IMC_INFO*)gstFwkCb[usFwkIndex].stFwkQueue.pstQHead->pvData;
		
		pstQueue = gstFwkCb[usFwkIndex].stFwkQueue.pstQHead;

		gstFwkCb[usFwkIndex].stFwkQueue.pstQHead = gstFwkCb[usFwkIndex].stFwkQueue.pstQHead->pstQNext;

		/* Adjust Tail element*/
		if (gstFwkCb[usFwkIndex].stFwkQueue.pstQHead == NAP_NULL)
			gstFwkCb[usFwkIndex].stFwkQueue.pstQTail = gstFwkCb[usFwkIndex].stFwkQueue.pstQHead;
		
		/* Invoke destination module handlers to handle the data. */
		//bRetVal = FWK_ProcessQueue(gstFwkCb[usFwkIndex].pstModuleList,pstIMCData, psError);

		/* Free the IMC structure. */
		FWK_Free(&pstIMCData);
		FWK_Free(&pstQueue);
	}

	pstModuleList = gstFwkCb[usFwkIndex].pstModuleList;

	/* Delete all Module list entry */
	while(pstModuleList)	
	{
		S_FWK_MODULE_LIST *pstTempList = pstModuleList;

		pstModuleList = pstModuleList->next;

		FWK_Free(&pstTempList);
		
	}
#ifdef NAP_ENV_WINDOWS
   	ReleaseMutex(gFwkMutex);    
#endif
	return NAP_SUCCESS;
}// End of FWK_MNG_DeleteFramework
		
/*This function is used for registering the module to the frame work. The module 
entry function pfxnModule is registered to the framework. Hereafter moduleID can 
be used as a reference to the module. */
NAP_BOOL	FWK_MNG_Registration
(
	FWK_ID				usFrameworkId,
	NAP_UINT8			srcModuleID,
	CB_MODULE_HANDLER	CbModuleHandler,
	NAP_INT16			*psError
)
{
	S_FWK_MODULE_LIST	*pstModuleList;
	NAP_UINT16			usFwkIndex;

	/* Check usFrameworkId and get corresponding Framework Index*/
	for(usFwkIndex=1;usFwkIndex<MAX_FWK_ID;usFwkIndex++)
	{
		if(gstFwkCb[usFwkIndex].bInUse == 1)
		{
			if(gstFwkCb[usFwkIndex].ulFwkId == usFrameworkId)
				break;
		}
	}

	if(usFwkIndex == MAX_FWK_ID)
		return NAP_FAILURE;


	/*create module list to store the module info */
	pstModuleList = (S_FWK_MODULE_LIST*) FWK_Malloc(sizeof(S_FWK_MODULE_LIST));
	if(pstModuleList == NAP_NULL)
	{
		*psError = e_Err_Fwk_MallocFailed;
		return NAP_FAILURE;
	}

	/* add element at starting*/
	pstModuleList->stModuleInfo.srcId = srcModuleID;
	pstModuleList->stModuleInfo.cbModuleHandler = CbModuleHandler;
	pstModuleList->next = gstFwkCb[usFwkIndex].pstModuleList;
	gstFwkCb[usFwkIndex].pstModuleList = pstModuleList;

	return NAP_SUCCESS;
}// End of FWK_MNG_Registration

/*This function is used to de-register the module from the frame work. */
NAP_BOOL	FWK_MNG_Deregistration
(
	FWK_ID		usFrameworkId,
	NAP_UINT8	srcModuleID,
	NAP_INT16	*psError
)
{
	S_FWK_MODULE_LIST	*pstModuleList;	
	NAP_UINT16			usFwkIndex;

	/* Check usFrameworkId and get corresponding Framework Index*/
	for(usFwkIndex=1;usFwkIndex<MAX_FWK_ID;usFwkIndex++)
	{
		if(gstFwkCb[usFwkIndex].bInUse == 1)
		{
			if(gstFwkCb[usFwkIndex].ulFwkId == usFrameworkId)
				break;
		}
	}
	if(usFwkIndex == MAX_FWK_ID)
		return NAP_FAILURE;


	if(NAP_NULL == gstFwkCb[usFwkIndex].pstModuleList)
	{
		/* No module is registered with this framework ID*/
		return NAP_FAILURE;
	}
	
	/* Check first element */
	if(gstFwkCb[usFwkIndex].pstModuleList->stModuleInfo.srcId == srcModuleID)
	{
		S_FWK_MODULE_LIST *pstTempList = gstFwkCb[usFwkIndex].pstModuleList;
	
		gstFwkCb[usFwkIndex].pstModuleList = gstFwkCb[usFwkIndex].pstModuleList->next;

		FWK_Free(&pstTempList);
		return NAP_SUCCESS;
	}	
	
	pstModuleList = gstFwkCb[usFwkIndex].pstModuleList;

	/* travers the module list and find the module to deregister*/
	while(pstModuleList->next && 
	(pstModuleList->next->stModuleInfo.srcId != srcModuleID))
	{
		pstModuleList = pstModuleList->next;
	}

	if(pstModuleList->next)
	{
		S_FWK_MODULE_LIST *pstTempList = pstModuleList->next;
		pstModuleList->next = pstModuleList->next->next;

		FWK_Free(&pstTempList);
		return NAP_SUCCESS;
	}

	return NAP_FAILURE;
}// End of FWK_MNG_Deregistration

/*This function invokes the corresponding module based on the information in 
S_FWK_IMC_INFO structure. */
NAP_BOOL	FWK_MNG_AddToQueue
(
	FWK_ID			usFrameworkId,
	S_FWK_IMC_INFO	*pstImcInfo,
	NAP_INT16		*psError	
)
{
	S_FWK_QUEUE_ELEM *pstQueue;
	NAP_UINT16	usFwkIndex;

	for(usFwkIndex=1;usFwkIndex<MAX_FWK_ID;usFwkIndex++)
	{
		if(gstFwkCb[usFwkIndex].bInUse == 1)
		{
			if(gstFwkCb[usFwkIndex].ulFwkId == usFrameworkId)
				break;
		}
	}

	if(usFwkIndex == MAX_FWK_ID)
		return NAP_FAILURE;
	
	/* create queue element to store the data came from IPC queue. */
	pstQueue = (S_FWK_QUEUE_ELEM*)FWK_Malloc(sizeof(S_FWK_QUEUE_ELEM));
	
	pstQueue->pvData = pstImcInfo;
	pstQueue->pstQNext = NAP_NULL;

	if (gstFwkCb[usFwkIndex].stFwkQueue.pstQTail == NAP_NULL)
	{
		gstFwkCb[usFwkIndex].stFwkQueue.pstQTail = pstQueue;
		gstFwkCb[usFwkIndex].stFwkQueue.pstQHead = gstFwkCb[usFwkIndex].stFwkQueue.pstQTail;
	}
	else
	{
		gstFwkCb[usFwkIndex].stFwkQueue.pstQTail->pstQNext = pstQueue;
		gstFwkCb[usFwkIndex].stFwkQueue.pstQTail = pstQueue;
	}

	return NAP_SUCCESS;
}// End of FWK_MNG_AddToQueue

/* process the generic queue*/
NAP_BOOL	FWK_MNG_ProcessQueue
(
	FWK_ID		usFrameworkId,
	NAP_INT16	*psError	
)
{
	//PoC_FWK_QUEUE *pstQueue;		
	S_FWK_IMC_INFO *pstIMCData;	
	S_FWK_QUEUE_ELEM *pstQueue;
	NAP_UINT16	usFwkIndex;
	NAP_BOOL bRetVal = NAP_SUCCESS;

	for(usFwkIndex=1;usFwkIndex<MAX_FWK_ID;usFwkIndex++)
	{
		if(gstFwkCb[usFwkIndex].bInUse == 1)
		{
			if(gstFwkCb[usFwkIndex].ulFwkId == usFrameworkId)
				break;
		}
	}

	if(usFwkIndex == MAX_FWK_ID)
		return NAP_FAILURE;

	/*traverse the queue list and call the corresponding function
	 *basing on the state and event
	 */
	while(gstFwkCb[usFwkIndex].stFwkQueue.pstQHead)
	{
		pstIMCData = (S_FWK_IMC_INFO*)gstFwkCb[usFwkIndex].stFwkQueue.pstQHead->pvData;
		
		pstQueue = gstFwkCb[usFwkIndex].stFwkQueue.pstQHead;

		gstFwkCb[usFwkIndex].stFwkQueue.pstQHead = gstFwkCb[usFwkIndex].stFwkQueue.pstQHead->pstQNext;

		/* Adjust Tail element*/
		if (gstFwkCb[usFwkIndex].stFwkQueue.pstQHead == NAP_NULL)
			gstFwkCb[usFwkIndex].stFwkQueue.pstQTail = gstFwkCb[usFwkIndex].stFwkQueue.pstQHead;
		
		/* Invoke destination module handlers to handle the data. */
		bRetVal = FWK_ProcessQueue(gstFwkCb[usFwkIndex].pstModuleList,pstIMCData, psError);

		/* Free the IMC structure. */
		FWK_Free(&pstIMCData);
		FWK_Free(&pstQueue);
	}

	return bRetVal;
}// End of FWK_MNG_ProcessQueue

NAP_BOOL	FWK_ProcessQueue (
	S_FWK_MODULE_LIST	*pstModuleList,
	S_FWK_IMC_INFO		*pstIMCData,
	NAP_INT16			*piError
)
{
	/* travers the module list and find the module handler for the destination module */
	while(pstModuleList && 
	(pstModuleList->stModuleInfo.srcId != pstIMCData->destId))
	{
		pstModuleList = pstModuleList->next;
	}
	if(pstModuleList == NAP_NULL)
	{
		FWK_ERROR_MINOR(0, "FWK_ProcessQueue::pstModuleList is NULL");
		return NAP_FAILURE;		
	}
	/* call the corresponding Entry function of the module*/
	if(NAP_SUCCESS == (pstModuleList->stModuleInfo.cbModuleHandler)
		(pstIMCData->srcId,
		pstIMCData->actionEventId,
		pstIMCData->pvData,
		piError)
	  )
	{
		return NAP_SUCCESS;
	}

	FWK_ERROR_MINOR(0, "FWK_ProcessQueue::Failed");
	return NAP_FAILURE;
}// End of FWK_ProcessQueue

NAP_BOOL FWK_FreeNwParam
(
	S_FWK_NW_PARAMS **ppstNw
)
{
	S_FWK_NW_PARAMS *pstnw;

	if(NAP_NULL == ppstNw)
		return NAP_FAILURE;

	if(NAP_NULL == *ppstNw)
		return NAP_FAILURE;

	pstnw = *ppstNw;

	FWK_Free(&pstnw->pucInBuffer);
	FWK_Free(&pstnw->stTptInfo.pucHost);
	FWK_Free(&pstnw->stTptInfo.pucIpaddr);

	/* SIM_PF_UT */
	FWK_Free(&pstnw->stTptInfo.pucLocalIpaddr);

	FWK_Free(ppstNw);
	
	return NAP_SUCCESS;

}// End of FWK_FreeNwParam