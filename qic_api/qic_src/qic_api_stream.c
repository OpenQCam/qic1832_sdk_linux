/*
--       This software proprietary belong to Quanta computer Inc. and may be used        --
--        only as expressly authorized agreement from          --
--                            Quanta Computer Inc..                             --
--                   (C) COPYRIGHT 2010 Quanta Computer Inc.                    --
--                            ALL RIGHTS RESERVED                             --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
*/

#include "../qic_include/qic_api_stream.h"
#include "../qic_include/qic_api_video.h"
#include "../qic_include/qic_api_md.h"
#include "../qic_include/demux_h264.h"
#include "../qic_include/demux_vp8.h"

// Common part
extern char debug_str[1024];
extern char debug_xuctrl_str[512];
extern int config_is_commit;
extern qic_module *dev_pt;

SUPPORT_FORMAT YUV_bind_format[] = {	\
    {384, 216, 0}, \
    {352, 288, 0}, \
    {320, 240, 0}, \
    {320, 200, 0}, \
    {320, 180, 0}, \
    {176, 144, 0}, \
    {160, 128, 0}, \
    {160, 120, 0}, \
    {160, 90, 0}, \
    {0,0,0} \
};

SUPPORT_FORMAT YUV_format[] = {		\
    {2592,1944, 2}, {1920,1080, 6},{1280, 800, 9},{1280, 720, 9}, \
    {768, 480, 30}, {768, 480, 25}, {768, 480, 20}, {768, 480, 15}, {768, 480, 10}, {768, 480, 5}, \
    {640, 480, 30}, {640, 480, 25}, {640, 480, 20}, {640, 480, 15}, {640, 480, 10}, {640, 480, 5}, \
    {640, 400, 30}, {640, 400, 25}, {640, 400, 20}, {640, 400, 15}, {640, 400, 10}, {640, 400, 5}, \
    {640, 360, 30}, {640, 360, 25}, {640, 360, 20}, {640, 360, 15}, {640, 360, 10}, {640, 360, 5}, \
    {480, 272, 30}, {480, 272, 25}, {480, 272, 20}, {480, 272, 15}, {480, 272, 10}, {480, 272, 5}, \
    {384, 216, 30}, {384, 216, 25}, {384, 216, 20}, {384, 216, 15}, {384, 216, 10}, {384, 216, 5}, \
    {352, 288, 30}, {352, 288, 25}, {352, 288, 20}, {352, 288, 15}, {352, 288, 10}, {352, 288, 5}, \
    {320, 240, 30}, {320, 240, 25}, {320, 240, 20}, {320, 240, 15}, {320, 240, 10}, {320, 240, 5}, \
    {320, 200, 30}, {320, 200, 25}, {320, 200, 20}, {320, 200, 15}, {320, 200, 10}, {320, 200, 5}, \
    {320, 180, 30}, {320, 180, 25}, {320, 180, 20}, {320, 180, 15}, {320, 180, 10}, {320, 180, 5}, \
    {176, 144, 30}, {176, 144, 25}, {176, 144, 20}, {176, 144, 15}, {176, 144, 10}, {176, 144, 5}, \
    {160, 128, 30}, {160, 128, 25}, {160, 128, 20}, {160, 128, 15}, {160, 128, 10}, {160, 128, 5}, \
    {160, 120, 30}, {160, 120, 25}, {160, 120, 20}, {160, 120, 15}, {160, 120, 10}, {160, 120, 5}, \
    {160, 90, 30}, {160, 90, 25}, {160, 90, 20}, {160, 90, 15}, {160, 90, 10}, {160, 90, 5}, \
    {0,0,0} \
};

SUPPORT_FORMAT MJPEG_bind_format[] = {	\
    {1920, 1080, 0},\
    {1280, 720, 0}, \
    {768, 480, 0}, \
    {640, 480, 0}, \
    {640, 400, 0},\
    {640, 360, 0}, \
    {384, 216, 0},  \
    {352, 288, 0}, \
    {320, 240, 0},  \
    {320, 200, 0}, \
    {320, 180, 0},  \
    {176, 144, 0},  \
    {160, 128, 0},  \
    {160, 120, 0}, \
    {160, 90, 0},  \
    {0,0,0} \
};

SUPPORT_FORMAT MJPEG_format[] = {	\
    {1920, 1080, 30},{1920, 1080, 25}, {1920, 1080, 20}, {1920, 1080, 15}, {1920, 1080, 10}, {1920, 1080, 5}, \
    {1280, 720, 30},{1280, 720, 25}, {1280, 720, 20}, {1280, 720, 15}, {1280, 720, 10}, {1280, 720, 5}, \
    {768, 480, 30}, {768, 480, 25}, {768, 480, 20}, {768, 480, 15}, {768, 480, 10}, {768, 480, 5}, \
    {640, 480, 30}, {640, 480, 25}, {640, 480, 20}, {640, 480, 15}, {640, 480, 10}, {640, 480, 5}, \
    {640, 400, 30}, {640, 400, 25}, {640, 400, 20}, {640, 400, 15}, {640, 400, 10}, {640, 400, 5}, \
    {640, 360, 30}, {640, 360, 25}, {640, 360, 20}, {640, 360, 15}, {640, 360, 10}, {640, 360, 5}, \
    {384, 216, 30}, {384, 216, 25}, {384, 216, 20}, {384, 216, 15}, {384, 216, 10}, {384, 216, 5}, \
    {352, 288, 30}, {352, 288, 25}, {352, 288, 20}, {352, 288, 15}, {352, 288, 10}, {352, 288, 5}, \
    {320, 240, 30}, {320, 240, 25}, {320, 240, 20}, {320, 240, 15}, {320, 240, 10}, {320, 240, 5}, \
    {320, 200, 30}, {320, 200, 25}, {320, 200, 20}, {320, 200, 15}, {320, 200, 10}, {320, 200, 5}, \
    {320, 180, 30}, {320, 180, 25}, {320, 180, 20}, {320, 180, 15}, {320, 180, 10}, {320, 180, 5}, \
    {176, 144, 30}, {176, 144, 25}, {176, 144, 20}, {176, 144, 15}, {176, 144, 10}, {176, 144, 5}, \
    {160, 128, 30}, {160, 128, 25}, {160, 128, 20}, {160, 128, 15}, {160, 128, 10}, {160, 128, 5}, \
    {160, 120, 30}, {160, 120, 25}, {160, 120, 20}, {160, 120, 15}, {160, 120, 10}, {160, 120, 5}, \
    {160, 90, 30}, {160, 90, 25}, {160, 90, 20}, {160, 90, 15}, {160, 90, 10}, {160, 90, 5}, \
    {0,0,0} \
};

