#include "nutdec.h"
#include "../utils/utils_std.h"

size_t _varlenReadenBytes = 0;//TODO: keep state in struct

void resetVarlenReadenBytes(){
    _varlenReadenBytes = 0;
}

size_t getVarlenReadenBytes(){
    return _varlenReadenBytes;
}

uint8_t avio_r8()
{
    uint8_t byte;
    size_t size = fread_stdin(&byte, 1, 1);
    if(size <= 0){ return 0; }
    fwrite_stdout(&byte, 1, 1);
    _varlenReadenBytes++;
    return byte;
}

uint64_t ffio_read_varlen(){
    uint64_t val = 0;
    uint8_t tmp;

    do{
        tmp = avio_r8();
        val= (val<<7) + (tmp&127);
    }while(tmp&128);
    fflush(stdout);
    return val;
}

uint64_t get_s(){
    return ffio_read_varlen();
}

uint64_t skip_binary_varlen(){
    uint64_t len = ffio_read_varlen();
    skip(len);
    _varlenReadenBytes += len;
    return len;
}