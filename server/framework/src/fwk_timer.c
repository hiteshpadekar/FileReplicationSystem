


#include "fwk_timer.h"
#include "fwk_trace.h"
#include "fwk_rtos.h"
#include "fwk_structures.h"

/*
*******************************************************************************
*                               GLOBAL VARIABLES
*******************************************************************************
*/

static int g_TimerID[MAX_TIMERS];//Timer ID pool

S_FWK_TIMER_LIST *g_pstActiveList;//Timer Active List pointer

S_FWK_TIMER_LIST *g_pstInActiveList;//Timer InActive List pointer

#ifdef NAP_ENV_WINDOWS
HANDLE	gTimerMutex;
#endif

/*
*******************************************************************************
*                          LOCAL FUNCTION PROTOTYPES
*******************************************************************************
*/



/******************************************************************************
* Function name 	: FWK_InitTimer
* Description   	: This function initialises all global variables and
*				: head and tail ponters of the timer nodes
* Return type   	: NAP_VOID
* Arguments     	: 
* Side Effect   	: None 
* NOTE          		: Memory for Global Active & In-Active List is allocated.
*				: Memory gets released while releasing last node.
******************************************************************************/
NAP_BOOL FWK_InitTimer 
(
	void
)
{
	NAP_BOOL retStatus;
    NAP_UINT16 usLoop = 0;

	FWK_TRACE_FXN(eTrace_Framework, "BEGIN :: FWK_InitTimer");

	/* Allocating memory to g_pstActiveList global */
	g_pstActiveList = (S_FWK_TIMER_LIST*)FWK_Malloc(
												sizeof(S_FWK_TIMER_LIST));
	g_pstActiveList->pstTail = NAP_NULL;
	g_pstActiveList->pstHead = NAP_NULL;

	/*Allocating memory to g_pstInActiveList global pointers */
	g_pstInActiveList = (S_FWK_TIMER_LIST*)FWK_Malloc(
												sizeof(S_FWK_TIMER_LIST));
	g_pstInActiveList->pstTail = NAP_NULL;
	g_pstInActiveList->pstHead = NAP_NULL;

	// Making all the timer ID for use except first one.
	for(usLoop = 1; usLoop < MAX_TIMERS; usLoop++)
	{
		g_TimerID[usLoop] = 0;
	}
	g_TimerID[0] = 1; //This timer ID(i.e.,index 0) is never used


	/* Create Mutext for Timer Structure protection*/
#ifdef NAP_ENV_WINDOWS
	gTimerMutex = CreateMutex(
								NULL, // No Secrity attributes
								FALSE,//Initially Not Owned
								"MutexToProtectTimer" // Name of the Mutex
							 );

	if(gTimerMutex==NULL)
	{
		FWK_TRACE_DETAIL(eTrace_Framework,"FWK_InitTimer :: Not able to create Timer Mutex");
	}
#endif

	retStatus = FWK_Timer_RegTask();

	FWK_TRACE_FXN(eTrace_Framework, "END :: FWK_InitTimer");

	return retStatus;
}

/******************************************************************************
* Function name 	: FWK_CreateTimer
* Description   	: This function creates new timer node and appends it into 
*				: the InActiveList. To start using this timer, user has
*				: to call start timer with the timer-id retuned by this API.
* Return type   	: NAP_BOOL
* Arguments     	: 
*	[OUT]		: pustimerId:Timer id created
*	[IN]			: eModule:Module name which is creating the timer.
*	[IN]			: queueId:IPC channel to place the data when timer expires
*	[IN]			: unDuration:Duration for timer(ms)
*	[IN]			: unResetFlag:Flag for whether to repeat the timer or not
*	[IN]			: cbTimerHandler:Callback function to be called when 
*				: its timer is expires
*	[OUT]		: vpData:Timer Data
*	[OUT]		: piErrNo:Error code
* Side Effect   	: None 
* NOTE          		: None
******************************************************************************/
NAP_BOOL	FWK_CreateTimer 
(
	NAP_UINT16			*pustimerId,
	NAP_UINT8			usModuleId,
	NAP_UINT32			uiQueueId,
	NAP_UINT32			uiDuration,
	NAP_UINT16			usResetFlag,
	CB_TIMERHANDLER		cbTimerHandler,
	NAP_VOID			*pvData,
	NAP_INT16			*piErrNo
)
{

	NAP_UINT16 usLoopCount	= 0 ;
	NAP_UINT16 usTimerId	= 0 ;
	
	*pustimerId = 0;

	FWK_TRACE_FXN(eTrace_Framework, "BEGIN :: FWK_CreateTimer");

	// Store locally current set of Interrupts . Disable them.
#ifdef NAP_ENV_WINDOWS
   	WaitForSingleObject(gTimerMutex,INFINITE);    
#endif

#ifdef NAP_ENV_REX
   	rex_task_lock();    
#endif


	for( usLoopCount = 1 ; usLoopCount < MAX_TIMERS ; usLoopCount++ )
	{
		if( 0 == g_TimerID[usLoopCount] )
		{
			S_FWK_TIMER      *pstTimer     = NAP_NULL ;
			S_FWK_TIMER_NODE *pstTimerNode = NAP_NULL ;
			
			// Allocating memory of size S_FWK_TIMER to timer node
			pstTimer = (S_FWK_TIMER *) FWK_Malloc (
													sizeof(S_FWK_TIMER));
			//Checking for Memory Allocation
			if(NAP_NULL == pstTimer)
			{
				// Restore & Enable the Interrupts
#ifdef NAP_ENV_WINDOWS
   				ReleaseMutex(gTimerMutex);    
#endif				
#ifdef NAP_ENV_REX
				rex_task_free();
#endif
				return NAP_FAILURE;
			}

			// Setting the timerId ON
			g_TimerID[usLoopCount] = 1;
			usTimerId	=   usLoopCount;
			*pustimerId	=	usLoopCount;

			// Setting the values of timer node members
			pstTimer->usModuleId	= usModuleId;
			pstTimer->uiDuration	= uiDuration;
			pstTimer->usTimerId		= usTimerId;
			pstTimer->iDelta		= uiDuration;
			pstTimer->pvData		= pvData;
			pstTimer->uiQueueId		= uiQueueId;			
			pstTimer->cbTimerHandler= cbTimerHandler;
			pstTimer->usResetFlag	= usResetFlag;
			
			// Prepare Timer Node							
			pstTimerNode = (S_FWK_TIMER_NODE*)FWK_Malloc ( 
											    sizeof(S_FWK_TIMER_NODE) );

			//Checking for Memory Allocation
			if(NAP_NULL == pstTimerNode)
			{
				*piErrNo = e_Err_Fwk_MallocFailed;
				//Freeing memory allocated in previous allocation
				//Otherwise it will cause menory leak
				FWK_Free(&pstTimer);

				// Restore & Enable the Interrupts
#ifdef NAP_ENV_WINDOWS
   		ReleaseMutex(gTimerMutex);    
#endif		
#ifdef NAP_ENV_REX
                rex_task_free();
#endif
				return NAP_FAILURE;
			}
			pstTimerNode->pstTimerNode	= pstTimer;
			pstTimerNode->pstNext		= NAP_NULL;
			
			pstTimer = NAP_NULL;
			
			// Adding the new timer node in the InActiveList 
			if(g_pstInActiveList->pstTail)
			{
				g_pstInActiveList->pstTail->pstNext = pstTimerNode;
				g_pstInActiveList->pstTail	= pstTimerNode;
			}
			else
			{
				g_pstInActiveList->pstTail = pstTimerNode;
				g_pstInActiveList->pstHead = g_pstInActiveList->pstTail;
			}

			//Breaking the loop after assigningn free timer ID from Timer pool 
			//and further search for free Timer ID is unnecessary
			break;	
		}
	}

	if(usLoopCount >=  MAX_TIMERS)
	{
		*piErrNo = e_Err_Timer_NotCreated;

		// @todo: Check if mem is reqd to be released when failure occurs ?

		// Restore & Enable the Interrupts
#ifdef NAP_ENV_WINDOWS
	ReleaseMutex(gTimerMutex);    
#endif		
#ifdef NAP_ENV_REX
        rex_task_free();
#endif

		return NAP_FAILURE;
	}
	else
	{
		// Restore & Enable the Interrupts
#ifdef NAP_ENV_WINDOWS
   	      ReleaseMutex(gTimerMutex);    
#endif		
#ifdef NAP_ENV_REX
		rex_task_free();
#endif

		FWK_TRACE_FXN(eTrace_Framework, "END :: FWK_CreateTimer");
		return NAP_SUCCESS;
	}

    return NAP_FAILURE;
}//End of NAP_FWK_OS_CreateTimer()