SUPPORT_FORMAT AVC_format[] = { \
    {1920, 1080, 30},{1920, 1080, 25}, {1920, 1080, 20}, {1920, 1080, 15}, {1920, 1080, 10}, {1920, 1080, 5}, \
    {1280, 720, 30},{1280, 720, 25},{1280, 720, 22}, {1280, 720, 20}, {1280, 720, 15}, {1280, 720, 10}, {1280, 720, 5}, \
    {768, 480, 30}, {768, 480, 25}, {768, 480, 20}, {768, 480, 15}, {768, 480, 10}, {768, 480, 5}, \
    {640, 480, 30}, {640, 480, 25}, {640, 480, 20}, {640, 480, 15}, {640, 480, 10}, {640, 480, 5}, \
    {640, 400, 30}, {640, 400, 25}, {640, 400, 20}, {640, 400, 15}, {640, 400, 10}, {640, 400, 5}, \
    {640, 360, 30}, {640, 360, 25}, {640, 360, 20}, {640, 360, 15}, {640, 360, 10}, {640, 360, 5}, \
    {352, 288, 30}, {352, 288, 25}, {352, 288, 20}, {352, 288, 15}, {352, 288, 10}, {352, 288, 5}, \
    {320, 240, 30}, {320, 240, 25}, {320, 240, 20}, {320, 240, 15}, {320, 240, 10}, {320, 240, 5}, \
    {320, 200, 30}, {320, 200, 25}, {320, 200, 20}, {320, 200, 15}, {320, 200, 10}, {320, 200, 5}, \
    {320, 180, 30}, {320, 180, 25}, {320, 180, 20}, {320, 180, 15}, {320, 180, 10}, {320, 180, 5}, \
    {176, 144, 30}, {176, 144, 25}, {176, 144, 20}, {176, 144, 15}, {176, 144, 10}, {176, 144, 5}, \
    {160, 128, 30}, {160, 128, 25}, {160, 128, 20}, {160, 128, 15}, {160, 128, 10}, {160, 128, 5}, \
    {160, 120, 30}, {160, 120, 25}, {160, 120, 20}, {160, 120, 15}, {160, 120, 10}, {160, 120, 5}, \
    {160, 90, 30}, {160, 90, 25}, {160, 90, 20}, {160, 90, 15}, {160, 90, 10}, {160, 90, 5}, \
    {0,0,0} \
};

vp8_frames_ex_t vp8_frames;
int last_frame_sizes[4];

unsigned int yuyv_data_length, yuyv_bad_frame_count, vp8_h264_bad_frame_count;
unsigned char generate_key_frame;

// Internal function
static void send_avc_simulcast_frame(unsigned int dev_id, char *data, unsigned int length,
                                     unsigned long timestamp,unsigned int stream_id, unsigned short key_frame_interval,
                                     unsigned char demux);

static void send_vp8_simulcast_frame(unsigned int dev_id, char *data, unsigned int length,
                                     unsigned long timestamp,unsigned int stream_id, unsigned short key_frame_interval,
                                     unsigned char demux);

#ifdef DEBUG_LOG
static void debug_print_initial (int level, char *string);
#endif
static void frame_output_initial(unsigned int dev_id, unsigned int length, char *data, unsigned long timestamp);

static int qic_open_device (qic_dev *cam);
static int qic_clean_device(qic_dev *cam);
static int qic_initial_device (qic_dev *cam);

static void set_scheduler(void);

int enum_device_formats(int fd, char* format, char print);
int enum_frame_intervals(int fd, unsigned int pixfmt, unsigned int width, unsigned int height);

static int format_check(SUPPORT_FORMAT* format, unsigned short width, unsigned short height, unsigned char framerate);

/**********************************************
 *  initial functions
**********************************************/
qic_module* qic_initialize(int num_devices)
{
    /* no more init*/
    if ((config_is_commit) &&(dev_pt != NULL)){
        LOG_PRINT(debug_str, DEBUG_INFO, "QIC module config is committed, no more init\n");
        return dev_pt;
    }

    qic_module *my_dev;
    qic_dev *cams;
    unsigned int dev_id = 0x01;

    int i;

    /* lock to 8 devices */
    if ((num_devices > MAX_SUPPORT_DEVICES) || (num_devices < 1)) {
        printf("QIC module library does not support more than %d devices\n", MAX_SUPPORT_DEVICES);
        return NULL;
    }
    my_dev = malloc(sizeof(qic_module));
    cams = calloc(num_devices, sizeof(qic_dev));

    my_dev->num_devices = num_devices;
    my_dev->cam = cams;

    my_dev->frame_output = NULL;
    my_dev->debug_print = NULL;

    /* default - no change prio */
    my_dev->high_prio = 0;

    for ( i = 0; i < num_devices; i++) {
        my_dev->cam[i].dev_name = NULL;
        my_dev->cam[i].fd = -1;
        my_dev->cam[i].dev_id = dev_id;
        my_dev->cam[i].is_bind = 0;
        my_dev->cam[i].is_on = 0;
        my_dev->cam[i].num_mmap_buffer = 12; /* more for jitter buffer? */
        my_dev->cam[i].format = V4L2_PIX_FMT_MJPEG;
        my_dev->cam[i].bitrate = 0;
        my_dev->cam[i].width = 0;
        my_dev->cam[i].height = 0;
        my_dev->cam[i].framerate = 0;
        my_dev->cam[i].gop = 0;
        my_dev->cam[i].slicesize = 0; /* auto */
        my_dev->cam[i].is_demux = 0;
        my_dev->cam[i].buffers = NULL;
        my_dev->cam[i].nri_iframe = 1; /* default as 1, can't be zero*/
        my_dev->cam[i].nri_pps = 1; /* default as 1, can't be zero*/
        my_dev->cam[i].nri_sps = 1; /* default as 1, can't be zero*/
        my_dev->cam[i].nri_set = 0;

        dev_id = dev_id << 1;
    }
    dev_pt = my_dev;

    return my_dev;
}

