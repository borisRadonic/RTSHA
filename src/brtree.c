#include "allocator.h"
#include "errors.h"
#include "brtree.h"


HBTREE rtsha_btree_create()
{
    if (_last_btree_num >= MAX_BTREES)
    {
        return INVALID_BTREE_HANDLE;
    }

    _btree[_last_btree_num].root = NULL;

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
/*
int32_t static height(rtsha_btree_node* node)
{
    if (node == NULL)
        return -1;
    return node->height;
}
*/
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

        if (node->right != NULL)
        {
            if (node->right->parent != node)
            {
                node->right->parent = node;
            }
        }

        if (node->left != NULL)
        {
            if (node->left->parent != node)
            {
                node->left->parent = node;
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
            if (temp->left == node)
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

        if (node->right != NULL)
        {
            if (node->right->parent != node)
            {
                node->right->parent = node;
            }
        }

        if (node->left != NULL)
        {
            if (node->left->parent != node)
            {
                node->left->parent = node;
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

    if ((*node)->key == 82)
    {
        int a = 1;
        a++;
    }

    if (root->balance == 2)
    {
        rtsha_btree_node* left_child = root->left;

        if ((left_child != NULL) && (left_child->balance == -1))
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

            if (root_new->balance == 1)
            {
                root_new->balance = 0;
                root->balance = 0;
                
            }
            else
            {
                root_new->balance = 0;
                root->balance = 0;
            }
            if ((root->left == NULL) && (root->right == NULL))
            {
                //root->height = 0;
            }
        }
    }
    else if (root->balance == -2)
    {
        rtsha_btree_node* right_child = root->right;
        if ((right_child != NULL) && (right_child->balance == 1))
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
                /*standard case*/
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
                /*standard case*/
                root_new->balance = 0;
                root->balance = 0;
            }
        }
        if ((root->left == NULL) && (root->right == NULL))
        {
            root->balance = 0;
        }
    }
    else
    {
        return;
    }
    *node = root_new;
}

bool btree_node_insert(HBTREE h, size_t address, uint32_t key)
{
    rtsha_btree_node* pNode, * temp;
    int32_t tempH = 0;

    pNode = NULL;
    temp = NULL;

    if (h >= MAX_BTREES)
    {
        return false;
    }

    if (_btree[h].root == NULL)
    {
        _btree[h].root = rtsha_btree_node_create(address, key);
        return (NULL != _btree[h].root);
    }
    pNode = _btree[h].root;

    if (pNode != NULL)
    {
        while (pNode != NULL)
        {
            if (key < pNode->key)
            {
                if (pNode->left != NULL)
                {
                    pNode = pNode->left;
                }
                else
                {
                    break;
                }
            }
            else if (key >= pNode->key)
            {
                if (pNode->right != NULL)
                {
                    pNode = pNode->right;
                }
                else
                {
                    break;
                }
            }
        }

        if ((pNode != NULL) && (key < pNode->key))
        {
            pNode->left = rtsha_btree_node_create(address, key);
            pNode->left->parent = pNode;
            pNode = pNode->left;
        }
        else if (key >= pNode->key)
        {
            pNode->right = rtsha_btree_node_create(address, key);
            pNode->right->parent = pNode;
            pNode = pNode->right;
        }

        /*After the Insert, go back up to the root node by node, updating heights*/

        int hl, hr;
        temp = pNode;
        while (temp != NULL)
        {                                   
            if (temp->left != NULL)
            {
                hl = tempH-1;
            }
            else
            {
                hl = -1;
            }

            if (temp->right != NULL)
            {
                hr = tempH-1;
            }
            else
            {
                hr = -1;
            }
            temp->balance = -(hr - hl);
            tempH++;
            if ((temp->balance < -1) || (temp->balance > 1))
            {
                btree_balance(&temp);
                if (temp->balance == 0)
                {
                    break;
                }
            }
            temp = temp->parent;
        }

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
        return true;
    }
    return false;
}

static rtsha_btree_node* find_last_leaf_right(rtsha_btree_node* node)
{
    if (node == NULL)
        return NULL;

    while (node->right != NULL)
        node = node->right;

    return node;
}

bool btree_node_delete(HBTREE h, size_t address, uint32_t key)
{
    rtsha_btree_node* successor, * successor_parent;
    rtsha_btree_node* parent;
    rtsha_btree_node* left_child;
    rtsha_btree_node* right_child;
    rtsha_btree_node* pNode = (rtsha_btree_node*)address;
    successor = NULL;
    successor_parent = NULL;
    left_child = NULL;
    right_child = NULL;



    if (h >= MAX_BTREES || _btree[h].root == NULL)
    {
        return false;
    }
    parent = pNode->parent;

    if (pNode->key != key)
    {
        return false;
    }

    /*Case 1: Node has no children (leaf node)*/
    if ((pNode->right == NULL) && (pNode->left == NULL))
    {
        if (parent != NULL)
        {
            if (parent->left == pNode)
            {
                parent->left = NULL;
            }
            else if (parent->right == pNode)
            {
                parent->right = NULL;
            }
        }
        else
        {
            _btree[h].root = NULL;
        }
    }
    else if ((pNode->right != NULL) && (pNode->left != NULL))
    {
        /*Case 2: Node has two children
            Find the successor
        */

        successor = pNode->left;
        successor_parent = pNode;

        while (successor->right != NULL)
        {
            successor_parent = successor;
            successor = successor->right;
        }

        if (parent == NULL)
        {
            _btree[h].root = successor;
        }
        else if (parent->left == pNode)
        {
            parent->left = successor;
        }
        else
        {
            parent->right = successor;
        }
        if (successor_parent != NULL)
        {
            if (successor_parent->left == successor)
            {
                successor_parent->left = successor->left;
            }
            else
            {
                successor_parent->right = successor->right;
            }
        }

        if (successor->right != NULL)
        {
            successor->right->parent = successor_parent;
        }

        if ((pNode != NULL) && (successor != NULL))
        {
            if (pNode->left != NULL)
            {
                successor->left = pNode->left;
            }
            if (pNode->right != NULL)
            {
                successor->right = pNode->right;
            }
        }
        if (successor != NULL)
        {
            successor->parent = parent;
        }
        if (pNode != NULL)
        {
            if (pNode->left != NULL)
            {
                pNode->left->parent = successor;
            }

            if (pNode->right != NULL)
            {
                pNode->right->parent = successor;
            }
        }
    }
    else
    {
        /* Case 3: Node has one child*/
        if ((pNode->right == NULL) && (pNode->left != NULL))
        {
            if (parent != NULL)
            {
                if (parent->right == pNode)
                {
                    parent->right = pNode->left;
                }
                else if (parent->left == pNode)
                {
                    parent->left = pNode->left;
                }
            }
            else
            {
                pNode->left->parent = NULL;
            }
        }
        else if ((pNode->right != NULL) && (pNode->left == NULL))
        {
            if (parent != NULL)
            {
                if (parent->right == pNode)
                {
                    parent->right = pNode->right;
                }
                else if (parent->left == pNode)
                {
                    parent->left = pNode->right;
                }
            }
            else
            {
                pNode->right->parent = NULL;
            }
        }

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
        return true;
    }
    return false;
}