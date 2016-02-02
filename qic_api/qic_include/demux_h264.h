/*
--       This software proprietary belong to Quanta computer Inc. and may be used        --
--        only as expressly authorized agreement from          --
--                            Quanta Computer Inc..                             --
--                   (C) COPYRIGHT 2010 Quanta Computer Inc.                    --
--                            ALL RIGHTS RESERVED                             --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
*/

#ifndef _QIC_DEMUX_H264_H_
#define _QIC_DEMUX_H264_H_

#include "qic_xuctrl.h"

#define MAXFRAMESIZE				128*1024

#define SYNCWORD	0x30385056

typedef struct
{
    unsigned char data[MAXFRAMESIZE];		//store the raw H.264 bitstream
    int length;						//indicate the data length
    unsigned long  timestamp;
}H264DATAFORMAT;

typedef struct
{
    int length;						//indicate the data length
    unsigned long  timestamp;
}H264DATAFORMAT_IN;
;
typedef struct
{
    int ts_counter;
    int ts_payload_len;
    int pes_payload_len;
    int ts_adapt_len;
    int wait_start;
    int pid;
    int unit_start;
    int pos;
    int byteused;
    unsigned long timestamp;
    unsigned char data[MAXFRAMESIZE];
}Packet_Source;

typedef struct
{
    H264DATAFORMAT_IN  frame;
    int frame_count;
    int frame_pre_send;
    int bad_count;
    int drop_count;
    int wait_i;
    int not_complete;
    int adopt_recover;
    int maxbuffer;
    Packet_Source usb;
}info;

//typedef int(*fun_parse) (info* data, unsigned char *src, int size, H264DATAFORMAT **out) ;
//typedef void(*fun_init)(info *data);
typedef struct
{
    info data;
    //	fun_init	init;
    //	fun_parse parse;
}demux;

typedef enum _quanta_enum_debug_type_t
{
    DBG_SEI_FRAME_SIZE          = 14,
    DBG_SEI_LAST_FRAME_SIZE     = 47
}quanta_enum_debug_type_t;

typedef struct{
    unsigned int syncword;
    unsigned int payload_length;
}quanta_debug_header_t;

void demux_VP8_H264_check_bad_frame_initial(void);
int demux_VP8_check_bad_frame( char **src , unsigned int *size);
int demux_H264_check_bad_frame( char *src , unsigned int size);

/*api for h264 simulcast stream*/
int get_avc_stream_id(unsigned char* data, unsigned int data_size);
int get_stream_temporal_id(unsigned char* data, unsigned int data_size);
int check_for_P_frame(unsigned char* data, unsigned int data_size);

#endif