/**********************************************
 *  setup functions
**********************************************/
int qic_config_commit(void)
{
    unsigned int i;
    int check = 0;

    /* check if committed */
    if ((config_is_commit) &&(dev_pt != NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is committed already\n");
        return 0;
    }

    /* function pointer check */
#ifdef DEBUG_LOG
    if (dev_pt->debug_print == NULL) {
        dev_pt->debug_print = &debug_print_initial;
        LOG_PRINT(debug_str, DEBUG_ERROR, "debug_print is set to NULL, default to stdout\n");
    }
#endif

    if (dev_pt->frame_output == NULL) {
        dev_pt->frame_output = &frame_output_initial;
        LOG_PRINT(debug_str, DEBUG_ERROR, "frame_output is set to NULL, default to stdout\n");
    }

    /* change the prio if set */
    if (dev_pt->high_prio) {
        set_scheduler();
        LOG_PRINT(debug_str, DEBUG_INFO, "set scheduler to high priority\n");
    }
    /* check default configuration */
    for ( i = 0; i < dev_pt->num_devices; i++) {
        if (dev_pt->cam[i].dev_name == NULL) {
            LOG_PRINT(debug_str, DEBUG_ERROR, "cam #%d dev_name is not set, NULL\n", dev_pt->cam[i].dev_id);
            check = 1;
        } else {
            LOG_PRINT(debug_str, DEBUG_INFO, "cam #%d dev_name = %s\n", dev_pt->cam[i].dev_id, dev_pt->cam[i].dev_name);
        }

        /* check mmap buffer setting */
        if (dev_pt->cam[i].num_mmap_buffer < 2 ) {
            dev_pt->cam[i].num_mmap_buffer = 12;
            LOG_PRINT(debug_str, DEBUG_ERROR, "cam #%d mmap_buffer is too small, set to default 12\n", dev_pt->cam[i].dev_id);
        } else {
            LOG_PRINT(debug_str, DEBUG_INFO, "cam #%d mmap_buffer size = %d\n", dev_pt->cam[i].dev_id, dev_pt->cam[i].num_mmap_buffer);
        }

        if (check) {

            LOG_PRINT(debug_str, DEBUG_ERROR, "cam #%d config error, dev_name not defined, aborted\n", dev_pt->cam[i].dev_id);
            return check;
        }
        /* check video format configuration */
        if (dev_pt->cam[i].format == V4L2_PIX_FMT_MJPEG) { /* MJPEG image device */
            /* remove bitrate, slice, demux setting */
            //	dev_pt->cam[i].bitrate = 0;
            //	dev_pt->cam[i].slicesize = 0;
            //dev_pt->cam[i].is_demux = 0;
            if(dev_pt->cam[i].is_demux){
                vp8_h264_bad_frame_count=0;
                generate_key_frame=0;
                demux_VP8_H264_check_bad_frame_initial();
            }
            //	dev_pt->cam[i].gop = 0;
            LOG_PRINT(debug_str, DEBUG_INFO, "cam #%d is MJPEG, set bitrate/slice/demux/gop to 0\n", dev_pt->cam[i].dev_id);
            /* is_bind check */
            dev_pt->cam[i].is_bind = dev_pt->cam[i].is_bind & 0x01;
            LOG_PRINT(debug_str, DEBUG_INFO, "cam #%d format source: %s\n", dev_pt->cam[i].dev_id, dev_pt->cam[i].is_bind == 1?"MJPEG_bind_format":"MJPEG_format");

            /* check video format configuration */
            check = 0;
            if (dev_pt->cam[i].is_bind) {
                /*clear the framerate */
                dev_pt->cam[i].framerate = 0;
                check = format_check((SUPPORT_FORMAT * )MJPEG_bind_format, dev_pt->cam[i].width, dev_pt->cam[i].height, dev_pt->cam[i].framerate);
            }
            else
                check = format_check((SUPPORT_FORMAT * )MJPEG_format, dev_pt->cam[i].width, dev_pt->cam[i].height, 0);
            if (!check) {
                if (dev_pt->cam[i].is_bind) {
                    dev_pt->cam[i].width = MJPEG_bind_format[0].width;
                    dev_pt->cam[i].height = MJPEG_bind_format[0].height;
                    dev_pt->cam[i].framerate = MJPEG_bind_format[0].framerate;
                }
                else {
                    dev_pt->cam[i].width = MJPEG_format[0].width;
                    dev_pt->cam[i].height = MJPEG_format[0].height;
                    dev_pt->cam[i].framerate = MJPEG_format[0].framerate;
                }

                LOG_PRINT(debug_str, DEBUG_ERROR, "cam #%d format error , set to %dx%d, %dfps\n", dev_pt->cam[i].dev_id, \
                          dev_pt->cam[i].width, \
                          dev_pt->cam[i].height, \
                          dev_pt->cam[i].framerate);
            } else {
                LOG_PRINT(debug_str, DEBUG_INFO, "cam #%d format: %dx%d, %dfps\n", dev_pt->cam[i].dev_id, \
                          dev_pt->cam[i].width, \
                          dev_pt->cam[i].height, \
                          dev_pt->cam[i].framerate);
            }
        }
        else if (dev_pt->cam[i].format == V4L2_PIX_FMT_YUYV) { /* YUV raw image device */
            /* remove bitrate, slice, demux setting */
            dev_pt->cam[i].bitrate = 0;
            dev_pt->cam[i].slicesize = 0;
            //dev_pt->cam[i].is_demux = 0;
            if(dev_pt->cam[i].is_demux){
                yuyv_bad_frame_count=0;
                yuyv_data_length = dev_pt->cam[i].width*dev_pt->cam[i].height*2;
            }
            dev_pt->cam[i].gop = 0;
            LOG_PRINT(debug_str, DEBUG_INFO, "cam #%d is YUYV, set bitrate/slice/demux/gop to 0\n", dev_pt->cam[i].dev_id);
            /* is_bind check */
            dev_pt->cam[i].is_bind = dev_pt->cam[i].is_bind & 0x01;
            LOG_PRINT(debug_str, DEBUG_INFO, "cam #%d format source: %s\n", dev_pt->cam[i].dev_id, dev_pt->cam[i].is_bind == 1?"YUV_bind_format":"YUV_format");

            /* check video format configuration */
            check = 0;
            if (dev_pt->cam[i].is_bind) {
                /*clear the framerate */
                dev_pt->cam[i].framerate = 0;
                check = format_check((SUPPORT_FORMAT * )YUV_bind_format, dev_pt->cam[i].width, dev_pt->cam[i].height, dev_pt->cam[i].framerate);
            }
            else
                check = format_check((SUPPORT_FORMAT * )YUV_format, dev_pt->cam[i].width, dev_pt->cam[i].height, /*dev_pt->cam[i].framerate*/0);
            if (!check) {
                if (dev_pt->cam[i].is_bind) {
                    dev_pt->cam[i].width = YUV_bind_format[0].width;
                    dev_pt->cam[i].height = YUV_bind_format[0].height;
                    dev_pt->cam[i].framerate = YUV_bind_format[0].framerate;
                }
                else {
                    dev_pt->cam[i].width = YUV_format[0].width;
                    dev_pt->cam[i].height = YUV_format[0].height;
                    dev_pt->cam[i].framerate = YUV_format[0].framerate;
                }

                LOG_PRINT(debug_str, DEBUG_ERROR, "cam #%d format error , set to %dx%d, %dfps\n", dev_pt->cam[i].dev_id, \
                          dev_pt->cam[i].width, \
                          dev_pt->cam[i].height, \
                          dev_pt->cam[i].framerate);
            }
            else {
                LOG_PRINT(debug_str, DEBUG_INFO, "cam #%d format: %dx%d, %dfps\n", dev_pt->cam[i].dev_id, \
                          dev_pt->cam[i].width, \
                          dev_pt->cam[i].height, \
                          dev_pt->cam[i].framerate);
            }
        }
        else {
            /* printf("  Frame format: "FOURCC_FORMAT"\n", FOURCC_ARGS(my_dev->cam[i].format)); */
            LOG_PRINT(debug_str, DEBUG_ERROR, "cam #%d format unknown("FOURCC_FORMAT"), aborted\n", dev_pt->cam[i].dev_id, FOURCC_ARGS(dev_pt->cam[i].format));

            return 1; /*error~*/
        }

        /* clear flag */
        dev_pt->cam[i].is_on = 0;
        /* initial device */
        check = qic_open_device(&dev_pt->cam[i]);
        if (check) {
            LOG_PRINT(debug_str, DEBUG_ERROR, "cam %s open failed\n", dev_pt->cam[i].dev_name);
            return check;
        }
        else {
            LOG_PRINT(debug_str, DEBUG_INFO, "cam %s open succeeded\n", dev_pt->cam[i].dev_name);
        }
        check = qic_initial_device(&dev_pt->cam[i]);
        if (check) {
            LOG_PRINT(debug_str, DEBUG_ERROR, "cam %s initial failed\n", dev_pt->cam[i].dev_name);
            return check;
        }
        else {
            LOG_PRINT(debug_str, DEBUG_INFO, "cam %s initial succeeded\n", dev_pt->cam[i].dev_name);
        }
    }
    /* signal the commit */
    config_is_commit = 1;
    //	LOG_PRINT(debug_str, DEBUG_DETAIL, "qic_config_commit - OUT\n");
    return 0;
}

int qic_force_config(void)
{
    int check = 0;
    int ret;

    if (dev_pt->high_prio) {
        set_scheduler();
    }

    dev_pt->cam[0].is_on = 0;

    check = qic_open_device(&dev_pt->cam[0]);

    if (check) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "cam #%d open failed\n", 0);
        return 1;
    }

    struct v4l2_format fmt;
    CLEAR(fmt);

    fmt.type				= V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width		= dev_pt->cam[0].width;
    fmt.fmt.pix.height		= dev_pt->cam[0].height;
    fmt.fmt.pix.pixelformat = dev_pt->cam[0].format;
    fmt.fmt.pix.field		= V4L2_FIELD_ANY;

    if (-1 == xioctl(dev_pt->cam[0].fd, VIDIOC_S_FMT, &fmt)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "cam %s ioctl VIDIOC_S_FMT error, (%d)%s\n", dev_pt->cam[0].dev_name, errno, strerror(errno));
        ret=1;
    }
    else {
        LOG_PRINT(debug_str, DEBUG_INFO, "cam %s ioctl VIDIOC_S_FMT success\n", dev_pt->cam[0].dev_name);
        ret=0;
    }

    if(!ret)
        config_is_commit = 1;

    ret = QicSetDeviceHandle(dev_pt->cam[0].fd);
    if (ret) {
        printf("QicSetDeviceHandle error\n");
        return 1;
    }
    return ret;
}

