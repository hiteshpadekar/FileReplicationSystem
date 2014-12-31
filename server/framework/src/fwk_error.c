
#include "fwk_error.h"
#include "fwk_string.h"
#include "fwk_file.h"
#include <time.h>

/*
*******************************************************************************
*                               GLOBAL VARIABLES
*******************************************************************************
*/
//NAP_CHAR gacErrorFileName[]="nap_error.txt";
NAP_CHAR gacErrorFileName[128]="";
NAP_FILE *pErrorFile=NAP_NULL;

extern NAP_CHAR gacSoftVersion[];
/* 
* This mutex is being used for synchronize writing to Error file 
* -Need of Mutex:when multiple threads going to write trace 
*  into same shared log file
*/
#ifdef NAP_ENV_WINDOWS
	HANDLE		gErrorFileMutex;
#endif

#define MAX_ERROR_FILE_SIZE	5242880 //=5 Mb

static NAP_UINT32	guiCountErrorNo = 0;
extern NAP_CHAR		gstrLogDirPath[128];
extern NAP_CHAR		gacLogDirInitTime[128];

void print_time1(void)
{
	time_t raw_time;
	struct tm *lSystemTime;

	time(&raw_time);
	lSystemTime = localtime(&raw_time);

	FWK_Fprintf(pErrorFile,"\n\n[%d/%d/%d][%d:%d:%d]", 
		       lSystemTime->tm_year, 
		       lSystemTime->tm_mon, 
			   lSystemTime->tm_wday, 
			   lSystemTime->tm_hour, 
			   lSystemTime->tm_min, 
			   lSystemTime->tm_sec);
}

void FWK_ERROR_LOG
(
	 NAP_INT16 errorCode,
	 NAP_CHAR *pcTrace,
	 NAP_CHAR *pFile, 
	 NAP_INT32 iLine
)
{
#ifdef FWK_ERROR_TO_CONSOLE
	if(NAP_NULL != pString)											
	{																
		if (module & guiModulesToTrace)								
		{	
			FWK_Printf ("[LGSI NAP] <%s: %i> :: %s", pFile, iLine,pcTrace);
		}
	}
#endif // FWK_ERROR_TO_CONSOLE

#ifdef FWK_ERROR_TO_FILESYSTEM
	if(NAP_NULL != pcTrace)											
	{
		NAP_UCHAR	tBuf[10];
		NAP_UINT64 uiFileSize = 0;
		/* Allocation of Mutex gErrorFileMutex */
	#ifdef NAP_ENV_WINDOWS
			WaitForSingleObject(gErrorFileMutex,INFINITE);    
	#endif
		fseek(pErrorFile, 0, SEEK_END);
		uiFileSize = ftell(pErrorFile); 

		if(uiFileSize > MAX_ERROR_FILE_SIZE)
		{
			FWK_FilePuts("\nFile size exceded max file size limit so creating new Error file\n", pErrorFile);

			FWK_CloseFile(pErrorFile);
			pErrorFile = NAP_NULL;
			guiCountErrorNo++;
	
			FWK_Sprintf(gacErrorFileName,"%s\\%s_%.3d_AS_Error.txt", 
				gstrLogDirPath,
				gacLogDirInitTime, 
				guiCountErrorNo);

			FWK_OpenFile(gacErrorFileName,"w",&pErrorFile);
			if(NAP_NULL==pErrorFile)
			{
				// *piError = e_Err_Trace_FileCreateFail;
			#ifdef NAP_ENV_WINDOWS
				ReleaseMutex(gErrorFileMutex);    
			#endif	
				return ;
			}
			FWK_Printf("\nError File Name :: %s",gacErrorFileName);

			FWK_FilePuts("\n\n\n ",pErrorFile);
			FWK_FilePuts("******** LGSI-NAP AS ERROR LOG ***************  ",pErrorFile);
			
			/* To print Software version */
			FWK_Fprintf (pErrorFile,"\n\nSoftware Version:%s\n",gacSoftVersion);
			FWK_Fprintf (pErrorFile,"-------------------------------------------------------\n");
			FWK_FilePuts("\n\nSTRATED-ON",pErrorFile);
			print_time1();

			FWK_FilePuts("\n ",pErrorFile);

			print_time1();
		FWK_Itoa (errorCode,tBuf,10);
		FWK_Fprintf (pErrorFile,"[LGSI NAP-AS] <%s: %i> :: [%s]%s", pFile, iLine,tBuf,pcTrace);
		FWK_Fflush (pErrorFile);
		}
		else
		{
			print_time1();

			FWK_Itoa (errorCode,tBuf,10);
			FWK_Fprintf (pErrorFile,"[LGSI NAP-AS] <%s: %i> :: [%s]%s", pFile, iLine,tBuf,pcTrace);
			FWK_Fflush (pErrorFile);
		}

		#ifdef NAP_ENV_WINDOWS
			ReleaseMutex(gErrorFileMutex);    
		#endif	

	}
#endif // FWK_ERROR_TO_FILESYSTEM

#ifdef FWK_ERROR_TO_TARGET

#endif // FWK_ERROR_TO_TARGET
}

