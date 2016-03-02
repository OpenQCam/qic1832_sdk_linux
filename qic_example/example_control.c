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
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>

#define _GNU_SOURCE
#include <getopt.h>

#include "../qic_api/qic_include/qic_include_all.h"
#include "misc_writefile.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

/*
// Developing
typedef struct{
    signed int max;
    signed int min;
    signed int def;
    signed int cur;
}api_param_attr;

typedef struct {
    api_param_attr api_obj[50];
}api_list;

int ShowApiParameterRange(int api_id, api_param_attr* api_);
*/

int GetControlMmio(int id, unsigned int mmio_addr, unsigned int *mmio_value);
int SetControlMmio(int id, unsigned int mmio_addr, unsigned int mmio_value);

////////////////////////////////////////////////
///
/// Controls
///

typedef enum tagControls
{
    Ctrl_CT_Pan,
    Ctrl_CT_Tilt,
    Ctrl_CT_Roll,
    Ctrl_CT_Zoom,
    Ctrl_CT_Exposure,
    Ctrl_CT_Iris,
    Ctrl_CT_Focus,
    Ctrl_PU_Brightness,
    Ctrl_PU_Contrast,
    Ctrl_PU_Hue,
    Ctrl_PU_Saturation,
    Ctrl_PU_Sharpness,
    Ctrl_PU_Gamma,
    Ctrl_PU_ColorEnable,
    Ctrl_PU_WhiteBalance,
    Ctrl_PU_BacklightComp,
    Ctrl_PU_Gain,
    Ctrl_EU_CPBSize,
    Ctrl_EU_AverageBitRate,
    Ctrl_EU_RateControlMode,	// probe commit use,
    Ctrl_EU_Resolution,
    Ctrl_EU_Qp,
    Ctrl_EU_SyncLongTermRefFrame,
    Ctrl_EU_FrameInterval,
    Ctrl_EU_SelectLayer,
    Ctrl_EU_SupportControl, // read-only
    Ctrl_EU_TemporalLayer,	// probe commit use
    Ctrl_EU_UsageMode,		// probe commit use
    Ctrl_XU_LTRPictureControl,
    Ctrl_XU_LTRValidationControl,
    Ctrl_XU_IR,
    Ctrl_XU_ALS,
    Ctrl_PU_PowerLineFrequency,
    Ctrl_PU_Mirror,
    Ctrl_PU_Flip,
    Ctrl_XU_MMIO
}Controls;

#define NYI printf("%s: id %d is not implemented yet.\n", __FUNCTION__, id)

static int g_fd;

