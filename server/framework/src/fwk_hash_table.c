#include "fwk_hash_table.h"

/******************************************************************************
* Function name   : Fwk_HashInit
* Description     : This function initailizes the hash table .This function
					also allocates and initialize the hash elements.
* Return type     : NAP_BOOL
					NAP_SUCCESS on successful initialization of hash table.
					NAP_FAILURE if any error occurs during initialization of
					hash table.
* Arguments       : [IN_OUT] pstHash: Hash table instance.
					[IN] iNumBuckets:	Num of key/value pairs supported.
					[IN] iMaxElements:	Max elements supported in each
										bucket.
					[OUT] psError:	 Variable to used to indicate the error
								to the caller function. This varibale is set to
								zero if there are not errors. Otherswise this
								variable is set to appropriate error number.
* Side Effect     : None
* NOTE            : None
******************************************************************************/
NAP_BOOL Fwk_HashInit (IN_OUT S_NAP_FWK_HASHTABLE	*pstHash,
					IN	NAP_UINT32	iNumBuckets,
					IN	NAP_UINT32	iMaxElements,
					OUT	NAP_INT16	*psError)
{

	NAP_UINT32 iIndex = 0;

	/* Initialize the max number of buckets.*/
	pstHash->uiSearchBuckets = iNumBuckets;

	pstHash->uiNumOfSearchElements = 0;

	/* Initialize the max number of elements supported in each buckets.*/
	pstHash->uiMaxNumberOfElements = (iMaxElements * iNumBuckets);

	/* Initialize the hash elements */
	pstHash->ppstHashSearchChains = (S_NAP_FWK_SEARCHELEMENT **) FWK_Malloc(
							 sizeof(S_NAP_FWK_SEARCHELEMENT*)*iNumBuckets);
	
    if (pstHash->ppstHashSearchChains == NAP_NULL)
	{
		*psError = e_Err_Fwk_MallocFailed;
		return NAP_FAILURE;
	}

	for(iIndex=0; iIndex<iNumBuckets; iIndex++)
	{
		pstHash->ppstHashSearchChains[iIndex] = NAP_NULL;
	}

#ifdef DOUBLE_HASH_KEY
	/* Initialize the max number of buckets.*/
	pstHash->uiShutdownBuckets = iNumBuckets;
	pstHash->uiNumOfShutdownElements = 0;

	/* Initialize the hash elements */
	pstHash->ppstHashShotdownChains = (S_NAP_FWK_SHUTDOWNELEMENT **) FWK_Malloc(
							 sizeof(S_NAP_FWK_SHUTDOWNELEMENT*)*iNumBuckets);
	if (pstHash->ppstHashShotdownChains == NAP_NULL)
	{
		*psError = e_Err_Fwk_MallocFailed;
		return NAP_FAILURE;
	}
	for(iIndex=0; iIndex<iNumBuckets; iIndex++)
	{
		pstHash->ppstHashShotdownChains[iIndex] = NAP_NULL;
	}
#endif
	return NAP_SUCCESS;
}

/******************************************************************************
* Function name   : Fwk_HashDeInit
* Description     : This function removes all the entries from hash table.
* Return type     : NAP_BOOL

* Arguments       : [IN_OUT] pstHash: Hash table instance.
					[OUT] psError:	 Variable to used to indicate the error
							to the caller function. This varibale is set to
							zero if there are not errors. Otherswise this
							variable is set to appropriate error number.
* Side Effect     : None
* NOTE            : None
******************************************************************************/
NAP_BOOL Fwk_HashDeInit (IN_OUT S_NAP_FWK_HASHTABLE *pstHash,
						OUT	NAP_INT16	*psError)
{

	NAP_UINT32 iIndex = 0;

	for (iIndex = 0; iIndex <pstHash->uiSearchBuckets ; iIndex++)
	{
		/* Remove the entries from the seach list.*/
		if (pstHash->uiNumOfSearchElements >0)
		{
			S_NAP_FWK_SEARCHELEMENT *pstElement = pstHash->ppstHashSearchChains[iIndex];
			S_NAP_FWK_SEARCHELEMENT *pstTempElement = NAP_NULL;
			while (pstElement != NAP_NULL)
			{
				if (pstElement->pvSearchKey != NAP_NULL)
				{

					pstTempElement = pstElement;
					pstElement = pstElement->pstNextElement;
					FWK_Free(&pstTempElement->pvSearchKey);
					pstTempElement->pvElement = NAP_NULL;
					FWK_Free(&pstTempElement);

				}
			}
		}
		else
			FWK_Free(&pstHash->ppstHashSearchChains[iIndex]);
	}
#ifdef DOUBLE_HASH_KEY
	for (iIndex = 0; iIndex< pstHash->uiNumOfShutdownElements ; iIndex++)
	{
		/* Remove the entries from the shutdown list.*/
		if (pstHash->uiNumOfShutdownElements >0)
		{
			S_NAP_FWK_SHUTDOWNELEMENT *pstElement = pstHash->ppstHashShotdownChains[iIndex];
			S_NAP_FWK_SHUTDOWNELEMENT *pstTempElement = NAP_NULL;
			while (pstElement != NAP_NULL )
			{
				if (pstElement->pvShutDownKey != NAP_NULL)
				{
					pstTempElement = pstElement;
					pstElement = pstElement->pstNextElement;
					FWK_Free(&pstTempElement->pvShutDownKey);
					pstTempElement->pvElement = NAP_NULL;
					FWK_Free(&pstTempElement);
				}
			}
		}
		else
			FWK_Free(&pstHash->ppstHashShotdownChains[iIndex]);
	}
#endif
	/* Delete search and close nodes.*/
	FWK_Free(&pstHash->ppstHashSearchChains);
	FWK_Free(&pstHash->ppstHashShotdownChains);
	FWK_Memset(pstHash,0,sizeof(S_NAP_FWK_HASHTABLE));
	return NAP_SUCCESS;
}

/******************************************************************************
* Function name   : Fwk_HashKeyCompareFunction
* Description     : This function compares the 2 given hash keys.
* Return type     : NAP_BOOL
					NAP_SUCCESS on successful deletion of the strcture pointer.
					NAP_FAILURE if any error occurs during delete operation.
* Arguments       : [IN] pvKey1:
					[IN] pvKey2:
					[OUT] psError:	 Variable to used to indicate the error
								to the caller function. This varibale is set to
								zero if there are not errors. Otherswise this
								variable is set to appropriate error number.
* Side Effect     : None
* NOTE            : None
******************************************************************************/
NAP_INT32  Fwk_HashKeyCompare(IN NAP_VOID *pvKey1,
										IN NAP_VOID *pvKey2,
										OUT NAP_INT16 *psError)
{
	NAP_INT32 iResult = 0;
	iResult = FWK_Strcmp((NAP_CHAR*) pvKey1, (NAP_CHAR*) pvKey2);
	return iResult;
}


