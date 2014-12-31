#ifndef _FWK_TIMER_H_
#define _FWK_TIMER_H_

/*
*******************************************************************************
*                               INCLUDE FILES
*******************************************************************************
*/


#include "fwk_datatypes.h"
#include "fwk_memory.h"
#include "fwk_error.h"
#include "fwk_rtos.h"
#include "fwk_string.h"

/*
*******************************************************************************
*                               MACRO DEFINITIONS
*******************************************************************************
*/

#define MAX_TIMERS			700 //100::24-Jul-2007
#define FWK_DELTA_TIME    	100   // ms

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

/*
*******************************************************************************
*                               STRUCTURE DEFINITIONS
*******************************************************************************
*/

typedef NAP_VOID (*CB_TIMERHANDLER)(NAP_VOID *pvData, NAP_UINT16 timerId);

//Timer structure
typedef struct _S_FWK_TIMER
{
	NAP_UINT16        	usTimerId;
	NAP_UINT8			usModuleId;
	NAP_UINT32          uiQueueId;
	NAP_UINT32        	uiDuration;	
	NAP_INT32		  	iDelta;
	NAP_VOID		  	*pvData;
	CB_TIMERHANDLER   	cbTimerHandler;
	NAP_UINT16        	usResetFlag;
	
} S_FWK_TIMER;


/* Structure of Timer Node */
typedef struct _S_FWK_TIMER_NODE
{
	S_FWK_TIMER			       *pstTimerNode;//Pointer to the timer node 					
	struct _S_FWK_TIMER_NODE   *pstNext;     //Pointer to the next node
}S_FWK_TIMER_NODE;

/* Structure of Timer List */
typedef struct _S_FWK_TIMER_LIST
{
	S_FWK_TIMER_NODE	*pstHead;   //First Node
	S_FWK_TIMER_NODE	*pstTail;	//Last Node
	
}S_FWK_TIMER_LIST;

/*
*******************************************************************************
*                               EXTERNAL API
*******************************************************************************
*/

/* Initialize Timer */
NAP_BOOL FWK_InitTimer (void);

/*This function is called to create a timer */
NAP_BOOL	FWK_CreateTimer(
									NAP_UINT16			*pusTimerId,
									NAP_UINT8			usModuleId,		
									NAP_UINT32			uiQueueId,
									NAP_UINT32			uiDuration,
									NAP_UINT16		    usResetFlag,
									CB_TIMERHANDLER		TimerHandler,
									NAP_VOID			*pvData,
									NAP_INT16			*piErrNo
								  );

/*This function is  called to start a timer */
NAP_BOOL	FWK_StartTimer(
								   NAP_UINT16  timerId,
								   NAP_INT16  *piErrNo
								 );

/*This function is a called to stop a timer*/
NAP_BOOL	FWK_StopTimer(
								  NAP_UINT16  timerId,
								  NAP_INT16  *piErrNo
								);

/*This function is a called to process a timer */ 
NAP_VOID	FWK_ProcessTimer (void);

/*This function is a called to reset a timer */
NAP_BOOL	FWK_ResetTimer(
								   NAP_UINT16   timerId,
								   NAP_UINT32   uiNewDuration,
					               NAP_INT16   *piErrNo
								 );

/* Register Timer with the REX Clock. Done in InitTimer */
NAP_BOOL FWK_Timer_RegTask (void);

/* De-Register Timer with the REX Clock. Done in DeInit Timer*/
NAP_VOID FWK_Timer_DeRegTask (void);

/* De-Initialize Timer */
NAP_VOID FWK_DeInitTimer (void);

/*This function does exactly the same as FWK_StopTimer. In addition to 
that it also returns the application data.It returns (NAP_VOID *) App Data,
or returns NAP_NULL in case of failure.*/
NAP_VOID * FWK_StopTimer_Ex
(
	NAP_UINT16			usTimerId,
	NAP_INT16			*piErrNo
);


#endif // _FWK_TIMER_H_
