
/*
*******************************************************************************
*                               INCLUDE FILES
*******************************************************************************
*/
#include"fwk_file.h"
#include"fwk_string.h"


NAP_CHAR gacLogDirInitTime[128]= "";
NAP_CHAR gstrLogDirPath[128] = {0};
/******************************************************************************
* Function name	: FWK_OpenFile
* Description	: Opens a file with a file name in a particular mode
* Return type	: NAP_BOOL
* Argument		:[IN] pcName    :Name of the file to be opened
*				 [IN] pcMode    :Mode of opening the file
*	             [OUT]ppFileName:Pointer to file descriptor pointer
*				  
* Side Effects	: None
* NOTE 			: None
******************************************************************************/
NAP_BOOL FWK_OpenFile(
							NAP_CHAR *pcName,
							NAP_CHAR *pcMode,
							NAP_FILE **ppFileName //OUT
						  )
{

	NAP_FILE *pst = NAP_NULL;
	pst = fopen(pcName,pcMode);
	if(NAP_NULL == pst)
		return NAP_FAILURE;
	else
	{
		*ppFileName = pst;
		return NAP_SUCCESS;
	}

}

/******************************************************************************
* Function name	: FWK_FilePuts
* Description	: Stores messages into a particular file
* Return type	: Boolean - NAP_SUCCESS indicates successfully initialized.
* Argument		: [IN] pMsg    :Message to be written into the file
*				  [IN] pFile   :File descriptor pointer	
*				  
* Side Effects	: None
* NOTE 			: None
******************************************************************************/
NAP_BOOL FWK_FilePuts(
						 const NAP_CHAR *pMsg,
						 NAP_FILE *pFile
						)
{
	fputs(pMsg,pFile);
	return NAP_SUCCESS;
}


NAP_BOOL FWK_CloseFile(
						 NAP_FILE *pFile
						)
{
	fclose(pFile);
	return NAP_SUCCESS;
}
/*
	Description: Creates the Date (yyyymmdd_time) at the input path
*/
NAP_BOOL	FWK_CreateLogDateFolder 
( 
	NAP_CHAR *pInputPath,
	NAP_CHAR *pOutPath 
)
{
	NAP_CHAR strPath[256] = {0};
	NAP_UINT16 usPathLen = 0;
	time_t raw_time;
        struct tm *lSystemTime;
	
	FWK_Strcpy ( strPath, pInputPath );
	FWK_Strcat ( strPath, "\\" );
	usPathLen = FWK_Strlen ( strPath );
	
        time(&raw_time);
        lSystemTime = localtime(&raw_time);

	FWK_Sprintf(gacLogDirInitTime,"%d%.2d%.2d_%.2d%.2d%.2d", 
                   lSystemTime->tm_year,
		   lSystemTime->tm_mon,
		   lSystemTime->tm_wday,
		   lSystemTime->tm_hour,
		   lSystemTime->tm_min,
		   lSystemTime->tm_sec);


	FWK_Sprintf (	strPath+usPathLen, 
					"%d%.2d%.2d_%.2d%.2d%.2d",
                   lSystemTime->tm_year,
		   lSystemTime->tm_mon,
		   lSystemTime->tm_wday,
		   lSystemTime->tm_hour,
		   lSystemTime->tm_min,
		   lSystemTime->tm_sec);


	CreateDirectory ( strPath, NULL );
	FWK_Strcpy ( pOutPath, strPath );

	return NAP_SUCCESS;
}
