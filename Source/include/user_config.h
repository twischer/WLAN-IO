#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_

#include "ets_sys.h"
#include "uart_hw.h"


/* enables the stdout and prints denug messages */
#define DEBUG

/*
 * Enables the dmx output.
 * Do not activating debug and dmx output,
 * if dmx output is configured to use the same uart port
 * as the debug output.
 */
//#define USE_DMX_OUTPUT

#define DMX_IO_MUX		PERIPHS_IO_MUX_U0TXD_U
#define DMX_IO_GPIO		FUNC_GPIO1
#define DMX_IO_TXD		FUNC_U0TXD
#define DMX_IO_NUM		1
#define DMX_IO_BIT		BIT1
#define DMX_UART		UART0


#ifdef DEBUG
#define PDBG	os_printf
#else
#define PDBG	nullPrint
#endif

inline void nullPrint(const char *format, ...) {}

#endif /* _USER_CONFIG_H_ */
