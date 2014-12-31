
#include "fwk_memory.h"
#include "fwk_file.h"
#include "fwk_string.h"

NAP_CHAR gacMemFileName[]= "nap_mem_trace.txt";

NAP_FILE *pMemFile=NAP_NULL;

void print_time2(void)
{
        time_t raw_time;
        struct tm *lSystemTime;

        time(&raw_time);
        lSystemTime = localtime(&raw_time);

	FWK_Fprintf(pMemFile,"\n\n[%d/%d/%d][%d:%d:%d]",
                   lSystemTime->tm_year,
                   lSystemTime->tm_mon,
                   lSystemTime->tm_wday,
                   lSystemTime->tm_hour,
                   lSystemTime->tm_min,
                   lSystemTime->tm_sec);
}

/******************************************************************************
* Function name	: FWK_MEM_INIT
* Description	: Initialize function will set the trace module for tracing 
*				  logs.In case of tracing into file system, file will be 
*				  created & closed. 
* Return type	: NAP_BOOL 
* Argument		:[IN] ulModuleTrace:One of the enum values of E_TRACE_MODULE or                                     
*									combination of them.
*				 [OUT]piError      :Trace Error Code 
*
* Side Effects	: 
* NOTE 			:Must be used before using any other trace functions
******************************************************************************/

#ifdef NAP_MEM_DEBUG

NAP_BOOL 	FWK_MEM_Init 
(
	NAP_INT16		*piError
)
{
		
	FWK_OpenFile(gacMemFileName,"a",&pMemFile);
	if(NAP_NULL==pMemFile)
	{
		// *piError = e_Err_Trace_FileCreateFail;
		return NAP_SUCCESS;
	}
	FWK_Printf("\n\n Mem Trace File Name = %s is  opened \n",gacMemFileName);

	FWK_FilePuts("\n\n\n ",pMemFile);

	FWK_FilePuts("******** LGSI-NAP NEW MEMORY TRACE LOG ***************  ",pMemFile);
	
	print_time2();

	FWK_FilePuts("\n ",pMemFile);


	return NAP_SUCCESS;
}


void FWK_MEM_LOG(NAP_CHAR *pMemPtr, NAP_CHAR *pString,NAP_CHAR *pFile,NAP_INT32 iLine)
{
	if(NAP_NULL != pString)											
	{																
		print_time2();
		FWK_Fprintf (pMemFile,"[LGSI NAP] <%s: %i> :: %s:0x%s", pFile, iLine,pString,pMemPtr);
		FWK_Fflush (pMemFile);
	
	}
}
	
NAP_VOID 	FWK_MEM_DeInit 
(
	NAP_VOID
)
{	
	FWK_CloseFile(pMemFile);

	return;
}

#endif // #ifdef NAP_MEM_DEBUG

/******************************************************************************
* Function name   : FWK_Malloc
* Description     : This function is use to allocate block of memory
* Return type     : NAP_VOID
* Arguments       : [IN] usMemBlockSize:Size of the memory block to be allocated  
* Side Effect     : None 
* NOTE            : None
*******************************************************************************/
#ifdef NAP_MEM_DEBUG
NAP_VOID* FWK_Malloc_Debug(
							FWK_SIZE_T sztMemBlockSize,
							char*	file,
							int		line
						  )
{
#ifdef NAP_ENV_WINDOWS

	NAP_VOID  *pMem;

	NAP_UCHAR	tBuf[10];
	
	pMem =  malloc(sztMemBlockSize);

	FWK_Itoa ((NAP_INT32)pMem,tBuf,16);

	FWK_MEM_LOG(tBuf,"Memory Allocated",file,line);

	return pMem;


#endif
#ifdef NAP_ENV_REX
	return nap_malloc(sztMemBlockSize, e_nap_framework, file, line);
#endif
}

#else

#if !RTRT_MEM_EN
NAP_VOID*	FWK_Malloc(
				FWK_SIZE_T sztMemBlockSize
							 )
{
#ifdef NAP_ENV_WINDOWS
	return malloc(sztMemBlockSize);
#endif
#ifdef NAP_ENV_REX
	return nap_malloc(sztMemBlockSize);
#endif
}// End of FWK_Malloc
#endif
#endif /* End of NAP_MEM_DEBUG. */

/******************************************************************************
* Function name   : FWK_Memset
* Description     : This function is use to set memory with new
* Return type     : NAP_VOID
* Arguments       : [IN] pvMem_block:Pointer to the memory block to be set
*					[IN] uChars:Characters to be set with
*					[IN] sztSize:Size of the charcter
* Side Effect     : None 
* NOTE            : None
*****************************************************************************/
NAP_VOID	FWK_Memset(
				NAP_VOID *pvMem_block , 
				NAP_UCHAR uChars      , 
				FWK_SIZE_T sztSize
							 )
{
	//Source Code
	memset(pvMem_block,uChars,sztSize);
}// End of FWK_Memset