/**********************************************
 *  teardown functions
**********************************************/
int qic_release(void)
{
    unsigned int i;

    /* check if it is commited */
    if ((!config_is_commit)&&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_INFO, "QIC module library is not committed, need no release\n");
        return 0;
    }

    /* release all cam resources */
    for (i = 0; i < dev_pt->num_devices ; i++) {

        qic_clean_device(&dev_pt->cam[i]);
        LOG_PRINT(debug_str, DEBUG_INFO, "cam %s all buffer released\n", dev_pt->cam[i].dev_name);

        /* close the device */
        if (-1 == close(dev_pt->cam[i].fd)) {
            LOG_PRINT(debug_str, DEBUG_ERROR, "cam %s fd close error, (%d)%s\n", dev_pt->cam[i].dev_name, errno, strerror(errno));
        }
        else {
            dev_pt->cam[i].fd = -1;
            LOG_PRINT(debug_str, DEBUG_INFO, "cam %s closed\n", dev_pt->cam[i].dev_name);
        }
    }
    /* release all memory */
    free(dev_pt->cam);
    LOG_PRINT(debug_str, DEBUG_INFO, "camera memory is released\n");

    free(dev_pt);
    dev_pt = NULL;
#ifdef ENABLE_PRINT
    printf( "QIC module library terminated\n");
#endif
    config_is_commit = 0;

    return 0;
}

int qic_start_capture( unsigned int dev_id)
{
    unsigned int index;
    enum v4l2_buf_type type;
    int ret = 0;

    //	LOG_PRINT(debug_str, DEBUG_DETAIL, "qic_start_capture - IN\n");
#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) && (dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif
    /* device array loop */
    for ( index = 0; index < dev_pt->num_devices; index++) {
        if ((dev_pt->cam[index].dev_id & dev_id)&&(dev_pt->cam[index].is_on == 0)) {
            type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

            if (-1 == xioctl(dev_pt->cam[index].fd, VIDIOC_STREAMON, &type)) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "cam %s ioctl VIDIOC_STREAMON failed, (%d)%s\n", dev_pt->cam[index].dev_name, errno, strerror(errno));
                ret = 1;
                continue;
            }
            else {
                LOG_PRINT(debug_str, DEBUG_INFO, "cam %s ioctl VIDIOC_STREAMON succeeded\n", dev_pt->cam[index].dev_name);
            }
            dev_pt->cam[index].is_on = 1;

            LOG_PRINT(debug_str, DEBUG_INFO, "cam %s start capturing\n", dev_pt->cam[index].dev_name);

            if(dev_pt->cam[index].codec_type==CODEC_VP8_SIMULCAST ||dev_pt->cam[index].codec_type==CODEC_H264_SIMULCAST ){
                last_frame_sizes[0]=0;
                last_frame_sizes[1]=0;
                last_frame_sizes[2]=0;
                last_frame_sizes[3]=0;
            }
        }
    }
    //	LOG_PRINT(debug_str, DEBUG_DETAIL, "qic_start_capture - OUT\n");
    return ret;
}

int qic_stop_capture(unsigned int dev_id)
{
    unsigned int index;
    enum v4l2_buf_type type;
    int ret = 0;

#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif
    /* device array loop */
    for ( index = 0; index < dev_pt->num_devices; index++) {
        if ((dev_pt->cam[index].dev_id & dev_id)&&(dev_pt->cam[index].is_on == 1)) {
            type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

            if (-1 == xioctl(dev_pt->cam[index].fd, VIDIOC_STREAMOFF, &type)) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "cam %s ioctl VIDIOC_STREAMOFF failed, (%d)%s\n", dev_pt->cam[index].dev_name, errno, strerror(errno));
                ret = 1;
                continue;
            }
            else {
                LOG_PRINT(debug_str, DEBUG_INFO, "cam %s ioctl VIDIOC_STREAMOFF succeeded\n", dev_pt->cam[index].dev_name);
            }
            dev_pt->cam[index].is_on = 0;
            LOG_PRINT(debug_str, DEBUG_INFO, "cam %s stop capturing\n", dev_pt->cam[index].dev_name);

            /* just sleep and wake a while */
            usleep(50*1000);
        }
    }
    return ret;
}

unsigned int qic_get_yuyv_bad_frame_count(void)
{
    return yuyv_bad_frame_count;
}

unsigned int qic_get_vp8_bad_frame_count(void)
{
    return vp8_h264_bad_frame_count;
}

unsigned int qic_get_h264_bad_frame_count(void)
{
    return vp8_h264_bad_frame_count;
}

