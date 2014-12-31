

#include "fwk_list.h"
#include "fwk_trace.h"

/*
*******************************************************************************
* Function name	: FWK_LIST_AddNode
* Description		: This function adds nodes to a list and also creates a list, 
*				: if it does not exist.
*
* Return type		: NAP_BOOL
*			      NAP_SUCCESS, if success else NAP_FAILURE.
* Argument      	:  
*    [IN-OUT]		: ppstList - List in which the node must be added.		
*    [IN]			: pvData   - Data to be stored in the node.
*    [OUT]		: piError - The error type while returning NAP_FAILURE.  
*
* Side Effects		: None
* NOTE 			: None
*******************************************************************************
*/
NAP_BOOL FWK_LIST_AddNode
(
	S_FWK_LIST			**ppstList,
	NAP_VOID			*pvData,
	NAP_INT16			*piError
)
{
	// Check for the List existence
	if(NAP_NULL == *ppstList)
	{
		// If List is not created, then create the list first 
		*ppstList = (S_FWK_LIST *) FWK_Malloc (
													sizeof(S_FWK_LIST));
        if(NAP_NULL == *ppstList)
        {
            return NAP_FAILURE;
        }
			
		// Allocate memory for the first Node 
		(*ppstList)->pstHead = (S_FWK_LIST_NODE *) FWK_Malloc (
													sizeof(S_FWK_LIST_NODE));
        if(NAP_NULL == (*ppstList)->pstHead)
        {
            FWK_Free(ppstList);
            return NAP_FAILURE;
        }

		// Copy the data's reference into the Node 
		(*ppstList)->pstHead->pvData = pvData;
        
        // Set the next node pointer to NAP_NULL
		(*ppstList)->pstHead->pstNext = NAP_NULL;

		// Set the tail of List
		// First Node = Last Node
		(*ppstList)->pstTail = (*ppstList)->pstHead; 
		//Set the node count
		(*ppstList)->usCount = 1;				   
	}
	else
	{
		// List already exist. Add a node in the list.
		(*ppstList)->pstTail->pstNext = (S_FWK_LIST_NODE *) FWK_Malloc (
													sizeof(S_FWK_LIST_NODE));
        if(NAP_NULL == (*ppstList)->pstTail->pstNext)
        {
            return NAP_FAILURE;
        }

		// Copy the data into the Node
		(*ppstList)->pstTail->pstNext->pvData = pvData;
        
        // Set the next node pointer to NAP_NULL 
		(*ppstList)->pstTail->pstNext->pstNext = NAP_NULL;
		
		// Set the List Node
		(*ppstList)->usCount++;
		//Last Node = New Node
		(*ppstList)->pstTail = (*ppstList)->pstTail->pstNext;
	}
    return NAP_SUCCESS;
}//End of FWK_LIST_AddNode

/*
*******************************************************************************
* Function name	: FWK_LIST_DeleteNode
* Description		: This function deletes a node from the list and also deletes 
*				: the list, if all the nodes are deleted.
* Return type		: NAP_BOOL
*			      NAP_SUCCESS, if success else NAP_FAILURE.
*                  
* Argument      	:  
*    [IN-OUT]		: ppstList - List in which the node must be deleted.		
*    [IN]			: usNodePos- The node to be deleted.
*	 [OUT]		: ppvData  - Pointer to the address of data give by user.
*    [OUT]		: piError  - The error type while returning NAP_FAILURE.  
*
* Side Effects		: None
* NOTE 			: None
*******************************************************************************
*/
NAP_BOOL FWK_LIST_DeleteNode
(
	S_FWK_LIST		**ppstList,
	NAP_UINT16		usNodePos,
	NAP_VOID		**ppvData,
	NAP_INT16		*piError
)
{
    S_FWK_LIST_NODE	*psttemp1	= NAP_NULL ;
	S_FWK_LIST_NODE	*psttemp2	= NAP_NULL;
	NAP_UINT16		uiLoopCount = 0 ; 
	
	// Checking for the List existence
	if(NAP_NULL == *ppstList) 
	{
		*ppvData = NAP_NULL;
		return NAP_FAILURE;
	}

    // Check whether the Node exists 
    if((0 == usNodePos) || (usNodePos > (*ppstList)->usCount))
    {
		*ppvData = NAP_NULL;
		return NAP_FAILURE;
    }

	//Initialising temporary variable with list head pointer
	psttemp1 = (*ppstList)->pstHead;
    
    // If the First Node should be deleted
    if(1 == usNodePos)
    {
        (*ppstList)->pstHead = (*ppstList)->pstHead->pstNext;

		// return node data
		*ppvData = psttemp1->pvData;
		psttemp1->pvData = NAP_NULL;

        FWK_Free((NAP_VOID**)&psttemp1);
        //psttemp1 = NAP_NULL;
    }
	else if(usNodePos > 1)
	{
		// Traversing the linkedlist till the node 
		// previous to one to be deleted
		for(uiLoopCount = 1; uiLoopCount < usNodePos-1 ; uiLoopCount++)
		{
			psttemp1 = psttemp1->pstNext;
		}
	
        // Node to be deleted 
	    psttemp2 = psttemp1->pstNext;

        // Link the list with the node next to the deleted one
        psttemp1->pstNext = psttemp2->pstNext;
        
		// Check whether deleted node is last or not
		if(NAP_NULL == psttemp1->pstNext)
		{
			(*ppstList)->pstTail = psttemp1;
		}

		// return node data
		*ppvData = psttemp2->pvData;
        psttemp2->pvData = NAP_NULL;

        // Free the node 
	    FWK_Free((NAP_VOID**)&psttemp2);
	    //psttemp2 = NAP_NULL;
    }

	// Decrement the count of nodes
	(*ppstList)->usCount--;

    // If no node exists in the list, update head & tail with NULL
    if(0 == (*ppstList)->usCount)
    {
        (*ppstList)->pstHead = NAP_NULL;
		(*ppstList)->pstTail = NAP_NULL;
        
        // Free the parent list 
		FWK_Free((NAP_VOID**)ppstList);
		*ppstList = NAP_NULL;
    }

    return NAP_SUCCESS;
}//End of FWK_LIST_DeleteNode