/******************************************************************************
* Function name : FWK_InternalStartTimer
* Description   : This function moves the timer node from the InActiveList
*				: to the ActveList and also deletes it form InActiveList
* Return type   : NAP_BOOL
* Arguments     : 
*	[IN]		: usTimerId:Timer id to start timer for that Id
*	[OUT]		: piErrNo  :Error Code
* Side Effect   : None 
* NOTE          : Intererrupt enable and disable is not done in this function.
				  This fxn is called by NAP_FWK_OS_StartTimer and 
				  NAP_FWK_OS_ProcessTimer which are doing  the interrupt 
				  enable/disable action.

******************************************************************************/
NAP_BOOL	FWK_InternalStartTimer
(
	NAP_UINT16			usTimerId,
	NAP_INT16			*piErrNo
)
{
	S_FWK_TIMER_NODE *pstPrevNode		= NAP_NULL ;
	S_FWK_TIMER_NODE *pstCurNode		= NAP_NULL ;
	S_FWK_TIMER_NODE *pstRequestedNode	= NAP_NULL;

	FWK_TRACE_FXN(eTrace_Framework, "BEGIN :: FWK_InternalStartTimer");

#if 0
	{
		char buf[128];
		sprintf(buf, " FWK_InternalStartTimer::timer id = %d \n",usTimerId);
		FWK_TRACE_DETAIL(eTrace_Framework, buf);
	}
#endif	
	
	// Assigning list head pointer to the temporary variable
	pstCurNode	= g_pstInActiveList->pstHead;
	pstPrevNode = pstCurNode;
	
	// Get the Timer node from the g_pstInActiveList
	while(pstCurNode != NAP_NULL)
	{		
		// Checking whether passed Id matches with the timerId of node
		if(pstCurNode->pstTimerNode->usTimerId == usTimerId)
		{
			// Assigning the address of the timer node whose timerId matched
			// with the passed Id
			pstRequestedNode = pstCurNode;

			//Delete that node from the InActiveList
			if(pstPrevNode == pstCurNode)
			{
				// It is first node
				g_pstInActiveList->pstHead = 
								g_pstInActiveList->pstHead->pstNext;
				if( NAP_NULL ==	g_pstInActiveList->pstHead )
				{
					// Update head
					g_pstInActiveList->pstTail = NAP_NULL;
				}
			}
			else
			{
				// Scan for next node
				pstPrevNode->pstNext = pstCurNode->pstNext;
				if(NAP_NULL == pstPrevNode->pstNext)
				{
					// It is last Node so update Tail
					g_pstInActiveList->pstTail = pstPrevNode;
				}
			}
			break;
		}
		pstPrevNode = pstCurNode;

		// Going to the next node of the InActiveList for traversing
		pstCurNode = pstCurNode->pstNext;		
	}

	if(NAP_NULL == pstRequestedNode)
	{
		// The Timer ID does not exist;
		return NAP_FAILURE;
	}

	// Moving the Timer node from InActiveList to ActiveList
	pstCurNode	= g_pstActiveList->pstHead;
	pstPrevNode = pstCurNode;
	
	if(NAP_NULL == pstCurNode)
	{
		// Currently no Timer exist in Active list
		pstRequestedNode->pstNext	= NAP_NULL;
		g_pstActiveList->pstHead	= pstRequestedNode;
		g_pstActiveList->pstTail	= pstRequestedNode;
	}
	else
	{
		// Atleast 1 timer exist in active list;

		NAP_UINT32 uisum			= 0;
		NAP_UINT32 uiprevSum		= 0;
		S_FWK_TIMER *pstTimer	= NAP_NULL;

		pstTimer = pstRequestedNode->pstTimerNode;
		
		// Traverse through the ActiveList 
		while(1)
		{
			uiprevSum = uisum;
			uisum = uisum + pstCurNode->pstTimerNode->iDelta;

			if(pstTimer->uiDuration > uisum)
			{
				pstPrevNode = pstCurNode;
				pstCurNode = pstCurNode->pstNext;
				
				if(NAP_NULL == pstCurNode)
				{
					// Update Delta
					pstTimer->iDelta = pstTimer->uiDuration - uisum;

					// pstPrevNode node was the last node. So insert 
					// pstRequestedNode at the end
					pstRequestedNode->pstNext = NAP_NULL;
					g_pstActiveList->pstTail->pstNext = pstRequestedNode;
					g_pstActiveList->pstTail = pstRequestedNode;
					break;
				}
				// Go to next node
				continue;
			}
			else
			{	
				// Calculate Delta for ???
				pstTimer->iDelta = pstTimer->uiDuration - uiprevSum;
				pstCurNode->pstTimerNode->iDelta -= pstTimer->iDelta;

				// Insert pstRequestedNode (new timer node)
				if(pstPrevNode == pstCurNode)
				{	// only one node exist 	
					g_pstActiveList->pstHead	= pstRequestedNode;
					pstRequestedNode->pstNext	= pstCurNode;
				}
				else//when more than one node exists
				{
					pstPrevNode->pstNext		= pstRequestedNode;
					pstRequestedNode->pstNext	= pstCurNode;
				}

				break;
			}
		}// End of while(1)
	}

	FWK_TRACE_FXN(eTrace_Framework, "END :: FWK_InternalStartTimer");
	return NAP_SUCCESS;
	
}//End of NAP_FWK_OS_StartTimer() 