int qic_getframe2(unsigned int dev_id)
{
    unsigned int index;
    int kl;
    int cur_fd = -1;
    int ret = 0;
    struct v4l2_buffer buf;
    char *video_buffer;
    unsigned int video_buffer_len;
    unsigned long timestamp=0;
    int avc_stream_id;
    out_frame_t sframe;
    //	struct timespec ts;

    /*LOG_PRINT(debug_str, DEBUG_DETAIL, "qic_getframe - IN\n");*/
#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif

    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) { /* check all nodes for active device */
        if ((dev_pt->cam[index].dev_id & dev_id)&&(dev_pt->cam[index].is_on)) { /* get a device , and is_on*/
            cur_fd = dev_pt->cam[index].fd;

            /* dequeue the buffer */
            CLEAR(buf);
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;

            if (-1 == xioctl(cur_fd, VIDIOC_DQBUF, &buf)) {
                switch (errno) {
                case EAGAIN:
                    LOG_PRINT(debug_str, DEBUG_ERROR, "cam %s getframe error, EAGAIN, (%d)%s\n", dev_pt->cam[index].dev_name, errno, strerror(errno));
                    ret = 1;
                    break;
                case EIO:
                    /* Could ignore EIO, see spec. */
                    break;
                default:
                    LOG_PRINT(debug_str, DEBUG_ERROR, "cam %s VIDIOC_DQBUF error, (%d)%s\n", dev_pt->cam[index].dev_name, errno, strerror(errno));
                }
            }
            kl=buf.index;

            /* sync data */
            msync(dev_pt->cam[index].buffers[buf.index].start, buf.bytesused, MS_SYNC);

            if (dev_pt->cam[index].format == V4L2_PIX_FMT_YUYV){
                sframe.bencding_stream=0;
                sframe.stream_id=0;
                sframe.frame_data=dev_pt->cam[index].buffers[buf.index].start;
                sframe.frame_len=buf.bytesused;
                sframe.timestamp=(unsigned long)((buf.timestamp.tv_sec*1000)+(buf.timestamp.tv_usec/1000));
                (*dev_pt->frame_output2)(dev_id, sframe);
            }
            else if (dev_pt->cam[index].format == V4L2_PIX_FMT_MJPEG){
                video_buffer = dev_pt->cam[index].buffers[buf.index].start;
                video_buffer_len = buf.bytesused;
                timestamp= (unsigned long)((buf.timestamp.tv_sec*1000)+(buf.timestamp.tv_usec/1000));

                if(dev_pt->cam[index].codec_type==CODEC_VP8){
                    send_vp8_simulcast_frame(dev_pt->cam[index].dev_id,video_buffer, video_buffer_len, timestamp,STREAM0, dev_pt->cam[index].key_frame_interval,dev_pt->cam[index].is_demux);
                }
                else if(dev_pt->cam[index].codec_type==CODEC_H264){
                    send_avc_simulcast_frame(dev_pt->cam[index].dev_id,video_buffer, video_buffer_len, timestamp,STREAM0, dev_pt->cam[index].key_frame_interval,dev_pt->cam[index].is_demux);
                }
                else if(dev_pt->cam[index].codec_type==CODEC_VP8_SIMULCAST){
                    if(vp8demux_ex((BYTE*)video_buffer,video_buffer_len,&vp8_frames)){
                        if(vp8_frames.frames[STREAM0].size!=0){
                            send_vp8_simulcast_frame(dev_pt->cam[index].dev_id,(char*) vp8_frames.frames[STREAM0].data, vp8_frames.frames[STREAM0].size, timestamp,STREAM0, dev_pt->cam[index].key_frame_interval,dev_pt->cam[index].is_demux);
                        }
                        if(vp8_frames.frames[STREAM1].size!=0){
                            send_vp8_simulcast_frame(dev_pt->cam[index].dev_id, (char*) vp8_frames.frames[STREAM1].data, vp8_frames.frames[STREAM1].size, timestamp,STREAM1, dev_pt->cam[index].key_frame_interval,dev_pt->cam[index].is_demux);
                        }
                        if(vp8_frames.frames[STREAM2].size!=0){
                            send_vp8_simulcast_frame(dev_pt->cam[index].dev_id, (char*) vp8_frames.frames[STREAM2].data, vp8_frames.frames[STREAM2].size, timestamp,STREAM2, dev_pt->cam[index].key_frame_interval,dev_pt->cam[index].is_demux);
                        }
                        if(vp8_frames.frames[STREAM3].size!=0){
                            send_vp8_simulcast_frame(dev_pt->cam[index].dev_id, (char*) vp8_frames.frames[STREAM3].data, vp8_frames.frames[STREAM3].size, timestamp,STREAM3, dev_pt->cam[index].key_frame_interval,dev_pt->cam[index].is_demux);
                        }
                    }
                    else{
                        LOG_PRINT(debug_str, DEBUG_ERROR, "vp8demux return error !!\n");
                    }
                }
                else if(dev_pt->cam[index].codec_type==CODEC_H264_SIMULCAST){
                    avc_stream_id=get_avc_stream_id((unsigned char*)video_buffer,video_buffer_len);
                    send_avc_simulcast_frame(dev_pt->cam[index].dev_id,video_buffer, video_buffer_len, timestamp,avc_stream_id, dev_pt->cam[index].key_frame_interval,dev_pt->cam[index].is_demux);
                }
                else{
                    sframe.bencding_stream=0;
                    sframe.stream_id=0;
                    sframe.frame_data=video_buffer;
                    sframe.frame_len=video_buffer_len;
                    sframe.timestamp=timestamp;
                    (*dev_pt->frame_output2)(dev_id, sframe);
                }
            }
            /* re-queue the buffer */
            if (-1 == xioctl(cur_fd, VIDIOC_QBUF, &buf)) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "dev(YUV) %s VIDIOC_QBUF error: %s\n", dev_pt->cam[index].dev_name, strerror(errno));
                ret = 1;
            }
        }
    }
    /*LOG_PRINT(debug_str, DEBUG_DETAIL, "qic_getframe - OUT\n");*/
    /* success */
    return ret;
}

int qic_enum_device_formats(qic_dev_name_s *name)
{
    int ret=0;
    int fd;
    char found_video0=0;
    char found_video1=0;
    char dev_name[16];
    int i;
    char dev_format[32];

    for(i=0; i<10; i++){

        memset (dev_name, 0, sizeof(dev_name));
        snprintf(dev_name, sizeof(dev_name), "/dev/video%d", i);

        fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

        if(fd>0){
            QicSetDeviceHandle(fd);
            if(QicQueryDevice()==0){

                memset (dev_format, 0, sizeof(dev_format));
                ret=	enum_device_formats(fd,dev_format,0);

                if(!ret){
                    if(found_video0&&(strcmp(dev_format,"MPEG2-TS")==0
                                    ||strcmp(dev_format,"MJPEG")==0
                                    ||strcmp(dev_format,"H.264")==0)){
#ifdef ENABLE_PRINT
                        printf("AVC is %s \n",dev_name);
#endif
                        strcpy(name->dev_avc,dev_name);
                        found_video1=1;
                    }

                    if(!found_video0&&(strcmp(dev_format,"YUV 4:2:2 (YUYV)")==0
                                       ||strcmp(dev_format,"MJPEG")==0)){
#ifdef ENABLE_PRINT
                        printf("YUV is %s \n",dev_name);
#endif
                        strcpy(name->dev_yuv,dev_name);
                        found_video0=1;
                    }
                }
            }
            else{
                printf("Not supported video device\n");
                //ret =1;
            }
            close(fd);
        }
        else{
#ifdef ENABLE_PRINT
            printf("opne failed i=%d\n",i );
#endif
        }
    }

    if(found_video0)
        ret=0;
    else
        ret=1;

    return ret;
}


