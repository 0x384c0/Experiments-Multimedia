#include "utils_std.h"

//Private
size_t _readenBytes = 0;
int64_t swap_int64( int64_t val ){
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL ) | ((val >> 8) & 0x00FF00FF00FF00FFULL );
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL ) | ((val >> 16) & 0x0000FFFF0000FFFFULL );
    return (val << 32) | ((val >> 32) & 0xFFFFFFFFULL);
}


int is_equal_array_mem(uint8_t array[], char *mem, size_t size){
    for (int i=0;i<size;i++){
        uint8_t a = array[i];
        uint8_t m = *(mem + i);
        if (a != m){
            return 1;
        }
    }
    return 0;
}

//Public
size_t getReadenBytes(){
    return _readenBytes;
}
size_t fread_stdin(void *ptr, size_t size, size_t nmemb){
    size_t ret = fread(ptr,size,nmemb,stdin);
    _readenBytes += nmemb * size;
    return ret;
}
size_t fwrite_stdout(const void * ptr, size_t size, size_t count ){
    return fwrite(ptr,size,count,stdout);
}

void seek(char *mem, size_t arraySize){
    uint8_t buffer[arraySize];
    size_t size = fread_stdin(&buffer, 1, arraySize);
    if(size <= 0){ return; }

    int initialBufferNotWritten = 1;

    while(is_equal_array_mem(buffer,mem, arraySize) != 0){
        if (initialBufferNotWritten){
            fwrite_stdout(&buffer, 1, size);
            initialBufferNotWritten = 0;
        }

        uint8_t byte;
        size_t size = fread_stdin(&byte, 1, 1);
        if(size <= 0){ return; }

        for (int i = 0; i < arraySize - 1; i++){        
            buffer[i] = buffer[i+1];
        }
        buffer[arraySize - 1] = byte;

        fwrite_stdout(&byte, 1, 1);
    }


    if (initialBufferNotWritten){
        fwrite_stdout(&buffer, 1, size);
        initialBufferNotWritten = 0;
    }

    fflush(stdout);
}

void skip(size_t skipSize){
    uint8_t buffer[skipSize];
    size_t size = fread_stdin(&buffer, 1, skipSize);
    fwrite_stdout(buffer, 1, size);
    fflush(stdout);
}

uint8_t read_uint8_t(){
    uint8_t buffer = -1;
    size_t size = fread_stdin(&buffer, 1, sizeof(buffer));
    if(size <= 0){ return buffer; } //TODO: return error
    fwrite_stdout(&buffer, 1, size);
    fflush(stdout);
    return buffer;
}


uint64_t read_uint64_t(){ 
    uint64_t buffer = -1;
    size_t size = fread_stdin(&buffer, 1, sizeof(buffer));
    if(size <= 0){ return buffer; } //TODO: return error
    fwrite_stdout(&buffer, 1, size);
    fflush(stdout);
    return swap_int64(buffer); //big endian
}


int read_uint8_t_no_write(uint8_t *out_buffer){
    uint64_t buffer = -1;
    size_t size = fread_stdin(&buffer, 1, sizeof(uint8_t));
    if(size <= 0){ return EOF; }
    *out_buffer = buffer;
    return 0;
}

void write_uint8_t(uint8_t buffer){
    fwrite_stdout(&buffer, 1, sizeof(buffer));
    fflush(stdout);
}

uint64_t read_uint64_t_first_byte(uint8_t first_byte){ 
    uint64_t buffer = 0;
    size_t size = fread_stdin(&buffer, 1, sizeof(buffer) - sizeof(first_byte));
    if(size <= 0){ return buffer; } //TODO: return error

    buffer =  buffer << 8;
    buffer += first_byte;

    fwrite_stdout(&buffer, 1, size + sizeof(first_byte));
    fflush(stdout);
    return swap_int64(buffer); //big endian
}