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

#define USE_YUYV   /* define /dev/video0 as device 0(YUYV) */
//#define USE_MJPEG   /* define /dev/video0 as device 0(MJPEG) */
#define CALC_FPS

/* capture 500 frames for demo */
int frame_count = 500;

void debug_log(int level, char *string)
{
    /* debug mesg level */
    char *debug_level[] = {" ", "ERROR", "INFO", " ", "FRAME", "ERROR/FRAME", "INFO/FRAME", " ",  "DETAIL", " ", " ", " ", " ", " ", " ", " " };
    char buffer [1024] = {0};
    int length;

    length = snprintf(buffer, sizeof(buffer), "QIC module dbg(%s):%s", debug_level[level], string);
    if(level == DEBUG_ERROR)	printf("%s", buffer);

    printf("QIC module debug_print (%s):%s", debug_level[level], string);
}

void frame_process(unsigned int dev_id, out_frame_t frame)
{
    static unsigned int yuv_count = 0;
#ifdef USE_MJPEG 	
    static unsigned int mjpeg_count = 0;
#endif
    static unsigned int avc_count = 0;

    /* { calculate current frame rate*/
#ifdef CALC_FPS
    static struct timespec start_ts,video0_end_ts,video1_end_ts;
    static unsigned int video0_frame_num=0,video1_frame_num=0;
    long diff_sec,diff_nsec;
    double diff_msec;
    float fps;
#endif
    /*  calculate current frame rate}*/

    switch (dev_id) {

    case DEV_ID_0:
#ifdef USE_MJPEG 
        if(frame_count > 0)
        {
            mjpeg_count++;
            write_mjpeg(frame.frame_data,frame.frame_len);
            putchar('M');fflush(stdout);
        }
#endif
#ifdef USE_YUYV		
        /* is YUV stream, log to screen */
        if(frame_count > 0)
        {
            yuv_count++;
            write_yuv(frame.frame_data,frame.frame_len);
            putchar('R');fflush(stdout);
        }
#endif

        /*  {calculate current frame rate*/
#ifdef CALC_FPS

        video0_frame_num++;
        if(video0_frame_num>=30){
            clock_gettime(0,&start_ts);
            diff_sec=start_ts.tv_sec- video0_end_ts.tv_sec;
            diff_nsec=start_ts.tv_nsec-video0_end_ts.tv_nsec;
            diff_msec=(diff_sec*1000.)+(diff_nsec/1000000.);
            fps=(video0_frame_num*1000.)/diff_msec;
            printf("\n(YUYV/MJPEG) frame rate= %2.2f fps \n", fps);

            video0_frame_num=0;

            clock_gettime(0,&video0_end_ts);
        }
#endif
        /*  calculate current frame rate}*/
        break;

    case DEV_ID_1:
        /* is H.264/AVC stream, write to file */

        if(frame_count > 0)
        {
            avc_count++;
            write_avc(frame.frame_data,frame.frame_len);
            putchar('C');fflush(stdout);
        }

        /*  {calculate current frame rate*/
#ifdef CALC_FPS
        video1_frame_num++;
        if(video1_frame_num>=30){
            clock_gettime(0,&start_ts);
            diff_sec=start_ts.tv_sec- video1_end_ts.tv_sec;
            diff_nsec=start_ts.tv_nsec-video1_end_ts.tv_nsec;
            diff_msec=(diff_sec*1000.)+(diff_nsec/1000000.);
            fps=(video1_frame_num*1000.)/diff_msec;
            printf("\n(H264) frame rate= %2.2f fps \n", fps);

            video1_frame_num=0;

            clock_gettime(0,&video1_end_ts);
        }
#endif
        /*  calculate current frame rate}*/
        break;
    }

    frame_count--;
    if((avc_count)%100 == 0)
        printf("\n");
}