/******************************************************************************
* Function name   : Fwk_Generate_Key
*
* Description     : This function generates Hash Keys
*
* Return type     : NAP_UINT32
*					Hask Key on successful deletion of the strcture pointer.
*					NAP_FAILURE if any error occurs during delete operation
*
* Arguments       :
*					IN pvKeySrc: The String from which the Key has to be generated
*
* Side Effect     : None
*
* NOTE            : None
******************************************************************************/
NAP_UINT32
Fwk_Generate_Key(NAP_VOID *pvKeySrc)
{
	NAP_UINT32 uiKey = 0;
	NAP_UINT32 uiTempVal=0;
	NAP_INT8 *piSrc = NAP_NULL;

	if (pvKeySrc == NAP_NULL)
		return NAP_FAILURE;

	piSrc = (NAP_INT8 *)pvKeySrc;
	while ( *piSrc )
	{
		uiKey = ( uiKey << 4 ) + *piSrc++;
		if ( (uiTempVal = uiKey & 0xF0000000) )
			uiKey ^= uiTempVal >> 24;
		uiKey &= ~uiTempVal;
	}

	return uiKey;
}



/******************************************************************************
* Function name   : Fwk_HashCheck_Duplicates
* Description     : This function checks for duplicate.
* Return type     : NAP_BOOL
					NAP_SUCCESS if there no duplicate element.
					NAP_FAILURE if duplicate element is found.
* Arguments       : [IN] pstHash: Hash table instance.
					[IN] pSerachKey: Key used in NAP sessions.
					[IN] pShutdownKey: Key used for shutting down the session.
					[OUT] psError:	 Variable to used to indicate the error
								to the caller function. This varibale is set to
								zero if there are not errors. Otherswise this
								variable is set to appropriate error number.
* Side Effect     : None
* NOTE            : None
******************************************************************************/
NAP_BOOL Fwk_HashCheck_Duplicates(S_NAP_FWK_HASHTABLE *pstHash,
								NAP_UCHAR *pSerachKey,
								NAP_UCHAR *pShutdownKey,
								NAP_INT16 *psError)
{


	S_NAP_FWK_SEARCHELEMENT *pstSearchIterator = NAP_NULL;
	S_NAP_FWK_SHUTDOWNELEMENT *pstCloseIterator = NAP_NULL;
	NAP_BOOL bSearchStatus = NAP_FAILURE;
	NAP_BOOL bCloseStatus = NAP_FAILURE;
	NAP_UINT32 iHashKey =0;
	NAP_UINT32 iBucket = 0;
	NAP_UINT32 uiSearchCount =0;
	NAP_UINT32 uiCloseCount =0;

#ifdef DOUBLE_HASH_KEY
	iHashKey = Fwk_Generate_Key(pShutdownKey);
	iBucket = iHashKey % pstHash->uiShutdownBuckets;
	pstCloseIterator = pstHash->ppstHashShotdownChains[iBucket];
	while(pstCloseIterator != NAP_NULL)
	{
		if(Fwk_HashKeyCompare(pstCloseIterator->pvShutDownKey,pShutdownKey,psError) == 0)
		{
			bCloseStatus = NAP_SUCCESS;
			break;
		}
		pstCloseIterator = pstCloseIterator->pstNextElement;
	}
	pstCloseIterator = pstHash->ppstHashShotdownChains[iBucket];
	while(pstCloseIterator != NAP_NULL)
	{
		uiCloseCount++;
		pstCloseIterator = pstCloseIterator->pstNextElement;
	}
#endif

	iHashKey = Fwk_Generate_Key(pSerachKey);
	iBucket = iHashKey % pstHash->uiSearchBuckets;
	pstSearchIterator = pstHash->ppstHashSearchChains[iBucket];
	while(pstSearchIterator != NAP_NULL)
	{
		if(Fwk_HashKeyCompare(pstSearchIterator->pvSearchKey, pSerachKey,psError) == 0)
		{
			bSearchStatus = NAP_SUCCESS;
			break;
		}
		pstSearchIterator = pstSearchIterator->pstNextElement;
	}
	pstSearchIterator = pstHash->ppstHashSearchChains[iBucket];
	while(pstSearchIterator != NAP_NULL)
	{
		uiSearchCount++;
		pstSearchIterator = pstSearchIterator->pstNextElement;
	}
	if ( (bSearchStatus == NAP_SUCCESS || bCloseStatus == NAP_SUCCESS) ||
			(uiSearchCount >pstHash->uiNumOfSearchElements ||
			uiCloseCount >pstHash->uiNumOfShutdownElements))
		return NAP_FAILURE;
	else
		return NAP_SUCCESS;
}


/******************************************************************************
* Function name   : Fwk_HashRemove_SearchElementt
* Description     : This function deletes the hash element from the hash table
					based on specified hash key.
* Return type     : NAP_BOOL
					NAP_SUCCESS on successful deletion of the strcture pointer.
					NAP_FAILURE if any error occurs during delete operation.
* Arguments       : [IN_OUT] pstHash: Hash table instance.
					[IN] pvKey:	Hash key for hash element to be deleted.
					[OUT] psError:	 Variable to used to indicate the error
								to the caller function. This varibale is set to
								zero if there are not errors. Otherswise this
								variable is set to appropriate error number.
* Side Effect     : None
* NOTE            : None
******************************************************************************/
NAP_BOOL Fwk_HashRemove_SearchElement(S_NAP_FWK_HASHTABLE *pstHash,
									NAP_VOID *pvSearchKey,
									NAP_INT16 *psError)
{
	S_NAP_FWK_SEARCHELEMENT *pstTempElement = NAP_NULL;
	S_NAP_FWK_SEARCHELEMENT **ppstIterator = NAP_NULL;
	NAP_UINT32 iHashKey = 0;
	NAP_UINT32 iBucket = 0;

	if ((pvSearchKey == NAP_NULL) || (pstHash == NAP_NULL))
	{
		*psError = e_Err_Fwk_Hash_InvalidParam;
		return NAP_FAILURE;
	}

	iHashKey = Fwk_Generate_Key(pvSearchKey);
	iBucket = iHashKey % pstHash->uiSearchBuckets;
	ppstIterator = &(pstHash->ppstHashSearchChains[iBucket]);
	while(*ppstIterator != NAP_NULL)
	{
		if(Fwk_HashKeyCompare((*ppstIterator)->pvSearchKey, pvSearchKey,psError) == 0)
			break;
		ppstIterator = &((*ppstIterator)->pstNextElement);
	}
	if(*ppstIterator == NAP_NULL)
	{
		*psError = e_Err_Fwk_HashElementsNotFound;
		return NAP_FAILURE;
	}
	else if(*ppstIterator != NAP_NULL)
	{
		pstTempElement = *ppstIterator;
		*ppstIterator = (*ppstIterator)->pstNextElement;
		if (pstTempElement->pvSearchKey != NAP_NULL)
			FWK_Free(&pstTempElement->pvSearchKey);
		pstTempElement->pvElement = NULL;
		FWK_Free(&pstTempElement);
		pstHash->uiNumOfSearchElements--;
	}
	return NAP_SUCCESS;
}

