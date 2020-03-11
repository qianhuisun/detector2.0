#ifndef FORMAT_H
#define FORMAT_H

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

inline
void format_uint(char *buf, uint64_t value, unsigned int base);

inline
void format_int(char *buf, int64_t value, unsigned int base);

#endif