static void usage(FILE * fp, int argc, char **argv)
{
    fprintf(fp,
            "Usage: %s [options]\n\n"
            "Options:\n"
            "-o | --output		H.264/AVC output [filename]\n"
            "-s | --AVCsize		H.264/AVC stream [width]x[height]\n"
            "-y | --YUVsize		YUV raw stream [width]x[height]\n"
            "-f | --fps		Framerate\n"
            "-b | --bitrate	Bitrate\n"
            "-g | --gop		GOP value\n"
            "-c | --count		Capture Counter\n"
            "-r | --rawdump		dump raw data from V4L2 buffer\n"
            "-d | --demux		H264 bad frame check\n"
            "-h | --help		Print this message\n"
            "",
            argv[0]);
}

static const char short_options [] = "ho:s:y:f:b:g:c:rd";

static const struct option long_options [] =
{
    { "output",		required_argument,	NULL,	'o' },
    { "AVCsize",		required_argument,	NULL,	's' },
    { "YUVsize",		required_argument,	NULL,	'y' },
    { "fps",		required_argument,	NULL,	'f' },
    { "bitreate",		required_argument,	NULL,	'b' },
    { "gop",		required_argument,	NULL,	'g' },
    { "count",		required_argument,	NULL,	'c' },
    { "rawdump",		no_argument,	NULL,	'r' },
    { "demux",		no_argument,	NULL,	'd' },
    { "help",		no_argument,		NULL,	'h' },
    { 0, 0, 0, 0 }
};

