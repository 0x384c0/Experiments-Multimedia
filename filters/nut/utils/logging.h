#include <stdint.h>
#include <stdio.h>

void log_array(uint8_t array[],size_t size);
void log_number(uint64_t number, const char *descr);
void log_hex(uint64_t number, const char *descr);
void log_string(const char *format, ...);