
#ifndef _FWK_RTOS_H_
#define _FWK_RTOS_H_

#include "fwk_datatypes.h"
#include "fwk_error.h"

#ifdef NAP_ENV_WINDOWS
#include "windows.h"
#include "process.h"
#include "time.h"
#include <sys\timeb.h>
#endif

/* Seconds elapsed from from year 1900 to year 1970 */
#define FWK_SECS_BETWEEN_1900_1970 2208988800u

/* Representation of seconds in 64Bit Timestamp*/
#define FWK_SECS_IN_64_BIT_TIMESTAMP(sec) \
					((NAP_UINT64)(((NAP_UINT64)(sec)) * (10000000)))


/* Current local time */
#define FWK_FTime _ftime
/* Structure returned by _ftime system call */
typedef struct _timeb S_FWK_TimeB;

//Structure of SystemTimer
typedef struct _S_NAP_FWK_TIMESTAMP
{
	NAP_UINT16 wYear;  
	NAP_UINT16 wMonth;  
	NAP_UINT16 wDayOfWeek;  
	NAP_UINT16 wDay;  
	NAP_UINT16 wHour;  
	NAP_UINT16 wMinute;  
	NAP_UINT16 wSecond;  
	NAP_UINT16 wMilliseconds;
}S_NAP_FWK_TIMESTAMP;

#ifdef  __cplusplus
extern "C" {
#endif


/*
 *	This function will get the current system time 
 *  with the current timezone adjustment. Eventually, it gives the
 *  local time
 */
NAP_BOOL FWK_GetSystemTime
(
	S_NAP_FWK_TIMESTAMP *pstTime
);
#if 0
/*
 *	This function will get the current system time 
 *  in UTC.
 */
NAP_BOOL FWK_GetSystemTimeInUTC
(
	S_NAP_FWK_TIMESTAMP *pstTime
);

#if 0
/*
 *	This function will get the 64 bit NTP timestamp
 */
NAP_UINT64 FWK_GetNTPTimestamp(NAP_VOID);
#endif

/*
 *	This function will get the 64 bit current System timestamp in UTC
 */
NAP_UINT64 FWK_Get64BitSysTimestamp(NAP_VOID);


/*
 *	This function will convert the 
 *  64 bit timestamp into S_NAP_FWK_TIMESTAMP
 */
NAP_BOOL FWK_ConvertFrom64BitTimestamp\
(\
	const NAP_UINT64 u64Timestamp,\
	S_NAP_FWK_TIMESTAMP *pstTime\
);

/*
 *	This function will convert the 
 *  S_NAP_FWK_TIMESTAMP into 64 bit timestamp
 */
NAP_UINT64 FWK_ConvertInto64BitTimestamp(S_NAP_FWK_TIMESTAMP *pstTime);

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
);

#endif
/*
 *This function is called to initialize the IPC queue.
 */

NAP_BOOL FWK_IPC_Initialize(NAP_VOID *queueId,NAP_INT16 *piError);


/*
 *This function is called to put the data into IPC queue.
 */

NAP_BOOL FWK_IPC_Put_Data(NAP_UINT32 ui_sig,NAP_VOID *vpInData,	NAP_INT16 *piError);



/*
 *This function is called to read the data from IPC queue.
 */

NAP_BOOL FWK_IPC_Get_Data(NAP_VOID *queueId,NAP_VOID **vpOutData,NAP_INT16 *piError);

NAP_BOOL OS_GetSystemTime 
(
	S_NAP_FWK_TIMESTAMP *pstTimeStamp
);

/*****************************************************************************
* Function name : FWK_Queue_Error
* Description   : Reads the error while reading from and writing to the queue.
* Return type   : NAP_BOOL
* Arguments     : [OUT] piError : ErrorType.
* Side Effect   : None
* NOTE          : None
*****************************************************************************/
NAP_BOOL  FWK_Queue_Error(NAP_INT32 *piError);


#ifdef __cplusplus
}
#endif


#endif // _FWK_RTOS_H_