/******************************************************************************
* Function name : FWK_StartTimer
* Description   : This function moves the timer node from the InActiveList
*				: to the ActveList and also deletes it form InActiveList
* Return type   : NAP_BOOL
* Arguments     : 
*	[IN]		: usTimerId:Timer id to start timer for that Id
*	[OUT]		: piErrNo  :Error Code
* Side Effect   : None 
* NOTE          : None
******************************************************************************/
NAP_BOOL	FWK_StartTimer
(
	NAP_UINT16			usTimerId,
	NAP_INT16			*piErrNo
)
{
	NAP_BOOL retVal;

	FWK_TRACE_FXN(eTrace_Framework, "BEGIN :: FWK_StartTimer");
#ifdef NAP_ENV_WINDOWS
   	WaitForSingleObject(gTimerMutex,INFINITE);    
#endif
#ifdef NAP_ENV_REX
    rex_task_lock();
#endif
    
    retVal = FWK_InternalStartTimer(usTimerId,piErrNo);

#ifdef NAP_ENV_WINDOWS
   	ReleaseMutex(gTimerMutex);    
#endif		
#ifdef NAP_ENV_REX
	rex_task_free();
#endif

	FWK_TRACE_FXN(eTrace_Framework, "END :: FWK_StartTimer");
	return (NAP_BOOL)retVal;
}

/******************************************************************************
* Function name : FWK_InternalStopTimer
* Description   : This function deletes timer node either from the ActiveList
*				: or inActive list.
* Return type   : NAP_BOOL
* Arguments     : 
*	[IN]		: ustimerId:Timer id to stop timer for that Id
*	[OUT]		: piErrNo:Error Code
*					
* Side Effect   : None 
* NOTE          : @todo: Write more on memory release done here...
*			Intererrupt enable and disable is not done in this function. This 
			fxn is called by NAP_FWK_OS_StopTimer and NAP_FWK_OS_ProcessTimer
			which are doing  the interrupt enable/disable action.
******************************************************************************/