char* ToString(Controls ctrl)
{
    // retrieve name
    char* pName;
    switch(ctrl){
    case Ctrl_CT_Pan:					pName = "Pan"; break;
    case Ctrl_CT_Tilt:					pName = "Tilt"; break;
    case Ctrl_CT_Roll:					pName = "Roll"; break;
    case Ctrl_CT_Zoom:					pName = "Zoom"; break;
    case Ctrl_CT_Exposure:				pName = "Exposure"; break;
    case Ctrl_CT_Iris:					pName = "Iris"; break;
    case Ctrl_CT_Focus:					pName = "Focus"; break;
    case Ctrl_PU_Brightness:			pName = "Brightness"; break;
    case Ctrl_PU_Contrast:				pName = "Constrast"; break;
    case Ctrl_PU_Hue:					pName = "Hue"; break;
    case Ctrl_PU_Saturation:			pName = "Saturation"; break;
    case Ctrl_PU_Sharpness:				pName = "Sharpness"; break;
    case Ctrl_PU_Gamma:					pName = "Gamma"; break;
    case Ctrl_PU_ColorEnable:			pName = "ColorEnable"; break;
    case Ctrl_PU_WhiteBalance:			pName = "WhiteBalance"; break;
    case Ctrl_PU_BacklightComp:			pName = "BacklightComp"; break;
    case Ctrl_PU_Gain:					pName = "Gain"; break;
    case Ctrl_EU_CPBSize:				pName = "CPBSize"; break;
    case Ctrl_EU_AverageBitRate:		pName = "AverageBitRate"; break;
    case Ctrl_EU_RateControlMode:		pName = "RateControlMode"; break;
    case Ctrl_EU_Resolution:			pName = "Resolution"; break;
    case Ctrl_EU_Qp:					pName = "QuantazationParameter"; break;
    case Ctrl_EU_SyncLongTermRefFrame:	pName = "SyncLongTermRefFrame"; break;
    case Ctrl_EU_FrameInterval:			pName = "FrameInterval"; break;
    case Ctrl_EU_SelectLayer:			pName = "SelectLayer"; break;
    case Ctrl_EU_SupportControl:        pName = "SupportControl"; break;
    case Ctrl_EU_TemporalLayer:         pName = "TemporalLayer"; break;
    case Ctrl_EU_UsageMode:             pName = "UsageMode"; break;
    case Ctrl_XU_LTRPictureControl:		pName = "LTRPictureControl"; break;
    case Ctrl_XU_LTRValidationControl:	pName = "LTRValidationControl"; break;
    case Ctrl_XU_IR:					pName = "IR"; break;
    case Ctrl_XU_ALS:					pName = "ALS"; break;
    case Ctrl_PU_PowerLineFrequency:	pName = "PowerLineFrequency"; break;
    case Ctrl_PU_Mirror:				pName = "Mirror"; break;
    case Ctrl_PU_Flip:					pName = "Flip"; break;
    case Ctrl_XU_MMIO:                  pName = "MMIO"; break;
    default:							pName = "UNKNOWN"; break;
    }

    return pName;
}

