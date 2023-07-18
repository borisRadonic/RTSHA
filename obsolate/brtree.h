#pragma once


#include <assert.h>
#include "structures.h"
#include <math.h>

static struct BtreeStruct _btree[MAX_BTREES];
static uint8_t _last_btree_num = 0U;



HBTREE rtsha_btree_create();
bool btree_node_insert(HBTREE h, size_t address, uint64_t key);
bool btree_node_delete(HBTREE h, size_t address, uint64_t key);
rtsha_btree_node* btree_seek(HBTREE h, uint64_t key);
void btree_reinit(HBTREE h);


