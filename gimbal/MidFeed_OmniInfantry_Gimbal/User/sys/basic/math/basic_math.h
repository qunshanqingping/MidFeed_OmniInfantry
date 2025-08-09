//
// Created by honor on 25-7-15.
//

#ifndef BASIC_MATH_H
#define BASIC_MATH_H

#include "stdlib.h"
#include <stdint.h>
#include "cmsis_os.h"
#include "memory.h"

#ifndef user_malloc
#ifdef _CMSIS_OS_H
#define user_malloc pvPortMalloc
#else
#define user_malloc malloc
#endif
#endif

#ifndef user_free
#ifdef _CMSIS_OS_H
#define user_free vPortFree
#else
#define user_malloc free
#endif
#endif


#endif //BASIC_MATH_H
