/*
--       This software proprietary belong to Quanta computer Inc. and may be used        --
--        only as expressly authorized agreement from          --
--                            Quanta Computer Inc..                             --
--                   (C) COPYRIGHT 2010 Quanta Computer Inc.                    --
--                            ALL RIGHTS RESERVED                             --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
*/

#ifndef _MISC_WRITEFILE_H_
#define _MISC_WRITEFILE_H_


/*vp8 header*/
void add_vp8_file_header(unsigned short width, unsigned short height,unsigned int framerate, int nof);
void add_vp8_frame_header(int frame_len, long timestamp);

/*init*/
void open_file_dump(char* filename);
void open_file_log(char* filename);

/*close*/
void close_file(void);

/*write*/
void write_avc(char* data, int length);
void write_yuv(char* data, int length);
void write_mjpeg(char* data, int length);
void write_vp8(char* data, int length,long timestamp); 
void write_log(char* data, int length);

/* ts_dump */
void ts_dump(int length, char*data);

#endif
