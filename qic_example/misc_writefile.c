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

#ifdef DMALLOC
#include <dmalloc.h>
#endif

#ifdef USE_FWRITE
/*FILE *file = NULL;*/
FILE *file_264 = NULL;
FILE *file_ts  = NULL;
FILE *file_yuv = NULL;
FILE *file_mjpeg = NULL;
FILE *file_ivf = NULL;
FILE *file_log = NULL;
#else
/*int file = -1;*/
int file_264 = -1;
int file_ts = -1;
int file_yuv= -1;
int file_mjpeg = -1;
int file_ivf = -1;
int file_log = -1;
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


static void errno_exit(const char*  s)
{
    fprintf(stderr, "%s error %d, %s\n",
            s, errno, strerror(errno));

    exit(EXIT_FAILURE);
}

void add_vp8_file_header(unsigned short width, unsigned short height,unsigned int framerate, int nof)
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
    if (file_ivf != NULL) {
        if( -1 == fwrite(&header, 32, 1, file_ivf))
            errno_exit("write_file_error");
    }
#else
    if (file_ivf != -1) {
        if( -1 == write(file_ivf, &header, 32))
            errno_exit("write_file_error");
    }
#endif
}

void add_vp8_frame_header(int frame_len, long timestamp)
{
    static IVFFrameHeader frame_header;
    frame_header.size = frame_len;
    frame_header.timestamp = timestamp;
#ifdef USE_FWRITE
    if (file_ivf != NULL) {
        if( -1 == fwrite(&frame_header, 12, 1, file_ivf))
            errno_exit("write_file_error");
    }
#else
    if (file_ivf != -1) {
        if( -1 == write(file_ivf, &frame_header, 12))
            errno_exit("write_file_error");
    }
#endif
}

void write_avc (char* data, int length)
{
#ifdef USE_FWRITE
    if (file_264 != NULL) {
        if( -1 == fwrite(data, length, 1, file_264))
            errno_exit("write_file_error");
    }
    struct timeval tv;
    gettimeofday(&tv,NULL);
    unsigned long current_ts=((tv.tv_sec*1000)+(tv.tv_usec/1000));
    if (file_ts != NULL) {
        if( fprintf(file_ts, "%d %lu\n",length, current_ts) < 0 )
        errno_exit("write_file_error");
    }
#else
    if (file_264 != -1) {
        if( -1 == write(file_264, data, length))
            errno_exit("write_file_error");
    }
    //TODO write with "write" function
    /*if (file_ts != NULL) {

    }*/
#endif
}

void write_yuv (char* data, int length)
{
#ifdef USE_FWRITE
    if (file_yuv != NULL) {
        if( -1 == fwrite(data, length, 1, file_yuv))
            errno_exit("write_file_error");
    }
#else
    if (file_yuv != -1) {
        if( -1 == write(file_yuv, data, length))
            errno_exit("write_file_error");
    }
#endif
}

void write_mjpeg(char* data, int length)
{
#ifdef USE_FWRITE
    if (file_mjpeg != NULL) {
        if( -1 == fwrite(data, length, 1, file_mjpeg))
            errno_exit("write_file_error");
    }
#else
    if (file_mjpeg != -1) {
        if( -1 == write(file_mjpeg, data, length))
            errno_exit("write_file_error");
    }
#endif
}

void write_vp8(char* data, int length,long timestamp)
{
#ifdef USE_FWRITE
    add_vp8_frame_header(length,timestamp);

    if (file_ivf != NULL) {
        if( -1 == fwrite(data, length, 1, file_ivf))
            errno_exit("write_file_error");
    }
#else
    if (file_ivf != -1) {
        if( -1 == write(file_ivf, data, length))
            errno_exit("write_file_error");
    }
#endif
}

void write_log(char* str, int length)
{
#ifdef USE_FWRITE
    if (file_log != NULL) {
        if( -1 == fwrite(str, length, 1, file_log))
            errno_exit("write_log_file_error");
    }
#else
    if (file_log != -1) {
        if( -1 == write(file_log, str, length))
            errno_exit("write_log_file_error");
    }
#endif
}

