#include <stdint.h>
#include <stdio.h>

size_t getReadenBytes();
size_t fread_stdin(void *ptr, size_t size, size_t nmemb);
size_t fwrite_stdout(const void * ptr, size_t size, size_t count );
void seek(char * bytes, size_t arraySize);
void skip(size_t skipSize);
uint8_t read_uint8_t();
uint64_t read_uint64_t();
int read_uint8_t_no_write(uint8_t *out_buffer);
void write_uint8_t(uint8_t buffer);
uint64_t read_uint64_t_first_byte(uint8_t first_byte);