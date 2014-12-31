
#ifndef	__FWK_HASH_TABLE_H__
#define	__FWK_HASH_TABLE_H__

#include <stdio.h>
#include <string.h>
#include "fwk_memory.h"
#include "fwk_datatypes.h"
#include "fwk_string.h"
#include "fwk_error.h"


#define FWK_THREAD_SAFE		1

/* If the thread safety is required */
#ifdef FWK_THREAD_SAFE
#include "pthread.h"
typedef pthread_mutex_t* FWK_MUTEX;
#endif /* FWK_THREAD_SAFE */

#define  DOUBLE_HASH_KEY	1

/*
 * This structure stores the hash table element and the search key to
 * retrieve the hash element.
 */
typedef struct _S_NAP_FWK_SEARCHELEMENT
{
	NAP_VOID*	pvElement;		/* Hash element*/
	NAP_VOID*	pvSearchKey;	/* Hash key*/
	
	struct _S_NAP_FWK_SEARCHELEMENT *pstNextElement;
	
	/*
	 * Flag that will be set to true if the API FWK_HashRemove is
	 * unable to free the memory for the hash element at time of
	 * invocation. As soon as the structures refCount reduces to zero, the
	 * memory will be freed if this flag is set.
	*/
	NAP_BOOL	bRemove;

	/*
	 * Keep track of how many check-outs of this data have happened. If the
	 * refCount is greater than 1 and FWK_HashRemove is invoked,
	 * memory will not be freed and the bRemove flag will be set.
	*/
	NAP_UINT32	uiRefCount;

}S_NAP_FWK_SEARCHELEMENT;

/*
 * This structure stores the hash table element and the search key to
 * retrieve the hash element. This is used to retrieve the hash element
 * during session shutdown operation.
 */
typedef struct _S_NAP_FWK_SHUTDOWNELEMENT
{
	NAP_VOID*	pvElement;		/* Hash element*/
	NAP_VOID*	pvShutDownKey;	/* Hash key */
	struct _S_NAP_FWK_SHUTDOWNELEMENT  *pstNextElement;
}S_NAP_FWK_SHUTDOWNELEMENT;


#define FWK_HashError	NAP_UINT32

/*
 * The function to be used to calculate the hash value for the key passed.
*/
typedef NAP_UINT32 (*FWK_HashFunc) (NAP_VOID *pData);


/*
 * The function to be used to compare keys of 2 hash elements.
 */
typedef NAP_UINT8 (*FWK_HashKeyCompareFunc) (NAP_VOID *pKey1,\
					NAP_VOID *pKey2);

/*
 * The function to free the data stored in the hash element.
 */
typedef NAP_VOID (*FWK_HashElementFreeFunc) (NAP_VOID *pElement);

/*
 * The function to free the key stored in the hash element.
 */
typedef NAP_VOID (*FWK_HashKeyFreeFunc) (NAP_VOID *pKey);

/*
 * Iterator needed to operate on all the hash elements. User will need to
 * do FWK_HashIterateNext() to get the next element from the hash table.
 */
typedef struct
{
	S_NAP_FWK_SEARCHELEMENT		*pCurrentElement;
	NAP_UINT32					uiCurrentBucket;
} S_NAP_FWK_HASH_ITERATOR;


/*
 * This structure stores the all the hash table element details.
 */
typedef struct  _S_NAP_FWK_HASHTABLE
{
	/* function to calculate hash value */
	FWK_HashFunc				fpHashFunc; 
	
	/* function to compare keys of 2 elements */
	FWK_HashKeyCompareFunc		fpCompareFunc; 
	
	/* function to free the data being stored */ 
	FWK_HashElementFreeFunc		fpElementFreeFunc; 
	
	/* function to free the key given for an element */
	FWK_HashKeyFreeFunc			fpKeyFreeFunc; 



	NAP_UINT32	uiSearchBuckets;			/* Max number of buckets*/
	/* Current number of hash element in each bucket */
	NAP_UINT32	uiNumOfSearchElements;
	/* Max number of hash elements in each bucket*/
	NAP_UINT32	uiMaxNumberOfElements;
	/* Hash elements structure*/
	S_NAP_FWK_SEARCHELEMENT    **ppstHashSearchChains;

#ifdef DOUBLE_HASH_KEY
	/* Max number of buckets*/
	NAP_UINT32	uiShutdownBuckets;
	/* Current number of hash element in each bucket */
	NAP_UINT32	uiNumOfShutdownElements;
	/* Hash elements structure*/
	S_NAP_FWK_SHUTDOWNELEMENT  **ppstHashShotdownChains;
#endif

#ifdef FWK_THREAD_SAFE
	/* One for each function - it should be ok for the current architechure where
	not more than one thread is making call for hash otherwise per buket a mutex
	can be assigned on the cost of memory */
	FWK_MUTEX	pMutex;
#endif

}S_NAP_FWK_HASHTABLE;


