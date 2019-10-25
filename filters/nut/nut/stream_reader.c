#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/logging.h"
#include "../utils/utils_std.h"

#include "nut.h"
#include "nutdec.h"

#ifdef _WIN32
    #include <io.h>
    #include <fcntl.h>
#endif

#define	E_UNKNOWN_START_CODE	2001

//filter
int _process_video_frame(uint64_t width, uint64_t height){
    int pixelsPerFrame = width * height,
    RGBframeBufferSize = pixelsPerFrame * 3; //RGB 24
    uint8_t 
    pixels[RGBframeBufferSize], //frame buffer
    *pix; // pointer to current pixel
    if(fread_stdin(&pixels, 1, RGBframeBufferSize) <= 0){
        return 1;
    }
    // // process image
    pix = (uint8_t *) &pixels;
    for(int q = 0; q < pixelsPerFrame; q++){
            *pix = *pix * 2; // multiple red to 2
            pix++;
            *pix = *pix + 120; // shift green channel
            pix++;
            *pix = *pix + q/10; // lines in blue channel
            pix++;
    }
    // write frame back
    fwrite_stdout(&pixels, 1, RGBframeBufferSize);

    return 0;
}

//nut
void _skip_packet(){
    uint64_t forward_ptr = ffio_read_varlen();
    log_number(forward_ptr,"    forward_ptr");
    if(forward_ptr > 4096){
        uint64_t header_checksum = ffio_read_varlen();
        log_number(header_checksum,"header_checksum");      
    }
    skip(forward_ptr);
}

struct StreamInfo{
    uint64_t
    stream_id,
    stream_class,
    //video
    width,
    height,
    //audio
    samplerate;
};

struct NUTContext {
    uint8_t header_len[128]; //elision_header
};

struct DecoderContext {
    uint64_t 
    flags[NUT_MAX_STREAMS],
    reserved_count[NUT_MAX_STREAMS],
    stream_id[NUT_MAX_STREAMS],
    size_lsb[NUT_MAX_STREAMS],
    size_mul[NUT_MAX_STREAMS];
    struct StreamInfo* streams[NUT_MAX_STREAMS];
    uint64_t header_idx[NUT_MAX_STREAMS];
};

//state
struct DecoderContext decoderContext;
struct NUTContext nut;

void _reset(){
    for (int i = 0;i < NUT_MAX_STREAMS;i++){
        if (decoderContext.streams[i] != 0){
            free(decoderContext.streams[i]);
        }
    }
    memset(&decoderContext, 0, sizeof decoderContext);
    memset(&nut, 0, sizeof nut);
}

void _decode_main_header(){
    _reset();
    uint64_t forward_ptr = ffio_read_varlen();
    log_number(forward_ptr,"    forward_ptr");

    uint64_t tmp_stream, tmp_mul, tmp_pts, tmp_size, tmp_res, tmp_head_idx;
    int i, j, count, ret;

    resetVarlenReadenBytes();

    if(forward_ptr > 4096){
        uint64_t header_checksum = ffio_read_varlen();
        log_number(header_checksum,"    header_checksum");      
    }

    log_hex( ffio_read_varlen(),"    version");
    log_hex( ffio_read_varlen(),"    stream_count");//TODO: handle error: illegal stream number
    log_hex( ffio_read_varlen(),"    max_distance");
    uint64_t time_base_count  = ffio_read_varlen();
    log_hex(time_base_count ,"    time_base_count");

    for(i=0; i<time_base_count; i++){
        log_hex( ffio_read_varlen(),"    time_base_num");
        log_hex( ffio_read_varlen(),"    time_base_denom");
    }
    tmp_pts      = 0;
    tmp_mul      = 1;
    tmp_stream   = 0;
    tmp_head_idx = 0;

    for (i = 0; i < NUT_MAX_STREAMS;) {
        uint64_t tmp_flags  = ffio_read_varlen();
        uint64_t tmp_fields = ffio_read_varlen();

        if (tmp_fields > 0)
            tmp_pts = get_s();
        if (tmp_fields > 1)
            tmp_mul = ffio_read_varlen();
        if (tmp_fields > 2)
            tmp_stream = ffio_read_varlen();
        if (tmp_fields > 3)
            tmp_size = ffio_read_varlen();
        else
            tmp_size = 0;
        if (tmp_fields > 4)
            tmp_res = ffio_read_varlen();
        else
            tmp_res = 0;
        if (tmp_fields > 5)
            count = ffio_read_varlen();
        else
            count = tmp_mul - tmp_size;
        if (tmp_fields > 6)
            get_s();
        if (tmp_fields > 7){
            tmp_head_idx = ffio_read_varlen();
        }

        while (tmp_fields-- > 8) {
            ffio_read_varlen();
        }

        for (j = 0; j < count; j++, i++) {
            if (i == 'N') {
                decoderContext.flags[i] = FLAG_INVALID;
                j--;
                continue;
            }
            decoderContext.flags[i]          = tmp_flags;
            decoderContext.stream_id[i]      = tmp_stream;
            decoderContext.size_mul[i]       = tmp_mul;
            decoderContext.size_lsb[i]       = tmp_size + j;
            decoderContext.reserved_count[i] = tmp_res;
            decoderContext.header_idx[i]     = tmp_head_idx;
        }
    }

    uint64_t header_count_minus1 = ffio_read_varlen();
    for(i=0; i<header_count_minus1; i++)
        nut.header_len[i+1] = ffio_read_varlen();//TODO: add check tmp > 0 && tmp < 256

    size_t readenBytes = getVarlenReadenBytes();
    skip(forward_ptr - readenBytes);
}

