
/*
*******************************************************************************
*                               INCLUDE FILES
*******************************************************************************
*/
#include "fwk_trace.h"
#include "fwk_string.h"
#include "fwk_file.h"

#ifdef FWK_TRACE_TO_LOGGER
#include "fwk_structures.h"
#include "fwk_memory.h"
#include "fwk_rtos.h"
extern NAP_INT32 uiQpfThreadId;
#endif
/*
*******************************************************************************
*                               GLOBAL VARIABLES
*******************************************************************************
*/
#define MAX_TRACE_FILE_SIZE		5242880 //=5 Mb
NAP_UINT32 	guiModulesToTrace = (NAP_UINT32)0x00000000;

//NAP_CHAR gacTraceFileName[]= "nap_trace.txt";
NAP_CHAR gacTraceFileName[128]= "";

NAP_CHAR gacSoftVersion[]= "BL_NAPSM_RCS_IM_CRBug_Fixes_Intermediate_Rel_02_11_2011";
extern NAP_CHAR gacLogDirInitTime[128];
NAP_FILE *pTraceFile=NAP_NULL;

/* 
* This mutex is being used for synchronize writing to Trace file 
* -Need of Mutex:when multiple threads going to write trace 
*  into same shared log file
*/
#ifdef NAP_ENV_WINDOWS
	HANDLE		gTraceFileMutex;
#endif

static NAP_UINT32	guiCountTraceNo = 0;
extern NAP_CHAR		gstrLogDirPath[128];

void print_time(void)
{
        time_t raw_time;
        struct tm *lSystemTime;

        time(&raw_time);
        lSystemTime = localtime(&raw_time);


	FWK_Fprintf(pTraceFile,"\n\n[%d/%d/%d][%d:%d:%d]", 
                   lSystemTime->tm_year,
                   lSystemTime->tm_mon,
                   lSystemTime->tm_wday,
                   lSystemTime->tm_hour,
                   lSystemTime->tm_min,
                   lSystemTime->tm_sec);

}

