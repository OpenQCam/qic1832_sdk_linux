/*
--       This software proprietary belong to Quanta computer Inc. and may be used        --
--        only as expressly authorized agreement from          --
--                            Quanta Computer Inc..                             --
--                   (C) COPYRIGHT 2010 Quanta Computer Inc.                    --
--                            ALL RIGHTS RESERVED                             --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
*/

#ifndef _QIC_API_V4L2_CTPU_H_
#define _QIC_API_V4L2_CTPU_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "qic_api_common_define.h"

//#define USE_K26_24
#ifdef USE_K26_24
//#define V4L2_CTRL_CLASS_USER 0x00980000
//#define V4L2_CID_BASE			(V4L2_CTRL_CLASS_USER | 0x900)
//#define V4L2_CID_USER_BASE 		V4L2_CID_BASE
#define V4L2_CTRL_CLASS_CAMERA 0x009a0000	/* Camera class controls */
#define V4L2_CID_POWER_LINE_FREQUENCY	(V4L2_CID_BASE+24)
enum v4l2_power_line_frequency {
    V4L2_CID_POWER_LINE_FREQUENCY_DISABLED	= 0,
    V4L2_CID_POWER_LINE_FREQUENCY_50HZ	= 1,
    V4L2_CID_POWER_LINE_FREQUENCY_60HZ	= 2,
};
#define V4L2_CID_HUE_AUTO                       (V4L2_CID_BASE+25)
#define V4L2_CID_WHITE_BALANCE_TEMPERATURE      (V4L2_CID_BASE+26)
#define V4L2_CID_SHARPNESS                      (V4L2_CID_BASE+27)
#define V4L2_CID_BACKLIGHT_COMPENSATION         (V4L2_CID_BASE+28)
#define V4L2_CID_CHROMA_AGC                     (V4L2_CID_BASE+29)
#define V4L2_CID_COLOR_KILLER                   (V4L2_CID_BASE+30)
/* last CID + 1 */
#define V4L2_CID_LASTP1                         (V4L2_CID_BASE+31)

/*  Camera class control IDs */
#define V4L2_CID_CAMERA_CLASS_BASE      (V4L2_CTRL_CLASS_CAMERA | 0x900)
#define V4L2_CID_CAMERA_CLASS           (V4L2_CTRL_CLASS_CAMERA | 1)

#define V4L2_CID_EXPOSURE_AUTO			(V4L2_CID_CAMERA_CLASS_BASE+1)
enum  v4l2_exposure_auto_type {
    V4L2_EXPOSURE_AUTO = 0,
    V4L2_EXPOSURE_MANUAL = 1,
    V4L2_EXPOSURE_SHUTTER_PRIORITY = 2,
    V4L2_EXPOSURE_APERTURE_PRIORITY = 3
};
#define V4L2_CID_EXPOSURE_ABSOLUTE      (V4L2_CID_CAMERA_CLASS_BASE+2)
#define V4L2_CID_EXPOSURE_AUTO_PRIORITY (V4L2_CID_CAMERA_CLASS_BASE+3)

#define V4L2_CID_PAN_RELATIVE			(V4L2_CID_CAMERA_CLASS_BASE+4)
#define V4L2_CID_TILT_RELATIVE			(V4L2_CID_CAMERA_CLASS_BASE+5)
#define V4L2_CID_PAN_RESET              (V4L2_CID_CAMERA_CLASS_BASE+6)
#define V4L2_CID_TILT_RESET             (V4L2_CID_CAMERA_CLASS_BASE+7)

#define V4L2_CID_PAN_ABSOLUTE			(V4L2_CID_CAMERA_CLASS_BASE+8)
#define V4L2_CID_TILT_ABSOLUTE			(V4L2_CID_CAMERA_CLASS_BASE+9)

#define V4L2_CID_FOCUS_ABSOLUTE			(V4L2_CID_CAMERA_CLASS_BASE+10)
#define V4L2_CID_FOCUS_RELATIVE			(V4L2_CID_CAMERA_CLASS_BASE+11)
#define V4L2_CID_FOCUS_AUTO			    (V4L2_CID_CAMERA_CLASS_BASE+12)
#define V4L2_CID_ZOOM_ABSOLUTE          (V4L2_CID_CAMERA_CLASS_BASE+13)
#endif

typedef struct{
    signed int max;
    signed int min;
    signed int def;
    signed int now;
}sqicv4l2value;

typedef struct {
    sqicv4l2value Brightness;
    sqicv4l2value Contrast;
    sqicv4l2value Hue;
    sqicv4l2value BC;
    sqicv4l2value WB;
    sqicv4l2value Saturation;
    sqicv4l2value Focus;
    sqicv4l2value Sharpness;
    sqicv4l2value Zoom;
    sqicv4l2value Gamma;
    sqicv4l2value Gain;
    sqicv4l2value Plf;
    sqicv4l2value E_priority;
    sqicv4l2value Exposure;
    sqicv4l2value Pan;
    sqicv4l2value Tilt;
}sqicV4L2;

int qic_V4L2_Control(int fd,unsigned long cmd,int Get,signed long *value,signed long invalue);
int qic_change_V4L2_FOCUS_ABSOLUTE(unsigned int dev_id,  unsigned int Auto,signed long absolute);
int qic_change_V4L2_BACKLIGHT_COMPENSATION(unsigned int dev_id, signed long BC) ;
int qic_change_V4L2_flip(unsigned int dev_id, unsigned char flip) ;
int qic_change_V4L2_WHITE_BALANCE(unsigned int dev_id, unsigned int Auto,signed long WB);
int qic_change_V4L2_BRIGHTNESS(unsigned int dev_id,signed long BRIGHTNESS);
int qic_change_V4L2_CONTRAST(unsigned int dev_id, signed long CONTRAST);
int qic_change_V4L2_HUE(unsigned int dev_id, signed long HUE);
int qic_change_V4L2_SATURATION(unsigned int dev_id, signed long SATURATION);
int qic_change_V4L2_SHARPNESS(unsigned int dev_id, signed long SHARPNESS);
int qic_change_V4L2_GAMMA(unsigned int dev_id, signed long GAMMA);
int qic_change_V4L2_GAIN(unsigned int dev_id, signed long GAIN);
int qic_change_V4L2_POWER_LINE_FREQUENCY(unsigned int dev_id, signed long PLF);
int qic_change_V4L2_EXPOSURE(unsigned int dev_id, unsigned int Auto, signed long absolute_or_mode);
int qic_change_V4L2_EXPOSURE_AUTO_PRIORITY(unsigned int dev_id, signed long priority);
int qic_change_V4L2_ZOOM_ABSOLUTE(unsigned int dev_id, signed long absolute);

int qic_get_ctpu_setting(int dev_id, sqicV4L2 *camerav4l2);

#ifdef __cplusplus
}
#endif

#endif