int SetControl(int id, __uint64_t val, int bAuto)
{
    int qic_ret;
    signed short pan, tilt;
    unsigned char h_flip, v_flip;

    switch(id)
    {
    case Ctrl_CT_Pan:
        qic_ret = QicGetPanTilt(&pan, &tilt);
        if(qic_ret){
            printf("QicGetPanTilt() fails, ret=%d\n", qic_ret);
            return qic_ret;
        }
        if((signed short)val<0){
            pan=(~(signed short)val)+0x8001;
        }
        else
            pan = (signed short)val;

        qic_ret = QicSetPanTilt(pan, tilt);
        if(qic_ret){
            printf("QicSetPanTilt() fails, ret=%d\n", qic_ret);
            return qic_ret;
        }
        return 0;

    case Ctrl_CT_Tilt:
        qic_ret = QicGetPanTilt(&pan, &tilt);
        if(qic_ret){
            printf("QicGetPanTilt() fails, ret=%d\n", qic_ret);
            return qic_ret;
        }
        if((signed short)val<0){
            tilt=(~(signed short)val)+0x8001;
        }
        else
            tilt = (signed short)val;

        qic_ret = QicSetPanTilt(pan, tilt);
        if(qic_ret){
            printf("QicSetPanTilt() fail, ret=%d\n", qic_ret);
            return qic_ret;
        }
        return 0;

    case Ctrl_CT_Roll:
        NYI;
        return -1;

    case Ctrl_CT_Zoom:
        return qic_V4L2_Control(g_fd, V4L2_CID_ZOOM_ABSOLUTE, 0, 0, val);

    case Ctrl_CT_Iris:
        return qic_V4L2_Control(g_fd, V4L2_CID_IRIS_ABSOLUTE, 0, 0, val);

    case Ctrl_CT_Focus:
        if(bAuto == 1){
            if(qic_V4L2_Control(g_fd, V4L2_CID_FOCUS_AUTO, 0, 0, val)){
                return -1;
            }
        }
        else{
            if(qic_V4L2_Control(g_fd, V4L2_CID_FOCUS_AUTO, 0, 0, 0)){
                return -1;
            }
            return qic_V4L2_Control(g_fd, V4L2_CID_FOCUS_ABSOLUTE, 0, 0, val);
        }
        return 0;

    case Ctrl_CT_Exposure:
        if(bAuto == 1){
            if(qic_V4L2_Control(g_fd, V4L2_CID_EXPOSURE_AUTO, 0, 0, val)){
                return -1;
            }
        }
        else{
            if(qic_V4L2_Control(g_fd, V4L2_CID_EXPOSURE_AUTO, 0, 0, 1)){
                return -1;
            }
            return qic_V4L2_Control(g_fd, V4L2_CID_EXPOSURE_ABSOLUTE, 0, 0, val);
        }
        return 0;

    case Ctrl_PU_Brightness:
        return qic_V4L2_Control(g_fd, V4L2_CID_BRIGHTNESS, 0, 0, val);

    case Ctrl_PU_Contrast:
        return qic_V4L2_Control(g_fd, V4L2_CID_CONTRAST, 0, 0, val);

    case Ctrl_PU_Hue:
        return qic_V4L2_Control(g_fd, V4L2_CID_HUE, 0, 0, val);

    case Ctrl_PU_Saturation:
        return qic_V4L2_Control(g_fd, V4L2_CID_SATURATION, 0, 0, val);

    case Ctrl_PU_Sharpness:
        return qic_V4L2_Control(g_fd, V4L2_CID_SHARPNESS, 0, 0, val);

    case Ctrl_PU_Gamma:
        return qic_V4L2_Control(g_fd, V4L2_CID_GAMMA, 0, 0, val);

    case Ctrl_PU_ColorEnable:
        NYI;
        return -1;

    case Ctrl_PU_WhiteBalance:
        if(bAuto == 1){
            if(qic_V4L2_Control(g_fd, V4L2_CID_AUTO_WHITE_BALANCE, 0, 0, val)){
                return -1;
            }
        }
        else{
            if(qic_V4L2_Control(g_fd, V4L2_CID_AUTO_WHITE_BALANCE, 0, 0, 0)){
                return -1;
            }
            return qic_V4L2_Control(g_fd, V4L2_CID_WHITE_BALANCE_TEMPERATURE, 0, 0, val);
        }
        return 0;

    case Ctrl_PU_BacklightComp:
        return qic_V4L2_Control(g_fd, V4L2_CID_BACKLIGHT_COMPENSATION, 0, 0, val);

    case Ctrl_PU_Gain:
        return qic_V4L2_Control(g_fd, V4L2_CID_GAIN, 0, 0, val);

    case Ctrl_EU_CPBSize:
        qic_ret = QicEuSetCpbSizeControl((unsigned int)val);
        if(qic_ret){
            return -1;
        }
        return 0;

    case Ctrl_EU_AverageBitRate:
        qic_ret = QicEuSetAverageBitrateControl((unsigned int)val);
        if(qic_ret){
            return -1;
        }
        return 0;

    case Ctrl_EU_RateControlMode:
        qic_ret = QicEuSetRateControlMode((unsigned char)val);
        if(qic_ret){
            return -1;
        }
        return 0;

    case Ctrl_EU_Resolution:
        qic_ret = QicEuSetVideoResolution((unsigned short)val, (unsigned short)(val>>16));
        if(qic_ret){
            return -1;
        }
        return 0;

    case Ctrl_EU_Qp:
        qic_ret = QicEuSetQuantizationParameter((unsigned short)val, (unsigned short)(val>>16), (unsigned short)(val>>32));
        if(qic_ret){
            return -1;
        }
        return 0;

    case Ctrl_EU_SyncLongTermRefFrame:
        qic_ret = QicEuSetSynchronizationAndLongTermReferenceFrame((unsigned char)val, (unsigned short)(val>>8), (unsigned char)(val>>24));
        if(qic_ret){
            return -1;
        }
        return 0;

    case Ctrl_EU_FrameInterval:
        qic_ret = QicEuSetMinimumFrameInterval((unsigned int)val);
        if(qic_ret){
            return -1;
        }
        return 0;

    case Ctrl_EU_SelectLayer:
        qic_ret = QicEuSetSelectLayer((unsigned short)val);
        if(qic_ret){
            return -1;
        }
        return 0;

    case Ctrl_EU_SupportControl:
        NYI;
        return -1;

    case Ctrl_EU_TemporalLayer:
        qic_ret = QicEuExSetTsvc((unsigned char)val);
        if(qic_ret){
            return -1;
        }
        return 0;

    case Ctrl_EU_UsageMode:
        NYI;
        return -1;

    case Ctrl_XU_LTRPictureControl:
    case Ctrl_XU_LTRValidationControl:
        NYI;
        return -1;

    case Ctrl_XU_IR:
        return QicSetIR((unsigned char)val);

    case Ctrl_XU_ALS:
        NYI;
        return -1;

    case Ctrl_PU_PowerLineFrequency:
        return qic_V4L2_Control(g_fd, V4L2_CID_POWER_LINE_FREQUENCY, 0, 0,val);

    case Ctrl_PU_Mirror:
        qic_ret = QicSetMirror(val);
        if(qic_ret){
            return qic_ret;
        }
        return 0;

    case Ctrl_PU_Flip:
        h_flip = val & H_FLIP;
        v_flip = (val & V_FLIP)>>1;

        QicChangeFD(g_fd);
        return QicSetFlipMode(v_flip, h_flip);
    }
    return -1;
}

