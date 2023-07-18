#include "internal.h"
#include "errors.h"

static uint8_t _last_free_list_num = 0U;

uint8_t free_list_create()
{
    if (_last_free_list_num >= MAX_PAGES)
    {
        return RTSHA_FreeListInvalidHandle;
    }
    _last_free_list_num++;
    return (_last_free_list_num - 1U);
}

static inline rtsha_free_list_node* free_list_node_create(size_t address)
{
    rtsha_free_list_node* node = (rtsha_free_list_node*)(void*)(address);
    node->next = NULL;
    return node;
}

bool free_list_insert(uint8_t h, size_t address, rtsha_free_list_node** free_list_ptr, rtsha_free_list_node** last_free_list_ptr)
{
    rtsha_free_list_node* pNode;
    pNode = NULL;
  
    if ( (h >= MAX_PAGES) || (free_list_ptr == NULL) || (last_free_list_ptr == NULL) )
    {
        return false;
    }

    pNode = free_list_node_create(address);
    
    if (*free_list_ptr == NULL )
    {
        *free_list_ptr = pNode;
        *last_free_list_ptr = pNode;
        return true;
    }
    /*update old last next*/
    (*last_free_list_ptr)->next = pNode;
    /*set new last*/
    (*last_free_list_ptr) = pNode;
     
       
    return true;

}

void free_list_delete(uint8_t h, size_t address, rtsha_free_list_node** free_list_ptr, rtsha_free_list_node** last_free_list_ptr)
{
    rtsha_free_list_node* pNode = (rtsha_free_list_node*)(void*)address;
    rtsha_free_list_node* temp;

    size_t adr = 0U;
    temp = NULL;


    if( (h < MAX_PAGES) && (pNode != NULL) && (*last_free_list_ptr != NULL) && (*free_list_ptr != NULL) )
    {
        if( (pNode == *last_free_list_ptr) && (pNode == *free_list_ptr) )
        {
            /*delete first and the last list*/
            *free_list_ptr = NULL;
            *last_free_list_ptr = NULL;
            return;
        }
        else if ( (pNode != *last_free_list_ptr) && (pNode == *free_list_ptr) )
        {
            /*delete first list*/
            *free_list_ptr = (*free_list_ptr)->next;
        }
        else if ((pNode == *last_free_list_ptr) && (pNode != *free_list_ptr))
        {
            /*delete last list*/
            adr = ((size_t)(void*) (*last_free_list_ptr)) - sizeof(rtsha_free_list_node);
            temp = (rtsha_free_list_node*)(void*)adr;
            *last_free_list_ptr = temp;
        }
        else
        {
            /*delete list in the middle*/
            adr = ((size_t)(void*)pNode) - sizeof(rtsha_free_list_node);
            temp = (rtsha_free_list_node*)(void*)adr;
            temp->next = pNode->next;
        }            
    }
}