
/*
*******************************************************************************
*                               INCLUDE FILES
*******************************************************************************
*/
#include "fwk_statistics.h"

/*
*******************************************************************************
*                               GLOBAL VARIABLES
*******************************************************************************
*/
NAP_INT16 gasStatValues[NO_STAT_OBJ];
extern NAP_UINT32 guiModulesToTrace;
extern NAP_CHAR gacTraceFileName[];

/******************************************************************************
* Function name	: NAP_STATS_INIT
* Description	: Initializes Statistics Module
* Return type	: Boolean - NAP_SUCCESS indicates successfully initialized.
* Argument		: 
*	[OUT]		: psiError - Statistics Error Code 
* Side Effects	: Global Variable (gasStatValues) gets initialized with 0.
* NOTE 			: 
******************************************************************************/
NAP_BOOL	NAP_STATS_INIT
(
	NAP_INT16		*psiError
)
{
	NAP_UINT16 usLoopCount;
	if(NAP_NULL == psiError)
	{
		return NAP_FAILURE;
	}
	else
	{
		for(usLoopCount=0;usLoopCount < NO_STAT_OBJ;usLoopCount++)
		{
			gasStatValues[usLoopCount] = 0;
		}
		
	}
	return NAP_SUCCESS;
}

/******************************************************************************
* Function name	: NAP_STATS_UPDATE
* Description	: Updates Statistics of the requested eStatId.
* Return type	: Boolean - NAP_SUCCESS indicates successfully updated.
* Argument		: 
*	[IN]		: eStatId  - Stat enumerator to be updated
*	[IN]		: siUpdateIncrmnt - Update enumerate by this value.
*	[OUT]		: psiError - Statistics Error Code 
* Side Effects	: Global Variable (gasStatValues) may get effected.
* NOTE 			: 
******************************************************************************/
NAP_BOOL 	NAP_STATS_UPDATE 
(
	E_STATS_OBJ_ID 	eStatId,
	NAP_INT16		siUpdateIncrmnt, 
	NAP_INT16		*psiError
)
{
	if(eStatId > NO_STAT_OBJ || eStatId < 0)
	{
		return NAP_FAILURE;
	}
	else
	{
		gasStatValues[eStatId] = (NAP_INT16)(gasStatValues[eStatId] + 
											siUpdateIncrmnt);
	}

	return NAP_SUCCESS;
}

/******************************************************************************
* Function name	: NAP_STATS_GET
* Description	: Retrieve current statistics value of the requested 
*				: enumerator.
* Return type	: Boolean - NAP_SUCCESS indicates successfully retrieved
* Argument		: 
*	[IN]		: eStatId  - Stat enumerator requested for statistics
*	[OUT]		: psiStatValue - Curr Stat value hold by the enumerator.
*	[OUT]		: psiError - Statistics Error Code 
* Side Effects	: None
* NOTE 			: 
******************************************************************************/
NAP_BOOL	NAP_STATS_GET 
(
	E_STATS_OBJ_ID 	eStatId,
	NAP_INT16		*psiStatValue,
	NAP_INT16		*psiError
)
{
	if(NAP_NULL == psiStatValue  || eStatId > NO_STAT_OBJ || eStatId < 0 )
	{
		return NAP_FAILURE;
	}
	else
	{
		*psiStatValue = gasStatValues[eStatId];
	}
	return NAP_SUCCESS;
}

/******************************************************************************
* Function name	: NAP_STATS_DISPLAY 
* Description	: Retrieve & display current statistics value of all stat enums
* Return type	: Boolean - NAP_SUCCESS indicates successfully retrieved
* Argument		: 
*	[OUT]		: psiError - Statistics Error Code 
* Side Effects	: None
* NOTE 			: 
******************************************************************************/
NAP_BOOL	NAP_STATS_DISPLAY 
(
	NAP_INT16		*psiError
)
{
	NAP_UINT16 usLoopCount;
	NAP_CHAR acDisplayBuffer[50];
	if(NAP_NULL == psiError)
	{
		return NAP_FAILURE;
	}
	else
	{
		for( usLoopCount = 0 ; usLoopCount < NO_STAT_OBJ ; usLoopCount++ )
		{
			FWK_Sprintf(acDisplayBuffer,
						"Statistic Values \nSTAT_OBJ_ID=%2d-->VALUE=%2d\n",
						usLoopCount, 
						gasStatValues[usLoopCount]);
		}
		return NAP_SUCCESS;
	}
}
