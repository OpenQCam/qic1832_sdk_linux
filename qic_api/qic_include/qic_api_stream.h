/*
--       This software proprietary belong to Quanta computer Inc. and may be used        --
--        only as expressly authorized agreement from          --
--                            Quanta Computer Inc..                             --
--                   (C) COPYRIGHT 2010 Quanta Computer Inc.                    --
--                            ALL RIGHTS RESERVED                             --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
*/

#ifndef _QIC_API_STREAM_H_
#define _QIC_API_STREAM_H_

#include "qic_api_common_define.h"

#define MAX_SUPPORT_DEVICES 8

typedef struct {
    unsigned short width;
    unsigned short height;
    unsigned char framerate;
} SUPPORT_FORMAT;

/* intial, setup, teardown */
qic_module* qic_initialize(int num_devices);
int qic_config_commit(void);
int qic_config_commit_open_only(void);
int qic_force_config(void);
int qic_release(void);

/* run-time control */
int qic_start_capture(unsigned int dev_id);
int qic_stop_capture(unsigned int dev_id);

unsigned int qic_get_yuyv_bad_frame_count(void);
unsigned int qic_get_vp8_bad_frame_count(void);
unsigned int qic_get_h264_bad_frame_count(void);

int qic_getframe2(unsigned int dev_id);

int enum_device_formats(int fd, char* format,char print);

int qic_enum_device_formats(qic_dev_name_s *name);

#endif