void _decode_stream_header(){
    uint64_t
    stream_class, stream_id,
    width = 0, height = 0, 
    samplerate_num = 0, samplerate_denom = 0;
    uint64_t forward_ptr = ffio_read_varlen();
    log_number(forward_ptr,"    forward_ptr");

    resetVarlenReadenBytes();

    stream_id = ffio_read_varlen();
    log_hex( stream_id,"    stream_id");
    stream_class = ffio_read_varlen();
    log_hex( stream_class ,"    stream_class");
    log_hex( skip_binary_varlen(),"    skipped fourcc");
    log_hex( ffio_read_varlen(),"    time_base_id");
    log_hex( ffio_read_varlen(),"    msb_pts_shift");
    log_hex( ffio_read_varlen(),"    max_pts_distance");
    log_hex( ffio_read_varlen(),"    decode_delay");
    log_hex( ffio_read_varlen(),"    stream_flags");
    log_hex( skip_binary_varlen(),"    skipped codec_specific_data");

    switch(stream_class){
        case NUT_STREAM_CLASS_VIDEO:
            width = ffio_read_varlen(); 
            height = ffio_read_varlen();    
            log_hex( width,"    width");
            log_hex( height,"    height");
            log_hex( ffio_read_varlen(),"    sample_width");
            log_hex( ffio_read_varlen(),"    sample_height");
            log_hex( ffio_read_varlen(),"    colorspace_type");
        break;
        case NUT_STREAM_CLASS_AUDIO:
            samplerate_num = ffio_read_varlen();
            samplerate_denom = ffio_read_varlen();
            log_hex( samplerate_num,"    samplerate_num");
            log_hex( samplerate_denom,"    samplerate_denom");
            log_hex( ffio_read_varlen(),"    channel_count");
        break;
        default:break;
    }

    size_t readenBytes = getVarlenReadenBytes();
    skip(forward_ptr - readenBytes);

    //generate stream struct
    struct StreamInfo *streamInfo = malloc(sizeof(struct StreamInfo));
    streamInfo->stream_id = stream_id;
    streamInfo->stream_class = stream_class;
    switch(stream_class){
        case NUT_STREAM_CLASS_VIDEO:
            streamInfo->width = width;
            streamInfo->height = height;
            decoderContext.streams[stream_id] = streamInfo;
        break;
        case NUT_STREAM_CLASS_AUDIO:
            streamInfo->samplerate = samplerate_num / samplerate_denom;
            decoderContext.streams[stream_id] = streamInfo;
        break;
        default: free(streamInfo); break;
    }
}

int _read_packet(uint64_t startcode){
    switch(startcode){
        case      MAIN_STARTCODE: log_string("MAIN_STARTCODE        "); _decode_main_header(); break;
        case    STREAM_STARTCODE: log_string("STREAM_STARTCODE      "); _decode_stream_header(); break;
        case      INFO_STARTCODE: log_string("INFO_STARTCODE        "); _skip_packet(); break;
        case     INDEX_STARTCODE: log_string("INDEX_STARTCODE       "); _skip_packet(); break;
        case SYNCPOINT_STARTCODE: log_string("SYNCPOINT_STARTCODE   "); _skip_packet(); break;
        default: log_hex(startcode,"UNKOWN_STARTCODE"); _skip_packet(); break; //TODO: skip unknown packets
    }
    return 0;
}