/******************************************************************************
* Function name   : Fwk_HashRead_SearchElement
* Description     : This function retrieves the strcture pointer of type
					S_NAP_FWK_SEARCHELEMENT in the hash table depending upon.
					the hash key specified.
* Return type     : NAP_BOOL
					NAP_SUCCESS on successful retrieval of the strcture pointer.
					NAP_FAILURE if any error occurs during retrieval operation.
* Arguments       : [IN_OUT] pstHash: Hash table instance.
					[IN] pvKey:	Key for the hash element.
					[OUT] psError:	 Variable to used to indicate the error
								to the caller function. This varibale is set to
								zero if there are not errors. Otherswise this
								variable is set to appropriate error number.
* Side Effect     : None
* NOTE            : None
******************************************************************************/
NAP_VOID* Fwk_HashRead_SearchElement(S_NAP_FWK_HASHTABLE *pstHash,
									NAP_VOID *pvSearchKey,
									NAP_INT16 *psError)
{

	NAP_UINT32 iHashKey =0;
	NAP_UINT32 iBucket = 0;
	S_NAP_FWK_SEARCHELEMENT* pstIterator = NAP_NULL;


	if ((pvSearchKey == NAP_NULL) || (pstHash == NAP_NULL))
	{
		*psError = e_Err_Fwk_Hash_InvalidParam;
		return NAP_NULL;
	}
	iHashKey = Fwk_Generate_Key(pvSearchKey);
	iBucket = iHashKey % pstHash->uiSearchBuckets;
	pstIterator = pstHash->ppstHashSearchChains[iBucket];

	while(pstIterator != NAP_NULL)
	{
		if(Fwk_HashKeyCompare(pstIterator->pvSearchKey,pvSearchKey,psError) == 0)
		{

			break;
		}
		pstIterator = pstIterator->pstNextElement;
	}
	if (pstIterator == NAP_NULL)
	{
		*psError = e_Err_Fwk_HashElementsNotFound;
	 	return NAP_NULL;
	}
	else
		return pstIterator->pvElement;
}

/******************************************************************************
* Function name   : Fwk_HashAdd_SearchElement
* Description     : This function adds the strcture pointer of type
					S_NAP_FWK_SEARCHELEMENT in the hash table.
* Return type     : NAP_BOOL
					NAP_SUCCESS on successful addition of the strcture pointer.
					NAP_FAILURE if any error occurs during addition operation.
* Arguments       : [IN_OUT] pstHash: Hash table instance.
					[IN] pvElement:	Hash element to be added.
					[IN] pvKey:	Key for the hash element.
					[OUT] psError:	 Variable to used to indicate the error
								to the caller function. This varibale is set to
								zero if there are not errors. Otherswise this
								variable is set to appropriate error number.
* Side Effect     : None
* NOTE            : None
******************************************************************************/
NAP_BOOL Fwk_HashAdd_SearchElement (S_NAP_FWK_HASHTABLE *pstHash,
						NAP_VOID **pvSearchElement,
						NAP_VOID **pvSearchKey,
						NAP_INT16 *psError)
{
	NAP_UINT32 iHashKey = 0;
	NAP_UINT32 iBucket = 0;
	S_NAP_FWK_SEARCHELEMENT* pstNewElement = NAP_NULL;
	S_NAP_FWK_SEARCHELEMENT **ppstIterator = NAP_NULL;


	if ((pstHash == NAP_NULL) || (pvSearchKey == NAP_NULL))
	{
		*psError = e_Err_Fwk_Hash_InvalidParam;
		return NAP_FAILURE;
	}
	if (pstHash->uiNumOfSearchElements == (pstHash->uiMaxNumberOfElements))
	{
		*psError = e_Err_Fwk_HashElementsExceeded;/*renamed e_Err_MsrpFwk_HashElementsExceeded
											to remove redefinition between msrp and framework*/
		return NAP_FAILURE;
	}
	iHashKey = Fwk_Generate_Key(*pvSearchKey);
	iBucket = iHashKey % pstHash->uiSearchBuckets;

	pstNewElement = (S_NAP_FWK_SEARCHELEMENT *) FWK_Malloc(sizeof(S_NAP_FWK_SEARCHELEMENT));
	if (pstNewElement == NAP_NULL)
	{
		*psError = e_Err_Fwk_MallocFailed;
		return NAP_FAILURE;
	}
	pstNewElement->pvElement = *pvSearchElement;
	pstNewElement->pvSearchKey = *pvSearchKey;
	pstNewElement->pstNextElement = NAP_NULL;

	ppstIterator = &(pstHash->ppstHashSearchChains[iBucket]);
	if ((*ppstIterator) == NAP_NULL)
		(*ppstIterator) = pstNewElement;
	else
	{
		while((*ppstIterator) != NAP_NULL)
			ppstIterator = &((*ppstIterator)->pstNextElement);
		(*ppstIterator) = pstNewElement;

	}
	pstHash->uiNumOfSearchElements++;
	return NAP_SUCCESS;
}