int GetControl(int id, __uint64_t* val, signed long* bAuto)
{
    int qic_ret;
    signed short pan, tilt;
    unsigned char h_flip, v_flip;

    *val = 0;
    *bAuto = 0;

    switch(id){
    case Ctrl_CT_Pan:
        qic_ret = QicGetPanTilt(&pan, &tilt);
        if(qic_ret){
            return qic_ret;
        }
        *val = pan;
        return 0;

    case Ctrl_CT_Tilt:
        qic_ret = QicGetPanTilt(&pan, &tilt);
        if(qic_ret){
            return qic_ret;
        }
        *val = tilt;
        return 0;

    case Ctrl_CT_Roll:
        NYI;
        return -1;

    case Ctrl_CT_Zoom:
        return qic_V4L2_Control(g_fd, V4L2_CID_ZOOM_ABSOLUTE, 1, (signed long*)val, 0);

    case Ctrl_CT_Iris:
        return qic_V4L2_Control(g_fd, V4L2_CID_IRIS_ABSOLUTE, 1, (signed long*)val, 0);

    case Ctrl_CT_Focus:
        return qic_V4L2_Control(g_fd, V4L2_CID_FOCUS_ABSOLUTE, 1, (signed long*)val, 0);

    case Ctrl_CT_Exposure:
        return qic_V4L2_Control(g_fd, V4L2_CID_EXPOSURE, 1, (signed long*)val, 0);

    case Ctrl_PU_Brightness:
        return qic_V4L2_Control(g_fd, V4L2_CID_BRIGHTNESS, 1, (signed long*)val, 0);

    case Ctrl_PU_Contrast:
        return qic_V4L2_Control(g_fd, V4L2_CID_CONTRAST, 1, (signed long*)val, 0);

    case Ctrl_PU_Hue:
        return qic_V4L2_Control(g_fd, V4L2_CID_HUE, 1, (signed long*)val, 0);

    case Ctrl_PU_Saturation:
        return qic_V4L2_Control(g_fd, V4L2_CID_SATURATION, 1, (signed long*)val, 0);

    case Ctrl_PU_Sharpness:
        return qic_V4L2_Control(g_fd, V4L2_CID_SHARPNESS, 1, (signed long*)val, 0);

    case Ctrl_PU_Gamma:
        return qic_V4L2_Control(g_fd, V4L2_CID_GAMMA, 1, (signed long*)val, 0);

    case Ctrl_PU_ColorEnable:
        NYI;
        return -1;

    case Ctrl_PU_WhiteBalance:
        return qic_V4L2_Control(g_fd, V4L2_CID_WHITE_BALANCE_TEMPERATURE, 1, (signed long*)val, 0);

    case Ctrl_PU_BacklightComp:
        return qic_V4L2_Control(g_fd, V4L2_CID_BACKLIGHT_COMPENSATION, 1, (signed long*)val, 0);

    case Ctrl_PU_Gain:
        return qic_V4L2_Control(g_fd, V4L2_CID_GAIN, 1, (signed long*)val, 0);

    case Ctrl_EU_CPBSize:
        qic_ret = QicEuGetCpbSizeControl((unsigned int*)val);
        if(qic_ret){
            return -1;
        }
        return 0;

    case Ctrl_EU_AverageBitRate:
        qic_ret = QicEuGetAverageBitrateControl((unsigned int*)val);
        if(qic_ret){
            return -1;
        }
        return 0;

    case Ctrl_EU_RateControlMode:
        qic_ret = QicEuGetRateControlMode((unsigned char*)val);
        if(qic_ret){
            return -1;
        }
        return 0;

    case Ctrl_EU_Resolution:
    {
        unsigned short width, height;
        qic_ret = QicEuGetVideoResolution(&width, &height);
        if(qic_ret){
            return -1;
        }
        printf("width=%d, height=%d\n", width, height);
        *val = height;
        *val = ((*val) << 16) | width;
        return 0;
    }

    case Ctrl_EU_Qp:
    {
        unsigned short wQpPrime_I, wQpPrime_P, wQpPrime_B;
        qic_ret = QicEuGetQuantizationParameter(&wQpPrime_I, &wQpPrime_P, &wQpPrime_B);
        if(qic_ret){
            return -1;
        }
        printf("wQpPrime_I=%d, wQpPrime_P=%d, wQpPrime_B=%d\n", wQpPrime_I, wQpPrime_P, wQpPrime_B);
        *val = wQpPrime_B;
        *val = ((*val)<<16) | wQpPrime_P;
        *val = ((*val)<<16) | wQpPrime_I;
        return 0;
    }

    case Ctrl_EU_SyncLongTermRefFrame:
    {
        unsigned char bSyncFrameType;
        unsigned short wSyncFrameInterval;
        unsigned char bGradualDecoderRefresh;
        qic_ret = QicEuGetSynchronizationAndLongTermReferenceFrame(&bSyncFrameType, &wSyncFrameInterval, &bGradualDecoderRefresh);
        if(qic_ret){
            return -1;
        }
        printf("bSyncFrameType=%d, wSyncFrameInterval=%d, bGradualDecoderRefresh=%d\n", bSyncFrameType, wSyncFrameInterval, bGradualDecoderRefresh);
        *val = bGradualDecoderRefresh;
        *val = ((*val)<<16) | wSyncFrameInterval;
        *val = ((*val)<<8) | bSyncFrameType;
        return 0;
    }

    case Ctrl_EU_FrameInterval:
        qic_ret = QicEuGetMinimumFrameInterval((unsigned int*)val);
        if(qic_ret){
            return -1;
        }
        return 0;

    case Ctrl_EU_SelectLayer:
        qic_ret = QicEuGetSelectLayer((unsigned short*)val);
        if(qic_ret){
            return -1;
        }
        return 0;

    case Ctrl_EU_SupportControl:
        NYI;
        return -1;

    case Ctrl_EU_TemporalLayer:
        qic_ret = QicEuExGetTsvc((unsigned char*)val);
        if(qic_ret){
            return -1;
        }
        return 0;

    case Ctrl_EU_UsageMode:
        NYI;
        return -1;

    case Ctrl_XU_LTRPictureControl:
    case Ctrl_XU_LTRValidationControl:
        NYI;
        return -1;

    case Ctrl_XU_IR:
    {
        unsigned char als_mode, ir_status;
        qic_ret = QicGetIR((unsigned char*)val);
        if(qic_ret){
            return -1;
        }
        als_mode = (*val>>1) & 0x1;
        ir_status = (*val) & 0x1;
        printf("ALS mode=%d, IR status=%d\n", als_mode, ir_status);
        return 0;
    }

    case Ctrl_XU_ALS:
    {
        qic_ret = QicGetALS((unsigned short*)val);
        if(qic_ret){
            return -1;
        }
        return 0;
    }

    case Ctrl_PU_PowerLineFrequency:
        return qic_V4L2_Control(g_fd, V4L2_CID_POWER_LINE_FREQUENCY, 1, (signed long*)val, 0);

    case Ctrl_PU_Mirror:
    {
        qic_ret = QicGetMirror((unsigned char*)val);
        if(qic_ret){
            return -1;
        }
        return 0;
    }

    case Ctrl_PU_Flip:
    {
        qic_ret = QicGetFlipMode(&v_flip, &h_flip);
        if(qic_ret){
            return -1;
        }
        printf("H_FLIP=%s, V_FLIP=%s\n", h_flip?"on":"off", v_flip?"on":"off");

        return 0;
    }

    default:
        return -1;
    }
}

