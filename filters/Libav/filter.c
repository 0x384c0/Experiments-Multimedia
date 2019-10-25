#include <stdlib.h>
#include <libavutil/error.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

// #define DUMP_FORMAT

#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define RESET   "\033[0m"

int log_error(const char *fmt, int err){
    av_log(NULL, AV_LOG_ERROR,
            "Error: %s",
            av_err2str(err));
    return EXIT_FAILURE;
}

void log_debug(const char *fmt, ...){
    fprintf(stderr, BOLDCYAN "[MAIN] " RESET);
    va_list argptr;
    va_start(argptr, fmt);
    av_vlog(NULL, AV_LOG_DEBUG,fmt,argptr);
}

AVFormatContext* getFormatContext(const char* filename){
    int err;
    AVFormatContext* format_context = NULL;
    
    // Open input
    err = avformat_open_input(&format_context, filename, NULL, NULL);
    if (err < 0){
        log_error("Unable to open input file: %s",err);
        exit(EXIT_FAILURE);
    }
    // Retrieve stream information
    err = avformat_find_stream_info(format_context, NULL);
    if (err < 0){
        log_error("Unable to find stream info: %s",err);
        exit(EXIT_FAILURE);
    }
    return format_context;
}

int findVideoStream(AVFormatContext* format_context){//TODO: remove if unused
    int video_stream;
    for (video_stream = 0; video_stream < format_context->nb_streams; ++video_stream) {
        if (format_context->streams[video_stream]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            break;
        }
    }
    if (video_stream == format_context->nb_streams){
        log_error("Unable to find video stream :%s", -1);
        exit(EXIT_FAILURE);
    }
    return video_stream;
}

AVCodecContext* createCodecContext(AVFormatContext* format_context, int stream_id){//TODO: remove if unused
    int err;
    AVStream* stream = format_context->streams[stream_id];
    AVCodecParameters* codec_par = stream->codecpar;
    AVCodec* codec = avcodec_find_decoder(codec_par->codec_id);
    //alloc AVCodecContext
    AVCodecContext *codec_context = avcodec_alloc_context3(codec);
    if (!codec_context) {
        log_error("Unable to allocate a AVCodecContext :%s", err);
        exit(EXIT_FAILURE);
    }
    //copy paramteters from AVFormatContext for stream_id
    err = avcodec_parameters_to_context(codec_context, codec_par);
    if (err < 0){
        log_error("Unable to copy parameters from AVCodecParameters to AVCodecContext :%s",err);
        exit(EXIT_FAILURE);
    }
    //guess sample rate for audio
    if (codec_context->codec_type == AVMEDIA_TYPE_VIDEO)
        codec_context->framerate = av_guess_frame_rate(format_context, stream, NULL);
    //Initialize codec_context
    err = avcodec_open2(codec_context, codec, NULL);
    if (err < 0){
        log_error("Unable to open codec: %s",err);
        exit(EXIT_FAILURE);
    }
    return codec_context;
}

// This does not quite work like avcodec_decode_audio4/avcodec_decode_video2.
// There is the following difference: if you got a frame, you must call
// it again with pkt=NULL. pkt==NULL is treated differently from pkt->size==0
// (pkt==NULL means get more output, pkt->size==0 is a flush/drain packet)
static int decode(AVCodecContext *avctx, AVFrame *frame, int *got_frame, AVPacket *pkt)
{
    int ret;

    *got_frame = 0;

    if (pkt) {
        ret = avcodec_send_packet(avctx, pkt);
        // In particular, we don't expect AVERROR(EAGAIN), because we read all
        // decoded frames with avcodec_receive_frame() until done.
        if (ret < 0 && ret != AVERROR_EOF)
            return ret;
    }

    ret = avcodec_receive_frame(avctx, frame);
    if (ret < 0 && ret != AVERROR(EAGAIN))
        return ret;
    if (ret >= 0)
        *got_frame = 1;

    return 0;
}

typedef struct StreamContext {
    int out_stream_id;
    AVCodecContext *dec_ctx;
    AVCodecContext *enc_ctx;
} StreamContext;