#ifdef DOUBLE_HASH_KEY
/******************************************************************************
* Function name   : Fwk_HashRemove_ShutdownElement
* Description     : This function deletes the hash element from the hash table
					based on specified hash key.
* Return type     : NAP_BOOL
					NAP_SUCCESS on successful deletion of the strcture pointer.
					NAP_FAILURE if any error occurs during delete operation.
* Arguments       : [IN_OUT] pstHash: Hash table instance.
					[IN] pvKey:	Hash key for hash element to be deleted.
					[OUT] psError:	 Variable to used to indicate the error
								to the caller function. This varibale is set to
								zero if there are not errors. Otherswise this
								variable is set to appropriate error number.
* Side Effect     : None
* NOTE            : None
******************************************************************************/
NAP_BOOL	Fwk_HashRemove_ShutdownElement(IN S_NAP_FWK_HASHTABLE *pstHash,
										IN NAP_VOID *pvShutdownKey,
										OUT NAP_INT16 *psError)
{

	S_NAP_FWK_SHUTDOWNELEMENT *pstTempElement = NAP_NULL;
	S_NAP_FWK_SHUTDOWNELEMENT **ppstIterator = NAP_NULL;
	NAP_UINT32 iHashKey = 0;
	NAP_UINT32 iBucket = 0;

	if ((pvShutdownKey == NAP_NULL) || (pstHash == NAP_NULL))
	{
		*psError = e_Err_Fwk_Hash_InvalidParam;
		return NAP_FAILURE;
	}

	iHashKey = Fwk_Generate_Key(pvShutdownKey);
	iBucket = iHashKey % pstHash->uiShutdownBuckets;
	ppstIterator = &(pstHash->ppstHashShotdownChains[iBucket]);
	while(*ppstIterator != NAP_NULL)
	{
		if(Fwk_HashKeyCompare((*ppstIterator)->pvShutDownKey, pvShutdownKey,psError) == 0)
			break;
		ppstIterator = &((*ppstIterator)->pstNextElement);
	}
	if(*ppstIterator == NAP_NULL)
	{
		*psError = e_Err_Fwk_HashElementsNotFound;
		return NAP_FAILURE;
	}
	else if(*ppstIterator != NAP_NULL)
	{
		pstTempElement = *ppstIterator;
		*ppstIterator = (*ppstIterator)->pstNextElement;
		pstTempElement->pvElement = NULL;
		if (pstTempElement->pvShutDownKey != NULL)
			FWK_Free(&pstTempElement->pvShutDownKey);
		FWK_Free(&pstTempElement);
		pstHash->uiNumOfShutdownElements--;
	}
	return NAP_SUCCESS;
}

/******************************************************************************
* Function name   : Fwk_HashAdd_ShutdownElement
* Description     : This function adds the strcture pointer of type
					S_Fwk_SHUTDOWNLEMENT in the hash table.
* Return type     : NAP_BOOL
					NAP_SUCCESS on successful addition of the strcture pointer.
					NAP_FAILURE if any error occurs during addition operation.
* Arguments       : [IN] pstHash: Hash table instance.
					[IN] pvElement:	Hash element to be added.
					[IN] pvKey:	Key for the hash element.
					[OUT] psError:	 Variable to used to indicate the error
								to the caller function. This varibale is set to
								zero if there are not errors. Otherswise this
								variable is set to appropriate error number.
* Side Effect     : None
* NOTE            : None
******************************************************************************/
NAP_BOOL Fwk_HashAdd_ShutdownElement (IN S_NAP_FWK_HASHTABLE *pstHash,
									IN NAP_VOID **pvShutdownElement,
									IN NAP_VOID **pvShutdownKey,
									IN NAP_INT16 *psError)
{
	NAP_UINT32 iHashKey = 0;
	NAP_UINT32 iBucket = 0;
	S_NAP_FWK_SHUTDOWNELEMENT **ppstIterator = NAP_NULL;


	S_NAP_FWK_SHUTDOWNELEMENT* pstNewElement = NAP_NULL;
	if ((pstHash == NAP_NULL) || (pvShutdownKey == NAP_NULL))
	{
		*psError = e_Err_Fwk_Hash_InvalidParam;
		return NAP_FAILURE;
	}
	if (pstHash->uiNumOfShutdownElements == (pstHash->uiMaxNumberOfElements))
	{
		*psError = e_Err_Fwk_HashElementsExceeded;/*renamed e_Err_MsrpFwk_HashElementsExceeded
											to remove redefinition between msrp and framework*/
		return NAP_FAILURE;
	}
	iHashKey = Fwk_Generate_Key((*pvShutdownKey));
	iBucket = iHashKey % pstHash->uiShutdownBuckets;
	ppstIterator = &(pstHash->ppstHashShotdownChains[iBucket]);
	pstNewElement = (S_NAP_FWK_SHUTDOWNELEMENT *) FWK_Malloc(sizeof(S_NAP_FWK_SHUTDOWNELEMENT));
	if (pstNewElement == NAP_NULL)
	{
		*psError =e_Err_Fwk_MallocFailed;
		return NAP_FAILURE;
	}
	pstNewElement->pvElement = *pvShutdownElement;
	pstNewElement->pvShutDownKey = *pvShutdownKey;
	pstNewElement->pstNextElement = NAP_NULL;

	ppstIterator = &(pstHash->ppstHashShotdownChains[iBucket]);
	if ((*ppstIterator) == NAP_NULL)
		(*ppstIterator) = pstNewElement;
	else
	{
		while((*ppstIterator) != NAP_NULL)
			ppstIterator = &((*ppstIterator)->pstNextElement);
		(*ppstIterator) = pstNewElement;

	}
	pstHash->uiNumOfShutdownElements++;
	return NAP_SUCCESS;

}

/******************************************************************************
* Function name   : Fwk_HashRead_ShutdownElement
* Description     : This function retrieves the strcture pointer of type
					S_NAP_FWK_SHUTDOWNELEMENT in the hash table depending upon.
					the hash key specified.
* Return type     : NAP_BOOL
					NAP_SUCCESS on successful retrieval of the strcture pointer.
					NAP_FAILURE if any error occurs during retrieval operation.
* Arguments       : [IN] pstHash: Hash table instance.
					[IN] pvKey:	Key for the hash element.
					[OUT] psError:	 Variable to used to indicate the error
								to the caller function. This varibale is set to
								zero if there are not errors. Otherswise this
								variable is set to appropriate error number.
* Side Effect     : None
* NOTE            : None
******************************************************************************/
NAP_VOID* Fwk_HashRead_ShutdownElement(IN S_NAP_FWK_HASHTABLE *pstHash,
									IN NAP_VOID *pvShtudownKey,
									OUT NAP_INT16 *psError)
{
	NAP_UINT32 iHashKey =0;
	NAP_UINT32 iBucket = 0;
	S_NAP_FWK_SHUTDOWNELEMENT* pstIterator = NAP_NULL;

	if ((pvShtudownKey == NAP_NULL) || (pstHash == NAP_NULL))
	{
		*psError = e_Err_Fwk_Hash_InvalidParam;
		return NAP_NULL;
	}
	iHashKey = Fwk_Generate_Key(pvShtudownKey);
	iBucket = iHashKey % pstHash->uiShutdownBuckets;
	pstIterator = pstHash->ppstHashShotdownChains[iBucket];
	while(pstIterator != NAP_NULL)
	{
		if(Fwk_HashKeyCompare(pstIterator->pvShutDownKey, pvShtudownKey,psError) == 0)
		{
			break;
		}
		pstIterator = pstIterator->pstNextElement;
	}
	if (pstIterator == NAP_NULL)
	{
		*psError = e_Err_Fwk_HashElementsNotFound;
	 	return NAP_NULL;
	}
	else
		return pstIterator->pvElement;
}







/******************************************************************************
					GENERIC - HASH FUNCTIONS FOR ALL DATATYPES
******************************************************************************/


