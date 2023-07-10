#pragma once


#include <assert.h>
#include "structures.h"
#include <math.h>

static struct BtreeStruct _btree[MAX_BTREES];
static uint8_t _last_btree_num = 0U;

#define HBTREE uint8_t
#define INVALID_BTREE_HANDLE MAX_BTREES

static inline HBTREE rtsha_btree_create()
{
    size_t index;
    if (_last_btree_num >= MAX_BTREES)
    {
        return INVALID_BTREE_HANDLE;
    }
    _btree[_last_btree_num].index = 0U;

    index = _btree[_last_btree_num].index;

    _btree[_last_btree_num].root = NULL;
    _btree[_last_btree_num].ptr_btree_nodes[index] = NULL;

    _last_btree_num++;
    return (_last_btree_num - 1U);
}

static inline rtsha_btree_node* rtsha_btree_node_create(size_t address, uint32_t key)
{
    rtsha_btree_node* node = (rtsha_btree_node*)(void*)(address);

    node->key = key;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->balance = 0;
    return node;
}

static inline rtsha_btree_node* btree_node_right_rotate(rtsha_btree_node* node, rtsha_btree_node* parent)
{
    rtsha_btree_node* temp;

    if (node != NULL)
    {
        temp = node->left;
        node->left = temp->right;

        temp->right = node;

        if (temp->right != NULL)
        {
            temp->right->parent = temp;
            if (temp->parent == temp->right)
            {
                temp->parent = parent;
            }
        }
        if (temp->parent != NULL)
        {
            if (temp->right == node)
            {
                if (temp->key < temp->parent->key)
                {
                    temp->parent->left = temp;
                }
                else
                {
                    temp->parent->right = temp;
                }
            }
        }
        return temp;
    }
    return NULL;
}

static inline rtsha_btree_node* btree_node_left_rotate(rtsha_btree_node* node, rtsha_btree_node* parent)
{
    rtsha_btree_node* temp;
    if (node != NULL)
    {
        temp = node->right;
        node->right = temp->left;

        temp->left = node;

        if (temp->left != NULL)
        {
            temp->left->parent = temp;
            if (temp->parent == temp->left)
            {
                temp->parent = parent;
            }
        }
        
        if (temp->parent != NULL)
        {         
            if (temp->left = node)
            {
                if (temp->key >= temp->parent->key)
                {
                    temp->parent->right = temp;
                }
                else
                {
                    temp->parent->left = temp;
                }
            }            
        }
        
        return temp;
    }
    return NULL;
}

static void btree_balance(rtsha_btree_node** node)
{
    rtsha_btree_node* root;
    rtsha_btree_node* root_new = NULL;
    root = *node;

    if (root->balance == -2)
    {
        rtsha_btree_node* left_child = root->left;

        if (left_child->balance == 1)
        {
             root->left = btree_node_left_rotate(left_child, root->parent);
            root_new = btree_node_right_rotate(root, root->parent);

            if (root_new->parent != NULL)
            {
                root_new->parent->left = root_new;
            }

            if (root_new->balance == -1)
            {
                left_child->balance = 0;
                root->balance = 1;
            }
            else if (root_new->balance == 0)
            {
                left_child->balance = 0;
                root->balance = 0;
            }
            else
            {
                left_child->balance = -1;
                root->balance = 0;
            }
            root_new->balance = 0;
        }
        else
        {
            
            root_new = btree_node_right_rotate(root, root->parent);

            if (root_new->balance == 0)
            {
                root_new->balance = 1;
                root->balance = -1;
            }
            else
            {
                root_new->balance = 0;
                root->balance = 0;
            }
        }
    }
    else if (root->balance == 2)
    {
        rtsha_btree_node* right_child = root->right;
        if (right_child->balance == -1)
        {
            root->right = btree_node_right_rotate(right_child, root->parent);
            root_new = btree_node_left_rotate(root, root->right->parent);
             
            if (root_new->balance == 1)
            {
                right_child->balance = 0;
                root->balance = -1;
            }
            else if (root_new->balance == 0)
            {
                right_child->balance = 0;
                root->balance = 0;
            }
            else
            {
                right_child->balance = 1;
                root->balance = 0;
            }
            root_new->balance = 0;
        }
        else
        {
            root_new = btree_node_left_rotate(root, root->parent);

            if (root_new->balance == 0)
            {
                root_new->balance = -1;
                root->balance = 1;
            }
            else
            {
                root_new->balance = 0;
                root->balance = 0;
            }
        }
    }
    else
    {
        return;
    }
    *node = root_new;
}

