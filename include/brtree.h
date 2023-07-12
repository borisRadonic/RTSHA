#pragma once


#include <assert.h>
#include "structures.h"
#include <math.h>

static struct BtreeStruct _btree[MAX_BTREES];
static uint8_t _last_btree_num = 0U;

#define HBTREE uint8_t
#define INVALID_BTREE_HANDLE MAX_BTREES

HBTREE rtsha_btree_create();
bool btree_node_insert(HBTREE h, size_t address, uint32_t key);
bool btree_node_delete(HBTREE h, size_t address, uint32_t key);