NAP_BOOL	FWK_InternalStopTimer
(
	NAP_UINT16			usTimerId,
	NAP_INT16			*piErrNo
)
{

	S_FWK_TIMER_NODE *pstPrevNode = NAP_NULL;
	S_FWK_TIMER_NODE *pstCurNode  = NAP_NULL;

	FWK_TRACE_FXN(eTrace_Framework, "BEGIN :: FWK_InternalStopTimer");

	// First Check in g_pstInActiveList
	pstCurNode	= g_pstInActiveList->pstHead;
	pstPrevNode = pstCurNode;

	while(NAP_NULL != pstCurNode )
	{		
		// Checking whether passed Id matches with the current nodes timerId
		if(usTimerId == pstCurNode->pstTimerNode->usTimerId)
		{
			// Delete that node from the InActiveList
			if(pstPrevNode == pstCurNode) // It is first node
			{
				g_pstInActiveList->pstHead = 
										g_pstInActiveList->pstHead->pstNext;
				if( NAP_NULL == g_pstInActiveList->pstHead )
				{
					// It is last node so update tail
					g_pstInActiveList->pstTail = NAP_NULL;
				}
			}
			else
			{
				// Scan successive node
				pstPrevNode->pstNext = pstCurNode->pstNext ;
				if(NAP_NULL == pstCurNode->pstNext)
				{
					// It is last node so update tail
					g_pstInActiveList->pstTail = pstPrevNode ;
				}
			}

			// Free Timer Node			
			FWK_Free(&pstCurNode->pstTimerNode);
			FWK_Free(&pstCurNode);

			// Make Timer ID for reuse
			g_TimerID[usTimerId] = 0;

			FWK_TRACE_FXN(eTrace_Framework, "END :: FWK_InternalStopTimer");

			return NAP_SUCCESS;		
		}
		pstPrevNode = pstCurNode;

		// Going to the next node of the InActiveList for traversing
		pstCurNode = pstCurNode->pstNext;		
	}
	
	//Checking for existence of time ID
	if(NAP_NULL == pstCurNode)
	{

		FWK_TRACE_DETAIL(eTrace_Framework,
				      "FWK_InternalStopTimer :: Not in Inactive List ");
		*piErrNo = e_Err_Timer_NotCreated;
		// Don't return from here. As you have to still check for  
		// requested stop timer within Active List.
		//return NAP_FAILURE;
	}

	// @todo: Performance Upgradation: First check stop timer within
	// Active-List followed by In-Active list, as there are more chances of
	// stopping active timers compared to In-active timers.


	// Check and update and get the Timer node from the g_pstInActiveList
	pstCurNode	= g_pstActiveList->pstHead;
	pstPrevNode = pstCurNode;

	while(NAP_NULL != pstCurNode)
	{
		//Checking whether passed Id matches with the timerId of node
		if(usTimerId == pstCurNode->pstTimerNode->usTimerId )
		{
			// Delete that node from the g_pstActiveList
			if(pstPrevNode == pstCurNode) // It is first node
			{
				g_pstActiveList->pstHead = g_pstActiveList->pstHead->pstNext;
				if(NAP_NULL ==	g_pstActiveList->pstHead)
				{
					g_pstActiveList->pstTail = NAP_NULL;
				}
				else
				{
					// Update Delta of Next Node
					g_pstActiveList->pstHead->pstTimerNode->iDelta += 
											pstCurNode->pstTimerNode->iDelta;
					if(g_pstActiveList->pstHead->pstTimerNode->iDelta < 0)
					{
						g_pstActiveList->pstHead->pstTimerNode->iDelta = 0;
					}
				}
			}
			else
			{
				pstPrevNode->pstNext = pstCurNode->pstNext;
				if(pstCurNode->pstNext)
				{
					// Update Next Node Delta time
					pstCurNode->pstNext->pstTimerNode->iDelta += 
											pstCurNode->pstTimerNode->iDelta;
					if(pstCurNode->pstNext->pstTimerNode->iDelta < 0)
					{
						pstCurNode->pstNext->pstTimerNode->iDelta = 0;
					}
				}
				else
				{
					g_pstActiveList->pstTail = pstPrevNode ;
				}
			}

			// Free Timer Node			
			FWK_Free(&pstCurNode->pstTimerNode);
			FWK_Free(&pstCurNode);

			// Make Timer ID for reuse
			g_TimerID[usTimerId] = 0;

			FWK_TRACE_FXN(eTrace_Framework, "END :: FWK_InternalStopTimer");
			return NAP_SUCCESS;		
		}
		pstPrevNode = pstCurNode;
		//Go to next Node
		pstCurNode = pstCurNode->pstNext;		
	}

	if(g_TimerID[usTimerId] == 1)
	{
		FWK_ERROR_CRITICAL(e_Err_Timer_NotCreated,
			     "FWK_InternalStopTimer :: Timer Module Problem ???? "); 
	}
	
    return NAP_FAILURE;

}//End of FWK_InternalStopTimer()

 /******************************************************************************
* Function name : FWK_StopTimer
* Description   : This function deletes timer node either from the ActiveList
*				: or inActive list.
* Return type   : NAP_BOOL
* Arguments     : 
*	[IN]		: ustimerId:Timer id to stop timer for that Id
*	[OUT]		: piErrNo:Error Code
*					
* Side Effect   : None 
* NOTE          : @todo: Write more on memory release done here...
******************************************************************************/
NAP_BOOL	FWK_StopTimer
(
	NAP_UINT16			usTimerId,
	NAP_INT16			*piErrNo
)
{
    NAP_BOOL retVal;
	FWK_TRACE_FXN(eTrace_Framework, "BEGIN :: FWK_StopTimer");

	{
	    NAP_UINT8   aucBuf[64];
	    FWK_Sprintf(aucBuf, "Stop Timer Id = %d", usTimerId);
	    FWK_TRACE_DETAIL(eTrace_Framework, aucBuf);
	}
#ifdef NAP_ENV_WINDOWS
   	WaitForSingleObject(gTimerMutex,INFINITE);    
#endif
#ifdef NAP_ENV_REX
	rex_task_lock(); 
#endif

	retVal = FWK_InternalStopTimer(usTimerId,piErrNo);

#ifdef NAP_ENV_WINDOWS
   	ReleaseMutex(gTimerMutex);    
#endif		

#ifdef NAP_ENV_REX
	rex_task_free();
#endif

	FWK_TRACE_FXN(eTrace_Framework, "END :: FWK_StopTimer");
	return (NAP_BOOL)retVal;
}
 
/******************************************************************************
* Function name : FWK_ResetTimer
* Description   : This function resets the duration of the timer node
* Return type   : NAP_BOOL
* Arguments     : 
*	[IN]		: usTtimerId:Timer id to reset timer for that Id
*	[IN]		: unDuration:New Duration of timer
*	[OUT]		: piErrNo:Error Code
* Side Effect   : None 
* NOTE          : None
******************************************************************************/
NAP_BOOL	FWK_ResetTimer
(
	NAP_UINT16			usTimerId,
	NAP_UINT32			uiNewDuration,
	NAP_INT16			*piErrNo
)
{

	S_FWK_TIMER_NODE *pstPrevNode = NAP_NULL;
	S_FWK_TIMER_NODE *pstCurNode  = NAP_NULL;
	NAP_INT16 sError = 0;

	FWK_TRACE_FXN(eTrace_Framework, "BEGIN :: FWK_ResetTimer");
#ifdef NAP_ENV_WINDOWS
   	WaitForSingleObject(gTimerMutex,INFINITE);    
#endif
#ifdef NAP_ENV_REX
    rex_task_lock();         
#endif

	pstCurNode	= g_pstActiveList->pstHead;
	pstPrevNode = pstCurNode;
	while(NAP_NULL != pstCurNode )
	{
		// Checking whether passed Id matches with the timerId of node
		if(usTimerId == pstCurNode->pstTimerNode->usTimerId)
		{
			// Delete that node from the g_pstActiveList
			if(pstPrevNode == pstCurNode) // It is first node
			{
				g_pstActiveList->pstHead = g_pstActiveList->pstHead->pstNext;
				if(NAP_NULL == g_pstActiveList->pstHead)
				{
					g_pstActiveList->pstTail = NAP_NULL;
				}
				else
				{
					// Update Next Node Delta time
					g_pstActiveList->pstHead->pstTimerNode->iDelta += 
											pstCurNode->pstTimerNode->iDelta;
					if(g_pstActiveList->pstHead->pstTimerNode->iDelta < 0)
					{
						g_pstActiveList->pstHead->pstTimerNode->iDelta = 0;
					}
				}
			}
			else
			{
			        // More than 1 node exists in Active List.
				pstPrevNode->pstNext = pstCurNode->pstNext;
				if(pstCurNode->pstNext)
				{
					// Update Next Node Delta time
					pstCurNode->pstNext->pstTimerNode->iDelta += 
											pstCurNode->pstTimerNode->iDelta;
					if(pstCurNode->pstNext->pstTimerNode->iDelta < 0)
					{
						pstCurNode->pstNext->pstTimerNode->iDelta = 0;
					}
				}
				else
				{
				    // Re-setting last timer node. Update Tail of active list.
				    g_pstActiveList->pstTail = pstPrevNode ;
				}
			}

			// Update the Timer Node and add to the g_pstInActiveList	
			pstCurNode->pstTimerNode->uiDuration = uiNewDuration;
			pstCurNode->pstTimerNode->iDelta	 = uiNewDuration;
			pstCurNode->pstNext = NAP_NULL;

			// Insert node into g_pstInActiveList
			if(g_pstInActiveList->pstTail)
			{
				pstCurNode->pstNext = NAP_NULL;
				g_pstInActiveList->pstTail->pstNext = pstCurNode;
				g_pstInActiveList->pstTail			= pstCurNode;
			}
			else
			{
				pstCurNode->pstNext = NAP_NULL;
				g_pstInActiveList->pstTail = pstCurNode;
				g_pstInActiveList->pstHead = pstCurNode;
			}
			// Start Timer again
			FWK_InternalStartTimer(pstCurNode->pstTimerNode->usTimerId,
											&sError);
			*piErrNo = e_Err_Timer_ResetSuccess;
			#ifdef NAP_ENV_WINDOWS
			   	ReleaseMutex(gTimerMutex);    
			#endif		

			#ifdef NAP_ENV_REX
			rex_task_free();
			#endif
			
			FWK_TRACE_FXN(eTrace_Framework, "END :: FWK_ResetTimer");

			return NAP_SUCCESS;		
		}
		pstPrevNode = pstCurNode;
		//Go to next Node
		pstCurNode = pstCurNode->pstNext;		
	}
	//Checking for existence of time ID
	if(NAP_NULL == pstCurNode)
	{
		*piErrNo = e_Err_Timer_ResetFail;

#ifdef NAP_ENV_WINDOWS
   	ReleaseMutex(gTimerMutex);    
#endif		

#ifdef NAP_ENV_REX
		rex_task_free();
#endif
		return NAP_FAILURE;
	}

#ifdef NAP_ENV_WINDOWS
   	ReleaseMutex(gTimerMutex);    
#endif		

#ifdef NAP_ENV_REX
	rex_task_free();
#endif

	return NAP_FAILURE;
}//End of FWK_ResetTimer()