/******************************************************************************
 ** FUNCTION: 		FWK_HashInit
 ** 
 ** DESCRIPTION: 	This is the function to initialize a new 
 **  				hash table.
 **
 ** PARAMETERS:
 **	 pstHash 			(IN/OUT)	: Hash table to be initialized.
 **  fpHashFunc 	(IN)		: Function to be used to hash the key.
 **  fpCompareFunc 	(IN)		: Function to compare the hash keys of entries 
 **								  at time of doing a fetch. If the comparison 
 **								  function :
 **								  returns 0 - the keys that were compared match
 **								  returns 1 - the keys don't match
 **  fpElemFreeFunc (IN)		: Function to invoke to free the 
 **								  element data when the hash entry 
 **								  has be deleted.
 **  fpKeyFreeFunc	(IN)		: Function to invoke to free the 
 **								  element key when the hash entry 
 **								  has be deleted.
 **  uiNumBuckets	(IN)		: number of chains in the hash table.
 **  uiMaxElements	(IN)		: maximum number of elements to be allowed 
 **								  in the hash table.
 **	 pErr			(IN/OUT)	: Error variable returned in case of failure.
 ** 
 ******************************************************************************/
NAP_BOOL FWK_HashInit (FWK_Hash *pstHash,
			FWK_HashFunc fpHashFunc,
			FWK_HashKeyCompareFunc fpCompareFunc,
			FWK_HashElementFreeFunc fpElemFreeFunc,
			FWK_HashKeyFreeFunc fpKeyFreeFunc,
			NAP_UINT32 uiNumBuckets, 
			NAP_UINT32 uiMaxElements,
			FWK_HashError *pErr)
{
	NAP_UINT32 iIndex = 0;

	/* Initialize the max number of buckets.*/
	pstHash->uiSearchBuckets = uiNumBuckets;

	pstHash->uiNumOfSearchElements = 0;

	/* Initialize the max number of elements supported in each buckets.*/
	pstHash->uiMaxNumberOfElements = uiMaxElements;

	/* Initialize functions */
	pstHash->fpHashFunc			= fpHashFunc;
	pstHash->fpCompareFunc		= fpCompareFunc;
	pstHash->fpElementFreeFunc	= fpElemFreeFunc;
	pstHash->fpKeyFreeFunc		= fpKeyFreeFunc;
	
	/* Initialize the hash elements */
	pstHash->ppstHashSearchChains = (S_NAP_FWK_SEARCHELEMENT **) FWK_Malloc(
							 sizeof(S_NAP_FWK_SEARCHELEMENT*)*uiNumBuckets);
	
    if (pstHash->ppstHashSearchChains == NAP_NULL)
	{
		*pErr = e_Err_Fwk_MallocFailed;
		return NAP_FAILURE;
	}

	for(iIndex=0; iIndex<uiNumBuckets; iIndex++)
	{
		pstHash->ppstHashSearchChains[iIndex] = NAP_NULL;
	}

#ifdef FWK_THREAD_SAFE

	pstHash->pMutex = (FWK_MUTEX)FWK_Malloc(sizeof(pthread_mutex_t));

	if(NAP_NULL == pstHash->pMutex)
	{
		FWK_Free(&pstHash->ppstHashSearchChains);
		*pErr = e_Err_Fwk_MallocFailed;
		return NAP_FAILURE;	
	}

	if(pthread_mutex_init(pstHash->pMutex,NAP_NULL))
	{
		FWK_Free(&pstHash->ppstHashSearchChains);
		*pErr = e_Err_Fwk_MallocFailed;
		return NAP_FAILURE;		
	}

#endif /* FWK_THREAD_SAFE */


	return NAP_SUCCESS;	
}



	
	
/******************************************************************************
 ** FUNCTION: 		FWK_HashFree
 ** 
 ** DESCRIPTION: 	This is the function to free members from the hash table. 
 **					It does not free the hash elements, but frees other	member 
 **					variables malloced at the time of Init of the hash table
 **
 ** PARAMETERS:
 **	 pstHash 	(IN)	: Hash table to be freed.
 **  pErr	(OUT)	: Error variable returned in case of failure.
 **
 ******************************************************************************/
NAP_BOOL FWK_HashFree (FWK_Hash *pstHash, FWK_HashError *pErr)
{
	NAP_UINT32 iIndex = 0;

	if(pstHash == NAP_NULL)
	{
		/* e_Err_Fwk_Hash_InvalidParam */
		return NAP_FAILURE;
	}


	for (iIndex = 0; iIndex <pstHash->uiSearchBuckets ; iIndex++)
	{
		/* Remove the entries from the seach list.*/
		
		/* FWK_Printf("\nFWK_HashFree - Num of entries :%d",pstHash->uiNumOfSearchElements); */
		
		if (pstHash->uiNumOfSearchElements >0)
		{
			S_NAP_FWK_SEARCHELEMENT *pstElement = pstHash->ppstHashSearchChains[iIndex];
			S_NAP_FWK_SEARCHELEMENT *pstTempElement = NAP_NULL;
			while (pstElement != NAP_NULL)
			{
				if (pstElement->pvSearchKey != NAP_NULL)
				{

					pstTempElement = pstElement;
					pstElement = pstElement->pstNextElement;
					FWK_Free(&pstTempElement->pvSearchKey);
					pstTempElement->pvElement = NAP_NULL;
					FWK_Free(&pstTempElement);

				}
			}
		}
		else
			FWK_Free(&pstHash->ppstHashSearchChains[iIndex]);
	}
	/* Delete search and close nodes.*/
	FWK_Free(&pstHash->ppstHashSearchChains);

#ifdef FWK_THREAD_SAFE
	if(NAP_NULL != pstHash->pMutex)
	{
		pthread_mutex_destroy(pstHash->pMutex);
		FWK_Free(&pstHash->pMutex);
		pstHash->pMutex = NAP_NULL;
	}
#endif /* FWK_THREAD_SAFE */

	/* SIM_PF_UT: */
	FWK_Memset(pstHash,0,sizeof(FWK_Hash));
	return NAP_SUCCESS;
}


/******************************************************************************
 ** FUNCTION: 		FWK_HashAdd
 ** 
 ** DESCRIPTION: 	This is the function to add an entry into the hash table.
 **
 ** PARAMETERS:
 **	 pstHash 	  (IN)	: Hash table to which the entry has
 **					  to be added.
 **  pElement (IN)	: Hash Element that needs to be added
 **
 **  pErr	  (OUT)	: Error variable returned in case
 **					  of failure.
 **
 ******************************************************************************/