#define	FWK_Hash		S_NAP_FWK_HASHTABLE


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
 **	 pHash 			(IN/OUT)	: Hash table to be initialized.
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
NAP_BOOL FWK_HashInit (FWK_Hash *pHash,
			FWK_HashFunc fpHashFunc,
			FWK_HashKeyCompareFunc fpCompareFunc,
			FWK_HashElementFreeFunc fpElemFreeFunc,
			FWK_HashKeyFreeFunc fpKeyFreeFunc,
			NAP_UINT32 uiNumBuckets, 
			NAP_UINT32 uiMaxElements,
			FWK_HashError *pErr);



	
	
/******************************************************************************
 ** FUNCTION: 		FWK_HashFree
 ** 
 ** DESCRIPTION: 	This is the function to free members from the hash table. 
 **					It does not free the hash elements, but frees other	member 
 **					variables malloced at the time of Init of the hash table
 **
 ** PARAMETERS:
 **	 pHash 	(IN)	: Hash table to be freed.
 **  pErr	(OUT)	: Error variable returned in case of failure.
 **
 ******************************************************************************/
NAP_BOOL FWK_HashFree (FWK_Hash *pHash, FWK_HashError *pErr);	


/******************************************************************************
 ** FUNCTION: 		FWK_HashAdd
 ** 
 ** DESCRIPTION: 	This is the function to add an entry into the hash table.
 **
 ** PARAMETERS:
 **	 pHash 	  (IN)	: Hash table to which the entry has
 **					  to be added.
 **  pElement (IN)	: Hash Element that needs to be added
 **
 **  pErr	  (OUT)	: Error variable returned in case
 **					  of failure.
 **
 ******************************************************************************/
NAP_BOOL FWK_HashAdd (FWK_Hash *pHash, NAP_VOID *pElement, NAP_VOID* pKey);	
	




/******************************************************************************
 ** FUNCTION: 		FWK_HashFetch
 ** 
 ** DESCRIPTION: 	This is the function to fetch an entry from the hash table.
 **					NAP_NULL is returned in case the hash table does not 
 **					contain any entries corresponding to the key passed.
 **
 ** PARAMETERS:
 **	 pHash 	(IN)	: Hash table from which the entry has to be
 **					  extracted.
 **  pKey	(IN)	: Key corresponding to the element to be 
 **					  fetched from the hash table.
 **
 ******************************************************************************/
NAP_VOID* FWK_HashFetch (FWK_Hash *pHash, NAP_VOID *pKey);



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
 **	 pHash 	(IN)	: Hash table from which the entry has to 
 **					  be released.
 **  pKey	(IN)	: Key corresponding to the element to be 
 **					  released.
 **
 ******************************************************************************/
NAP_VOID FWK_HashRelease (FWK_Hash *pHash, NAP_VOID *pKey);


/******************************************************************************
 ** FUNCTION: FWK_HashRemove
 ** 
 ** DESCRIPTION: 	This is the function to remove an entry from the hash 
 **					table. If the element is in use at the time of the remove 
 **					request, then it is marked for removal and memory actually
 **					gets freed only when the other usage releases the entry.
 **
 ** PARAMETERS:
 **	 pHash 	(IN)	: Hash table from which the entry has to be released.
 **  pKey	(IN)	: Key corresponding to the element to be removed.
 **
 ******************************************************************************/
NAP_BOOL FWK_HashRemove (FWK_Hash *pHash, NAP_VOID *pKey);