/******************************************************************************
* Function name : FWK_ProcessTimer
* Description   : This function periodically checks the ActiveList for the 
*				: timer which has expired
* Return type   : NAP_VOID
* Arguments     : 
* Side Effect   : None 
* NOTE          : None
******************************************************************************/
NAP_VOID	FWK_ProcessTimer
(
	void
)
{		
	S_FWK_TIMER		 *pstTimer     = NAP_NULL;
	S_FWK_TIMER_NODE *pstFirstNode = NAP_NULL;
	NAP_INT16 sError = 0;
#ifdef NAP_ENV_WINDOWS
   	WaitForSingleObject(gTimerMutex,INFINITE);    
#endif

#ifdef NAP_ENV_REX
    rex_task_lock();    
#endif

	if(g_pstActiveList  == NAP_NULL)
	{
		// No Timer Exist
#ifdef NAP_ENV_WINDOWS
   	ReleaseMutex(gTimerMutex);    
#endif		

#ifdef NAP_ENV_REX
		rex_task_free();  
#endif

		return;
	}
	
	if(g_pstActiveList->pstHead == NAP_NULL)
	{
		// No Timer Exist

#ifdef NAP_ENV_WINDOWS
   	ReleaseMutex(gTimerMutex);    
#endif		

#ifdef NAP_ENV_REX
		rex_task_free();
#endif

		return;
	}

	pstFirstNode = g_pstActiveList->pstHead;
	pstTimer	 = pstFirstNode->pstTimerNode;

	// Decrement the delta value of the first timer in the ActiveList
	pstTimer->iDelta -= FWK_DELTA_TIME;
	
	// Check for Time out
	//if(pstTimer->iDelta <= 0)
	while(pstTimer->iDelta <= 0)
	{
		// Check for Periodic Timer by checking usResetFlag
		if(1 == pstTimer->usResetFlag )
		{
			FWK_TRACE_DETAIL(eTrace_Framework, 
			   "NAP_FWK_OS_ProcessTimer :: Periodic Timer processing started");
			
			// Remove First Node from g_pstActiveList
			g_pstActiveList->pstHead = g_pstActiveList->pstHead->pstNext;
			if(NAP_NULL == g_pstActiveList->pstHead)
			{
				g_pstActiveList->pstTail  = NAP_NULL;
			}
			else
			{
				g_pstActiveList->pstHead->pstTimerNode->iDelta += 
															pstTimer->iDelta;
				if(g_pstActiveList->pstHead->pstTimerNode->iDelta < 0)
				{
					g_pstActiveList->pstHead->pstTimerNode->iDelta = 0;
				}
			}
		 
			// Insert node into g_pstInActiveList
			if(g_pstInActiveList->pstTail)
			{
				// one or more node exists in InActiveList
				pstFirstNode->pstNext = NAP_NULL;
				g_pstInActiveList->pstTail->pstNext = pstFirstNode;
				g_pstInActiveList->pstTail = pstFirstNode;
			}
			else 
			{
				// no node exists in the InActiveList
				pstFirstNode->pstNext = NAP_NULL;
				g_pstInActiveList->pstTail = pstFirstNode;
				g_pstInActiveList->pstHead = pstFirstNode;
			}

			//Updating Delta value before inserting into Active List again
			pstTimer->iDelta = pstTimer->uiDuration;
			
			// Start Timer again
			FWK_InternalStartTimer(pstTimer->usTimerId,&sError);

			// Birender : Added for trace
			{
				char buf[128];
				FWK_Sprintf(buf, 
				      " FWK_ProcessTimer :: PeriodicTimeout ::Timeout(ID=%d) :: Dur = %d ms\n", 
				      pstTimer->usTimerId,pstTimer->uiDuration);
				FWK_TRACE_DETAIL (eTrace_Framework, buf);
			}

#ifdef NAP_ENV_WINDOWS
   	ReleaseMutex(gTimerMutex);    
#endif		

#ifdef NAP_ENV_REX
			rex_task_free();
#endif

			if(pstTimer->cbTimerHandler)
			{
				// Issue Callback function
				
				pstTimer->cbTimerHandler (	pstTimer->pvData, 
											pstTimer->usTimerId);
			}
			else
			{
				S_FWK_IMC_INFO *pstIMCInfo;
				pstIMCInfo = FWK_Malloc(sizeof(S_FWK_IMC_INFO));
				if(NAP_NULL == pstIMCInfo)
				{
					return;	
				}	
				pstIMCInfo->destId = pstTimer->usModuleId;
				pstIMCInfo->srcId = e_TimerModule;
				pstIMCInfo->actionEventId = (NAP_UINT8)pstTimer->usTimerId;
				pstIMCInfo->protocolId = 0;
				pstIMCInfo->pvData = pstTimer->pvData;
				FWK_IPC_Put_Data((NAP_UINT32)pstTimer->uiQueueId,
					(NAP_VOID*)pstIMCInfo,&sError);
			}
	
			//return;
	
		}		
		else 
		{
			NAP_UINT16 usTimerId;
			NAP_UINT32 uiDur;
			NAP_VOID *pvTimerData;
			NAP_UINT32 uiQueueId;
			CB_TIMERHANDLER	pCbkFxn;
			NAP_UINT8 destId;

			FWK_TRACE_DETAIL(eTrace_Framework,
			   "FWK_ProcessTimer :: One SHOT Timer processing started");
			
			/* BSP : 08May05 :Save Timer Parameters as it is deleted in Stoptimer fxn */
			uiDur = pstTimer->uiDuration;
			usTimerId = pstTimer->usTimerId;
			pvTimerData = pstTimer->pvData;
			pCbkFxn = pstTimer->cbTimerHandler;
			uiQueueId = pstTimer->uiQueueId;
			destId = pstTimer->usModuleId;
						
			FWK_InternalStopTimer(pstTimer->usTimerId,&sError);

			// Birender : Added for trace
			{
				char buf[128];
				FWK_Sprintf(buf,"FWK_ProcessTimer :: One SHOT Timeout ::Timeout(ID=%d) :: Dur = %d ms\n",usTimerId,uiDur);
				FWK_Printf("\nFWK_ProcessTimer :: One SHOT Timeout ::Timeout(ID=%d) :: Dur = %d ms\n",usTimerId,uiDur);
				FWK_TRACE_DETAIL(eTrace_Framework, buf);
			}
			#ifdef NAP_ENV_WINDOWS
			   	ReleaseMutex(gTimerMutex);    
			#endif		

			#ifdef NAP_ENV_REX
			rex_task_free();
			#endif
			
			if(pCbkFxn)
			{
				// Issue Callback function
				pCbkFxn(pvTimerData,usTimerId);
			}
			else
			{
				S_FWK_IMC_INFO *pstIMCInfo;
				pstIMCInfo = FWK_Malloc(sizeof(S_FWK_IMC_INFO));
				if(NAP_NULL == pstIMCInfo)
				{
					return;	
				}	
				pstIMCInfo->destId = destId;
				pstIMCInfo->srcId = e_TimerModule;
				pstIMCInfo->actionEventId = (NAP_UINT8)usTimerId;
				pstIMCInfo->protocolId = 0;
				pstIMCInfo->pvData = pvTimerData;
				FWK_IPC_Put_Data((NAP_UINT32)uiQueueId,
					(NAP_VOID*)pstIMCInfo,&sError);
			}

			//return;
		}

		/*************************************/
		#ifdef NAP_ENV_WINDOWS
   			WaitForSingleObject(gTimerMutex,INFINITE);    
		#endif

		#ifdef NAP_ENV_REX
			rex_task_lock();    
		#endif

		if(g_pstActiveList  == NAP_NULL)
		{
		// No Timer Exist
			#ifdef NAP_ENV_WINDOWS
   				ReleaseMutex(gTimerMutex);    
			#endif		

			#ifdef NAP_ENV_REX
					rex_task_free();  
			#endif

			return;
		}
	
		if(g_pstActiveList->pstHead == NAP_NULL)
		{
			// No Timer Exist

			#ifdef NAP_ENV_WINDOWS
   				ReleaseMutex(gTimerMutex);    
			#endif		

			#ifdef NAP_ENV_REX
					rex_task_free();
			#endif

			return;
		}

		pstFirstNode = g_pstActiveList->pstHead;
		pstTimer	 = pstFirstNode->pstTimerNode;

		// Decrement the delta value of the first timer in the ActiveList
		//pstTimer->iDelta -= FWK_DELTA_TIME;
		/*************************************/
	}

#ifdef NAP_ENV_WINDOWS
   	ReleaseMutex(gTimerMutex);    
#endif		

#ifdef NAP_ENV_REX
	rex_task_free();
#endif

    return;

}//End of FWK_ProcessTimer()

