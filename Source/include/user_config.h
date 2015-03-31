#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_

/* enables the stdout and prints denug messages */
//#define DEBUG

//#define USE_DMX_OUTPUT


#ifdef DEBUG
#define PDBG	os_printf
#else
#define PDBG	nullPrint
#endif

inline void nullPrint(const char *format, ...) {}

#endif /* _USER_CONFIG_H_ */