/*
*******************************************************************************
* Function name	: FWK_LIST_UpdateNode
* Description		: This function updates a node in the list.
*
* Return type		: NAP_BOOL
*			      	NAP_SUCCESS, if success else NAP_FAILURE.
*                  
* Argument     	:  
*    [IN]			: pstList - List in which the node must be updated.		
*    [IN]			: usNodePos - The node to be updated.
*    [IN]			: pvNewData - The data to replace the existing one in the node.
*    [OUT]		: piError -  The error type while returning NAP_FAILURE.  
*
* Side Effects		: None
* NOTE 			: None
*******************************************************************************
*/
NAP_BOOL FWK_LIST_UpdateNode
(
	S_FWK_LIST			*pstList,
	NAP_UINT16			usNodePos,
	NAP_VOID			*pvNewData,
	NAP_INT16			*piError
)
{
	NAP_UINT16		uiLoopCount	= 0;
	S_FWK_LIST_NODE	*psttemp1	= NAP_NULL;


	// Assert Input parameters
	if( (NAP_NULL == pstList ) || (NAP_NULL == pvNewData) || 
        (0 == usNodePos ) )
	{
		return NAP_FAILURE;
	}

    if(usNodePos > pstList->usCount)
    {
        return NAP_FAILURE;
    }

	// Initialising temporary variable with list head pointer
	psttemp1 = pstList->pstHead;

	for(uiLoopCount=1;uiLoopCount<usNodePos;uiLoopCount++)
	{
		psttemp1 = psttemp1->pstNext;
	}
	
    psttemp1->pvData = pvNewData;
		
	return NAP_SUCCESS;
}//End of FWK_LIST_UpdateNode

/*
*******************************************************************************
* Function name	: FWK_LIST_AccessNode
* Description		: This function fetches data from a node in the list.
*
* Return type		: NAP_BOOL
*			      NAP_SUCCESS, if success else NAP_FAILURE.
* Argument     	 :  
*    [IN]			: pstList - List in which the node must be accessed.		
*    [IN]			: usNodePos- The node to be updated.
*    [OUT]		: pvData - Pointer to date hold by the node.
*    [OUT]		: piError - The error type while returning NAP_FAILURE.  
*
* Side Effects		: None
* NOTE 			: None
*******************************************************************************
*/
NAP_BOOL FWK_LIST_AccessNode
(
	S_FWK_LIST			*pstList,
	NAP_UINT16			usNodePos,
	NAP_VOID			**ppvData,
	NAP_INT16			*piError
)
{
	NAP_UINT16 uiLoopCount		= 0;
	S_FWK_LIST_NODE *psttemp1	= NAP_NULL;


	// Assert Input parameters
	if( (NAP_NULL == pstList) || (NAP_NULL == ppvData) || 
        (0 == usNodePos) )
	{
		return NAP_FAILURE;
	}

    if(usNodePos > pstList->usCount)
    {
        return NAP_FAILURE;
    }

	// Initialising temporary variable with list head pointer
	psttemp1 = pstList->pstHead;

	for(uiLoopCount = 1 ; uiLoopCount < usNodePos ; uiLoopCount++ )
	{
		psttemp1 = psttemp1->pstNext;
	}
	*ppvData = psttemp1->pvData;

	return NAP_SUCCESS;

}//End of FWK_LIST_AccessNode

