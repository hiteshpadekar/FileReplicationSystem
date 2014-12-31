
#ifndef _FWK_LIST_H_
#define _FWK_LIST_H_

/*
*******************************************************************************
*                               INCLUDE FILES
*******************************************************************************
*/

#include "fwk_memory.h"
#include "fwk_datatypes.h"
//#include "nap_error.h" /* commented by Pawan 30 Nov 2004 */

/*
*******************************************************************************
*                               STRUCTURE DEFINITIONS
*******************************************************************************
*/

/* Structure of Node */
typedef struct _S_FWK_LIST_NODE
{
	NAP_VOID    			*pvData;	//Void type data 					
	struct _S_FWK_LIST_NODE	*pstNext;   //Pointer to the next node

}S_FWK_LIST_NODE;

/* Structure of List */
typedef struct _S_FWK_LIST
{
	S_FWK_LIST_NODE			*pstHead;   //First Node
    S_FWK_LIST_NODE			*pstTail;	//Last Node
	NAP_UINT16				usCount;	//Node Count
}S_FWK_LIST;

/*
 * The function to free the elements stored in the list.
 */
typedef NAP_VOID (*FWK_LIST_ElemFreeFunc) (NAP_VOID **pElement);

/*
*******************************************************************************
*                          INTERFACE FUNCTION PROTOTYPES
*******************************************************************************
*/

#ifdef __cplusplus    // If used by C++ code, 
extern "C" {          // we need to export the C interface
#endif

NAP_BOOL FWK_LIST_AddNode
(
	S_FWK_LIST		**ppstList,
	NAP_VOID		*pvData,	
	NAP_INT16		*piError
);

NAP_BOOL FWK_LIST_DeleteNode
(
	S_FWK_LIST			**pstList,
	NAP_UINT16			usNodePos,	
	NAP_VOID			**ppvData,
	NAP_INT16			*piError
);

NAP_BOOL FWK_LIST_UpdateNode
(
	S_FWK_LIST			*pstList,
	NAP_UINT16			usNodePos,
	NAP_VOID			*pvNewData,
	NAP_INT16			*piError
);
	
NAP_BOOL FWK_LIST_AccessNode
(
	S_FWK_LIST			*pstList,
	NAP_UINT16			usNodePos,
	NAP_VOID			**pvData,
	NAP_INT16			*piError
);


NAP_BOOL  FWK_LIST_GetCount
(
    S_FWK_LIST			*pstList,
    NAP_UINT16			*pusNumofNodes, 
    NAP_INT16			*piError
);

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
);


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
);

#ifdef __cplusplus
}
#endif

#endif // End of _FWK_LIST_H_

	
