#include "c_types.h"
#include "osapi.h"
#include "user_interface.h"
#include "espmissingincludes.h"

#define FIXPOINT_DOT	13

// every 16ms
#define SAMPLE_FREQUENCY	62
#define CUT_OFF_FREQUENCY	1

#define COEFFICIENT_CALC	(SAMPLE_FREQUENCY / (2 * M_PI * CUT_OFF_FREQUENCY))
#define COEFFICIENT			50

static const uint32 ERR_DIFF_TIME = 300000;


static uint32 startTime = 0;

static uint32 diff = 0;
static uint32 maxDiff = 0;

static uint32 execDiff = 0;
static uint32 maxExecDiff = 0;



void ICACHE_FLASH_ATTR diff_start()
{
	const uint32 newStartTime = system_get_time();

	const uint32 newDiff = newStartTime - startTime;
	startTime = newStartTime;

	if (newDiff > maxDiff)
		maxDiff = newDiff;

//	/* use irrfilter to calulate the avarage diff */
//	const uint64 valueFixPoint = (uint64)(newDiff) << FIXPOINT_DOT;

//	static uint64 IIRFilter_lastResultFixPoint = 0;
//	const sint64 diffFixPoint = (valueFixPoint - IIRFilter_lastResultFixPoint) / COEFFICIENT;
//	const uint64 resultFixPoint = IIRFilter_lastResultFixPoint + diffFixPoint;
//	IIRFilter_lastResultFixPoint = resultFixPoint;

	diff = newDiff;//resultFixPoint >> FIXPOINT_DOT;
}


void ICACHE_FLASH_ATTR diff_end()
{
	const uint32 endTime = system_get_time();

	execDiff = endTime - startTime;
	startTime = endTime;

	if (execDiff > maxExecDiff)
		maxExecDiff = execDiff;
}

uint8 diff_counter = 0;

void ICACHE_FLASH_ATTR diff_print()
{
	const bool printWarning = (maxDiff > ERR_DIFF_TIME);

	if (printWarning)
		os_printf("W: ");

	
	os_printf( "%02d: interval: %dµs (max: %d.%03dms), exec: %dµs (max: %dµs), free heap: %d Byte     \r",
			  diff_counter,
			  diff, (maxDiff / 1000), (maxDiff % 1000),
			  execDiff, maxExecDiff,
			  system_get_free_heap_size() );
	maxDiff = 0;
	maxExecDiff = 0;
	diff_counter++;

	if (printWarning)
		os_printf("\n");
}