/******************************************************************************
 ** FUNCTION: 		FWK_HashIterateNext
 ** 
 ** DESCRIPTION: 	This is the function to get the element next to an 
 **					iterator in the hash table
 **
 ** PARAMETERS:
 **	 pHash 		(IN)		: Hash table from which the next entry
 **							  has to be retrieved.
 **  pIterator	(IN/OUT)	: Hash iterator for which the next 
 **							  element has to be retrieved.
 **
 ******************************************************************************/
void FWK_HashIterateNext (FWK_Hash *pHash, S_NAP_FWK_HASH_ITERATOR *pIterator);


/******************************************************************************
 ** FUNCTION: FWK_HashInitIterator
 **
 ** DESCRIPTION:	Sets the iterator to the first element of the hashtable
 **
 ** PARAMETERS:
 **  pIterator	(IN/OUT)	: Hash iterator to be inited.
 **							
 ******************************************************************************/
void FWK_HashInitIterator (FWK_Hash *pHash, S_NAP_FWK_HASH_ITERATOR *pIterator);



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
NAP_BOOL FWK_HashIsFree (FWK_Hash *pHash, NAP_UINT32 *puiNumFreeEntries);









/******************************************************************************
				HASH FUNCTIONS - STRICTLY FOR STRING DATATYPES
******************************************************************************/




/******************************************************************************
* Function name   : Fwk_HashInit
* Description     : This function initailizes the hash table and required
					 function pointers. This function also allocates and
					 initialize the hash elements.
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
NAP_BOOL Fwk_HashInit (IN_OUT S_NAP_FWK_HASHTABLE *pstHash,
						IN	NAP_UINT32 iNumBuckets,
						IN	NAP_UINT32 iMaxElements,
						OUT	NAP_INT16 *psError);

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
						OUT	NAP_INT16	*psError);


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
NAP_VOID*	Fwk_HashRead_SearchElement (IN S_NAP_FWK_HASHTABLE *pstHash,
									IN NAP_VOID *pvKey,
									OUT NAP_INT16 *psError);


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
NAP_BOOL	Fwk_HashAdd_SearchElement (IN S_NAP_FWK_HASHTABLE *pstHash,
									IN NAP_VOID **pvElement,
									IN NAP_VOID **pvKey,
									OUT NAP_INT16 *psError);


/******************************************************************************
* Function name   : Fwk_HashRemove_SearchElement
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
NAP_BOOL	Fwk_HashRemove_SearchElement(IN S_NAP_FWK_HASHTABLE *pstHash,
									OUT NAP_VOID *pvKey,
									OUT NAP_INT16 *psError);



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
									OUT NAP_VOID *pvKey,
									OUT NAP_INT16 *psError);

/******************************************************************************
* Function name   : Fwk_HashAdd_ShutdownElement
* Description     : This function adds the strcture pointer of type
					S_NAP_FWK_SHUTDOWNLEMENT in the hash table.
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
NAP_BOOL	Fwk_HashAdd_ShutdownElement (IN S_NAP_FWK_HASHTABLE *pstHash,
									IN NAP_VOID **pvElement,
									IN NAP_VOID **pvKey,
									IN NAP_INT16 *psError);

/******************************************************************************
* Function name   : Fwk_HashRemove_ShutdownElement
* Description     : This function deletes the hash element from the hash table
					based on specified hash key.
* Return type     : NAP_BOOL
					NAP_SUCCESS on successful deletion of the strcture pointer.
					NAP_FAILURE if any error occurs during delete operation.
* Arguments       : [IN] pstHash: Hash table instance.
					[IN] pvKey:	Hash key for hash element to be deleted.
					[OUT] psError:	 Variable to used to indicate the error
								to the caller function. This varibale is set to
								zero if there are not errors. Otherswise this
								variable is set to appropriate error number.
* Side Effect     : None
* NOTE            : None
******************************************************************************/
NAP_BOOL	Fwk_HashRemove_ShutdownElement(IN S_NAP_FWK_HASHTABLE *pstHash,
									OUT NAP_VOID *pvKey,
									OUT NAP_INT16 *psError);




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
NAP_INT32	Fwk_HashKeyCompare(IN NAP_VOID *pvKey1,
							IN NAP_VOID *pvKey2,
							OUT NAP_INT16 *psError);




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
Fwk_Generate_Key(NAP_VOID *pvKeySrc);


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
								NAP_INT16 *psError);

#endif   /* Please dont add anything after this line*/
