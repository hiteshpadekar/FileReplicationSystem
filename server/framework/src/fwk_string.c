
#include "fwk_string.h"
#include "fwk_memory.h"

/*
*******************************************************************************
* Function name   : FWK_Strlen
* Description     : This function is used to get length of the string in bytes
* Return type     : NAP_INT16 : length in bytes..
* Arguments       : [IN] kpcsource   : Pointer to string  
* Side Effect     : None 
* NOTE            : None
*******************************************************************************
*/

NAP_UINT32 FWK_Strlen ( 
								const NAP_CHAR *kpcSource 
						    )

{
	return strlen(kpcSource);
}//End of FWK_Strlen ;


/*
*******************************************************************************
* Function name   : FWK_Strcpy
* Description     : This function is used to copy the source string to 
                    Destination String, including '\0'. 
* Return type     : pointer to the Destination String.
* Arguments       : [IN] pcDest   : Pointer to Destination String  
*					[IN] kpcsource: Constant Pointer to Source String.
* Side Effect     : None 
* NOTE            : None
*******************************************************************************
*/


NAP_CHAR *FWK_Strcpy ( 
							NAP_CHAR *pcDest,
							const NAP_CHAR *kpcSource 
						    )
{
	return strcpy(pcDest,kpcSource);
}//End of FWK_Strcpy ;

/*
*******************************************************************************
* Function name   : FWK_Strcmp
* Description     : This function is used to Compare Source String with 
                    Target String. 
* Return type     : NAP_INT16.
* Arguments       : [IN] kpcSource : Constant Pointer to Source String  
*					[IN] kpcDest   : Constant Pointer to destination String.
* Side Effect     : None 
* NOTE            : None
*******************************************************************************
*/

NAP_INT16 FWK_Strcmp ( 
							const NAP_CHAR *s1,
							const NAP_CHAR *s2
					        )
{
    char c1, c2;

    while (1) {
        c1 = *s1++;
        c2 = *s2++;
        if (c1 >= 'a' && c1 <= 'z')
            c1 += 'A' - 'a';
        if (c2 >= 'a' && c2 <= 'z')
            c2 += 'A' - 'a';
        if (c1 != c2)
            return (c1 - c2);
        if (c1 == 0)
            return 0;
    }
}//End of FWK_Strcmp ;

/*
*******************************************************************************
* Function name   : FWK_Strcasecmp
* Description     : This function is used to Case Compare Source String with 
                    Target String. 
* Return type     : NAP_INT16.
* Arguments       : [IN] kpcSource : Constant Pointer to Source String  
*					[IN] kpcDest   : Constant Pointer to destination String.
* Side Effect     : None 
* NOTE            : None
*******************************************************************************
*/

NAP_INT16 FWK_Strcasecmp ( 
							const NAP_CHAR *s1,
							const NAP_CHAR *s2
					        )
{
    char c1, c2;

    while (1) {
        c1 = *s1++;
        c2 = *s2++;
        if (c1 >= 'a' && c1 <= 'z')
            c1 += 'A' - 'a';
        if (c2 >= 'a' && c2 <= 'z')
            c2 += 'A' - 'a';
        if (c1 != c2)
            return (c1 - c2);
        if (c1 == 0)
            return 0;
    }
}//End of FWK_Strcasecmp ;


/*
*******************************************************************************
* Function name   : FWK_Strncpy
* Description     : This function is used to copy at most iNumchars characters
*					of string kpcSource to string  kpcDest and terminates
*					kpcDest with '/0' 
                    of Source String with Destination String. 
* Return type     : Pointer to  string: return kpcDest
* Arguments       : [IN] kpcDest   : Constant Pointer to Destination String  
*					[IN] kpcsource : Constant Pointer to source String.
*                 : [IN] iNumchars : No. of Characters to be Compared.
* Side Effect     : None 
* NOTE            : None
*******************************************************************************
*/

NAP_CHAR *FWK_Strncpy ( 
								NAP_CHAR *pcDest, 
								const NAP_CHAR *kpcSource, 
								NAP_INT16 iNumchars 
							 )
{
	return strncpy(pcDest,kpcSource,iNumchars);

}//End of FWK_Strncpy ;

/*
*******************************************************************************
* Function name   : NAP_FWK_OS_StrStr
* Description     : This function is used to find the first occurance of 
*					the string kpcSource in the string  kpcDest. 
* Return type     : Pointer to First occurence of Source String 
*                   in Destination String( or NULL.
* Arguments       : [IN] kpcDest   : Constant Pointer to destination String  
*					[IN] kpcSource : String to search.
* Side Effect     : None 
* NOTE            : None
*******************************************************************************
*/

NAP_CHAR *FWK_Strstr ( 
							const NAP_CHAR *kpcDest,
							const NAP_CHAR *kpcSource 
						    )
{
	return strstr(kpcDest,kpcSource);
}//End of FWK_Strstr ;

/*
*******************************************************************************
* Function name   : FWK_Strcat
* Description     : This function is used to Concatenate Source String
*                   to end of destination String. 
* Return type     : Pointer to Destination String.
* Arguments       : [IN] pcDest   : Pointer to Destination String  
*					[IN] kpcSource: Constant Pointer to source String.
* Side Effect     : None 
* NOTE            : None
*******************************************************************************
*/

NAP_CHAR *FWK_Strcat ( 
							NAP_CHAR *pcDest,
							const NAP_CHAR *kpcSource 
						    )
{
	return strcat(pcDest,kpcSource);
}//End of FWK_Strcat;

