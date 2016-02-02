/*
--       This software proprietary belong to Quanta computer Inc. and may be used        --
--        only as expressly authorized agreement from          --
--                            Quanta Computer Inc..                             --
--                   (C) COPYRIGHT 2010 Quanta Computer Inc.                    --
--                            ALL RIGHTS RESERVED                             --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "misc_writefile_simulcast.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

#ifdef USE_FWRITE
/*FILE *file = NULL;*/
FILE *file_stream0 = NULL;
FILE *file_stream1 = NULL;
FILE *file_stream2 = NULL;
FILE *file_stream3 = NULL;

FILE *file_templayer0 = NULL;
FILE *file_templayer1 = NULL;
FILE *file_templayer2 = NULL;
FILE *file_templayer3 = NULL;


#else
/*int file = -1;*/
int file_stream0 = -1;
int file_stream1= -1;
int file_stream2 = -1;
int file_stream3 = -1;

int file_templayer0 = -1;
int file_templayer1= -1;
int file_templayer2 = -1;
int file_templayer3 = -1;

#endif

#ifndef _WIN32
#define BYTE   unsigned char
#define WORD   short
#define DWORD  unsigned int
#endif

typedef struct 
{
    DWORD signature;
    WORD  version;
    WORD  length;
    DWORD fourcc;
    WORD  width;
    WORD  height;
    DWORD framerate;
    DWORD timescale;
    DWORD num_of_frames;
    DWORD unused;
}IVFFileHeader;

typedef struct
{
    DWORD size;
    long timestamp;
}IVFFrameHeader;


static char g_vp8=0;


static void errno_exit(const char*  s)
{
    fprintf(stderr, "%s error %d, %s\n",
            s, errno, strerror(errno));

    exit(EXIT_FAILURE);
}



void add_vp8_file_header0(unsigned short width, unsigned short height,unsigned int framerate, int nof)
{
    IVFFileHeader header;
    header.signature = 'FIKD';
    header.version = 0;
    header.length = sizeof(header);
    header.fourcc = '08PV';
    header.width = width;
    header.height = height;
    header.framerate = framerate;
    header.timescale = 1;
    header.num_of_frames = nof;
    header.unused=0;
#ifdef USE_FWRITE		
    if (file_stream0 != NULL) {
        if( -1 == fwrite(&header, 32, 1, file_stream0))
            errno_exit("write_file_error");
    }
#else
    if (file_stream0 != -1) {
        if( -1 == write(file_stream0, &header, 32))
            errno_exit("write_file_error");
    }

#endif
}		

void add_vp8_file_header1(unsigned short width, unsigned short height,unsigned int framerate, int nof)
{
    IVFFileHeader header;
    header.signature = 'FIKD';
    header.version = 0;
    header.length = sizeof(header);
    header.fourcc = '08PV';
    header.width = width;
    header.height = height;
    header.framerate = framerate;
    header.timescale = 1;
    header.num_of_frames = nof;
    header.unused=0;
#ifdef USE_FWRITE		
    if (file_stream1 != NULL) {
        if( -1 == fwrite(&header, 32, 1, file_stream1))
            errno_exit("write_file_error");
    }
#else
    if (file_stream1 != -1) {
        if( -1 == write(file_stream1, &header, 32))
            errno_exit("write_file_error");
    }

#endif
}		

void add_vp8_file_header2(unsigned short width, unsigned short height,unsigned int framerate, int nof)
{
    IVFFileHeader header;
    header.signature = 'FIKD';
    header.version = 0;
    header.length = sizeof(header);
    header.fourcc = '08PV';
    header.width = width;
    header.height = height;
    header.framerate = framerate;
    header.timescale = 1;
    header.num_of_frames = nof;
    header.unused=0;
#ifdef USE_FWRITE		
    if (file_stream2 != NULL) {
        if( -1 == fwrite(&header, 32, 1, file_stream2))
            errno_exit("write_file_error");
    }
#else
    if (file_stream2!= -1) {
        if( -1 == write(file_stream2, &header, 32))
            errno_exit("write_file_error");
    }

#endif
}		

