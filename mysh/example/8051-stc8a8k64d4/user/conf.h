#ifndef CONF_H
#define CONF_H

#include "STC8A8K64D4.H"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"

#define MCU_SYS_FOSC		35000000UL

typedef unsigned char       uint8_t;
typedef signed char         int8_t;
typedef unsigned short      uint16_t;
typedef signed short        int16_t;
typedef unsigned long       uint32_t;
typedef signed long         int32_t;
typedef float               real_t;

typedef uint32_t            tick_t;

typedef char                bool_t;
#define true                1
#define false               0

#endif	// CONF_H