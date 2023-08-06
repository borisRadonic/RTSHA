#pragma once


typedef void (*rtshLockPagePtr)		(void);
typedef void (*rtshUnLockPagePtr)	(void);
typedef void (*rtshErrorPagePtr)	(uint32_t);

typedef struct HeapCallbacksStruct
{
	rtshLockPagePtr		ptrLockFunction;
	rtshLockPagePtr		ptrUnLockFunction;
	rtshErrorPagePtr	ptrErrorFunction;
} HeapCallbacks;