void add_vp8_file_header3(unsigned short width, unsigned short height,unsigned int framerate, int nof)
{
    IVFFileHeader header;
    header.signature = 'FIKD';
    header.version = 0;
    header.length = sizeof(header);
    header.fourcc = '08PV';
    header.width = width;
    header.height = height;
    header.framerate = framerate;
    header.timescale = 1;
    header.num_of_frames = nof;
    header.unused=0;
#ifdef USE_FWRITE		
    if (file_stream3 != NULL) {
        if( -1 == fwrite(&header, 32, 1, file_stream3))
            errno_exit("write_file_error");
    }
#else
    if (file_stream3 != -1) {
        if( -1 == write(file_stream3, &header, 32))
            errno_exit("write_file_error");
    }

#endif
}		



void add_vp8_frame_header0(int frame_len, long timestamp)
{
    static IVFFrameHeader frame_header;
    frame_header.size = frame_len;
    frame_header.timestamp = timestamp;

#ifdef USE_FWRITE
    if (file_stream0 != NULL) {
        if( -1 == fwrite(&frame_header, 12, 1, file_stream0))
            errno_exit("write_file_error");
    }
#else
    if (file_stream0 != -1) {
        if( -1 == write(file_stream0, &frame_header, 12))
            errno_exit("write_file_error");
    }

#endif
}

void add_vp8_frame_header1(int frame_len, long timestamp)
{
    static IVFFrameHeader frame_header;
    frame_header.size = frame_len;
    frame_header.timestamp = timestamp;

#ifdef USE_FWRITE
    if (file_stream1 != NULL) {
        if( -1 == fwrite(&frame_header, 12, 1, file_stream1))
            errno_exit("write_file_error");
    }
#else
    if (file_stream1 != -1) {
        if( -1 == write(file_stream1, &frame_header, 12))
            errno_exit("write_file_error");
    }

#endif
}

void add_vp8_frame_header2(int frame_len, long timestamp)
{
    static IVFFrameHeader frame_header;
    frame_header.size = frame_len;
    frame_header.timestamp = timestamp;

#ifdef USE_FWRITE
    if (file_stream2 != NULL) {
        if( -1 == fwrite(&frame_header, 12, 1, file_stream2))
            errno_exit("write_file_error");
    }
#else
    if (file_stream2 != -1) {
        if( -1 == write(file_stream2, &frame_header, 12))
            errno_exit("write_file_error");
    }

#endif
}

void add_vp8_frame_header3(int frame_len, long timestamp)
{
    static IVFFrameHeader frame_header;
    frame_header.size = frame_len;
    frame_header.timestamp = timestamp;

#ifdef USE_FWRITE
    if (file_stream3 != NULL) {
        if( -1 == fwrite(&frame_header, 12, 1, file_stream3))
            errno_exit("write_file_error");
    }
#else
    if (file_stream3 != -1) {
        if( -1 == write(file_stream3, &frame_header, 12))
            errno_exit("write_file_error");
    }

#endif
}

void write_stream0(char* data, int length,long timestamp) {

    if(g_vp8)
        add_vp8_frame_header0(length,timestamp);

#ifdef USE_FWRITE
    if (file_stream0 != NULL) {
        if( -1 == fwrite(data, length, 1, file_stream0))
            errno_exit("write_file_error");
    }
#else


    if (file_stream0 != -1) {
        if( -1 == write(file_stream0, data, length))
            errno_exit("write_file_error");
    }

#endif

}


void write_stream1(char* data, int length,long timestamp) {

    if(g_vp8)
        add_vp8_frame_header1(length,timestamp);

#ifdef USE_FWRITE

    if (file_stream1 != NULL) {
        if( -1 == fwrite(data, length, 1, file_stream1))
            errno_exit("write_file_error");
    }
#else
    if (file_stream1 != -1) {
        if( -1 == write(file_stream1, data, length))
            errno_exit("write_file_error");
    }

#endif

}


void write_stream2(char* data, int length,long timestamp) {


    if(g_vp8)
        add_vp8_frame_header2(length,timestamp);

#ifdef USE_FWRITE

    if (file_stream2 != NULL) {
        if( -1 == fwrite(data, length, 1, file_stream2))
            errno_exit("write_file_error");
    }
#else
    if (file_stream2 != -1) {
        if( -1 == write(file_stream2, data, length))
            errno_exit("write_file_error");
    }

#endif

}


void write_stream3(char* data, int length,long timestamp) {

    if(g_vp8)
        add_vp8_frame_header3(length,timestamp);


#ifdef USE_FWRITE

    if (file_stream3 != NULL) {
        if( -1 == fwrite(data, length, 1, file_stream3))
            errno_exit("write_file_error");
    }
#else
    if (file_stream3 != -1) {
        if( -1 == write(file_stream3, data, length))
            errno_exit("write_file_error");
    }

#endif

}