/*
*******************************************************************************
* Function name	: FWK_LIST_GetCount
* Description		: This function returns number of nodes in the list.
*
* Return type		: NAP_BOOL
*			      	NAP_SUCCESS, if success else NAP_FAILURE.
* Argument      	:  
*    [IN]			: pstList - List whose node count is required.		
*    [OUT]		: pusNumofNodes - Number of nodes in the List.
*    [OUT]		: piError - Error type while returning NAP_FAILURE.  
*
* Side Effects		: None
* NOTE 			: None
*******************************************************************************
*/
NAP_BOOL  FWK_LIST_GetCount
(
    S_FWK_LIST		*pstList,
    NAP_UINT16		*pusNumofNodes, 
    NAP_INT16		*piError
)
{

    if(NAP_NULL == pstList)
    {
		*pusNumofNodes = 0;
		return NAP_FAILURE;
	}
	
	*pusNumofNodes = pstList->usCount;

  	return NAP_SUCCESS;;

}//End of FWK_LIST_GetCount


/* This will remove all elements from the list one by one and
call specified free function on the elements. If free function
is not specified it will not be called on elements but elements
will be removed and the list will be freed. 
*/
NAP_BOOL  FWK_LIST_DeleteList
(
    S_FWK_LIST					**ppstList,
	FWK_LIST_ElemFreeFunc		fpFreeElemFunc,
    NAP_INT16					*piError
)
{
	NAP_UINT16  usNumofNodes;
	/* Checking for the List existence */
	if((NAP_NULL == ppstList) || (NAP_NULL == *ppstList))
	{
		return NAP_FAILURE;
	}

    if((FWK_LIST_GetCount(*ppstList,
		&usNumofNodes,piError)) && (usNumofNodes > 0))
    {
        NAP_VOID *pvElement = NAP_NULL;

        while(usNumofNodes)
		{
            if(NAP_SUCCESS == FWK_LIST_DeleteNode(
                ppstList,
                1,
                &pvElement,
                piError))
            {
                if((NAP_NULL != fpFreeElemFunc) && (NAP_NULL != pvElement))
                {
                    fpFreeElemFunc(&pvElement);
                }
            }

            usNumofNodes--;
        }
    }

	return NAP_SUCCESS;

}


/* This will insert the new node and shift accordingly all the
existing nodes.This API is different from FWK_LIST_AddNode as
FWK_LIST_AddNode always adds the element in the end of the
list whereas this API can add element at the specified index.
This API can be used to implement Priority Queues.
*/
NAP_BOOL  FWK_LIST_InsertNode
(
	S_FWK_LIST			**ppstList,
	NAP_UINT16			usNodeInsertPos,	
	NAP_VOID			*pvData,
	NAP_INT16			*piError
)
{
	NAP_BOOL bRetVal = NAP_FAILURE;

	/* if this is the first element on an empty list */
	if((NAP_NULL == *ppstList) && (1 == usNodeInsertPos))
	{
		bRetVal = FWK_LIST_AddNode(ppstList,pvData,piError);
	}
	else if(NAP_NULL != *ppstList) /* Non Empty List */
	{
		if(1 == usNodeInsertPos) /* Insert in the beginning */
		{
			S_FWK_LIST_NODE *pstFirstNode 
				= (S_FWK_LIST_NODE *) FWK_Malloc (sizeof(S_FWK_LIST_NODE));

	        if(NAP_NULL != pstFirstNode)
			{
				pstFirstNode->pvData	= pvData;
				/* assign first nodes next element as the existing first element */
				pstFirstNode->pstNext	= (*ppstList)->pstHead;
				/* assign list head */
				(*ppstList)->pstHead	= pstFirstNode;
				/* Increment node count */
				(*ppstList)->usCount++;

				bRetVal = NAP_SUCCESS;
			}

		}
		else if(usNodeInsertPos <= (*ppstList)->usCount) /* Insert somewhere */
		{
			NAP_UINT16 uiLoopCount			= 0;
			S_FWK_LIST_NODE *pstPrevNode	= NAP_NULL;
			S_FWK_LIST_NODE *pstInsertNode; 

			// Initialising temporary variable with list head pointer
			pstPrevNode = (*ppstList)->pstHead;

			/* getting the previous existing node and adjust it */
			for(uiLoopCount=1;
				uiLoopCount < (usNodeInsertPos - 1); 
				uiLoopCount++)
			{
				pstPrevNode = pstPrevNode->pstNext;
			}
			
			
			pstInsertNode = 
				(S_FWK_LIST_NODE *) FWK_Malloc (sizeof(S_FWK_LIST_NODE));

	        if(NAP_NULL != pstInsertNode)
			{
				pstInsertNode->pvData	= pvData;
				/* assign first nodes next element as the existing first element */
				pstInsertNode->pstNext	= pstPrevNode->pstNext;
				/* assign inserted node */
				pstPrevNode->pstNext	= pstInsertNode;
				/* Increment node count */
				(*ppstList)->usCount++;

				bRetVal = NAP_SUCCESS;
			}

		}
	}

	return bRetVal;
}