int SetControlMmio(int id, unsigned int mmio_addr, unsigned int mmio_value)
{
    int qic_ret;

    switch(id)
    {
    case Ctrl_XU_MMIO:
        printf("MMIO Set, addr=0x%x, value=0x%x\n", mmio_addr, mmio_value);
        qic_ret = QicMmioWrite(mmio_addr, mmio_value);
        if(qic_ret){
            return qic_ret;
        }
        return 0;

    default:
        return -1;
    }
}

int GetControlMmio(int id, unsigned int mmio_addr, unsigned int *mmio_value)
{
    int qic_ret;

    *mmio_value = 0;

    switch(id){
    case Ctrl_XU_MMIO:
    {
        printf("before MMIO Get, addr=0x%x, value=0x%x\n", mmio_addr, *mmio_value);
        qic_ret = QicMmioRead(mmio_addr, mmio_value);
        printf("MMIO Get, addr=0x%x, value=0x%x\n", mmio_addr, *mmio_value);
        if(qic_ret){
            return -1;
        }
        return 0;
    }

    default:
        return -1;
    }
}

void ListControls(void)
{
    static const Controls suppported_control_list[] = {
        Ctrl_CT_Pan,
        Ctrl_CT_Tilt,
        Ctrl_CT_Roll,
        Ctrl_CT_Zoom,
        Ctrl_CT_Exposure,
        Ctrl_CT_Iris,
        Ctrl_CT_Focus,
        Ctrl_PU_Brightness,
        Ctrl_PU_Contrast,
        Ctrl_PU_Hue,
        Ctrl_PU_Saturation,
        Ctrl_PU_Sharpness,
        Ctrl_PU_Gamma,
        Ctrl_PU_ColorEnable,
        Ctrl_PU_WhiteBalance,
        Ctrl_PU_BacklightComp,
        Ctrl_PU_Gain,
        Ctrl_EU_CPBSize,
        Ctrl_EU_AverageBitRate,
        Ctrl_EU_RateControlMode,	// probe commit use,
        Ctrl_EU_Resolution,
        Ctrl_EU_Qp,
        Ctrl_EU_SyncLongTermRefFrame,
        Ctrl_EU_FrameInterval,
        Ctrl_EU_SelectLayer,
        Ctrl_EU_SupportControl, // read-only
        Ctrl_EU_TemporalLayer,	// probe commit use
        Ctrl_EU_UsageMode,		// probe commit use
        Ctrl_XU_LTRPictureControl,
        Ctrl_XU_LTRValidationControl,
        Ctrl_XU_IR,
        Ctrl_XU_ALS,
        Ctrl_PU_PowerLineFrequency,
        Ctrl_PU_Mirror,
        Ctrl_PU_Flip,
        Ctrl_XU_MMIO
    };

    int control_id;
    __uint64_t control_value;
    signed long control_value_auto = -1;
    int i;

    for(i=0;i<sizeof(suppported_control_list)/sizeof(suppported_control_list[0]);i++)
    {
        control_id = suppported_control_list[i];
        if(!GetControl(control_id, &control_value, &control_value_auto))
        {
            if(control_value_auto)
                printf("id: %d, %s, current:%lld. auto \n\n", control_id, ToString(control_id), control_value);
            else
                printf("id: %d, %s, current:%lld. \n\n", control_id, ToString(control_id), control_value);
        }
        else{
            printf("id: %d, %s, get fails. \n\n", control_id, ToString(control_id));
        }
    }
}