// Internal function:
static void send_vp8_simulcast_frame(unsigned int dev_id, char *data, unsigned int length,
                                     unsigned long timestamp,unsigned int stream_id, unsigned short key_frame_interval,
                                     unsigned char demux)
{
    int demux_ret=0;
    out_frame_t sframe;

    /*check if we got bad frame??*/
    extern int last_frame_size;

    if(demux){
        last_frame_size = last_frame_sizes[stream_id];

        demux_ret = demux_VP8_check_bad_frame( &data, &length);
        last_frame_sizes[stream_id] = last_frame_size;

        if(!demux_ret){
            if(generate_key_frame){
                if((data[3]&0x00ff)==0x9d && (data[4]&0x00ff)==0x01 && (data[5]&0x00ff)==0x2a){
                    generate_key_frame=0;
                    LOG_PRINT(debug_str, DEBUG_INFO, " Found VP8 IDR Frame......!!\n");
                }else{
                    //LOG_PRINT(debug_str, DEBUG_INFO, "dev %s VP8 P Frame...!!\n", dev_pt->cam[index].dev_name);
                }
            }
        }

        /*this is bad frame, we drop it and request key frame at once */
        if(demux_ret){
            LOG_PRINT(debug_str, DEBUG_INFO, "Error!! Bad Frame found: stream id %d, qic generate key frame!!\n\n", stream_id);
            qic_generate_key_frame_EU(dev_id,stream_id,1,key_frame_interval, 0);
            generate_key_frame=1;
            vp8_h264_bad_frame_count++;
        }
        else{
            if(!generate_key_frame){

                /*frame is good send to user*/
                sframe.bencding_stream=1;
                sframe.stream_id=stream_id;
                sframe.frame_data=data;
                sframe.frame_len=length;
                sframe.timestamp=timestamp;
                (*dev_pt->frame_output2)(dev_id, sframe);
            }
            else{
                //Drop VP8 P Frame
                LOG_PRINT(debug_str, DEBUG_INFO, "dev %s Drop steamd %d bad Frame...!!\n", stream_id);
            }
        }
    }
    else{
        sframe.stream_id=stream_id;
        sframe.bencding_stream=1;
        sframe.frame_data=data;
        sframe.frame_len=length;
        sframe.timestamp=timestamp;
        (*dev_pt->frame_output2)(dev_id, sframe);
    }
}

static void send_avc_simulcast_frame(unsigned int dev_id, char *data, unsigned int length,
                                     unsigned long timestamp,unsigned int stream_id, unsigned short key_frame_interval,
                                     unsigned char demux)
{
    int demux_ret=0;
    int is_pframe;
    out_frame_t sframe;
    int i;
#ifdef QIC_MD_API
    unsigned int sei_begin_offset;
#endif
    /*check if we got bad frame??*/
    extern int last_frame_size;

    if(demux){
        last_frame_size = last_frame_sizes[stream_id];

        demux_ret = demux_H264_check_bad_frame( data, length);
        last_frame_sizes[stream_id] = last_frame_size;
        if(!demux_ret){
            if(generate_key_frame){
                for(i=0;i<(length-4);i++){
                    if((data[i+0] == 0x00) && (data[i+1] == 0x00) && (data[i+2] == 0x00) && (data[i+3] == 0x01) && ((data[i+4]&0x1F) == 0x05)){
                        generate_key_frame=0;
                        LOG_PRINT(debug_str, DEBUG_INFO, "stream %d Found H264 IDR Frame i=%d......!!\n",stream_id, i);
                        break;
                    }
                }
                //LOG_PRINT(debug_str, DEBUG_INFO, "dev %s H264 P Frame...!!\n", dev_pt->cam[index].dev_name);
            }
        }

        /*this is bad frame, we drop it and request key frame at once */
        if(demux_ret){
            LOG_PRINT(debug_str, DEBUG_INFO, "Error!! Bad Frame found: stream id %d, qic generate key frame!!\n\n", stream_id);
            qic_generate_key_frame_EU(dev_id,stream_id,1,key_frame_interval, 0);
            generate_key_frame=1;
            vp8_h264_bad_frame_count++;
        }
        else{
            if(!generate_key_frame){
                is_pframe=check_for_P_frame((unsigned char*)data,length);
                sframe.bPframe=is_pframe;

                sframe.temporal_layer_id=get_stream_temporal_id((unsigned char*)data,length);
                //	printf("temp id=%d\n",sframe.temporal_layer_id);
#ifdef QIC_MD_API
                if(stream_id==STREAM0)
                    AnalyzeSEIMessages((unsigned char*)data,length,&sei_begin_offset,&sframe.md_status);
#endif
                /*frame is good send to user*/
                sframe.bencding_stream=1;
                sframe.stream_id=stream_id;
                sframe.frame_data=data;
                sframe.frame_len=length;
                sframe.timestamp=timestamp;
                (*dev_pt->frame_output2)(dev_id, sframe);
            }
            else{
                //Drop bad Frame
                LOG_PRINT(debug_str, DEBUG_INFO, "dev %s Drop steamd %d bad Frame...!!\n", stream_id);
            }
        }
    }
    else{
#ifdef QIC_MD_API
        memset(&sframe.md_status,0, sizeof(md_status_t));
        if(stream_id==STREAM0)
            AnalyzeSEIMessages((unsigned char*)data,length,&sei_begin_offset,&sframe.md_status);
#endif
        is_pframe=check_for_P_frame((unsigned char*)data,length);
        sframe.bPframe=is_pframe;
        sframe.temporal_layer_id=get_stream_temporal_id((unsigned char*)data,length);
        sframe.stream_id=stream_id;
        sframe.bencding_stream=1;
        sframe.frame_data=data;
        sframe.frame_len=length;
        sframe.timestamp=timestamp;
        (*dev_pt->frame_output2)(dev_id, sframe);
    }
}

#ifdef DEBUG_LOG
static void debug_print_initial (int level, char *string)
{
    printf("level=%d,string=%s\n",level,string);
}
#endif

static void frame_output_initial(unsigned int dev_id, unsigned int length, char *data, unsigned long timestamp)
{
    printf("dev_id=%d,  length=%d, data=%s,  timestamp=%lu\n",dev_id,  length, data,  timestamp);
}

