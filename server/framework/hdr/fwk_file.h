
#ifndef _FWK_FILE_H_
#define _FWK_FILE_H_

#ifdef __cplusplus    // If used by C++ code, 
extern "C" {          // we need to export the C interface
#endif

#include"fwk_datatypes.h"

NAP_BOOL FWK_OpenFile(NAP_CHAR *pcName,NAP_CHAR *pcMode,NAP_FILE **pFileName);

NAP_BOOL FWK_FilePuts(const NAP_CHAR *pMsg,NAP_FILE *pFile);

NAP_BOOL FWK_CloseFile(NAP_FILE *pFile);

#define STR_LOG_FOLDER_PATH	".\\AS_Logs"

NAP_BOOL	FWK_CreateLogDateFolder 
( 
	NAP_CHAR *pInputPath,
	NAP_CHAR *pOutPath 
);
#ifdef __cplusplus
}
#endif

#endif // _FWK_FILE_H_