NAP_VOID	FWK_RegisteredTimerFxn
(
	void
)
{
#ifdef NAP_ENV_REX
	rex_set_sigs (&timer_tcb, TIMERTASK_CMD_Q_SIG);
#endif
    return;
}

/******************************************************************************
* Function name : FWK_DeInitTimer
* Description   : This function de-registers timer utility to the clock of the 
*				: target. It releases all timers if it exists.
*				: 
* Return type   : 
* Arguments     : 
* Side Effect   : 
* NOTE          : 
*				: 
******************************************************************************/
NAP_VOID FWK_DeInitTimer
(
	void
)
{
	S_FWK_TIMER_NODE	*pstCurrNode = NAP_NULL;
	S_FWK_TIMER_NODE	*pstPrevNode = NAP_NULL;

    // De-Register call-back-ticks from the clock.
	FWK_Timer_DeRegTask();
#ifdef NAP_ENV_WINDOWS
   	WaitForSingleObject(gTimerMutex,INFINITE);    
#endif
#ifdef NAP_ENV_REX
	rex_task_lock();
#endif

	// Release all the Nodes of the Active List
	pstCurrNode = g_pstActiveList->pstHead;
	while(NAP_NULL != pstCurrNode)
	{
		pstPrevNode = pstCurrNode;
		pstCurrNode = pstCurrNode->pstNext;
		FWK_Free(&(pstPrevNode->pstTimerNode));
		FWK_Free(&pstPrevNode);
	}
	
	// Release all the Nodes of the In-Active List
	pstCurrNode = g_pstInActiveList->pstHead;
	while(NAP_NULL != pstCurrNode)
	{
		pstPrevNode = pstCurrNode;
		pstCurrNode = pstCurrNode->pstNext;
		FWK_Free(&(pstPrevNode->pstTimerNode));
		FWK_Free(&pstPrevNode);
	}
	
	// Release Global Active & In-Active List
	FWK_Free(&g_pstActiveList);
	FWK_Free(&g_pstInActiveList);

#ifdef NAP_ENV_WINDOWS
   	ReleaseMutex(gTimerMutex);    
#endif		

#ifdef NAP_ENV_REX
	rex_task_free();
#endif

#ifdef NAP_ENV_WINDOWS
   	CloseHandle(gTimerMutex);    
#endif		

}