////////////////////////////////////////////////
///
/// Main()
///

enum cmd_t{
    CMD_SET,
    CMD_GET,
    CMD_LIST,
    CMD_RESET,
    CMD_MMIO_SET,
    CMD_MMIO_GET
};

static void usage(FILE * fp, int argc, char **argv)
{
    fprintf(fp,
            "Usage: %s [options]\n\n"
            "Options:\n"
            "-d | --device <index>                      Device index, default is 0. \n"
            "-s | --set-control 'id val [auto/manual]'  Set current control to val, if there's option auto/manuel, 1:auto, 0:manual.\n"
            "-g | --get-control id                      Get current control value. \n"
            "-l | --list-controls                       List all supported controls. \n"
            "-r | --reset                               reset QIC1832. \n"
            "-h | --help                                Show this menu. \n"
            "\n"
            "Example\n"
            "    ./example_control                      list all controls\n"
            "    ./example_control -s \"24 0\"            select stream 0\n"
            "    ./example_control -s \"20 15728960\"     set resolution to 320x240\n"
            "    ./example_control -g 20                get current resolution\n"
            "",
            argv[0]);
}

static const char short_options [] = "d:s:g:lrh";

static const struct option long_options [] =
{
    { "device",         required_argument,  NULL,	'd' },
    { "set-control",    required_argument,	NULL,	's' },
    { "get-control",    required_argument,	NULL,	'g' },
    { "list-controls",  no_argument,        NULL,	'l' },
    { "reset",          no_argument,        NULL,	'r' },
    { "help",           no_argument,		NULL,	'h' },
    { 0, 0, 0, 0 }
};

