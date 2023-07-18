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

static inline rtsha_btree_node* rtsha_btree_node_create(size_t address, uint64_t key)
{
    rtsha_btree_node* node = (rtsha_btree_node*)(void*)(address);
    node->key       = key;
    node->left      = NULL;
    node->right     = NULL;
    node->parent    = NULL;
    node->balance   = 0;
    node->height    = 1;
    return node;
}

static int16_t balance_factor(rtsha_btree_node* root)
{
    int16_t lh, rh;
    if (root == NULL)
    {
        return 0;
    }
    if (root->left == NULL)
    {
        lh = 0;
    }
    else
    {
        lh = root->left->height;
    }
    if (root->right == NULL)
    {
        rh = 0;
    }
    else
    {
        rh = root->right->height;
    }
    return lh - rh;
}

static int16_t balance(rtsha_btree_node* node)
{
    int16_t lh, rh;
    if (node == NULL)
    {
        return 0;
    }
    if (node->left == NULL)
    {
        lh = 0;
    }
    else
    {
        lh = 1 + node->left->balance;
    }
    if (node->right == NULL)
    {
        rh = 0;
    }
    else
    {
        rh = 1 + node->right->balance;
    }

    if (lh > rh)
    {
        return (lh);
    }
    return (rh);
}

static inline rtsha_btree_node* btree_left_left_rotate(rtsha_btree_node* node, rtsha_btree_node* parent)
{
    rtsha_btree_node* temp = NULL;

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

        node->balance = balance(node);
        temp->balance = balance(temp);

        return temp;
    }
    return NULL;
}

static inline rtsha_btree_node* btree_node_right_right_rotate(rtsha_btree_node* node, rtsha_btree_node* parent)
{
    rtsha_btree_node* temp = NULL;
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

        node->balance = balance(node);
        temp->balance = balance(temp);
                
        return temp;
    }
    return NULL;
}