uint64_t _decode_frame_header(uint64_t *stream_id, uint8_t *header_idx, uint8_t frame_code){
    log_hex( frame_code,"    frame_code");

    uint64_t frame_flags = decoderContext.flags[frame_code];
    log_hex( frame_flags,"    frame_flags");

    uint64_t frame_res = decoderContext.reserved_count[frame_code];
    log_hex( frame_res,"    frame_res");

    *stream_id = decoderContext.stream_id[frame_code];
    log_hex( *stream_id,"    stream_id");

    uint64_t size_mul = decoderContext.size_mul[frame_code];
    log_hex( size_mul,"    size_mul");

    uint64_t size = decoderContext.size_lsb[frame_code];
    log_hex( size,"    size_lsb");

    //read flags
    if(frame_flags & FLAG_CODED){
        int64_t coded_flags = ffio_read_varlen();
        frame_flags ^= coded_flags;
        log_hex(coded_flags,"    coded_flags");
    }
    if(frame_flags & FLAG_STREAM_ID){
        *stream_id = ffio_read_varlen();
        log_hex(*stream_id,"    stream_id");
    }
    if(frame_flags & FLAG_CODED_PTS){
        log_hex(ffio_read_varlen(),"    coded_pts");
    }
    if(frame_flags & FLAG_SIZE_MSB){
        uint64_t data_size_msb = size_mul * ffio_read_varlen();
        size += data_size_msb;
        log_hex(data_size_msb,"    data_size_msb");
    }
    if(frame_flags & FLAG_MATCH_TIME){
        log_hex(get_s(),"    match_time_delta");
    }
    if(frame_flags & FLAG_HEADER_IDX){
        *header_idx = ffio_read_varlen();
        log_hex(*header_idx,"    header_idx");
    }
    if(frame_flags & FLAG_RESERVED){
        frame_res = ffio_read_varlen();
        log_hex(frame_res,"    frame_res");
    }

    skip(frame_res);

    //TODO: calculate size
    if (size > 4096)
        *header_idx = 0;
    size -= nut.header_len[*header_idx];

    //TODO: handle error: header_idx invalid

    if(frame_flags & FLAG_CHECKSUM){
        uint64_t checksum = ffio_read_varlen();
        log_hex(checksum,"    checksum");
    }

    //TODO: handle error: frame size > 2max_distance and no checksum\n
    return size;
}

int _read_frame(uint8_t frame_code){
    log_string("FRAME");
    uint64_t size, stream_id;
    uint8_t header_idx;

    size = _decode_frame_header(&stream_id, &header_idx, frame_code);

    uint64_t stream_class = decoderContext.streams[stream_id]->stream_class;
    switch(stream_class){
        case NUT_STREAM_CLASS_VIDEO:
            log_string("    NUT_STREAM_CLASS_VIDEO");
            uint64_t width,height;
            width = decoderContext.streams[stream_id]->width;
            height = decoderContext.streams[stream_id]->height;
            _process_video_frame(width,height);
        break;
        case NUT_STREAM_CLASS_AUDIO:
            log_string("    NUT_STREAM_CLASS_AUDIO");
            log_hex(size,"    skip size");
            skip(size);
        break;
        default:
            log_hex(size,"    skip size");
            skip(size);
        break;
    }
    return 0;
}

int read_next_data(){
    uint8_t next_byte = 0; 
    int ret = read_uint8_t_no_write(&next_byte);
    if (ret != 0){//eof
        return ret;
    } else if (next_byte == 'N'){//packet
        uint64_t startcode = read_uint64_t_first_byte(next_byte);
        return _read_packet(startcode);
    } else {//frame
        write_uint8_t(next_byte);
        return _read_frame(next_byte);
    }
}

int read_file_id_string(){
    seek(ID_STRING,sizeof ID_STRING);
    return 0;
}

//region Public
void StreamReader_init(){
    //windows workaround
    #ifdef _WIN32
        setmode(fileno(stdout),O_BINARY);
        setmode(fileno(stdin),O_BINARY);
    #endif
}

int StreamReader_read(){
    log_string("\nStreamReader started");
    int ret = 0;

    ret |= read_file_id_string();

    while(1){
        log_string("offset: %llx",getReadenBytes());
        int nexDataRet = read_next_data();
        if (nexDataRet != 0){
            if (nexDataRet != EOF){
                ret |= nexDataRet;
            }
            break;
        }
    }
    log_number(ret,"\nret");
    log_string("StreamReader finished");
    return ret;
}
//endregion