NAP_BOOL FWK_HashAdd (FWK_Hash *pstHash, 
					  NAP_VOID *pvSearchElement, 
					  NAP_VOID *pvSearchKey)
{
	NAP_UINT32 iHashKey = 0;
	NAP_UINT32 iBucket = 0;
	S_NAP_FWK_SEARCHELEMENT	*pstNewElement = NAP_NULL;
	S_NAP_FWK_SEARCHELEMENT *pstIterator = NAP_NULL;


	if ((pvSearchKey == NAP_NULL)
		|| (pstHash == NAP_NULL)
		|| (pstHash->fpHashFunc	== NAP_NULL)
		|| (pstHash->fpCompareFunc == NAP_NULL))
	{
		/* e_Err_Fwk_Hash_InvalidParam */
		return NAP_FAILURE;
	}

#ifdef FWK_THREAD_SAFE
	pthread_mutex_lock(pstHash->pMutex);
#endif

	if (pstHash->uiNumOfSearchElements == (pstHash->uiMaxNumberOfElements))
	{
		/* e_Err_MsrpFwk_HashElementsExceeded */
#ifdef FWK_THREAD_SAFE
		pthread_mutex_unlock(pstHash->pMutex);
#endif
		return NAP_FAILURE;
	}

	iHashKey = pstHash->fpHashFunc(pvSearchKey);
	iBucket = iHashKey % pstHash->uiSearchBuckets;

	/* Ensure that the key is not already present */
	pstIterator = pstHash->ppstHashSearchChains[iBucket];

	while(NAP_NULL != pstIterator)
	{
		if(pstHash->fpCompareFunc(pstIterator->pvSearchKey, pvSearchKey) == 0)
		{
			/* The key already exists */
#ifdef FWK_THREAD_SAFE
			pthread_mutex_unlock(pstHash->pMutex);
#endif
			return NAP_FAILURE;
		}
		pstIterator = pstIterator->pstNextElement;
	}

	pstNewElement = (S_NAP_FWK_SEARCHELEMENT *) 
		FWK_Malloc(sizeof(S_NAP_FWK_SEARCHELEMENT));

	if (pstNewElement == NAP_NULL)
	{
		/* e_Err_Fwk_MallocFailed */
#ifdef FWK_THREAD_SAFE
		pthread_mutex_unlock(pstHash->pMutex);
#endif
		return NAP_FAILURE;
	}
	
	pstNewElement->pvElement		= pvSearchElement;
	pstNewElement->pvSearchKey		= pvSearchKey;
	pstNewElement->pstNextElement	= NAP_NULL;
	pstNewElement->bRemove			= NAP_FALSE;
	pstNewElement->uiRefCount		= 1;

	pstIterator = pstHash->ppstHashSearchChains[iBucket];

	if(pstIterator == NAP_NULL)
		pstIterator = pstNewElement;
	else
	{
		while(pstIterator != NAP_NULL)
			pstIterator = pstIterator->pstNextElement;
		pstIterator = pstNewElement;

	}

	/* Push element into the bucket */
	pstNewElement->pstNextElement = pstHash->ppstHashSearchChains[iBucket];
	pstHash->ppstHashSearchChains[iBucket] = pstNewElement;
	pstHash->uiNumOfSearchElements++;

	/* FWK_Printf("\nFWK_HashAdd - Num of entries :%d",pstHash->uiNumOfSearchElements); */

#ifdef FWK_THREAD_SAFE
		pthread_mutex_unlock(pstHash->pMutex);
#endif
	return NAP_SUCCESS;
}
	




/******************************************************************************
 ** FUNCTION: 		FWK_HashFetch
 ** 
 ** DESCRIPTION: 	This is the function to fetch an entry from the hash table.
 **					NAP_NULL is returned in case the hash table does not 
 **					contain any entries corresponding to the key passed.
 **
 ** PARAMETERS:
 **	 pstHash 	(IN)	: Hash table from which the entry has to be
 **					  extracted.
 **  pKey	(IN)	: Key corresponding to the element to be 
 **					  fetched from the hash table.
 **
 ******************************************************************************/
NAP_VOID* FWK_HashFetch (FWK_Hash *pstHash, NAP_VOID *pvSearchKey)
{
	NAP_UINT32 iHashKey =0;
	NAP_UINT32 iBucket = 0;
	S_NAP_FWK_SEARCHELEMENT* pstIterator = NAP_NULL;


	if ((pvSearchKey == NAP_NULL) 
		|| (pstHash == NAP_NULL)
		|| (pstHash->fpHashFunc	== NAP_NULL)
		|| (pstHash->fpCompareFunc == NAP_NULL))
	{
		/* e_Err_Fwk_Hash_InvalidParam */
		return NAP_NULL;
	}

#ifdef FWK_THREAD_SAFE
	pthread_mutex_lock(pstHash->pMutex);
#endif

	iHashKey	= pstHash->fpHashFunc(pvSearchKey);
	iBucket		= iHashKey % pstHash->uiSearchBuckets;
	pstIterator = pstHash->ppstHashSearchChains[iBucket];

	while(pstIterator != NAP_NULL)
	{
		if(pstHash->fpCompareFunc(pstIterator->pvSearchKey,pvSearchKey) == 0)
		{
			pstIterator->uiRefCount++;
			break;
		}
		pstIterator = pstIterator->pstNextElement;
	}

#ifdef FWK_THREAD_SAFE
	pthread_mutex_unlock(pstHash->pMutex);
#endif
	
	if (pstIterator == NAP_NULL)
	{
		/* e_Err_Fwk_HashElementsNotFound */
	 	return NAP_NULL;
	}
	else
	{
		/* FWK_Printf("\nFWK_HashFetch - Ref count for this element :%d",pstIterator->uiRefCount); */

		return pstIterator->pvElement;
	}
		
}



/******************************************************************************
 ** FUNCTION: 		FWK_HashRelease
 ** 
 ** DESCRIPTION:	This function should be invoked to
 **					"check in" an element that was obtained
 **					from the hash table. Normally, it would
 **					just decrement reference count for the
 **					element. In case that the element has its
 **					remove flag set, this function frees
 **					the memory too.
 **
 ** PARAMETERS:
 **	 pstHash 	(IN)	: Hash table from which the entry has to 
 **					  be released.
 **  pKey	(IN)	: Key corresponding to the element to be 
 **					  released.
 **
 ******************************************************************************/