int main(int argc, char **argv){
    int err,ret;
    av_log_set_level(AV_LOG_DEBUG);
    log_debug("STARTED\n");

    //read args
    if (argc < 3) {
        printf("usage: %s input output\n"
                "API example program to remux a media file with libavformat and libavcodec.\n"
                "The output format is guessed according to the file extension.\n"
                "\n", argv[0]);
        return 1;
    }
    const char *in_filename, *out_filename;
    in_filename  = argv[1];
    out_filename = argv[2];

    //read input
    AVFormatContext* ifmt_ctx = getFormatContext(in_filename);

    #ifdef DUMP_FORMAT
    //Print detailed information about the input 
    av_dump_format(ifmt_ctx, 0, in_filename, 0);
    #endif

    //create output
    log_debug( "create output\n");
    AVFormatContext *ofmt_ctx = NULL;
    AVOutputFormat * outFmt = av_guess_format(NULL, out_filename, NULL); //TODO: use streamable fomat e.g. nut mediacontainer
    avformat_alloc_output_context2(&ofmt_ctx, outFmt, NULL, NULL);

    //init stream_mapping
    StreamContext *stream_mapping = NULL;
    int stream_mapping_size = 0;
    stream_mapping_size = ifmt_ctx->nb_streams;
    stream_mapping = av_mallocz_array(stream_mapping_size, sizeof(*stream_mapping));
    if (!stream_mapping) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    // copy streams info from input to output
    int current_stream_index = 0;
    for (int i = 0; i < ifmt_ctx->nb_streams; i++) {
        AVStream *out_stream;
        AVStream *in_stream = ifmt_ctx->streams[i];
        AVCodecParameters *in_codecpar = in_stream->codecpar;
        if (in_codecpar->codec_type != AVMEDIA_TYPE_AUDIO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_VIDEO) { //TODO: remux unsupported streams, such as AVMEDIA_TYPE_SUBTITLE
            stream_mapping[i].out_stream_id = -1;
            continue;
        }

        //generate id for stream
        stream_mapping[i].out_stream_id = current_stream_index++;

        //create decoder for stream
        stream_mapping[i].dec_ctx = createCodecContext(ifmt_ctx,i);
        log_debug("Initialized decoder for stream_id: %d\n",i);

        //create new stream for output
        out_stream = avformat_new_stream(ofmt_ctx, NULL);
        if (!out_stream) {
            av_log(NULL, AV_LOG_ERROR, "Failed allocating output stream\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }
        
        //copy stream parameters from corresponding stream from input
        ret = avcodec_parameters_copy(out_stream->codecpar, in_codecpar);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Failed to copy codec parameters\n");
            goto end;
        }
        out_stream->codecpar->codec_tag = 0; //TODO: findout, why codec_tag should set to 0
    }
    #ifdef DUMP_FORMAT
    av_dump_format(ofmt_ctx, 0, out_filename, 1);
    #endif

    //open output
    log_debug( "open output\n");
    AVOutputFormat *ofmt = ofmt_ctx->oformat;
    if (!(ofmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Could not open output file '%s'", out_filename);
            goto end;
        }
    }

    log_debug( "write output header\n");
    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error occurred when opening output file\n");
        goto end;
    }



    //get packets
    log_debug( "get packets\n");
    AVFrame* frame = av_frame_alloc();
    AVPacket pkt;


    /**
     * TODO:
     * custom filter for video and audio
     * pipe input and output avcodec_receive_frame avcodec_send_frame
     */

    while (1) {
        //read fram from input
        ret = av_read_frame(ifmt_ctx, &pkt);

        //handle av_read_frame errors
        if (ret < 0 && ret != AVERROR_EOF) {
            av_log(NULL, AV_LOG_ERROR, "av_read_frame Error: %s\n", av_err2str(ret));
            exit(EXIT_FAILURE);
        } else if (ret < 0)
            break;

        //remux all streams
        AVStream *in_stream, *out_stream;
        in_stream  = ifmt_ctx->streams[pkt.stream_index];

        //drop packet from unknown stream
        if (pkt.stream_index >= stream_mapping_size ||
            stream_mapping[pkt.stream_index].out_stream_id < 0) {
            av_packet_unref(&pkt);
            continue;
        }

        // if (ifmt_ctx->streams[pkt.stream_index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
        //     decode();
        // } else {
            //reuse same packet, only replace  stream_index with generated
            pkt.stream_index = stream_mapping[pkt.stream_index].out_stream_id;
            out_stream = ofmt_ctx->streams[pkt.stream_index];

            // remux this frame without reencoding 
            // log_debug( "av_packet_rescale_ts\n");
            av_packet_rescale_ts(&pkt,
                                    in_stream->time_base,
                                    out_stream->time_base); //maybe av_rescale_q_rnd should be used here instead of av_packet_rescale_ts 

            // log_debug( "av_interleaved_write_frame\n");
            ret = av_interleaved_write_frame(ofmt_ctx, &pkt);
            if (ret < 0)
                break;
        // }

        av_packet_unref(&pkt);
    }

    log_debug( "av_write_trailer\n");
    av_write_trailer(ofmt_ctx);
end:
    log_debug( "close streams\n");
    avformat_close_input(&ifmt_ctx);
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_closep(&ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);

    log_debug("FINISHED\n");
    if (ret < 0 && ret != AVERROR_EOF) {
        av_log(NULL, AV_LOG_ERROR, "Error occurred: %s\n", av_err2str(ret));
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}