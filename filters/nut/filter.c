#include <stdlib.h>
#include "nut/stream_reader.h"

//main
int main(){
    StreamReader_init();
    int ret = StreamReader_read();
    return ret ? EXIT_FAILURE : EXIT_SUCCESS;
}