int main(int argc,char ** argv)
{	
    char *separator;
    char *sizestring = NULL;
    char *filename = NULL;
    version_info_t qic_info;
    unsigned short avc_width = 1280;
    unsigned short avc_height = 720;
    unsigned short yuv_width = 320;
    unsigned short yuv_height = 240;
    unsigned short mjpeg_width = 320;
    unsigned short mjpeg_height = 240;
    unsigned char u_framerate = 30;
    unsigned int u_bitrate = 5000000;
    unsigned char raw_dump = 0, demux=0;
    int test =0;
    qic_module *my_qic = NULL;
    unsigned int u_frame_interval = 333333;  //frame rate interval, unit:100nS ->0.1uS
    unsigned short u_key_frame_interval = 0; //mS gop interval
    int ret;

    qic_dev_name_s video_name;

    if (getuid() != 0){
        printf("please run as root\n");
        exit(0);
    }

    memset(&video_name,0, sizeof(video_name));
    if(qic_enum_device_formats(&video_name)){
        printf("Error!! Not supported video device\n");
        return 1;
    }
    printf("encdoing video=%s, raw video=%s\n",video_name.dev_avc, video_name.dev_yuv);

    for (;;)
    {
        int index;
        int c;

        c = getopt_long(argc, argv,
                        short_options, long_options,
                        &index);

        if (-1 == c)
            break;

        switch (c)
        {

        case 0: /* getopt_long() flag */
            break;


        case 'o':
            filename = optarg;
            break;

        case 's':
            sizestring = strdup(optarg);
            avc_width = strtoul(sizestring, &separator, 10);
            if (*separator != 'x') {
                printf("Error in size use -s widthxheight\n");
                exit(1);
            }
            else {
                ++separator;
                avc_height = strtoul(separator, &separator, 10);
                if (*separator != 0)
                    printf("hmm.. dont like that!! trying this height\n");
            }
            break;

        case 'y':
            sizestring = strdup(optarg);
            yuv_width = strtoul(sizestring, &separator, 10);
            mjpeg_width = yuv_width;
            if (*separator != 'x') {
                printf("Error in size use -s widthxheight\n");
                exit(1);
            }
            else {
                ++separator;
                yuv_height = strtoul(separator, &separator, 10);
                mjpeg_height = yuv_height;
                if (*separator != 0)
                    printf("hmm.. dont like that!! trying this height\n");
            }
            break;

        case 'f':
            u_framerate = atoi(optarg);
            u_frame_interval=(10000000/u_framerate);
            printf("Set u_framerate=%d...\n",u_framerate);
            break;

        case 'b':
            u_bitrate = atoi(optarg);
            printf("Set u_bitrate=%d...\n",u_bitrate);
            break;

        case 'g':
            u_key_frame_interval = atoi(optarg);
            printf("Set u_key_frame_interval=%d...\n",u_key_frame_interval);
            break;

        case 'c':
            frame_count= atoi(optarg);
            printf("Set frame_count=%d...\n",frame_count);
            break;

        case 'r':
            raw_dump = 1;
            break;

        case 'd':
            printf("Enable H264 bad frame check...\n");
            demux=1;
            break;

        case 'h':
            usage(stdout, argc, argv);
            exit(EXIT_SUCCESS);

        default:
            usage(stderr, argc, argv);
            exit(EXIT_FAILURE);
        }
    }

    /************************************************
 *
 * first step, init the qic module capture library
 * two devices, /dev/video0 as YUV raw
 *                   /dev/video1 as H.264/AVC
 *
 *************************************************/
    my_qic = qic_initialize(2);

    if (my_qic == NULL) {
        printf("qic_initialize error\n");
        return 1;
    }

    /************************************************
 *
 * step 2: set the parameters and commit the settings
 * need to setup two call back functions, debug_print & frame_output
 *
 *************************************************/
    /* call back functions */
    my_qic->debug_print = &debug_log;
    my_qic->frame_output2 = &frame_process;

    /*  set scheduler */
    my_qic->high_prio = 0;

    /* set debug level */
    my_qic->debug_msg_type = DEBUG_ERROR  + DEBUG_INFO;

#ifdef USE_MJPEG 	
    /* set /dev/video0 as device 0(MJPEG) */
    if(strlen(video_name.dev_yuv)>0)
        my_qic->cam[0].dev_name = video_name.dev_yuv;
    else
        my_qic->cam[0].dev_name = "/dev/video0";
    my_qic->cam[0].format = V4L2_PIX_FMT_MJPEG;
    my_qic->cam[0].width = mjpeg_width;
    my_qic->cam[0].height = mjpeg_height;
    my_qic->cam[0].is_bind = 0; /* 2-way output from single QIC module */
    my_qic->cam[0].framerate= u_framerate;
    my_qic->cam[0].num_mmap_buffer = 6;
#endif
#ifdef USE_YUYV
    /* set /dev/video0 as device 0(YUV) */
    if(strlen(video_name.dev_yuv)>0)
        my_qic->cam[0].dev_name =video_name.dev_yuv;
    else
        my_qic->cam[0].dev_name = "/dev/video0";

    my_qic->cam[0].format = V4L2_PIX_FMT_YUYV;
    my_qic->cam[0].width = yuv_width;
    my_qic->cam[0].height = yuv_height;
    my_qic->cam[0].is_bind = 0; /* 2-way output from single QIC module */
    my_qic->cam[0].framerate= u_framerate;
    my_qic->cam[0].num_mmap_buffer = 6;
    if(demux){
        my_qic->cam[0].is_demux =1;  //Enable YUYV bad frame check
    }
#endif

    /*set /dev/video1 as device 1 (H.264/AVC) */
    if(strlen(video_name.dev_avc)>0)
        my_qic->cam[1].dev_name = video_name.dev_avc;
    else
        my_qic->cam[1].dev_name ="/dev/video1";
    my_qic->cam[1].format = V4L2_PIX_FMT_MJPEG;
    my_qic->cam[1].bitrate = u_bitrate;
    my_qic->cam[1].width = avc_width;
    my_qic->cam[1].height = avc_height;
    my_qic->cam[1].framerate = u_framerate;
    my_qic->cam[1].codec_type=CODEC_H264;
    my_qic->cam[1].is_encoding_video=1;
    my_qic->cam[1].key_frame_interval=u_key_frame_interval;
    my_qic->cam[1].frame_interval=u_frame_interval;
    if(demux){
        my_qic->cam[1].is_demux =1;  //Enable Encoding stream bad frame check
    }

    if (raw_dump)
        my_qic->cam[1].raw_dump = &ts_dump; /*raw dump*/

    /* commit and init the video dev */
    ret = qic_config_commit();
    if (ret) {
        printf("qic_config_commit error\n");
        return 1;
    }

    qic_get_system_version(DEV_ID_1, &qic_info);
    printf("qic(system) VID:0x%.4lX PID:0x%.4lX REV:0x%.4lX svn(%d), fw_new_old_version (%d)\n", qic_info.vid, qic_info.pid,
           qic_info.revision, qic_info.svn,qic_info.fw_api_version);


    qic_config_codec_EU(DEV_ID_1,CODEC_H264);
    qic_change_resolution_EU(DEV_ID_1, SIMULCAST_STREAM0, avc_width, avc_height);
    qic_change_bitrate_EU( DEV_ID_1,SIMULCAST_STREAM0,u_bitrate);
    qic_change_frame_interval_EU(DEV_ID_1,SIMULCAST_STREAM0,u_frame_interval);
    qic_generate_key_frame_EU(DEV_ID_1,SIMULCAST_STREAM0,1,u_key_frame_interval, 0);
    qic_start_stop_layer_EU(DEV_ID_1,SIMULCAST_STREAM0,LAYER_START);

    /************************************************
 *
 * step 3: config print debug can be called at any time
 *
 *************************************************/
    char *log_str = NULL;
    log_str = qic_print_config_param(DEV_ID_0 + DEV_ID_1); /* print both interface config */
    printf("%s", log_str);

    /* open file for dump */
    open_file_dump(filename);

    /************************************************
 *
 * step 4: activate the camera before capture frames
 *
 *************************************************/

    ret = qic_start_capture(DEV_ID_0 + DEV_ID_1); /* activate both video0 & video1 */
    if (ret) {
        printf("qic_start_capture error\n");
        return 1;
    }

    /************************************************
 *
 * step 5: capture the frames - use select function and
 *             multiplex with other i/o
 *
 *************************************************/
    int fd0 = qic_get_fd_from_devid(DEV_ID_0);
    int fd1 = qic_get_fd_from_devid(DEV_ID_1);
    int max_fd = (fd0 > fd1)? fd0+1 : fd1+1;

    unsigned int dev_id_set;

    if ((fd0 == -1) || (fd1 == -1)) {
        printf("fd error, exit\n");
        return 1;
    }

    /* custom select function for capture frames without pthread */
    fd_set fds;

    struct timeval tv;
    int r;
    test=0;
    unsigned int test_value=0;

    while (frame_count > 0) {
        FD_ZERO(&fds);
        FD_SET(fd0, &fds);
        FD_SET(fd1, &fds);

        /* Timeout. */
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        /* clear dev_id set*/
        dev_id_set = 0;

        r = select(max_fd, &fds, NULL, NULL, &tv);


        if (-1 == r) {
            if (EINTR == errno)
                continue;

            //			snprintf(str, sizeof(str), "select error %d, %s\n", errno, strerror(errno));
            //		debug_log(DEBUG_ERROR, str);
            exit(1);
        }

        if (0 == r) {
            printf("select timeout\n");
            exit(1);
        }

        if (FD_ISSET(fd0, &fds))
            dev_id_set += DEV_ID_0;

        if (FD_ISSET(fd1, &fds))
            dev_id_set += DEV_ID_1;

        ret = qic_getframe2(dev_id_set);

        if (ret) {
            printf("qic_getframe error\n");
            return 1;
        }

        test_value++;
    }

    /* print the mem usage */
    char *mem_info = NULL;
    mem_info = mem_usage_info();
    printf("%s", mem_info);

    /************************************************
 *
 * step 6: stop the camera
 *
 *************************************************/
    ret = qic_stop_capture(DEV_ID_0 + DEV_ID_1); /* deactivate both video0 & video1 */
    if (ret) {
        printf("qic_stop_capture error\n");
        return 1;
    }

    /************************************************
 *
 * step 7: release all fd and malloc(s) before exit
  *
 *************************************************/
    ret = qic_release();
    if (ret) {
        printf("qic_release error\n");
        return 1;
    }

    close_file();

    return 0;
}