void close_file(void)
{
#ifdef USE_FWRITE
    if (file_264 != NULL) {
        fflush(file_264);
        fclose(file_264);
        file_264 = NULL;
    }
    
    if (file_ts != NULL) {
        fflush(file_ts);
        fclose(file_ts);
        file_ts = NULL;
    }
    
    if (file_yuv != NULL) {
        fflush(file_yuv);
        fclose(file_yuv);
        file_yuv = NULL;
    }

    if (file_mjpeg != NULL) {
        fflush(file_mjpeg);
        fclose(file_mjpeg);
        file_mjpeg = NULL;
    }

    if (file_ivf != NULL) {
        fflush(file_ivf);
        fclose(file_ivf);
        file_ivf = NULL;
    }

    if (file_log != NULL) {
        fflush(file_log);
        fclose(file_log);
        file_log = NULL;
    }
#else
    if (file_264 != -1) {
        fsync(file_264);
        close(file_264);
        file_264 = -1;
    }
    
    if (file_ts != -1) {
        fsync(file_ts);
        close(file_ts);
        file_ts = -1;
    }
    
    if (file_yuv != -1) {
        fsync(file_yuv);
        close(file_yuv);
        file_yuv = -1;
    }

    if (file_mjpeg != -1) {
        fsync(file_mjpeg);
        close(file_mjpeg);
        file_mjpeg = -1;
    }

    if (file_ivf != -1) {
        fflush(file_ivf);
        fclose(file_ivf);
        file_ivf = -1;
    }

    if (file_log != -1) {
        fsync(file_log);
        close(file_log);
        file_log = -1;
    }
#endif
}

void open_file_dump(char* filename)
{
    char avc_name[128] = {0};
    char ts_name[128]  = {0};
    char yuv_name[128] = {0};
    char mjpeg_name[128] = {0};
    char ivf_name[128] = {0};

    if (filename != NULL) {
        snprintf(avc_name, 128, "%s.264", filename);
        snprintf(ts_name, 128, "%s.len", filename);
        snprintf(yuv_name, 128, "%s.yuv", filename);
        snprintf(mjpeg_name, 128, "%s.mjpg", filename);
        snprintf(ivf_name, 128, "%s.ivf", filename);
#ifdef USE_FWRITE
        /*file = fopen (filename, "w");*/
        file_264 = fopen (avc_name, "w");
        file_ts = fopen (ts_name, "w");
        file_yuv = fopen (yuv_name, "w");
        file_mjpeg = fopen (mjpeg_name, "w");
        file_ivf = fopen (ivf_name, "w");

#else
        /*file = open(filename, O_CREAT|O_WRONLY|O_NONBLOCK, S_IRWXU|S_IRWXG|S_IRWXO);*/
        file_264 = open(avc_name, O_CREAT|O_WRONLY|O_NONBLOCK, S_IRWXU|S_IRWXG|S_IRWXO);
        file_ts = open(ts_name, O_CREAT|O_WRONLY|O_NONBLOCK, S_IRWXU|S_IRWXG|S_IRWXO);
        file_yuv = open(yuv_name, O_CREAT|O_WRONLY|O_NONBLOCK, S_IRWXU|S_IRWXG|S_IRWXO);
        file_mjpeg = open(mjpeg_name, O_CREAT|O_WRONLY|O_NONBLOCK, S_IRWXU|S_IRWXG|S_IRWXO);
        file_ivf = open(ivf_name, O_CREAT|O_WRONLY|O_NONBLOCK, S_IRWXU|S_IRWXG|S_IRWXO);
#endif
    }
}

void open_file_log(char* filename)
{
#ifdef USE_FWRITE
    file_log = fopen (filename, "w");
#else
    file_log = open(filename, O_CREAT|O_WRONLY|O_NONBLOCK, S_IRWXU|S_IRWXG|S_IRWXO);
#endif
}

void ts_dump(int length, char*data)
{
    static FILE* fp = NULL;
    if ( fp == NULL)
        fp = fopen("./dump.ts", "w");

    if( -1 == fwrite(data, length, 1, fp))
        printf("ts_dump error: %s", strerror(errno));
}
