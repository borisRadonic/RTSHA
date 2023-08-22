/*
 * Empty C++ Application
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Xilinx includes. */
#include "xil_printf.h"
#include "xtime_l.h"
#include "xpm_counter.h"
#include "xparameters.h"

#include "platform.h"

#include "allocator.h"
#include "HeapCallbacks.h"

#include "TestClass.h"

#include <list>

#include "Heap.h"

#include "semphr.h"



using namespace rtsha;

extern Heap* _heap;


static void prvTestThread(void *pvParam);
static void prvUartreceiveThread(void *pvParam);


/*-----------------------------------------------------------*/

static TaskHandle_t xThread1;
static TaskHandle_t xThread2;


uint32_t _RTSHA_HeapStart = 0x10100000; /*DDR RAM at ps7_ddr_1*/
uint32_t _RTSHA_HeapSize	= 0xFA00000;  /*256MB*/

bool heapOk;

void onHeapError( uint32_t errorCode )
{
	heapOk = false;
}

SemaphoreHandle_t xSemaphore;
SemaphoreHandle_t xSemUart;

inline void HeapLock()
{
	xSemaphoreTake(xSemaphore, portMAX_DELAY);
}

inline void HeapUnLock()
{
	xSemaphoreGive(xSemaphore);
}


int main()
{

	 init_platform();


	_heap = NULL;

	HeapCallbacksStruct callbacks;

	callbacks.ptrLockFunction = HeapLock;
	callbacks.ptrUnLockFunction = HeapUnLock;
	callbacks.ptrErrorFunction = onHeapError;

	heapOk = false;

	/*
	 * All standard heap functions (malloc...free) from the standard library and FreeRTOS
	 * have been overridden with RTSHA functions.
	 *
	 * Refer to newmalloc.cpp for details.
	 */

	/*
	 * Heap memory starts at the location 0x100000 + (Application Size, accounting for
	 * the defined Standard Heap Size and Stack Size).
	 *
	 * To sidestep issues associated with the standard linker logic (which can create large
	 * executable files by including space for heap and stack), default definitions in 'ldscript.ld'
	 * have been modified.
	 *
	 * Changes in the linker file are as follows:
	 *
	 * The original DDR RAM setting (Line: 'ps7_ddr_0 : ORIGIN = 0x100000, LENGTH = 0x3FF00000')
	 * has been updated to four new lines:
	 *      ps7_ddr_0  0x100000 0x10000000    // for this application on CPU0
	 *      ps7_ddr_1  0x10100000 0x10000000  // for RTSHA Heap used in our example (256MB)
	 *      ps7_ddr_2  0x10200000 0x10000000  // Not currently used, but available for potential use with the second CPU core
	 *      ps7_ddr_3  0x10300000 0x0FF00000  // Not currently used, but available for potential use with the second CPU core
	 *
	 * The heap used by this demo on CPU0 starts at 0x10200000 and spans 256 MB.
	 */


	/*Operators 'new' and 'delete' are overloaded in newnew.cpp
	   *
	   * instead default 'malloc' function 'rtsha_alloc will be used  with 'new'
	   * and 'rtsha_free' default 'free' function
	   *
	   * All details can be found in newnew.cpp
	   *
	   * You need to ensure that you adhere to the principles of memory safety and management.
	   * Proper memory allocation and deallocation are crucial to prevent memory leaks and
	   * undefined behavior.
	   * */

	  /*create and configure the instance of the heap*/

	  if( true == rtsha_create_heap( (void*) _RTSHA_HeapStart, _RTSHA_HeapSize ) )
	  {

		  /*small 512K page for buffers smaller than 32bytes, with maximum*/
		  heapOk = heapOk | rtsha_add_page( &callbacks, RTSHA_PAGE_TYPE_32, 0x40000);

		  /*small 512K page for buffers between 32 and 64 bytes*/
		  heapOk = heapOk | rtsha_add_page( &callbacks, RTSHA_PAGE_TYPE_64, 0x40000);

		  /*small 512K page for buffers between 65 and 128 bytes*/
		  heapOk = heapOk | rtsha_add_page( &callbacks, RTSHA_PAGE_TYPE_128, 0x80000);

		  /*small 512K page for buffers between 129 and 256 bytes*/
		  heapOk = heapOk | rtsha_add_page( &callbacks, RTSHA_PAGE_TYPE_256, 0x80000);

		  /*small 2M page for buffers between 257 and 512 bytes*/
		  heapOk = heapOk | rtsha_add_page( &callbacks, RTSHA_PAGE_TYPE_512, 0x200000);


		  /*252M Power2 page for blocks between 1K and 16MB, maximum 8192 blocks*/
		  heapOk = rtsha_add_page( &callbacks, RTSHA_PAGE_TYPE_POWER_TWO, 0xF618000, 8192U, 1024, 0xFA0000);
	  }

	  if( !heapOk )
	  {
		  return 0;
	  }

	  xSemaphore = xSemaphoreCreateMutex();
	  xSemUart = xSemaphoreCreateMutex();



	  if( xSemaphore == NULL )
	  {
	       return 0;
	  }


	   /* for the next 3 allocations the Power2 page will be used automatically*/
	   void* memory1 = rtsha_malloc(1000U);

	   rtsha_memset(memory1, 1, 1000);

	   void* memory2 = rtsha_malloc(500U);
	   rtsha_memset(memory2, 1, 500);

	   void* memory3 = rtsha_malloc(80U);
	   rtsha_memset(memory3, 1, 80);

	   /*free*/
	   rtsha_free(memory3);
	   memory3 = rtsha_malloc(80U);


	   /*The small RTSHA_PAGE_TYPE_32 will be used automatically*/
	   void* memory32 = rtsha_malloc(10U);
	   rtsha_memset(memory32, 1, 10);


	   /*The small RTSHA_PAGE_TYPE_64 will be used automatically*/
	   void* memory64 = rtsha_malloc(40U);
	   rtsha_memset(memory64, 1, 40);


	   /*overloaded new operator will use RTSHA heap functions
	   and standard list allocator will use automatically small 32 bytes 'RTSHA' page (RTSHA_PAGE_TYPE_32)*/
	   std::list<double> lstDbl;
	   lstDbl.push_back(3.14159265358979323846); //Pi
	   lstDbl.push_back(2.71828182845904523536); //Euler's number
	   lstDbl.push_back(1.61803398874989484820); //Golden ratio
	   lstDbl.push_back(1.41421356237309504880); //Square root of 2


		xTaskCreate(prvTestThread, (const char *) "Thread1", 2048, NULL, tskIDLE_PRIORITY, &xThread1);

		xTaskCreate(prvUartreceiveThread, (const char *) "Thread2", 2048, NULL, tskIDLE_PRIORITY, &xThread2);

		vTaskStartScheduler();

		/* The following line should never execute!*/

		return 0;
}