static int format_check(SUPPORT_FORMAT* format, unsigned short width, unsigned short height, unsigned char framerate)
{
    int i = 0;
    int is_valid = 0;

    while ((format[i].width != 0) && (!is_valid)) {

        LOG_PRINT(debug_str,DEBUG_INFO, "check i[%d] %dx%d\n", i, format[i].width, format[i].height);

        if (framerate == 0) {
            if ((format[i].width == width) && (format[i].height == height)) {
                is_valid = 1;
            }
        }
        else if ((format[i].width == width) && (format[i].height == height) && (format[i].framerate == framerate)) {
            is_valid = 1;
        }

        i++;
    }
    return is_valid;
}


/**********************************************
 *  individual camera interface setup function - internal use only
**********************************************/
static int qic_open_device (qic_dev *cam)
{
    struct stat st;

    if (-1 == stat(cam->dev_name, &st)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "Cannot identify '%s', (%d)%s\n", cam->dev_name, errno, strerror(errno));
        return 1;
    }

    if (!S_ISCHR(st.st_mode)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "cam %s is no device\n", cam->dev_name);
        return 1;
    }

    cam->fd = open(cam->dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

    if (-1 == cam->fd) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "Cannot open '%s', (%d)%s\n", cam->dev_name, errno, strerror(errno));
        return 1;
    } else {
        LOG_PRINT(debug_str, DEBUG_INFO, "cam %s opened\n", cam->dev_name);
    }

    /* init device */
    struct v4l2_capability cap;

    /* check if device is video device */
    if (-1 == xioctl(cam->fd, VIDIOC_QUERYCAP, &cap)) {
        if (EINVAL == errno) {
            LOG_PRINT(debug_str, DEBUG_ERROR, "cam %s is no V4L2 device, (%d)%s\n", cam->dev_name, errno, strerror(errno));
            return 1;
        }
        else {
            LOG_PRINT(debug_str, DEBUG_ERROR, "cam %s ioctl VIDIOC_QUERYCAP failed, (%d)%s\n", cam->dev_name, errno, strerror(errno));
            return 1;
        }
    }

    LOG_PRINT(debug_str, DEBUG_INFO, "cam %s ioctl VIDIOC_QUERYCAP succeeded\n", cam->dev_name);

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "cam %s is no video capture device\n", cam->dev_name);
        return 1;
    }

    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "cam %s does not support streaming I/O\n", cam->dev_name);
        return 1;
    }
    //LOG_PRINT(debug_str, DEBUG_DETAIL, "qic_open_device - OUT\n");

    return 0;
}

static int qic_clean_device(qic_dev *cam) {
    /* clean malloc only */
    unsigned int index;

    /* release queue buffer */
    if (cam->buffers != NULL) {
        LOG_PRINT(debug_str, DEBUG_INFO, "cam %s release queue buffer\n", cam->dev_name);

        for (index = 0; index < cam->num_mmap_buffer; ++index) {
            if (-1 == munmap(cam->buffers[index].start, cam->buffers[index].length)) {

                LOG_PRINT(debug_str, DEBUG_ERROR, "buffers munmap error on cam %s, (%d)%s\n", cam->dev_name, errno, strerror(errno));

            } else {
                LOG_PRINT(debug_str, DEBUG_INFO, "cam %s buffer:%d munmap success\n", cam->dev_name, index);
            }
        }

        free(cam->buffers);
        cam->buffers = NULL;
    }
    //	LOG_PRINT(debug_str, DEBUG_DETAIL, "qic_clean_device - OUT\n");

    return 0;
}

static int qic_initial_device (qic_dev *cam)
{
    struct v4l2_format fmt;
    struct v4l2_streamparm setfps;
    //	SysInfo_t sysinfo;
    int ret;
    int qic_ret = 0;

    if (0){
    }
    else { /* YUV or MJPEG */
        LOG_PRINT(debug_str, DEBUG_INFO, "cam %s YUV or MJPEG configure\n", cam->dev_name);

        CLEAR(fmt);

        fmt.type				= V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width		= cam->width;
        fmt.fmt.pix.height		= cam->height;
        fmt.fmt.pix.pixelformat = cam->format;
        fmt.fmt.pix.field		= V4L2_FIELD_ANY;

        /* qic_module XU control*/
        qic_ret = QicSetDeviceHandle(cam->fd);

        LOG_XU_PRINT(debug_str, debug_xuctrl_str, 0); /* don't display this annoying message on monitor*/


        if (-1 == xioctl(cam->fd, VIDIOC_S_FMT, &fmt)) {
            LOG_PRINT(debug_str, DEBUG_ERROR, "cam %s ioctl VIDIOC_S_FMT error, (%d)%s\n", cam->dev_name, errno, strerror(errno));
            return 1;
        }
        else {
            LOG_PRINT(debug_str, DEBUG_INFO, "cam %s ioctl VIDIOC_S_FMT success\n", cam->dev_name);
        }


        if (cam->is_bind) { /* is_bind - limited function */
            LOG_PRINT(debug_str, DEBUG_INFO, "cam %s is bind, fps refer to H.264/AVC setting\n", cam->dev_name);
        }
        else { /* not bind YUV device */

            /* set the framerate */
            CLEAR(setfps);
            setfps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            setfps.parm.capture.timeperframe.numerator = 1;
            setfps.parm.capture.timeperframe.denominator = cam->framerate;
            ret = xioctl(cam->fd, VIDIOC_S_PARM, &setfps);
            if(ret == -1) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "cam %s unable to set frame rate, (%d)%s\n", cam->dev_name, errno, strerror(errno));
            }
            else {
                LOG_PRINT(debug_str, DEBUG_INFO, "cam %s set frame rate to %d\n", cam->dev_name, cam->framerate);
            }

        }
    }

    /* init mmap buffer */
    struct v4l2_requestbuffers req;

    CLEAR(req);

    req.count = cam->num_mmap_buffer;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(cam->fd, VIDIOC_REQBUFS, &req)) {
        if (EINVAL == errno) {
            LOG_PRINT(debug_str, DEBUG_ERROR, "cam %s does not support memory mapping, (%d)%s\n", cam->dev_name, errno, strerror(errno));
            return 1;
        }
        else {
            LOG_PRINT(debug_str, DEBUG_ERROR, "cam %s ioctl VIDIOC_REQBUFS failed, (%d)%s\n", cam->dev_name, errno, strerror(errno));
            return 1;
        }
    }

    LOG_PRINT(debug_str, DEBUG_INFO, "cam %s v4l2_requestbuffers number = %d\n", cam->dev_name, req.count);

    if (req.count < 2) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "cam %s insufficient buffer memory = %d\n", cam->dev_name, req.count);
        return 1;
    }

    cam->buffers = calloc(req.count, sizeof(*cam->buffers));

    if (!cam->buffers) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "malloc: out of memory\n");
        return 1;
    }

    LOG_PRINT(debug_str, DEBUG_INFO, "system getpagesize() = %d\n", getpagesize());

    for (cam->num_mmap_buffer = 0; cam->num_mmap_buffer < req.count; ++cam->num_mmap_buffer)
    {
        struct v4l2_buffer buf;

        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = cam->num_mmap_buffer;

        if (-1 == xioctl(cam->fd, VIDIOC_QUERYBUF, &buf)) {
            LOG_PRINT(debug_str, DEBUG_ERROR, "cam %s buffer:%d ioctl VIDIOC_QUERYBUF failed, (%d)%s\n", cam->dev_name, buf.index, errno, strerror(errno));
            return 1;
        } else {
            LOG_PRINT(debug_str, DEBUG_INFO, "cam %s buffer:%d ioctl VIDIOC_QUERYBUF succeeded\n", cam->dev_name, buf.index);
        }

        cam->buffers[cam->num_mmap_buffer].length = buf.length;

        cam->buffers[cam->num_mmap_buffer].start =
                mmap(NULL /* start anywhere */,
                     buf.length,
                     PROT_READ | PROT_WRITE /* required */,
                     MAP_SHARED /* recommended */,
                     cam->fd, buf.m.offset);

        LOG_PRINT(debug_str, DEBUG_INFO,  "mmap fd[%04d] offset[%7d] index[%02d] length[%7d] loc[%p]\n", \
                  cam->fd, \
                  buf.m.offset, \
                  cam->num_mmap_buffer, \
                  buf.length, \
                  cam->buffers[cam->num_mmap_buffer].start);

        if (MAP_FAILED == cam->buffers[cam->num_mmap_buffer].start) {
            LOG_PRINT(debug_str, DEBUG_ERROR, "cam %s buffer:%d mmap call failed, (%d)%s\n", cam->dev_name, buf.index, errno, strerror(errno));
            return 1;
        } else {
            LOG_PRINT(debug_str, DEBUG_INFO, "cam %s buffer:%d mmap succeeded\n", cam->dev_name, buf.index);
        }
    }

    /* Queue the buffer to kernel */
    unsigned int queue_index;
    for (queue_index = 0; queue_index < cam->num_mmap_buffer; ++queue_index)
    {
        struct v4l2_buffer buf;

        CLEAR(buf);

        buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory      = V4L2_MEMORY_MMAP;
        buf.index       = queue_index;

        if (-1 == xioctl(cam->fd, VIDIOC_QBUF, &buf)) {
            LOG_PRINT(debug_str, DEBUG_ERROR, "cam %s buffer:%d ioctl VIDIOC_QBUF failed, (%d)%s\n", cam->dev_name, buf.index, errno, strerror(errno));
            return 1;
        } else {
            LOG_PRINT(debug_str, DEBUG_INFO, "cam %s buffer:%d ioctl VIDIOC_QBUF succcess\n", cam->dev_name, buf.index);
        }
    }
    //	LOG_PRINT(debug_str, DEBUG_DETAIL, "qic_initial_device - OUT\n");
    return 0;
}

