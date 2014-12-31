
#ifndef __FWK_MEMORY_H__
#define __FWK_MEMORY_H__

#if defined _DEBUG && FWK_DETECT_MEMORY_ERRORS
#include <crtdbg.h>
#endif

#include "fwk_datatypes.h"
#include <memory.h>
#include <malloc.h>

/* The default size of most of the machines default is unsigned int */
typedef size_t FWK_SIZE_T;


/*This API  is used to allocate memory*/
#ifdef NAP_MEM_DEBUG

#define		FWK_Malloc(a)	FWK_Malloc_Debug(a, __FILE__, __LINE__)
#define		FWK_CleanMalloc(a)	NAP_FWK_OS_CleanMalloc_Debug(a, __FILE__, __LINE__)
#define		FWK_Free(a)		FWK_Free_Debug(a, __FILE__, __LINE__)

NAP_BOOL 	FWK_MEM_Init 
(
	NAP_INT16		*piError
);

NAP_VOID 	FWK_MEM_DeInit
(
	NAP_VOID
);

NAP_VOID* FWK_Malloc_Debug(
							FWK_SIZE_T sztMemBlockSize,
							char*	file,
							int		line
						  );
NAP_VOID* FWK_CleanMalloc_Debug(
							FWK_SIZE_T sztMemBlockSize,
							char*	file,
							int		line
						  );
/*This API is used to free memory block*/
NAP_VOID FWK_Free_Debug(
							NAP_VOID **pvMem_ptr,
							char*	file,
							int		line
						  );

#else

#ifdef FWK_DETECT_MEMORY_ERRORS
/* Added for RTRT. For Memory Profiling */
#define FWK_Malloc(sztMemBlockSize)	_malloc_dbg((sztMemBlockSize), _NORMAL_BLOCK, __FILE__, __LINE__ )

/* Added for RTRT. For Memory Profiling */
#define FWK_Free(ppMemPtr)\
{\
    if(ppMemPtr != NAP_NULL) \
	{\
		if(*ppMemPtr != NAP_NULL) \
		{\
			_free_dbg(*ppMemPtr,_NORMAL_BLOCK);\
			*ppMemPtr = NAP_NULL; \
		}\
	}\
}

#else

#define FWK_Malloc(sztMemBlockSize)	malloc(sztMemBlockSize)
/* Added for RTRT. For Memory Profiling */
#define FWK_Free(ppMemPtr)\
{\
    if(ppMemPtr != NAP_NULL) \
	{\
		if(*ppMemPtr != NAP_NULL) \
		{\
			free(*ppMemPtr);\
			*ppMemPtr = NAP_NULL; \
		}\
	}\
}

#endif
//NAP_VOID* FWK_Malloc(NAP_UINT16 usMemBlockSize);

NAP_VOID* FWK_CleanMalloc(FWK_SIZE_T sztMemBlockSize);


//NAP_VOID FWK_Free(NAP_VOID **pvMem_ptr);
#endif

/*This API is used to set memory with values*/
NAP_VOID FWK_Memset(
							NAP_VOID *pvMem_blocks , 
							NAP_UCHAR uChars       , 
							FWK_SIZE_T sztSize
						  );

/*This API is used to copy block of memory*/
NAP_VOID FWK_Memcpy(
						    NAP_VOID *pvMem_Destinaiton ,
							const NAP_VOID *pvMem_Source,
							FWK_SIZE_T sztSize
						  );
						  
/*This API is used to compare block of memory*/
NAP_UINT32 FWK_Memcmp(
						    const NAP_VOID *pvMem1 ,
							const NAP_VOID *pvMem2 ,
							FWK_SIZE_T sztSize
						  );
						  
/*This API is used to reallocate memory*/
NAP_VOID FWK_Realloc(
							 NAP_VOID *pvMem_ptr   ,
							 FWK_SIZE_T sztNew_Size 
						   );
 
#endif // End of __FWK_MEMORY_H__