/******************************************************************************
* Function name	: FWK_ERROR_Init
* Description		: Initialize function will set the error module for tracing 
*				  logs.In case of tracing into file system, file will be 
*				  created & closed. 
* Return type		: PNAP_BOOL
* Argument		:[IN] ulModuleTrace:One of the enum values of E_TRACE_MODULE or                                     
*				 combination of them.
*				 [OUT]piError      :Trace Error Code 
*
* Side Effects	: 
* NOTE 			: 
******************************************************************************/

NAP_BOOL 	FWK_ERROR_Init 
(
	NAP_INT16		*piError
)
{
	#ifdef FWK_ERROR_TO_TARGET
		// Open a connection to UART for writing logs. 
		//IF connection established THEN
			//RETURN TRUE
		//ELSE
			// Store reason for failure in piError string.
			//RETURN FALSE
	#endif // FWK_TRACE_TO_TARGET

	#ifdef FWK_ERROR_TO_FILESYSTEM

	#ifdef NAP_ENV_WINDOWS
		/* To Protect Trace Files */
		gErrorFileMutex = CreateMutex(
								NULL,	/* No Secrity attributes */
								FALSE,  /* Initially Not Owned   */
								NULL    /* UnNamed of the Mutex  */
							 );

		if(gErrorFileMutex==NULL)
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

		FWK_Sprintf(gacErrorFileName,"%s\\%s_%.3d_AS_Error.txt", 
				gstrLogDirPath,
				gacLogDirInitTime, 
				guiCountErrorNo);

		FWK_OpenFile(gacErrorFileName,"w",&pErrorFile);
		if(NAP_NULL==pErrorFile)
		{
			// *piError = e_Err_Trace_FileCreateFail;
			return NAP_SUCCESS;
		}
		FWK_Printf("\nError File Name :: %s",gacErrorFileName);

		FWK_FilePuts("\n\n\n ",pErrorFile);
		FWK_FilePuts("******** LGSI-NAP AS ERROR LOG ***************  ",pErrorFile);
		
		/* To print Software version */
		FWK_Fprintf (pErrorFile,"\n\nSoftware Version:%s\n",gacSoftVersion);
		FWK_Fprintf (pErrorFile,"-------------------------------------------------------\n");
		FWK_FilePuts("\n\nSTRATED-ON",pErrorFile);
		print_time1();

		FWK_FilePuts("\n ",pErrorFile);
		
	#endif
	
	return NAP_SUCCESS;
}

NAP_VOID 	FWK_ERROR_DeInit 
(
	NAP_VOID
)
{
	#ifdef FWK_ERROR_TO_FILESYSTEM		
		FWK_CloseFile(pErrorFile);

	#ifdef NAP_ENV_WINDOWS
		/* Framework Trace Mutex */
		CloseHandle(gErrorFileMutex);
	#endif
	#endif	
	return;
}