NAP_CHAR *FWK_Strncat ( 
							NAP_CHAR *pcDest,
							const NAP_CHAR *kpcSource,
							NAP_INT16 iNumchars
						    )
{
	return strncat(pcDest,kpcSource, iNumchars);
}//End of NAP_FWK_OS
/*
*******************************************************************************
* Function name   : FWK_Strtok.
* Description     : This function is used to search destination String 
*                   for tokens delimited by characters from Source String
* Return type     : Pointer to String or NULL.
* Arguments       : [IN] pcDest   : Constant Pointer to Destination String  
*					[IN] kpcsource: Constant Pointer to source String.
* Side Effect     : None 
* NOTE            : None
*******************************************************************************
*/

NAP_CHAR *FWK_Strtok ( 
							NAP_CHAR *pcDest,
							const NAP_CHAR *kpcSource
						    )
{
	return strtok(pcDest,kpcSource);
}// End of FWK_Strtok;
 
/*
*******************************************************************************
* Function name   : FWK_Strdup.
* Description     : This function is used to allocate memory for the string
*                   and copy the source string.
* Return type     : Pointer to String or NULL.
* Arguments       : [IN] kpcsource: Constant Pointer to source String.
* Side Effect     : None 
* NOTE            : None
*******************************************************************************
*/
#ifdef NAP_MEM_DEBUG
NAP_CHAR *FWK_Strdup_Debug ( 
							const NAP_CHAR *kpcSource,
							char*	file_name,
							int		line_num
						    )
{
	NAP_CHAR* pcReturn = NAP_NULL;
	if (kpcSource == NAP_NULL)
		return NAP_NULL;
	pcReturn = (NAP_CHAR*) FWK_Malloc_Debug((NAP_UINT16)(strlen(kpcSource)+1), file_name, line_num);
	if (pcReturn == NAP_NULL)
		return NAP_NULL;
	else
		return strcpy(pcReturn, kpcSource);
}
#else

NAP_CHAR *FWK_Strdup ( 
							const NAP_CHAR *kpcSource
						    )
{
	NAP_CHAR* pcReturn = NAP_NULL;
	if (kpcSource == NAP_NULL)
		return NAP_NULL;
	
	pcReturn = (NAP_CHAR*) FWK_Malloc((NAP_UINT32)(strlen(kpcSource)+1));
	if (pcReturn == NAP_NULL)
		return NAP_NULL;
	else
		return strcpy(pcReturn, kpcSource);
}
#endif

/*
*******************************************************************************
* Function name   : FWK_Strchr
* Description     : This function is used to find the first occurance of 
*					the character  ucChar in the string  kpcDest. 
* Return type     : Pointer to First occurence of ucChar 
*                   in Destination String  or NULL.
* Arguments       : [IN] kpcDest: Constant Pointer to destination String  
*					[IN] ucChar : Character to search for
* Side Effect     : None 
* NOTE            : None
*******************************************************************************
*/

NAP_CHAR *FWK_Strchr ( 
							const NAP_CHAR *kpcDest,
							const NAP_CHAR ucChar 
						    )
{
	return strchr(kpcDest,ucChar);
}//End of FWK_Strchr ;

 
/*
*******************************************************************************
* Function name   : FWK_Atoi.
* Description     : This function is used to  convert Source String
*                   to its integer equvalent.
* Return type     : Integer.
* Arguments       : [IN] kpcSource : Constant Pointer to Source String  
* Side Effect     : None 
* NOTE            : None
*******************************************************************************
*/

NAP_INT32 FWK_Atoi (
							const NAP_CHAR *kpcSource
						  )
{
	return atoi(kpcSource);

}//End of FWK_Atoi;

/*
*******************************************************************************
* Function name   : FWK_Itoa.
* Description     : This function is used to  convert Integer to 
*                   characters of String.
* Return type     : pointer to String.
* Arguments       : [IN] iNum     : Number to be converted.
                    [IN] cpDest   : Pointer to String  
* Side Effect     : None 
* NOTE            : None
*******************************************************************************
*/

NAP_CHAR* FWK_Itoa (
							NAP_INT32 iNum,
							NAP_CHAR *pcDest,
							NAP_UCHAR ucRadix
					     )
{
    if(ucRadix == 10)
        snprintf(pcDest, sizeof(pcDest), "%d", iNum);
    if(ucRadix == 16)
        snprintf(pcDest, sizeof(pcDest), "%x", iNum);

    return pcDest;
}//End of FWK_Itoa.

/*
*******************************************************************************
* Function name   : FWK_Isspace.
* Description     : This function check the space value and return zero if
*					value is not a space. It checks  SP, \t, \r, \n characters.
*                   characters of String.
* Return type     : NAP_INT16 : non zero is not a space
* Arguments       : [IN] usValue     : Value to be chaecked for space value.
* Side Effect     : None 
* NOTE            : None
*******************************************************************************
*/
NAP_INT16 FWK_Isspace( 
								NAP_UINT16 usValue 
						    )
{

	return isspace(usValue);
}

/*
*******************************************************************************
* Function name   : FWK_Strncmp.
* Description     : This function compares the both passed strings of specified
                    number of characters.
* Return type     : NAP_INT16 : non zero is not a space
* Arguments       : [IN] usValue     : Value to be chaecked for space value.
* Side Effect     : None 
* NOTE            : None
*******************************************************************************
*/

NAP_INT16 FWK_Strncmp ( 
							const NAP_CHAR *kpcSource,
							const NAP_CHAR *kpcDest,
							NAP_UINT16 uiSize
					        )

{
	
	return strncmp (kpcSource,kpcDest, uiSize);
}