/******************************************************************************
* Function name : 
* Description   :  
*				: 
*				: 
* Return type   : 
* Arguments     : 
* Side Effect   : 
* NOTE          : 
*				: 
******************************************************************************/
#ifdef NAP_ENV_REX

void timer_init ()
{
	rex_def_timer( &timer_rpt_timer, &timer_tcb, TIMERTASK_RPT_TIMER_SIG );
/*=
*  This function is called by each task when it is created.  The function
* then performs the startup processing for the calling task.  The function
* first sets the acknowledge signal( on which define task is waiting)
* back to TMC/MC to acknowledge that it has
* been created.  It then waits for the start signal back to it
* (mc_satrt_application have to set a signal to this task or function).  While
* waiting for the start signal the watchdog timer is kicked everytime the
* interval timer expires.  When it gets the start signal it acknowledges
* it back to TMC/MC and it then returns to the caller.  The caller task is
* now formally started and it can start normal application processing.


* The calling task stays here until it receives the START signal.

=*/
	task_start(TIMERTASK_RPT_TIMER_SIG, DOG_TIMERTASK_RPT, &timer_rpt_timer);

	if(FALSE == nap_task_heap_init(NAP_TIMER_TASK_MEMORY))
	{
		FWK_ERROR_MAJOR(e_Err_Fwk_MallocFailed,
										    "TIMER TASK : Error in heap init");
		FWK_Strcpy(ac_napTimerBuf,"Timer: Error in heap init");
	}
    return;
}

void	TIMER_Thread(DWORD params)
{
    rex_sigs_type sigs = 0;	
	
	timer_init();
	rex_set_sigs (&timer_tcb, TIMERTASK_RPT_TIMER_SIG);

	/*check the signals and invoke the corresponding handler*/
	for(;;)
	{
		sigs = rex_wait(TIMERTASK_WAIT_SIGNALS);	
		(void) rex_clr_sigs( &timer_tcb, sigs );
		
		if (sigs & TIMERTASK_RPT_TIMER_SIG)
		{
			timer_kick_dog(sigs);
		}			
		if (sigs & TASK_STOP_SIG)
		{
			task_stop();
		}
		if (sigs & TASK_OFFLINE_SIG)
		{
			task_offline();
		}
		if(sigs & TIMERTASK_CMD_Q_SIG)
		{
			NAP_FWK_OS_ProcessTimer();
		}
	}
	return

} // End of TIMER_Thread
#endif // NAP_ENV_REX

#ifdef NAP_ENV_WINDOWS
unsigned __stdcall TIMER_Thread( void* pArguments )
{

	/*check the signals and invoke the corresponding handler*/
	for(;;)
	{
		Sleep(FWK_DELTA_TIME);	

		FWK_ProcessTimer();
	}

	return 0;
}
#endif

/******************************************************************************
* Function name : FWK_Timer_RegTask
* Description   : This function registers timer utility to the clock of the 
*				: target. It registers a call-back function to be called
*				: periodically after requested duration to process on timer.
* Return type   : 
* Arguments     : 
* Side Effect   : 
* NOTE          : Dependent upon REX CLK Functionality.
*				: 
******************************************************************************/
NAP_BOOL FWK_Timer_RegTask
(
	void
)
{
#ifdef NAP_ENV_WINDOWS
	
	HANDLE hThread;
    unsigned threadID;
	
    FWK_Printf( "\nCreating Timer thread...\n\n" );

    // Create the Timer thread.
    hThread = (HANDLE)_beginthreadex( NULL, 0, &TIMER_Thread, NULL, 0,&threadID );

#endif

#ifdef NAP_ENV_REX
	gpstclbkTimerStruct = NAP_NULL;
	FWK_TRACE_FXN(eTrace_Framework, "BEGIN :: NAP_FWK_OS_Timer_RegTask");

	gpstclbkTimerStruct = (clk_cb_type *) FWK_Malloc (
														sizeof(clk_cb_type));
	if (NAP_NULL == gpstclbkTimerStruct)
	{
		return NAP_FAILURE;
	}

	// Initialize Call-back structure as per REX - CLK.
	clk_def(gpstclbkTimerStruct);

	// Register Call-back function with the call-back structure.
	clk_reg(gpstclbkTimerStruct,					// CLK- Call-back structure
			&FWK_RegisteredTimerFxn,	// Call-back function
			FWK_DELTA_TIME,	// First Time-out
			FWK_DELTA_TIME,	// Successive Time-out 
			1 // TRUE (1) for periodic interval
			);

#endif // NAP_ENV_REX

	return NAP_SUCCESS;
}

/******************************************************************************
* Function name : FWK_Timer_DeRegTask
* Description   : This function de-registers Timer Utility with the target
*				: clock for receiving clock-ticks.
*				: 
* Return type   : 
* Arguments     : 
* Side Effect   : 
* NOTE          : Dependent upon REX CLK Functionality.
*				: 
******************************************************************************/
NAP_VOID FWK_Timer_DeRegTask
(
	void
)
{
#ifdef NAP_ENV_REX
	clk_dereg(gpstclbkTimerStruct);

	FWK_Free(&gpstclbkTimerStruct);
#endif // #ifdef NAP_ENV_REX

    return;
}

/******************************************************************************
* Function name : FWK_InternalStopTimer_Ex
* Description   : This function does exactly what FWK_InternalStopTimer does.
*				  In addition to that it also returns the app data pointer in this
*				  API.
* Return type   : (NAP_VOID *) App Data, Returns NAP_NULL when fails.
* Arguments     : 
*	[IN]		: ustimerId:Timer id to stop timer for that Id
*	[OUT]		: piErrNo:Error Code
*					
* Side Effect   : None 
* NOTE          : 
*
******************************************************************************/