/******************************************************************************
* Function name	: FWK_TRACE_INIT
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

NAP_BOOL 	FWK_TRACE_Init 
(
	NAP_UINT32 		ulModuleTrace,
	NAP_INT16		*piError
)
{
	#ifdef FWK_TRACE_TO_TARGET
		// Open a connection to UART for writing logs. 
		//IF connection established THEN
			//RETURN TRUE
		//ELSE
			// Store reason for failure in piError string.
			//RETURN FALSE
	#endif // FWK_TRACE_TO_TARGET

	#ifdef FWK_TRACE_TO_FILESYSTEM

	#ifdef NAP_ENV_WINDOWS
		/* To Protect Trace Files */
		gTraceFileMutex = CreateMutex(
								NULL,	/* No Secrity attributes */
								FALSE,  /* Initially Not Owned   */
								NULL    /* UnNamed of the Mutex  */
							 );

		if(gTraceFileMutex==NULL)
		{
			printf("Failed To Create Trace File Mutex Handle \n");
			return NAP_FAILURE;
		}
	#endif
		if((FWK_Strcmp(gstrLogDirPath,"") == NAP_FALSE))
		{
			if ( NAP_FAILURE == FWK_CreateLogDateFolder ( STR_LOG_FOLDER_PATH, 
								&gstrLogDirPath[0] ) )
			{
				return NAP_FAILURE;
			}

		}
		FWK_Sprintf(gacTraceFileName,"%s\\%s_%.3d_AS_Trace.txt", 
				gstrLogDirPath,
				gacLogDirInitTime, 
				guiCountTraceNo);
		
		FWK_OpenFile(gacTraceFileName,"a+",&pTraceFile);
		if(NAP_NULL==pTraceFile)
		{
			// *piError = e_Err_Trace_FileCreateFail;
			return NAP_SUCCESS;
		}
		FWK_Printf("\nTrace File Name :: %s",gacTraceFileName);

		FWK_FilePuts("\n\n\n ",pTraceFile);

		FWK_FilePuts("******** LGSI-NAP AS TRACE LOG ***************  ",pTraceFile);
		
		/* To print Software version */
		FWK_Fprintf (pTraceFile,"\n\nSoftware Version:%s\n",gacSoftVersion);
		FWK_Fprintf (pTraceFile,"-------------------------------------------------------\n");
		FWK_FilePuts("\n\nSTRATED-ON",pTraceFile);
		print_time();

		FWK_FilePuts("\n ",pTraceFile);

	#endif
	
	guiModulesToTrace = ulModuleTrace;
	
	return NAP_SUCCESS;
}
#ifdef FWK_TRACE_TO_LOGGER
E_FWK_MODULE GetFwkModule (NAP_UINT32 eTraceModule)
{
	switch (eTraceModule)
	{
	case eTrace_SimPfSigServer:
		return e_SimSigPFModule;
	case eTrace_SimCfSigServer:
		return e_SimSigCFModule;
	case eTrace_SimDmfSigServer:
		return e_SimSigDMFModule;

	case eTrace_SimChfSigServer:
		return e_SimSigCFModule;
	default:
		return e_FwkModuleEnd;
	}

	return e_FwkModuleEnd;
}
#endif
void FWK_TRACE_LOG(NAP_UINT32 module,NAP_CHAR *pString,NAP_CHAR *pFile,NAP_INT32 iLine)
{

#ifdef FWK_TRACE_TO_LOGGER
	S_FWK_IMC_INFO *pstImcInfo = 0;
	NAP_INT16 sError = 0;
#endif
#ifdef FWK_TRACE_TO_CONSOLE
	if(NAP_NULL != pString)											
	{																
		if (module & guiModulesToTrace)								
		{	
			FWK_Printf ("[NAP SIP Core] <%s: %i> :: %s", pFile, iLine,pString);
		}
	}
#endif // FWK_TRACE_TO_CONSOLE

#ifdef FWK_TRACE_TO_FILESYSTEM
	if(NAP_NULL != pString)											
	{																
		if (module & guiModulesToTrace)								
		{	
			if(pTraceFile)
			{
				NAP_UINT64 uiFileSize = 0;

			/* Allocation of Mutex gTraceFileMutex */
			#ifdef NAP_ENV_WINDOWS
   				WaitForSingleObject(gTraceFileMutex,INFINITE);    
			#endif

				fseek(pTraceFile, 0, SEEK_END);
				uiFileSize = ftell(pTraceFile); 

				if(uiFileSize > MAX_TRACE_FILE_SIZE)
				{
					FWK_FilePuts("\nFile size exceded max file size limit so creating new trace file\n", pTraceFile);

					FWK_CloseFile(pTraceFile);
					pTraceFile = NAP_NULL;

					guiCountTraceNo++;

					FWK_Sprintf(gacTraceFileName,"%s\\%s_%.3d_AS_Trace.txt", 
							gstrLogDirPath,
							gacLogDirInitTime, 
							guiCountTraceNo);
					
					FWK_OpenFile(gacTraceFileName,"a+",&pTraceFile);
					if(NAP_NULL==pTraceFile)
					{
						// *piError = e_Err_Trace_FileCreateFail;
				#ifdef NAP_ENV_WINDOWS
   						ReleaseMutex(gTraceFileMutex);    
				#endif	
						return ;
					}
					FWK_Printf("\nTrace File Name :: %s",gacTraceFileName);

					FWK_FilePuts("\n\n\n ",pTraceFile);

					FWK_FilePuts("******** LGSI-NAP AS TRACE LOG ***************  ",pTraceFile);
					
					/* To print Software version */
					FWK_Fprintf (pTraceFile,"\n\nSoftware Version:%s\n",gacSoftVersion);
					FWK_Fprintf (pTraceFile,"-------------------------------------------------------\n");
					FWK_FilePuts("\n\nSTRATED-ON",pTraceFile);
					print_time();

					FWK_FilePuts("\n ",pTraceFile);

					print_time();
					FWK_Fprintf (pTraceFile,"[NAP AS] <%s: %i> :: \n",pFile, iLine);
					FWK_Fprintf (pTraceFile,"%s",pString);
					FWK_Fflush (pTraceFile);
				}
				else
				{
					print_time();
					FWK_Fprintf (pTraceFile,"[NAP AS] <%s: %i> :: \n",pFile, iLine);
					FWK_Fprintf (pTraceFile,"%s",pString);
					FWK_Fflush (pTraceFile);
				}
		/* Release of Mutex gTraceFileMutex */
		#ifdef NAP_ENV_WINDOWS
   			ReleaseMutex(gTraceFileMutex);    
		#endif

			}
		}
	}
#endif // FWK_TRACE_TO_FILESYSTEM

#ifdef FWK_TRACE_TO_TARGET

#endif // FWK_TRACE_TO_TARGET
#ifdef FWK_TRACE_TO_LOGGER

	if ((module == eTrace_Media) || ((module & guiModulesToTrace) == 0))								
	{
		return;
	}


	// Prepare IMC INFO and post it PQF thread
	pstImcInfo = (S_FWK_IMC_INFO*)FWK_Malloc ( sizeof(S_FWK_IMC_INFO) );

	if ( !pstImcInfo )
		return;
	
	pstImcInfo->srcId			= GetFwkModule(module);
	pstImcInfo->destId			= e_QPF_Log;
	pstImcInfo->actionEventId	= e_LoggerTraceEvent;
	pstImcInfo->pvData			= FWK_Strdup(pString);

	if ( !pstImcInfo->pvData )
	{
		FWK_Free ( &pstImcInfo );
		return;
	}

	FWK_IPC_Put_Data ( uiQpfThreadId,pstImcInfo, &sError );
#endif
}