static void debug_log(int level, char *string) {
    printf("QIC debug_print:%s", string);
}

int main(int argc,char ** argv)
{
    printf("Example code to control QIC1832 Parameter \n\r");

    //    if (getuid() != 0){
    //        printf("please run as root\n");
    //        exit(0);
    //    }

    // 1. Check input argument
    int device_index = 0;
    int control_id = 0;
    __uint64_t control_value;
    signed long control_value_auto = -1;
    unsigned int mmio_addr = 0;
    unsigned int mmio_value = 0;
    enum cmd_t command = CMD_LIST;

    for (;;)
    {
        int index;
        int c;
        char* tmp;

        c = getopt_long(argc, argv,
                        short_options, long_options,
                        &index);

        if (-1 == c)
            break;

        switch (c)
        {
        case 'd':
            device_index = atoi(optarg);
            break;

        case 's':
            /* get id */
            tmp = strtok(optarg, " ");
            if(tmp == NULL){
                printf("Invalid arguments %s\n", optarg);
            }
            control_id = atoi(tmp);

            if(control_id==Ctrl_XU_MMIO){// cmd example: -s '35 0x20020200 0x2'
                // Get addr
                tmp = strtok(NULL, " ");
                if(tmp == NULL){
                    printf("Invalid arguments %s\n", optarg);
                }
                mmio_addr = myatoi(tmp);

                // Get value
                tmp = strtok(NULL, " ");
                if(tmp == NULL){
                    printf("Invalid arguments %s\n", optarg);
                }
                mmio_value = myatoi(tmp);

                command = CMD_MMIO_SET;
            }
            else{
                /* get val */
                tmp = strtok(NULL, " ");
                if(tmp == NULL){
                    printf("Invalid arguments %s\n", optarg);
                }
                control_value = atoll(tmp);

                /* get auto/manual flag */
                tmp = strtok(NULL, " ");
                if(tmp != NULL){
                    if(strncmp("auto", tmp, 4) == 0)
                        control_value_auto = 1;
                    else
                        control_value_auto = 0;
                }
                else{
                    control_value_auto = -1;
                }

                command = CMD_SET;
            }

            break;

        case 'g':
            tmp = strtok(optarg, " ");
            if(tmp == NULL){
                printf("Invalid arguments %s\n", optarg);
            }
            control_id = atoi(tmp);

            if(control_id==Ctrl_XU_MMIO){// cmd example: -g '35 0x20020200'
                // Get addr
                tmp = strtok(NULL, " ");
                if(tmp == NULL){
                    printf("Invalid arguments %s\n", optarg);
                }
                mmio_addr = myatoi(tmp);

                command = CMD_MMIO_GET;
            }
            else{
                // No input

                command = CMD_GET;
            }
            break;

        case 'l':
            command = CMD_LIST;
            break;

        case 'r':
            command = CMD_RESET;
            break;

        case 'h':
            usage(stdout, argc, argv);
            exit(EXIT_SUCCESS);
        }
    }

    // 2. initialize
    char dev_name[NAME_MAX];
    sprintf(dev_name, "/dev/video%d", device_index);

    g_fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);
    if(g_fd <= 0)
    {
        printf("Open %s failed\n", dev_name);
        exit(EXIT_FAILURE);
    }
    int qic_ret = QicSetDeviceHandle(g_fd);
    if(qic_ret)
    {
        printf("QicSetDeviceHandle fails, ret=%d\n", qic_ret);
        exit(EXIT_FAILURE);
    }

    /* fix bug: segmentation fault in sdk, caused bby debug function is not set */
    qic_module* my_qic = qic_initialize(1);
    my_qic->debug_print = &debug_log;

    // 3. Based on command to execute command
    printf("================= \nDevice name: %s.\n\n", dev_name);
    switch(command)
    {
    case CMD_GET:
        if(!GetControl(control_id, &control_value, &control_value_auto))
        {
            if(control_value_auto){
                printf("Control %s get ok, is:%lld auto. \n\n", ToString(control_id), control_value);
            }
            else{
                printf("Control %s get ok, is:%lld. \n\n", ToString(control_id), control_value);
            }
        }
        else{
            printf("Control %s get fails. \n\n", ToString(control_id));
        }
        break;

    case CMD_MMIO_GET:
        if(!GetControlMmio(control_id, mmio_addr, &mmio_value))
        {
            printf("Control %s get ok \n\n", ToString(control_id));
        }
        else{
            printf("Control %s get fails. \n\n", ToString(control_id));
        }
        break;

    case CMD_SET:
        if(!SetControl(control_id, control_value, control_value_auto))
        {
            printf("Control %s set ok. \n\n", ToString(control_id));
        }
        else{
            printf("Control %s set fails. \n\n", ToString(control_id));
        }
        break;

    case CMD_MMIO_SET:
        if(!SetControlMmio(control_id, mmio_addr, mmio_value))
        {
            printf("Control %s set ok. \n\n", ToString(control_id));
        }
        else{
            printf("Control %s set fails. \n\n", ToString(control_id));
        }
        break;

    case CMD_LIST:
        ListControls();
        break;

    case CMD_RESET:
        printf("QIC Device reset \n");
        QicReset();
        break;
    }

    // Release qic before exit
    if(g_fd) close(g_fd);

    return 0;
}
