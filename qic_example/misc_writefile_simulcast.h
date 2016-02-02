/*
--       This software proprietary belong to Quanta computer Inc. and may be used        --
--        only as expressly authorized agreement from          --
--                            Quanta Computer Inc..                             --
--                   (C) COPYRIGHT 2010 Quanta Computer Inc.                    --
--                            ALL RIGHTS RESERVED                             --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
*/

#ifndef _WRITEFILE_H_SIMULCAST_
#define _WRITEFILE_H_SIMULCAST_


#define WRITE_H264 0
#define WRITE_VP8 1

/*vp8 header*/
void add_vp8_file_header0(unsigned short width, unsigned short height,unsigned int framerate, int nof);
void add_vp8_file_header1(unsigned short width, unsigned short height,unsigned int framerate, int nof);
void add_vp8_file_header2(unsigned short width, unsigned short height,unsigned int framerate, int nof);
void add_vp8_file_header3(unsigned short width, unsigned short height,unsigned int framerate, int nof);

void add_vp8_frame_header0(int frame_len, long timestamp);
void add_vp8_frame_header1(int frame_len, long timestamp);
void add_vp8_frame_header2(int frame_len, long timestamp);
void add_vp8_frame_header3(int frame_len, long timestamp);

/*init*/
void open_simulcast_files_dump(char vp8, char* filename);
void open_temporal_layer_files_dump(char vp8, char* filename);

/*close*/
void close_simulcast_files_file(void);
void close_temporal_layer_files(void);

/*write*/
void write_stream0(char* data, int length,long timestamp); 
void write_stream1(char* data, int length,long timestamp); 
void write_stream2(char* data, int length,long timestamp); 
void write_stream3(char* data, int length,long timestamp); 

void write_templayer0(char* data, int length,long timestamp); 
void write_templayer01(char* data, int length,long timestamp); 
void write_templayer012(char* data, int length,long timestamp); 
void write_templayer0123(char* data, int length,long timestamp); 

#endif