void FWK_TRACE_VA_LOG(NAP_UINT32 module, NAP_CHAR *pFmtStr,...)
{
	if (module & guiModulesToTrace)
	{
		NAP_CHAR acTempBuf[FWK_TRACE_MAX_TRACE_LEN] = "";

		FWK_Va_list ap;

		FWK_Va_start(ap, pFmtStr);

		FWK_Vsprintf(acTempBuf, pFmtStr, ap);
		
		FWK_Va_end(ap);


#ifdef FWK_TRACE_TO_CONSOLE
		if(NAP_NULL != pFmtStr)											
		{																
			if (module & guiModulesToTrace)								
			{	
				FWK_Printf ("[LGSI NAP]%s", acTempBuf);
			}
		}
#endif // FWK_TRACE_TO_CONSOLE

#ifdef FWK_TRACE_TO_FILESYSTEM
		if(NAP_NULL != pFmtStr)											
		{			
		/* Allocation of Mutex gTraceFileMutex */
		#ifdef NAP_ENV_WINDOWS
			WaitForSingleObject(gTraceFileMutex,INFINITE);    
		#endif
		
			print_time();
			FWK_Fprintf (pTraceFile,"%s",acTempBuf);
			FWK_Fflush (pTraceFile);
		/* Release of Mutex gTraceFileMutex */
		#ifdef NAP_ENV_WINDOWS
   			ReleaseMutex(gTraceFileMutex);    
		#endif
		}
#endif // FWK_TRACE_TO_FILESYSTEM

#ifdef FWK_TRACE_TO_TARGET

#endif // FWK_TRACE_TO_TARGET
	}
}

NAP_VOID FWK_LOG_MESSAGE 
( 
	NAP_UINT32  module,
	NAP_CHAR	*pcMessage /*NULL Terminated*/
) 
{

#ifdef FWK_TRACE_TO_LOGGER
	S_FWK_IMC_INFO *pstImcInfo = 0;
	NAP_INT16 sError = 0;
#endif

#ifdef FWK_TRACE_TO_FILESYSTEM
#endif

#ifdef FWK_TRACE_TO_LOGGER
	if(module == eTrace_Media)
	{
		return;
	}
	pstImcInfo = (S_FWK_IMC_INFO*)FWK_Malloc ( sizeof(S_FWK_IMC_INFO) );

	if ( !pstImcInfo )
		return;
	
	pstImcInfo->srcId			= module;
	pstImcInfo->destId			= e_QPF_Log;
	pstImcInfo->actionEventId	= e_LoggerMessageEvent;
	pstImcInfo->pvData			= FWK_Strdup(pcMessage);

	if ( !pstImcInfo->pvData )
	{
		FWK_Free ( &pstImcInfo );
		return;
	}

	FWK_IPC_Put_Data ( uiQpfThreadId,pstImcInfo, &sError );
#endif
}
	
NAP_VOID 	FWK_TRACE_DeInit 
(
	NAP_VOID
)
{

	#ifdef FWK_TRACE_TO_FILESYSTEM
		
		FWK_CloseFile(pTraceFile);
	
	#ifdef NAP_ENV_WINDOWS
		/* Framework Trace Mutex */
		CloseHandle(gTraceFileMutex);
	#endif

	#endif
	
	return;
}



