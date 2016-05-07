#ifndef REMEASURE_CLIENT_H_
#define REMEASURE_CLIENT_H_

#include <stdint.h>

int remeasure_init(void);
int remeasure_do(void);
uint32_t remeasure_last(void);
uint32_t remeasure_ask(void);
void remeasure_deinit(void);

int remeasure_is_error_set(void);

#endif// REMEASURE_CLIENT_H_
