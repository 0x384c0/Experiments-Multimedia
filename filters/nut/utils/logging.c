#include <stdarg.h>
#include "logging.h"

void log_array(uint8_t array[],size_t size){
    FILE *f;
    f = fopen("logs.log", "a+"); // a+ (create + append) option will allow appending which is useful in a log file
    if (f == NULL) { /* Something is wrong   */}
    for (int i=0;i<size;i++){
        fprintf(f, "%02X,",array[i]);
    }
    fprintf(f, "\n");
    fclose(f);
}

void log_number(uint64_t number, const char *descr){
    FILE *f;
    f = fopen("logs.log", "a+"); // a+ (create + append) option will allow appending which is useful in a log file
    if (f == NULL) { /* Something is wrong   */}
    fprintf(f, "%s: %llu\n",descr,number);
    fclose(f);
}

void log_hex(uint64_t number, const char *descr){
    FILE *f;
    f = fopen("logs.log", "a+"); // a+ (create + append) option will allow appending which is useful in a log file
    if (f == NULL) { /* Something is wrong   */}
    fprintf(f, "%s: %llx\n",descr,number);
    fclose(f);
}

void log_string(const char *format, ...){
    va_list argptr;
    va_start(argptr, format);
    FILE *f;
    f = fopen("logs.log", "a+"); // a+ (create + append) option will allow appending which is useful in a log file
    if (f == NULL) { /* Something is wrong   */}
    vfprintf(f,format,argptr);
    fprintf(f,"\n");
    va_end(argptr);
    fclose(f);
}