void close_simulcast_files_file(void) {
#ifdef USE_FWRITE
    if (file_stream0 != NULL) {
        fflush(file_stream0);
        fclose(file_stream0);
        file_stream0 = NULL;
    }
    if (file_stream1 != NULL) {
        fflush(file_stream1);
        fclose(file_stream1);
        file_stream1 = NULL;
    }

    if (file_stream2 != NULL) {
        fflush(file_stream2);
        fclose(file_stream2);
        file_stream2 = NULL;
    }

    if (file_stream3 != NULL) {
        fflush(file_stream3);
        fclose(file_stream3);
        file_stream3 = NULL;
    }

#else
    if (file_stream0 != -1) {
        fsync(file_stream0);
        close(file_stream0);
        file_stream0 = -1;
    }
    if (file_stream1 != -1) {
        fsync(file_stream1);
        close(file_stream1);
        file_stream1 = -1;
    }

    if (file_stream2 != -1) {
        fsync(file_stream2);
        close(file_stream2);
        file_stream2 = -1;
    }

    if (file_stream3 != -1) {
        fflush(file_stream3);
        fclose(file_stream3);
        file_stream3 = -1;
    }

#endif
}

void open_simulcast_files_dump(char vp8, char* filename) {

    char stream0_name[128] = {0};
    char stream1_name[128] = {0};
    char stream2_name[128] = {0};
    char stream3_name[128] = {0};

    if (filename == NULL)
        return;


    g_vp8=vp8;

    if(vp8){
        snprintf(stream0_name, 128, "%s_stream0.ivf", filename);
        snprintf(stream1_name, 128, "%s_stream1.ivf", filename);
        snprintf(stream2_name, 128, "%s_stream2.ivf", filename);
        snprintf(stream3_name, 128, "%s_stream3.ivf", filename);
    }else{
        snprintf(stream0_name, 128, "%s_stream0.264", filename);
        snprintf(stream1_name, 128, "%s_stream1.264", filename);
        snprintf(stream2_name, 128, "%s_stream2.264", filename);
        snprintf(stream3_name, 128, "%s_stream3.264", filename);

    }

#ifdef USE_FWRITE
    /*file = fopen (filename, "w");*/
    file_stream0 = fopen (stream0_name, "w");
    file_stream1 = fopen (stream1_name, "w");
    file_stream2 = fopen (stream2_name, "w");
    file_stream3 = fopen (stream3_name, "w");

#else
    /*file = open(filename, O_CREAT|O_WRONLY|O_NONBLOCK, S_IRWXU|S_IRWXG|S_IRWXO);*/
    file_stream0 = open(stream0_name, O_CREAT|O_WRONLY|O_NONBLOCK, S_IRWXU|S_IRWXG|S_IRWXO);
    file_stream1 = open(stream1_name, O_CREAT|O_WRONLY|O_NONBLOCK, S_IRWXU|S_IRWXG|S_IRWXO);
    file_stream2 = open(stream2_name, O_CREAT|O_WRONLY|O_NONBLOCK, S_IRWXU|S_IRWXG|S_IRWXO);
    file_stream3 = open(stream3_name, O_CREAT|O_WRONLY|O_NONBLOCK, S_IRWXU|S_IRWXG|S_IRWXO);

#endif




}