static void btree_balance(rtsha_btree_node** node, rtsha_btree_node* inserted_node)
{
    rtsha_btree_node* root;
    rtsha_btree_node* root_new = NULL;
    root = *node;

    if (root->balance == 2)
    {
        rtsha_btree_node* left_child = root->left;
        if ((left_child != NULL) &&
            (abs(left_child->balance) == 1) &&
            (inserted_node != NULL) &&
            (left_child->right != NULL ) &&
            (left_child->right == inserted_node) )            
        {
            root->left = btree_node_right_right_rotate(left_child, root->parent);
            root_new = btree_left_left_rotate(root, root->parent);

            if (root_new->parent != NULL)
            {
                root_new->parent->left = root_new;
            }
            left_child->height--;
            root->height = root->height - 2;

            if (root->height <= 0)
            {
                root->height = 1;
            }
            if (left_child->height <= 0)
            {
                left_child->height = 1;
            }

            root_new->height++;
         }
        else
        {
            root_new = btree_left_left_rotate(root, root->parent);        
            root->height = root->height - 2;
            if (root->height <= 0)
            {
                root->height = 1;
            }
        }
    }
    else if (root->balance == -2)
    {
        rtsha_btree_node* right_child = root->right;
        if ((right_child != NULL) && 
            //(right_child->balance == -1) &&            
            (inserted_node != NULL) &&
            (right_child->left != NULL) &&
            (right_child->left == inserted_node))
        {
            root->right = btree_left_left_rotate(right_child, root->parent);
            root_new = btree_node_right_right_rotate(root, root->right->parent);

            root->height = root->height - 2;
            if (root->height <= 0)
            {
                root->height = 1;
            }
            root_new->height++;
            right_child->height--;
            if (right_child->height <= 0)
            {
                right_child->height = 1;
            }
        }
        else
        {
            root_new = btree_node_right_right_rotate(root, root->parent);
           // root_new->height = root->height + 1;
            root->height = root->height - 2;
            if (root->height <= 0)
            {
                root->height = 1;
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


static inline void find_new_root(rtsha_btree_node* node, const uint8_t h)
{
    rtsha_btree_node* root = node;
    while (root != NULL)
    {
        if (root->parent == NULL)
        {
            _btree[h].root = root;
            _btree[h].root->parent = NULL;
            break;
        }
        root = root->parent;
    }
}

bool btree_node_insert(HBTREE h, size_t address, uint64_t key)
{
    rtsha_btree_node* pNode, *temp;
    int16_t height = 1;
    bool balanced = false;

    pNode = NULL;
    temp = NULL;

    if (h >= MAX_BTREES)
    {
        return false;
    }


    if (key == 0x51)
    {
        key = 0x50;
    }

    if (_btree[h].root == NULL || _btree[h].root->key == 0U)
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

        temp = pNode;        

        if (temp->right == temp)
        {
            int a = 0;
            a++;
        }

        balanced = false;
        while (temp != NULL)
        {           
            temp->height = height;
            temp->balance = balance_factor(temp);

            if ( ((temp->balance < -1) || (temp->balance > 1)) && (!balanced))
            {
                btree_balance(&temp, pNode);
                balanced = true;
            }
            if (temp == temp->parent)
            {
                /*something is not good*/
                temp->parent = NULL;
                break;
            }
            temp = temp->parent;
            height++;
        }

        find_new_root(pNode, h);
        return true;
    }
    return false;
}

bool btree_node_delete(HBTREE h, size_t address, uint64_t key)
{
    rtsha_btree_node* successor, * successor_parent;
    rtsha_btree_node* parent, *temp;
    rtsha_btree_node* left_child;
    rtsha_btree_node* right_child;
    rtsha_btree_node* pNode = (rtsha_btree_node*)address;
    successor = NULL;
    successor_parent = NULL;
    left_child = NULL;
    right_child = NULL;
    int16_t temphl = 0;
    int16_t temphr = 0;

    int16_t height = 1;
    bool balanced = false;

    if (h >= MAX_BTREES || _btree[h].root == NULL)
    {
        return false;
    }
    if (pNode->parent == NULL)
    {
        if (pNode != _btree[h].root)
        {
            /*something is not good
            */
            return false;
        }
       
    }

    /*
    if (_btree[h].root->key == key)
    {

        _btree[h].root->key = 0;
        _btree[h].root->left = NULL;
        _btree[h].root->right = NULL;
        _btree[h].root = NULL;
        return true;
    }
    */

    parent = pNode->parent;

    if (pNode->key != key)
    {
        return false;
    }

    temp = NULL;

    /*Case 1: Node has no children (leaf node)*/
    if ((pNode->right == NULL) && (pNode->left == NULL))
    {
        if (parent != NULL)
        {
            if ((parent->left != NULL) &&(parent->left == pNode) )
            {
                parent->left = NULL;
            }
            else if ((parent->right != NULL) && (parent->right == pNode))
            {
                parent->right = NULL;
            }
            if ((parent->right == NULL) && (parent->left == NULL))
            {
                parent->height = 1;
            }
            parent->balance = balance_factor(parent);
            if ( (parent->balance < -1) || (parent->balance > 1) )
            {
                btree_balance(&parent, NULL);
                /*we are done*/             
  
            }
            else
            {              
                find_new_root(parent, h);
            }
            if (parent == pNode)
            {
                int a = 0;
                a++;
            }
            temp = parent;
           
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

        if ((pNode != NULL) && (successor != NULL))
        {

            if (successor->right != NULL)
            {
                successor->right->parent = successor_parent;
            }

            if (pNode->left != NULL)
            {
                successor->left = pNode->left;
            }
            if (pNode->right != NULL)
            {
                successor->right = pNode->right;
            }
            successor->parent = parent;
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
                temp = successor;
            }


            pNode->key = 0;
            pNode->key = 0;
            pNode->parent = NULL;
            pNode->left = NULL;
            pNode->right = NULL;
            pNode->balance = 0;
            pNode->height = 0;

            while (temp != NULL)
            {
                temp->height = height;
                temp->balance = balance_factor(temp);

                if (((temp->balance < -1) || (temp->balance > 1)) && (!balanced))
                {
                    btree_balance(&temp, pNode);
                    break;
                }
                temp = temp->parent;
                height++;
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
                    /*connect parent right with right child of the node to be deleted*/
                    parent->right = pNode->right;

                    if (parent->right != NULL)
                    {
                        parent->right->height = 1;
                    }
                   
                    temp = parent->right;
                    if (parent->left != NULL)
                    {
                        temp = parent->left;
                    }
                }
                else if (parent->left == pNode)
                {
                    /*connect parent left with left child of the node to be deleted*/
                    parent->left = pNode->left;
                    parent->left->height = 1;
                }
                /*calculate balance factor of the parent*/
                parent->balance = balance_factor(parent);
                if ((parent->balance < -1) || (parent->balance > 1))
                {
                    btree_balance(&parent, NULL);                    
                }
                temp = parent;
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
                    /*connect parent right with right child of the node to be deleted*/
                    parent->right = pNode->right;

                    if (parent->right != NULL)
                    {
                        parent->right->height = 1;
                    }

                    temp = parent->right;
                    if (parent->left != NULL)
                    {
                        temp = parent->left;
                    }
                }
                else if (parent->left == pNode)
                {
                    /*connect parent left with left child of the node to be deleted*/
                    parent->left = pNode->right;
                    parent->left->height = 1;
                }
                /*calculate balance factor of the parent*/
                parent->balance = balance_factor(parent);
                if ((parent->balance < -1) || (parent->balance > 1))
                {
                    btree_balance(&parent, NULL);
                }
                temp = parent;
            }
            else
            {
                pNode->right->parent = NULL;
            }
        }       
    }
    /*find longest path down*/
    while (temp != NULL)
    {
        temphr = 0;
        temphl = 0;

        if (temp->right != NULL)
        {
            temphr = temp->right->height;
        }

        if (temp->left != NULL)
        {
            temphl = temp->left->height;
        }
        if ((temphr == 0) && (temphl == 0))
        {
            break;
        }
        if (temphr > temphl)
        {
            temp = temp->right;
        }
        else
        {
            temp = temp->left;
        }
    }
    /*set height*/

    balanced = false;
    while (temp != NULL)
    {
        temp->height = height;
        temp->balance = balance_factor(temp);

        if (((temp->balance < -1) || (temp->balance > 1)) && (!balanced))
        {
            btree_balance(&temp, pNode);
            balanced = true;
        }
        if (temp == temp->parent)
        {
            /*something is not good*/
            break;
        }
        temp = temp->parent;
        height++;
    }
    
    find_new_root(pNode, h);
    return true;
}


rtsha_btree_node* btree_seek(HBTREE h, uint64_t key)
{
    rtsha_btree_node* pNode;

    if (h >= MAX_BTREES)
    {
        return NULL;
    }
    pNode = _btree[h].root;

    if ((NULL == pNode) || pNode->key == 0U)
    {
        return NULL;
    }

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
    if (pNode != NULL)
    {
        if (pNode->parent == NULL)
        {
            return pNode;
        }
        if (pNode->key == key)
        {
            return pNode;
        }
        return pNode->parent;
    }
    return NULL;
}

void btree_reinit(HBTREE h)
{
    _btree[h].root = NULL;
}