static inline rtsha_btree_node* balance_after_insert(HBTREE h, rtsha_btree_node* node)
{
    rtsha_btree_node** current;
    rtsha_btree_node** child;
    size_t index = 0U;
    current = NULL;

    if ((node != NULL) && (h < MAX_BTREES))
    {
        while (_btree[h].index >= 1U)
        {
            index = _btree[h].index;
            current = &_btree[h].ptr_btree_nodes[index - 1];
            child = &_btree[h].ptr_btree_nodes[index];
            if ((*current == NULL) || (*child == NULL))
            {
                return NULL;
            }

            if ((*current)->left == *child)
            {
                (*current)->balance--;
            }
            else
            {
                (*current)->balance++;
            }
            (*child)->parent = (*current);

            btree_balance(current);

            if ((*current)->balance == 0)
            {
                break;
            }
            _btree[h].index--;
        }
        if ((current != NULL) && (*current != NULL))
        {
            return *current;
        }
    }
    return NULL;
}

static bool btree_node_insert(HBTREE h, size_t address, uint32_t key)
{
    rtsha_btree_node* pNode, * newNode, * temp;
    size_t index = 0U;;
    pNode = NULL;
    newNode = NULL;
    temp = NULL;

    if (h >= MAX_BTREES)
    {
        return false;
    }

    if (_btree[h].root == NULL)
    {
        _btree[h].root = rtsha_btree_node_create(address, key);
        index = _btree[h].index;
        if (index >= MAX_BTREE_HEIGHT)
        {
            return false;
        }
        return (NULL != _btree[h].root);
    }
    pNode = _btree[h].root;

    if (pNode == NULL)
    {
        return false;
    }
    _btree[h].index = 0;
    while (pNode != NULL)
    {
        if (key < pNode->key)
        {
            index = _btree[h].index;
            _btree[h].ptr_btree_nodes[index] = pNode;
            if (pNode->left != NULL)
            {
                pNode->left->parent = pNode;
                pNode = pNode->left;
            }
            else
            {
                break;
            }
        }
        else if (key >= pNode->key)
        {
            index = _btree[h].index;
            _btree[h].ptr_btree_nodes[index] = pNode;
            if (pNode->right != NULL)
            {
                pNode->right->parent = pNode;
                pNode = pNode->right;
            }
            else
            {
                break;
            }
        }

        _btree[h].index++;

    }

    if ((pNode != NULL) && (key < pNode->key))
    {
        pNode->left = rtsha_btree_node_create(address, key);
        pNode->left->parent = pNode;

        pNode = pNode->left;

        _btree[h].index++;
        index = _btree[h].index;
        _btree[h].ptr_btree_nodes[index] = pNode;

    }
    else if (key >= pNode->key)
    {
        pNode->right = rtsha_btree_node_create(address, key);
        pNode->right->parent = pNode;

        pNode = pNode->right;

        _btree[h].index++;
        index = _btree[h].index;
        _btree[h].ptr_btree_nodes[index] = pNode;
    }
    newNode = balance_after_insert(h, pNode);
    /*find new root*/
    while (pNode != NULL)
    {
        if (pNode->parent == NULL)
        {
            _btree[h].root = pNode;
            _btree[h].root->parent = NULL;
            break;
        }
        pNode = pNode->parent;
    }
}