void open_temporal_layer_files_dump(char vp8, char* filename) {

    char templayer0_name[128] = {0};
    char templayer1_name[128] = {0};
    char templayer2_name[128] = {0};
    char templayer3_name[128] = {0};

    if (filename == NULL)
        return;


    g_vp8=vp8;

    if(vp8){
        snprintf(templayer0_name, 128, "%s_templayer0.ivf", filename);
        snprintf(templayer1_name, 128, "%s_templayer01.ivf", filename);
        snprintf(templayer2_name, 128, "%s_templayer012.ivf", filename);
        snprintf(templayer3_name, 128, "%s_templayer0123.ivf", filename);
    }else{
        snprintf(templayer0_name, 128, "%s_templayer0.264", filename);
        snprintf(templayer1_name, 128, "%s_templayer01.264", filename);
        snprintf(templayer2_name, 128, "%s_templayer012.264", filename);
        snprintf(templayer3_name, 128, "%s_templayer0123.264", filename);

    }

#ifdef USE_FWRITE
    /*file = fopen (filename, "w");*/
    file_templayer0 = fopen (templayer0_name, "w");
    file_templayer1 = fopen (templayer1_name, "w");
    file_templayer2 = fopen (templayer2_name, "w");
    file_templayer3 = fopen (templayer3_name, "w");

#else
    /*file = open(filename, O_CREAT|O_WRONLY|O_NONBLOCK, S_IRWXU|S_IRWXG|S_IRWXO);*/
    file_templayer0 = open(templayer0_name, O_CREAT|O_WRONLY|O_NONBLOCK, S_IRWXU|S_IRWXG|S_IRWXO);
    file_templayer1 = open(templayer1_name, O_CREAT|O_WRONLY|O_NONBLOCK, S_IRWXU|S_IRWXG|S_IRWXO);
    file_templayer2 = open(templayer2_name, O_CREAT|O_WRONLY|O_NONBLOCK, S_IRWXU|S_IRWXG|S_IRWXO);
    file_templayer3 = open(templayer3_name, O_CREAT|O_WRONLY|O_NONBLOCK, S_IRWXU|S_IRWXG|S_IRWXO);

#endif




}


void close_temporal_layer_files(void) {
#ifdef USE_FWRITE
    if (file_templayer0 != NULL) {
        fflush(file_templayer0);
        fclose(file_templayer0);
        file_templayer0 = NULL;
    }
    if (file_templayer1 != NULL) {
        fflush(file_templayer1);
        fclose(file_templayer1);
        file_templayer1 = NULL;
    }

    if (file_templayer2 != NULL) {
        fflush(file_templayer2);
        fclose(file_templayer2);
        file_templayer2 = NULL;
    }

    if (file_templayer3 != NULL) {
        fflush(file_templayer3);
        fclose(file_templayer3);
        file_templayer3 = NULL;
    }

#else
    if (file_templayer0 != -1) {
        fsync(file_templayer0);
        close(file_templayer0);
        file_templayer0 = -1;
    }
    if (file_templayer1 != -1) {
        fsync(file_templayer1);
        close(file_templayer1);
        file_templayer1 = -1;
    }

    if (file_templayer2 != -1) {
        fsync(file_templayer2);
        close(file_templayer2);
        file_templayer2 = -1;
    }

    if (file_templayer3 != -1) {
        fflush(file_templayer3);
        fclose(file_templayer3);
        file_templayer3 = -1;
    }

#endif
}



void write_templayer0(char* data, int length,long timestamp) {

    if(g_vp8)
        add_vp8_frame_header0(length,timestamp);

#ifdef USE_FWRITE
    if (file_templayer0 != NULL) {
        if( -1 == fwrite(data, length, 1, file_templayer0))
            errno_exit("write_file_error");
    }
#else


    if (file_templayer0 != -1) {
        if( -1 == write(file_templayer0, data, length))
            errno_exit("write_file_error");
    }

#endif

}


void write_templayer01(char* data, int length,long timestamp) {

    if(g_vp8)
        add_vp8_frame_header1(length,timestamp);

#ifdef USE_FWRITE

    if (file_templayer1 != NULL) {
        if( -1 == fwrite(data, length, 1, file_templayer1))
            errno_exit("write_file_error");
    }
#else
    if (file_templayer1 != -1) {
        if( -1 == write(file_templayer1, data, length))
            errno_exit("write_file_error");
    }

#endif

}


void write_templayer012(char* data, int length,long timestamp) {


    if(g_vp8)
        add_vp8_frame_header2(length,timestamp);

#ifdef USE_FWRITE

    if (file_templayer2 != NULL) {
        if( -1 == fwrite(data, length, 1, file_templayer2))
            errno_exit("write_file_error");
    }
#else
    if (file_templayer2 != -1) {
        if( -1 == write(file_templayer2, data, length))
            errno_exit("write_file_error");
    }

#endif

}


void write_templayer0123(char* data, int length,long timestamp) {

    if(g_vp8)
        add_vp8_frame_header3(length,timestamp);


#ifdef USE_FWRITE

    if (file_templayer3 != NULL) {
        if( -1 == fwrite(data, length, 1, file_templayer3))
            errno_exit("write_file_error");
    }
#else
    if (file_templayer3 != -1) {
        if( -1 == write(file_templayer3, data, length))
            errno_exit("write_file_error");
    }

#endif

}