NAP_VOID FWK_HashRelease (FWK_Hash *pstHash, NAP_VOID *pvSearchKey)
{
	NAP_UINT32 iHashKey =0;
	NAP_UINT32 iBucket = 0;
	S_NAP_FWK_SEARCHELEMENT	**pstIterator = NAP_NULL;
	S_NAP_FWK_SEARCHELEMENT *pstTempElement = NAP_NULL;


	if ((pvSearchKey == NAP_NULL) 
		|| (pstHash == NAP_NULL)
		|| (pstHash->fpHashFunc	== NAP_NULL)
		|| (pstHash->fpCompareFunc == NAP_NULL))
	{
		/* e_Err_Fwk_Hash_InvalidParam */
		return;
	}

#ifdef FWK_THREAD_SAFE
	pthread_mutex_lock(pstHash->pMutex);
#endif

	iHashKey	= pstHash->fpHashFunc(pvSearchKey);
	iBucket		= iHashKey % pstHash->uiSearchBuckets;
	pstIterator = &(pstHash->ppstHashSearchChains[iBucket]);

	while(*pstIterator != NAP_NULL)
	{
		if(pstHash->fpCompareFunc((*pstIterator)->pvSearchKey,pvSearchKey) == 0)
		{
			(*pstIterator)->uiRefCount--;
			(*pstIterator)->uiRefCount--;
			break;
		}
		pstIterator = &((*pstIterator)->pstNextElement);
	}

	
	if ((*pstIterator) == NAP_NULL)
	{
#ifdef FWK_THREAD_SAFE
		pthread_mutex_unlock(pstHash->pMutex);
#endif
		/* e_Err_Fwk_HashElementsNotFound */
	 	return;
	}

	if((NAP_TRUE == (*pstIterator)->bRemove) 
		&& (0 == (*pstIterator)->uiRefCount))
	{
		pstTempElement = *pstIterator;
		*pstIterator = (*pstIterator)->pstNextElement;
		if (NAP_NULL != pstTempElement->pvSearchKey)
		{
			pstHash->fpKeyFreeFunc(pstTempElement->pvSearchKey);
		}
		if(NAP_NULL != pstTempElement->pvElement)
		{
			pstHash->fpElementFreeFunc(pstTempElement->pvElement);
		}

		pstHash->uiNumOfSearchElements--;
		pstTempElement->pvElement = NULL;
		FWK_Free(&pstTempElement);
		
		/* FWK_Printf("\nFWK_HashRelease - Element freed. Num Entries :%d",pstHash->uiNumOfSearchElements); */
	}
	else
	{
		(*pstIterator)->uiRefCount++;
		/* FWK_Printf("\nFWK_HashRelease - Ref count for this element :%d",(*pstIterator)->uiRefCount); */
	}


#ifdef FWK_THREAD_SAFE
	pthread_mutex_unlock(pstHash->pMutex);
#endif


}


/******************************************************************************
 ** FUNCTION: FWK_HashRemove
 ** 
 ** DESCRIPTION: 	This is the function to remove an entry from the hash 
 **					table. If the element is in use at the time of the remove 
 **					request, then it is marked for removal and memory actually
 **					gets freed only when the other usage releases the entry.
 **
 ** PARAMETERS:
 **	 pstHash 	(IN)	: Hash table from which the entry has to be released.
 **  pKey	(IN)	: Key corresponding to the element to be removed.
 **
 ******************************************************************************/
NAP_BOOL FWK_HashRemove (FWK_Hash *pstHash, NAP_VOID *pvSearchKey)
{
	S_NAP_FWK_SEARCHELEMENT *pstTempElement = NAP_NULL;
	S_NAP_FWK_SEARCHELEMENT **pstIterator = NAP_NULL;
	NAP_UINT32 iHashKey = 0;
	NAP_UINT32 iBucket = 0;


	if ((pvSearchKey == NAP_NULL) 
		|| (pstHash == NAP_NULL) 
		|| (pstHash->fpHashFunc == NAP_NULL)
		|| (pstHash->fpCompareFunc == NAP_NULL))
	{
		/* e_Err_Fwk_Hash_InvalidParam */
		return NAP_FAILURE;
	}

#ifdef FWK_THREAD_SAFE
	pthread_mutex_lock(pstHash->pMutex);
#endif

	iHashKey	= pstHash->fpHashFunc(pvSearchKey);
	iBucket		= iHashKey % pstHash->uiSearchBuckets;
	pstIterator = &(pstHash->ppstHashSearchChains[iBucket]);

	while(*pstIterator != NAP_NULL)
	{
		if(pstHash->fpCompareFunc((*pstIterator)->pvSearchKey,pvSearchKey) == 0)
		{
			break;
		}
		pstIterator = &((*pstIterator)->pstNextElement);
	}

	

	if(*pstIterator == NAP_NULL)
	{

#ifdef FWK_THREAD_SAFE
		pthread_mutex_unlock(pstHash->pMutex);
#endif
		/* e_Err_Fwk_HashElementsNotFound */
		return NAP_FAILURE;
	}

	(*pstIterator)->uiRefCount--;

	if(0 == (*pstIterator)->uiRefCount)
	{
		pstTempElement = *pstIterator;
		*pstIterator = (*pstIterator)->pstNextElement;
		if (NAP_NULL != pstTempElement->pvSearchKey)
		{
			pstHash->fpKeyFreeFunc(pstTempElement->pvSearchKey);
		}
		if(NAP_NULL != pstTempElement->pvElement)
		{
			pstHash->fpElementFreeFunc(pstTempElement->pvElement);
		}

		pstHash->uiNumOfSearchElements--;
		pstTempElement->pvElement = NULL;
		FWK_Free(&pstTempElement);
		/* FWK_Printf("\nFWK_HashRemove - Element freed. Num Entries :%d",pstHash->uiNumOfSearchElements); */
	}
	else
	{
		(*pstIterator)->bRemove = NAP_TRUE;
		(*pstIterator)->uiRefCount++;
		/* FWK_Printf("\nFWK_HashRemove - Could not remove as Ref count for this element :%d",(*pstIterator)->uiRefCount); */
	}


#ifdef FWK_THREAD_SAFE
		pthread_mutex_unlock(pstHash->pMutex);
#endif

	return NAP_SUCCESS;
}



/******************************************************************************
 ** FUNCTION: 		FWK_HashIterateNext
 ** 
 ** DESCRIPTION: 	This is the function to get the element next to an 
 **					iterator in the hash table
 **
 ** PARAMETERS:
 **	 pstHash 		(IN)		: Hash table from which the next entry
 **							  has to be retrieved.
 **  pstIterator	(IN/OUT)	: Hash iterator for which the next 
 **							  element has to be retrieved.
 **
 ******************************************************************************/
