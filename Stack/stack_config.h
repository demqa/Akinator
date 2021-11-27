#ifndef STACK_CONFIG_H

#define STACK_CONFIG_H

#include <stdio.h>
#include <math.h>

#define DEBUG_MODE 01

typedef char * Elem_t;

const Elem_t POISONED_ELEM = (char *) (1000 - 7);

#endif