void prvTestThread(void *pvParam)
{
	char data[256];
	void* memory1, *memory2, *memory3, *memory32;
	uint32_t val = 0U;
	uint32_t val32m = 0U;
	uint32_t val32f = 0U;
	uint32_t total_cycles_malloc, total_cycles_free, maxm, maxf;

	XTime tStart, tEnd,  sec0, sec1;

	maxm = 0U;
	maxf = 0U;
	while(1)
	{
		total_cycles_malloc = 0U;

		XTime_GetTime(&sec0);
		vTaskDelay(pdMS_TO_TICKS(1000U));
		XTime_GetTime(&sec1);

		 XTime_GetTime(&tStart);
		memory1 = rtsha_malloc(2000U);
		XTime_GetTime(&tEnd);
		total_cycles_malloc = tEnd- tStart;

		XTime_GetTime(&tStart);
		memory2 = rtsha_malloc(2500U);
		XTime_GetTime(&tEnd);
		val = tEnd- tStart; ;
		if( val > total_cycles_malloc )
		{
			total_cycles_malloc = val;
		}

		 XTime_GetTime(&tStart);
		memory3 = rtsha_malloc(5000U);
		XTime_GetTime(&tEnd);
		val = tEnd- tStart; ;
		if( val > total_cycles_malloc )
		{
			total_cycles_malloc = val;
		}

		if( total_cycles_malloc > maxm )
		{
			maxm = total_cycles_malloc;
		}


		 XTime_GetTime(&tStart);
		rtsha_free(memory1);
		XTime_GetTime(&tEnd);
		total_cycles_free = tEnd- tStart;


		 XTime_GetTime(&tStart);
		rtsha_free(memory1);
		XTime_GetTime(&tEnd);
		val = tEnd- tStart; ;
		if( val > total_cycles_free )
		{
			total_cycles_free = val;
		}

		 XTime_GetTime(&tStart);
		rtsha_free(memory1);
		XTime_GetTime(&tEnd);
		val = tEnd- tStart; ;
		if( val > total_cycles_free )
		{
			total_cycles_free = val;
		}

		if( total_cycles_free > maxf )
		{
			maxf = total_cycles_free;
		}


		/*The small RTSHA_PAGE_TYPE_32 will be used automatically*/
		XTime_GetTime(&tStart);
		memory32 = rtsha_malloc(10U);
		XTime_GetTime(&tEnd);
		val32m = tEnd- tStart;


		XTime_GetTime(&tStart);
		rtsha_free(memory32);
		XTime_GetTime(&tEnd);
		val32f = tEnd- tStart; ;



		if( (memory1 != NULL) && (memory2 != NULL) && (memory3 != NULL) )
		{
			sprintf(  (char*) data, (const char*) "Power2 Page rtsha_malloc: %u cycles, rtsha_free: %u cycles MaxM=%u MaxF=%u\n\r", (int) total_cycles_malloc, total_cycles_free, maxm, maxf);
		}
		else
		{
			sprintf(  (char*) data, (const char*) "Power2 Page rtsha_malloc: error\n\r");
		}

		xSemaphoreTake(xSemUart, portMAX_DELAY);
		xil_printf(data);

		if( memory32 != NULL)
		{
			sprintf(  (char*) data, (const char*) "Small Fix Page rtsha_malloc: %u cycles, rtsha_free: %u cycles\n\r", (int) val32m, val32f);
		}
		else
		{
			sprintf(  (char*) data, (const char*) "Small Fix Page rtsha_malloc: error\n\r");
		}

		xil_printf(data);
		xSemaphoreGive(xSemUart);
	}
}

void prvUartreceiveThread(void *pvParam)
{
	unsigned int count = 0;
	std::list<int> lst;
	XTime tStart, tEnd;
	while(1)
	{
		vTaskDelay(pdMS_TO_TICKS(3000U));
		count++;
		XTime_GetTime(&tStart);
		lst.push_back(count);
		XTime_GetTime(&tEnd);

		if( count > 100 )
		{
			lst.erase(lst.begin(), lst.end());
			count = 0;
		}
		xSemaphoreTake(xSemUart, portMAX_DELAY);
		xil_printf("push_back takes %u cycles\n\r", (uint32_t) (tEnd- tStart) );
		xSemaphoreGive(xSemUart);
	}
}

