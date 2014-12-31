
#ifndef _NAP_DATATYPES_H_
#define _NAP_DATATYPES_H_

#include <stdio.h>



#define NAP_NULL       ((void*)0)
#define NAP_TRUE	1
#define NAP_FALSE	0
#define RTRT_MEM_EN 1
#define NAP_ENUM			enum

/* Data Types */

typedef   void              NAP_VOID;
typedef   unsigned char     NAP_UCHAR;
typedef   char              NAP_CHAR;
typedef   unsigned char     NAP_UINT8;
typedef   char              NAP_INT8;
typedef   unsigned short    NAP_UINT16;
typedef   short             NAP_INT16;
typedef   unsigned int      NAP_UINT32;
typedef   int               NAP_INT32;

typedef   unsigned long long	NAP_UINT64;
typedef	  signed long long		NAP_INT64;

typedef unsigned char	NAP_UBYTE;
typedef signed   char	NAP_BYTE;
typedef void 			NAP_VOID;
typedef NAP_VOID  		NAP_FILE;

/* Function parameter notation */
#ifndef IN
	#define IN
#endif
#ifndef OUT
	#define OUT
#endif
#ifndef IN_OUT
	#define IN_OUT
#endif

typedef enum _NAP_BOOL
{
	NAP_FAILURE=0,
	NAP_SUCCESS=1

}NAP_BOOL;


#endif //_NAP_DATATYPES_H_
