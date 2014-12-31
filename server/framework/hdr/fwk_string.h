
#ifndef __FWK_STRING_H__
#define __FWK_STRING_H__

#include "fwk_datatypes.h"

#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "stdio.h"
#include "stdarg.h"
#include "time.h"
/*
*******************************************************************************
*                               MACRO DEFINITIONS
*******************************************************************************
*/
#define FWK_Va_list		va_list
#define FWK_Va_start	va_start
#define FWK_Va_arg		va_arg
#define FWK_Va_end		va_end



#define FWK_Sprintf		sprintf
#define FWK_Printf		printf
#define FWK_Fprintf		fprintf
#define	FWK_Fflush		fflush
#define FWK_Vsprintf	vsprintf
#define FWK_Vfprintf	vfprintf
#define FWK_Stricmp		stricmp

/* Windows specific snprintf */
#ifdef NAP_ENV_WINDOWS
#define FWK_Snprintf	_snprintf
#endif /* NAP_ENV_WINDOWS */

#define FWK_Srand		srand
#define FWK_Rand	    rand

/*
The time function returns the number of seconds elapsed 
since midnight (00:00:00), January 1, 1970, coordinated 
universal time (UTC), according to the system clock.
*/
#define FWK_TimeElapsed(x)     time(NAP_NULL)

/*
This API is used to get length of string
*/

#ifdef __cplusplus    // If used by C++ code, 
extern "C" {          // we need to export the C interface
#endif

NAP_UINT32 FWK_Strlen ( 
								const NAP_CHAR *kpcSource 
						    );

/*
This API is used to copy Strings
*/

NAP_CHAR *FWK_Strcpy ( 
							NAP_CHAR *pcDest,
							const NAP_CHAR *kpcSource 
						    );

/*
This API is used to compare two strings
*/
NAP_INT16 FWK_Strcmp ( 
							const NAP_CHAR *kpcSource,
							const NAP_CHAR *kpcDest 
					        );

NAP_INT16 FWK_Strcasecmp ( 
							const NAP_CHAR *kpcSource,
							const NAP_CHAR *kpcDest 
					        );

/*
This API is used to compare n no. of characters in two strings
*/

NAP_CHAR *FWK_Strncpy ( 
							NAP_CHAR *pcDest, 
							const NAP_CHAR *kpcSource, 
							NAP_INT16 iNumchars 
							 );

/*
This API is used to compare two sub strings
*/

NAP_CHAR *FWK_Strstr ( 
							const NAP_CHAR *kpcDest,
							const NAP_CHAR *kpcsource 
						    );

/*
This API is used to Concatenate two strings
*/

NAP_CHAR *FWK_Strcat ( 
							NAP_CHAR *pcDest,
							const NAP_CHAR *kpcSource 
						    );

NAP_CHAR *FWK_Strncat ( 
							NAP_CHAR *pcDest,
							const NAP_CHAR *kpcSource,
							NAP_INT16 iNumchars
						    );

/*
This API is used to extract token from String
*/

NAP_CHAR *FWK_Strtok ( 
							NAP_CHAR *pcDest,
							const NAP_CHAR *kpcSource
						    );

NAP_CHAR *FWK_Strchr ( 
							const NAP_CHAR *kpcDest,
							const NAP_CHAR ucChar 
						    );
/*
This API is used to allocate memory and string copy.
*/
#ifdef POC_MEM_DEBUG
#define	FWK_Strdup(a)	FWK_Strdup_Debug(a, __FILE__, __LINE__)
NAP_CHAR *FWK_Strdup_Debug ( 
							const NAP_CHAR *kpcSource,
							char*	file_name,
							int		line_num
						    );
#else

NAP_CHAR *FWK_Strdup ( const NAP_CHAR *kpcSource);

#endif /* End of POC_MEM_DEBUG. */

/*
This API is used to convert String to intiger
*/
NAP_INT32 FWK_Atoi (
							const NAP_CHAR *kpcSource
					      );

/*
This API is used to convert integer to characters in String
*/
NAP_CHAR* FWK_Itoa (
							NAP_INT32 iNum,
							NAP_CHAR *pcDest,
							NAP_UCHAR ucRadix
					     );

NAP_INT16 FWK_Isspace( 
								NAP_UINT16 usValue 
						    );

NAP_INT16 FWK_Strncmp ( 
							const NAP_CHAR *kpcSource,
							const NAP_CHAR *kpcDest,
							NAP_UINT16 uiSize
					        );
					        

#ifdef __cplusplus
}
#endif
					        

#endif // End of __FWK_STRING_H__