static void set_scheduler(void)
{
    struct sched_param param;

    sched_getparam( 0, &param);

    LOG_PRINT(debug_str, DEBUG_INFO, "Process Original Priority = %d\n", param.sched_priority);

    param.sched_priority = sched_get_priority_max(SCHED_RR);

    if( sched_setscheduler( 0, SCHED_RR, &param ) == -1 )  {
        LOG_PRINT(debug_str, DEBUG_ERROR,  "sched_setscheduler error, (%d)%s\n", errno, strerror(errno));
        /*perror("sched_setscheduler");*/
    }

    sched_getparam( 0, &param);
    LOG_PRINT(debug_str, DEBUG_INFO, "Process New Priority = %d\n", param.sched_priority);
}

int enum_device_formats(int fd, char* format, char print)
{
    int ret;
    struct v4l2_fmtdesc fmt;
    struct v4l2_frmsizeenum sizes;

    memset(&fmt, 0, sizeof(fmt));
    fmt.index = 0;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    while ((ret = ioctl(fd, VIDIOC_ENUM_FMT, &fmt)) == 0) {
        fmt.index++;

        if(print){
            printf("{ pixelformat = '%c%c%c%c', description = '%s' }\n",
                   fmt.pixelformat & 0xFF, (fmt.pixelformat >> 8) & 0xFF,
                   (fmt.pixelformat >> 16) & 0xFF, (fmt.pixelformat >> 24) & 0xFF,
                   fmt.description);

            /* Enumerate resolutions */
            CLEAR(sizes);
            sizes.pixel_format =fmt.pixelformat;
            sizes.index 	  = 0;

            if (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &sizes) == -1)
            {
                printf("Error enumerating frame sizes. Webcam does not support hardware MJPEG compression?");
            }
            if (sizes.type != V4L2_FRMSIZE_TYPE_DISCRETE)
            {
                printf("Frame size type is not discrete");
            }

            /* find out largest supported resolution */
            do
            {

                sizes.index++;
                printf("resolution:(%dx%d)\n",sizes.discrete.width,sizes.discrete.height);
                enum_frame_intervals( fd, sizes.pixel_format, sizes.discrete.width,sizes.discrete.height);

            }
            while ( ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &sizes) != -1 );

        }
        if(strcmp((char*)fmt.description,"YUV 4:2:2 (YUYV)")==0||strcmp((char*)fmt.description,"MPEG2-TS")==0
                ||strcmp((char*)fmt.description,"MJPEG")==0 ||strcmp((char*)fmt.description,"H.264")==0)
            strcpy(format,(char*)fmt.description);

    }
    if (errno != EINVAL) {
        printf("ERROR enumerating frame formats: %d\n", errno);
        return -1;
    }

    return 0;
}

int enum_frame_intervals(int fd, unsigned int pixfmt, unsigned int width, unsigned int height)
{
    int ret;
    struct v4l2_frmivalenum fival;

    memset(&fival, 0, sizeof(fival));
    fival.index = 0;
    fival.pixel_format = pixfmt;
    fival.width = width;
    fival.height = height;
    printf("\tTime interval between frame: ");
    while ((ret = ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &fival)) == 0) {
        if (fival.type == V4L2_FRMIVAL_TYPE_DISCRETE) {
                printf("%u/%u, ", fival.discrete.numerator, fival.discrete.denominator);
        } else if (fival.type == V4L2_FRMIVAL_TYPE_CONTINUOUS) {
                printf("{min { %u/%u } .. max { %u/%u } }, ",
                        fival.stepwise.min.numerator, fival.stepwise.min.numerator,
                        fival.stepwise.max.denominator, fival.stepwise.max.denominator);
                break;
        } else if (fival.type == V4L2_FRMIVAL_TYPE_STEPWISE) {
                printf("{min { %u/%u } .. max { %u/%u } / "
                        "stepsize { %u/%u } }, ",
                        fival.stepwise.min.numerator, fival.stepwise.min.denominator,
                        fival.stepwise.max.numerator, fival.stepwise.max.denominator,
                        fival.stepwise.step.numerator, fival.stepwise.step.denominator);
                break;
        }
        fival.index++;
    }
    printf("\n");
    if (ret != 0 && errno != EINVAL) {
        printf("ERROR enumerating frame intervals: %d\n", errno);
        return errno;
    }

    return 0;
}