NAP_VOID *	FWK_InternalStopTimer_Ex
(
	NAP_UINT16			usTimerId,
	NAP_INT16			*piErrNo
)
{

	NAP_VOID *pvRetVal = NAP_NULL;

	S_FWK_TIMER_NODE *pstPrevNode = NAP_NULL;
	S_FWK_TIMER_NODE *pstCurNode  = NAP_NULL;

	FWK_TRACE_FXN(eTrace_Framework, "BEGIN :: FWK_InternalStopTimer_Ex");

	// First Check in g_pstInActiveList
	pstCurNode	= g_pstInActiveList->pstHead;
	pstPrevNode = pstCurNode;

	while(NAP_NULL != pstCurNode )
	{		
		// Checking whether passed Id matches with the current nodes timerId
		if(usTimerId == pstCurNode->pstTimerNode->usTimerId)
		{
			// Delete that node from the InActiveList
			if(pstPrevNode == pstCurNode) // It is first node
			{
				g_pstInActiveList->pstHead = 
										g_pstInActiveList->pstHead->pstNext;
				if( NAP_NULL == g_pstInActiveList->pstHead )
				{
					// It is last node so update tail
					g_pstInActiveList->pstTail = NAP_NULL;
				}
			}
			else
			{
				// Scan successive node
				pstPrevNode->pstNext = pstCurNode->pstNext ;
				if(NAP_NULL == pstCurNode->pstNext)
				{
					// It is last node so update tail
					g_pstInActiveList->pstTail = pstPrevNode ;
				}
			}

			/* return the application data */
			pvRetVal = pstCurNode->pstTimerNode->pvData;
			// Free Timer Node			
			FWK_Free(&pstCurNode->pstTimerNode);
			FWK_Free(&pstCurNode);

			// Make Timer ID for reuse
			g_TimerID[usTimerId] = 0;

			FWK_TRACE_FXN(eTrace_Framework, "END :: FWK_InternalStopTimer_Ex");

			return pvRetVal;		
		}
		pstPrevNode = pstCurNode;

		// Going to the next node of the InActiveList for traversing
		pstCurNode = pstCurNode->pstNext;		
	}
	
	//Checking for existence of time ID
	if(NAP_NULL == pstCurNode)
	{

		FWK_TRACE_DETAIL(eTrace_Framework,
				      "FWK_InternalStopTimer_Ex :: Not in Inactive List ");
		*piErrNo = e_Err_Timer_NotCreated;
		// Don't return from here. As you have to still check for  
		// requested stop timer within Active List.
		//return NAP_FAILURE;
	}

	// @todo: Performance Upgradation: First check stop timer within
	// Active-List followed by In-Active list, as there are more chances of
	// stopping active timers compared to In-active timers.


	// Check and update and get the Timer node from the g_pstInActiveList
	pstCurNode	= g_pstActiveList->pstHead;
	pstPrevNode = pstCurNode;

	while(NAP_NULL != pstCurNode)
	{
		//Checking whether passed Id matches with the timerId of node
		if(usTimerId == pstCurNode->pstTimerNode->usTimerId )
		{
			// Delete that node from the g_pstActiveList
			if(pstPrevNode == pstCurNode) // It is first node
			{
				g_pstActiveList->pstHead = g_pstActiveList->pstHead->pstNext;
				if(NAP_NULL ==	g_pstActiveList->pstHead)
				{
					g_pstActiveList->pstTail = NAP_NULL;
				}
				else
				{
					// Update Delta of Next Node
					g_pstActiveList->pstHead->pstTimerNode->iDelta += 
											pstCurNode->pstTimerNode->iDelta;
					if(g_pstActiveList->pstHead->pstTimerNode->iDelta < 0)
					{
						g_pstActiveList->pstHead->pstTimerNode->iDelta = 0;
					}
				}
			}
			else
			{
				pstPrevNode->pstNext = pstCurNode->pstNext;
				if(pstCurNode->pstNext)
				{
					// Update Next Node Delta time
					pstCurNode->pstNext->pstTimerNode->iDelta += 
											pstCurNode->pstTimerNode->iDelta;
					if(pstCurNode->pstNext->pstTimerNode->iDelta < 0)
					{
						pstCurNode->pstNext->pstTimerNode->iDelta = 0;
					}
				}
				else
				{
					g_pstActiveList->pstTail = pstPrevNode ;
				}
			}

			/* return the application data */
			pvRetVal = pstCurNode->pstTimerNode->pvData;

			// Free Timer Node			
			FWK_Free(&pstCurNode->pstTimerNode);
			FWK_Free(&pstCurNode);

			// Make Timer ID for reuse
			g_TimerID[usTimerId] = 0;

			FWK_TRACE_FXN(eTrace_Framework, "END :: FWK_InternalStopTimer_Ex");
			return pvRetVal;		
		}
		pstPrevNode = pstCurNode;
		//Go to next Node
		pstCurNode = pstCurNode->pstNext;		
	}

	if(g_TimerID[usTimerId] == 1)
	{
		FWK_ERROR_CRITICAL(e_Err_Timer_NotCreated,
			     "FWK_InternalStopTimer_Ex :: Timer Module Problem ???? "); 
	}
	
    return pvRetVal;

}//End of FWK_InternalStopTimer_Ex()

/******************************************************************************
* Function name : FWK_StopTimer_Ex
* Description   : This function does exactly the same as FWK_StopTimer. In 
*				: addition to that it also returns the application data.
* Return type   : (NAP_VOID *) App Data, Returns NAP_NULL in case of failure.
* Arguments     : 
*	[IN]		: ustimerId:Timer id to stop timer for that Id
*	[OUT]		: piErrNo:Error Code
*					
* Side Effect   : None 
* NOTE          : @todo: Write more on memory release done here...
******************************************************************************/
NAP_VOID * FWK_StopTimer_Ex
(
	NAP_UINT16			usTimerId,
	NAP_INT16			*piErrNo
)
{
    NAP_VOID * pvRetVal = NAP_NULL;
	FWK_TRACE_FXN(eTrace_Framework, "BEGIN :: FWK_StopTimer_Ex");

	{
	    NAP_UINT8   aucBuf[64];
	    FWK_Sprintf(aucBuf, "Stop Timer Id = %d", usTimerId);
	    FWK_TRACE_DETAIL(eTrace_Framework, aucBuf);
	}
#ifdef NAP_ENV_WINDOWS
   	WaitForSingleObject(gTimerMutex,INFINITE);    
#endif
#ifdef NAP_ENV_REX
	rex_task_lock(); 
#endif

	pvRetVal = FWK_InternalStopTimer_Ex(usTimerId,piErrNo);

#ifdef NAP_ENV_WINDOWS
   	ReleaseMutex(gTimerMutex);    
#endif		

#ifdef NAP_ENV_REX
	rex_task_free();
#endif

	FWK_TRACE_FXN(eTrace_Framework, "END :: FWK_StopTimer_Ex");
	return pvRetVal;
}
