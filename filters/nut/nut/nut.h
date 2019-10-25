/*
 * "NUT" Container Format (de)muxer
 * Copyright (c) 2006 Michael Niedermayer
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef AVFORMAT_NUT_H
#define AVFORMAT_NUT_H


#define      MAIN_STARTCODE (0x7A561F5F04ADULL + (((uint64_t)('N'<<8) + 'M')<<48))
#define    STREAM_STARTCODE (0x11405BF2F9DBULL + (((uint64_t)('N'<<8) + 'S')<<48))
#define SYNCPOINT_STARTCODE (0xE4ADEECA4569ULL + (((uint64_t)('N'<<8) + 'K')<<48))
#define     INDEX_STARTCODE (0xDD672F23E64EULL + (((uint64_t)('N'<<8) + 'X')<<48))
#define      INFO_STARTCODE (0xAB68B596BA78ULL + (((uint64_t)('N'<<8) + 'I')<<48))

#define ID_STRING "nut/multimedia container"

#define MAX_DISTANCE (1024*32-1)

#define NUT_MAX_VERSION 4
#define NUT_STABLE_VERSION 3
#define NUT_MIN_VERSION 2

typedef enum{
    FLAG_KEY        =   1, // if set, frame is keyframe
    FLAG_EOR        =   2, // if set, stream has no relevance on presentation. (EOR)
    FLAG_CODED_PTS  =   8, // if set, coded_pts is in the frame header
    FLAG_STREAM_ID  =  16, // if set, stream_id is coded in the frame header
    FLAG_SIZE_MSB   =  32, // if set, data_size_msb is at frame header, otherwise data_size_msb is 0
    FLAG_CHECKSUM   =  64, // if set, the frame header contains a checksum
    FLAG_RESERVED   = 128, // if set, reserved_count is coded in the frame header
    FLAG_SM_DATA    = 256, // if set, side / meta data is stored in the frame header.
    FLAG_HEADER_IDX =1024, // If set, header_idx is coded in the frame header.
    FLAG_MATCH_TIME =2048, // If set, match_time_delta is coded in the frame header
    FLAG_CODED      =4096, // if set, coded_flags are stored in the frame header
    FLAG_INVALID    =8192, // if set, frame_code is invalid
} Flag;

typedef struct Syncpoint {
    uint64_t pos;
    uint64_t back_ptr;
//    uint64_t global_key_pts;
    int64_t ts;
} Syncpoint;

typedef struct FrameCode {
    uint16_t flags;
    uint8_t  stream_id;
    uint16_t size_mul;
    uint16_t size_lsb;
    int16_t  pts_delta;
    uint8_t  reserved_count;
    uint8_t  header_idx;
} FrameCode;


#define NUT_STREAM_CLASS_VIDEO 0
#define NUT_STREAM_CLASS_AUDIO 1
#define NUT_MAX_STREAMS 256

#endif /* AVFORMAT_NUT_H */