/******************************************************************************
* Function name   : FWK_Malloc
* Description     : This function is use to allocate block of memory
* Return type     : NAP_VOID
* Arguments       : [IN] usMemBlockSize:Size of the memory block to be allocated  
* Side Effect     : None 
* NOTE            : None
*******************************************************************************/
#ifdef NAP_MEM_DEBUG
NAP_VOID* FWK_CleanMalloc_Debug(
							FWK_SIZE_T sztMemBlockSize,
							char*	file,
							int		line
						  )
{
	void* pvData = NAP_NULL;
#ifdef NAP_ENV_WINDOWS
	pvData = malloc(sztMemBlockSize);
#endif
#ifdef NAP_ENV_REX
	pvData = nap_malloc(sztMemBlockSize, e_nap_framework, file, line);
#endif
	if (pvData != NAP_NULL)
		memset(pvData,0,sztMemBlockSize);
	return pvData;
}

#else

NAP_VOID*	FWK_CleanMalloc(
				FWK_SIZE_T sztMemBlockSize
							 )
{
	void* pvData = NAP_NULL;
#ifdef NAP_ENV_WINDOWS
	pvData = malloc(sztMemBlockSize);
#endif
#ifdef NAP_ENV_REX
	pvData = nap_malloc(sztMemBlockSize);
#endif
	if (pvData != NAP_NULL)
		memset(pvData,0,sztMemBlockSize);
	return pvData;
}// End of FWK_Malloc

#endif /* End of NAP_MEM_DEBUG. */

/******************************************************************************
* Function name   : FWK_Memcpy
* Description     : This function is used to copy block of memeory from one
*					to another
* Return type     : NAP_VOID
* Arguments       : [IN] pvMem_Destinaiton:Pointer to destination memory block
*					[IN] pvMem_Source:Pointer to the source memory block
*					[IN] sztSize:Size of the memory block to be copied
* Side Effect     : None 
* NOTE            : None
*****************************************************************************/
NAP_VOID	FWK_Memcpy(
				NAP_VOID *pvMem_Destinaiton ,
				const NAP_VOID *pvMem_Source      ,
				FWK_SIZE_T sztSize
							 )
{
	//Source Code
	memcpy(pvMem_Destinaiton,pvMem_Source,sztSize);
}// End of FWK_Memcpy

/******************************************************************************
* Function name   : NAP_FWK_OS_Memcmp
* Description     : This function is used to compare block of memeory.
* Return type     : NAP_VOID
* Arguments       : [IN] pvMem_Destinaiton:Pointer to destination memory block
*					[IN] pvMem_Source:Pointer to the source memory block
*					[IN] sztSize:Size of the memory block to be copied
* Side Effect     : None 
* NOTE            : None
*****************************************************************************/
NAP_UINT32	FWK_Memcmp(
				const NAP_VOID *pvMem1 ,
				const NAP_VOID *pvMem2,
				FWK_SIZE_T sztSize
							 )
{
	//Source Code
	return(memcmp(pvMem1,pvMem2,sztSize));
}// End of FWK_Memcpy

/******************************************************************************
* Function name   : FWK_Free
* Description     : This function is used to free memory block
* Return type     : NAP_VOID
* Arguments       : [IN] pvMem_ptr:Pointer to the memory block to be freed
* Side Effect     : None 
* NOTE            : None
*****************************************************************************/
#ifdef NAP_MEM_DEBUG
NAP_VOID FWK_Free_Debug(
							NAP_VOID **ppvMem_ptr,
							char*	file,
							int		line
						  )
{
	//Source Code
#ifdef NAP_ENV_WINDOWS

	NAP_UCHAR	tBuf[10];

	if(NAP_NULL == *ppvMem_ptr)
		return;

	FWK_Itoa ((NAP_INT32)(*ppvMem_ptr),tBuf,16);

	FWK_MEM_LOG(tBuf,"Memory Freed",file,line);
	
	free(*ppvMem_ptr);
	*ppvMem_ptr = NAP_NULL;
#endif
	
#ifdef NAP_ENV_REX
	if(NAP_NULL == *ppvMem_ptr)
		return;

	nap_free(*ppvMem_ptr, file, line);
	*ppvMem_ptr = NAP_NULL;
#endif

}// End of FWK_Free
#else

#if !RTRT_MEM_EN
NAP_VOID	FWK_Free(
				NAP_VOID **ppvMem_ptr
						   )
{
	//Source Code
#ifdef NAP_ENV_WINDOWS
	if(NAP_NULL == ppvMem_ptr)
		return;
	if(NAP_NULL == *ppvMem_ptr)
		return;

	free(*ppvMem_ptr);
	*ppvMem_ptr = NAP_NULL;
#endif
	
#ifdef NAP_ENV_REX
	if(NAP_NULL == *ppvMem_ptr)
		return;

	nap_free(*ppvMem_ptr);
	*ppvMem_ptr = NAP_NULL;
#endif

}// End of FWK_Free
#endif
#endif /* End of NAP_MEM_DEBUG. */

/******************************************************************************
* Function name   : FWK_Realloc
* Description     : This function is used to reallocate memory block
* Return type     : NAP_VOID
* Arguments       : [IN] pvMem_ptr:Pointer to memory block to be reallocated 
*					[IN] usNew_Size :New size of memory block
* Side Effect     : None 
* NOTE            : None
*****************************************************************************/
NAP_VOID	FWK_Realloc(
				NAP_VOID *pvMem_ptr,
				FWK_SIZE_T sztNew_Size 
							  )
{
	//Source Code


}// End of FWK_Realloc

