#include <stdint.h>
#include <stdio.h>

void resetVarlenReadenBytes();
size_t getVarlenReadenBytes();
uint8_t avio_r8();
uint64_t ffio_read_varlen();
uint64_t get_s();
uint64_t skip_binary_varlen();