void FWK_HashIterateNext (FWK_Hash *pstHash, S_NAP_FWK_HASH_ITERATOR *pstIterator)
{
	S_NAP_FWK_SEARCHELEMENT *pstNextElem = NAP_NULL, *pTempElement = NAP_NULL;
	S_NAP_FWK_SEARCHELEMENT **ppElement = NAP_NULL;
	NAP_VOID *pKey = NAP_NULL;

	if ((pstHash == NAP_NULL)
		|| (pstHash->fpHashFunc	== NAP_NULL)
		|| (pstHash->fpCompareFunc == NAP_NULL)
		|| (NAP_NULL == pstHash->ppstHashSearchChains))
	{
		/* e_Err_Fwk_Hash_InvalidParam */
		return;
	}

#ifdef FWK_THREAD_SAFE
	pthread_mutex_lock(pstHash->pMutex);
#endif

	
	/* 	If current element in the iterator points to a 
		null node - keep it null else make it point to 
		next element in the chain. */
	pstNextElem = (pstIterator->pCurrentElement == NAP_NULL)\
		? NAP_NULL : pstIterator->pCurrentElement->pstNextElement;

	/* 
	 * Check if the current element has to be removed.
	 * If so, do it here before fetching the next element
	 */
	if (pstIterator->pCurrentElement != NAP_NULL)
	{
		(pstIterator->pCurrentElement->uiRefCount)--;
		(pstIterator->pCurrentElement->uiRefCount)--;

		if ((0 == pstIterator->pCurrentElement->uiRefCount) && \
			(pstIterator->pCurrentElement->bRemove == NAP_TRUE))
		{
			pKey = pstIterator->pCurrentElement->pvSearchKey;
			ppElement = &(pstHash->ppstHashSearchChains[pstIterator->uiCurrentBucket]);
   	 
			while(*ppElement != NAP_NULL)
			{
				if(pstHash->fpCompareFunc((*ppElement)->pvSearchKey, pKey) == 0)
					break;
				ppElement = &((*ppElement)->pstNextElement);
			}
			if(*ppElement != NAP_NULL)
			{
				pTempElement = *ppElement;
				*ppElement = (*ppElement)->pstNextElement;
				if (pstHash->fpElementFreeFunc != NAP_NULL)
					pstHash->fpElementFreeFunc(pTempElement->pvElement);
				if (pstHash->fpKeyFreeFunc != NAP_NULL)
					pstHash->fpKeyFreeFunc(pTempElement->pvSearchKey);

				FWK_Free((NAP_VOID **)&pTempElement);
				pstHash->uiNumOfSearchElements--;
			}
		}
		else
		{
			pstIterator->pCurrentElement->uiRefCount++;
		}
	}

	if (pstNextElem != NAP_NULL)
	{
		/* 	Still in the middle of iteration through a chain. 
			We have already taken the next element. 
			Return now */
		pstIterator->pCurrentElement = pstNextElem;
		pstIterator->pCurrentElement->uiRefCount++;
#ifdef FWK_THREAD_SAFE
		pthread_mutex_unlock(pstHash->pMutex);
#endif
		return ;
	}
	
	/*	Since the next element is Null, we have reached the end
		of the chain. Check if its the end of the last chain.
		If so, return */
	if((pstNextElem == NAP_NULL) &&\
		(pstIterator->uiCurrentBucket == pstHash->uiSearchBuckets - 1))
	{
		pstIterator->pCurrentElement = pstNextElem;
#ifdef FWK_THREAD_SAFE
		pthread_mutex_unlock(pstHash->pMutex);
#endif

		return;
	}
	
	/* 	Find the next non-empty chain and 
		make the iterator point to that */
	pstIterator->uiCurrentBucket++;
	while (pstIterator->uiCurrentBucket != pstHash->uiSearchBuckets - 1)
	{
			
		if (pstHash->ppstHashSearchChains[pstIterator->uiCurrentBucket]!=NAP_NULL)
			break;
		pstIterator->uiCurrentBucket++;
	}

	pstIterator->pCurrentElement = \
		pstHash->ppstHashSearchChains[pstIterator->uiCurrentBucket];
	
	/* 	Increment reference count of the element being 
		returned unless we reached the end of the final 
		bucket */
	if (pstIterator->pCurrentElement != NAP_NULL)
		pstIterator->pCurrentElement->uiRefCount++;

#ifdef FWK_THREAD_SAFE
	pthread_mutex_unlock(pstHash->pMutex);
#endif

	return;
}


/******************************************************************************
 ** FUNCTION: FWK_HashInitIterator
 **
 ** DESCRIPTION:	Sets the iterator to the first element of the hashtable
 **
 ** PARAMETERS:
 **  pstIterator	(IN/OUT)	: Hash iterator to be inited.
 **							
 ******************************************************************************/
void FWK_HashInitIterator (FWK_Hash *pstHash, S_NAP_FWK_HASH_ITERATOR *pstIterator)
{

	if ((pstHash == NAP_NULL) 
		|| (pstIterator == NAP_NULL) 
		|| (NAP_NULL == pstHash->ppstHashSearchChains))
		return;

	pstIterator->uiCurrentBucket = 0;
	pstIterator->pCurrentElement = NAP_NULL;

#ifdef FWK_THREAD_SAFE
	pthread_mutex_lock(pstHash->pMutex);
#endif

	while(pstIterator->uiCurrentBucket <= pstHash->uiSearchBuckets - 1)
	{
		if(pstHash->ppstHashSearchChains[pstIterator->uiCurrentBucket] == NAP_NULL)
		{
			pstIterator->uiCurrentBucket++;
			continue;
		}
		else
		{
			pstIterator->pCurrentElement = pstHash->ppstHashSearchChains\
				[pstIterator->uiCurrentBucket];
			pstIterator->pCurrentElement->uiRefCount++;
			break;
		}
	}

	if(NAP_NULL == pstIterator->pCurrentElement)
	{
		pstIterator->uiCurrentBucket--;
	}
	

#ifdef FWK_THREAD_SAFE
		pthread_mutex_unlock(pstHash->pMutex);
#endif

	return;

}





/******************************************************************************
 ** FUNCTION: FWK_HashIsFree
 **
 ** DESCRIPTION: Tells whether there is space to accomodate additional
 **				 entries in the hash table or not
 **
 ** PARAMETERS:
 **  pHash 		(IN)		: Hash table
 ** 
 **  puiNumFreeEntries(OUT) : Number of free entries
 **							
 ******************************************************************************/
NAP_BOOL FWK_HashIsFree (FWK_Hash *pstHash, NAP_UINT32 *puiNumFreeEntries)
{

	if ((pstHash == NAP_NULL) || (puiNumFreeEntries == NAP_NULL))
		return NAP_FALSE;

#ifdef FWK_THREAD_SAFE
	pthread_mutex_lock(pstHash->pMutex);
#endif
	*puiNumFreeEntries = pstHash->uiMaxNumberOfElements\
							- pstHash->uiNumOfSearchElements;

#ifdef FWK_THREAD_SAFE
		pthread_mutex_unlock(pstHash->pMutex);
#endif

	return (0 != *puiNumFreeEntries);
}







#endif




