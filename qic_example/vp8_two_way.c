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

#include "qic_control.h"
#include "misc_writefile.h"
#include "misc_system.h"



#ifdef DMALLOC
#include <dmalloc.h>
#endif



#define USE_YUYV   /* define /dev/video0 as device 0(YUYV) */
//#define USE_MJPEG   /* define /dev/video0 as device 0(MJPEG) */

#define CALC_FPS


/* capture 100 frames for demo */
int frame_count = 500;

/* debug mesg level */
char *debug_level[] = {"INFO", "WARN", "CRIT", "FATL", "DEAD"};


void debug_log(int level, char *string) {

		printf("QIC module debug_print (%s):%s", debug_level[level], string);
}


void frame_process(unsigned int dev_id, out_frame_t frame) {

	static unsigned int vp8_count = 0;
	static unsigned int yuv_count = 0;
#ifdef USE_MJPEG
	static unsigned int mjpeg_count = 0;
#endif
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
	
			if(frame_count > 0)
			{
				//vp8_count++;
				//write_vp8((unsigned char*)data,length,timestamp);
				write_vp8((unsigned char*)frame.frame_data,frame.frame_len,vp8_count);
				vp8_count++;
				putchar('V');fflush(stdout);

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
	printf("\n(VP8) frame rate= %2.2f fps \n", fps);

	video1_frame_num=0;

	clock_gettime(0,&video1_end_ts);
	
}
#endif
/*  calculate current frame rate}*/

			break;

	}

	frame_count--;
	if((vp8_count)%100 == 0)
		printf("\n");
}


static void usage(FILE * fp, int argc, char **argv)
{
	fprintf(fp,
	"Usage: %s [options]\n\n"
	"Options:\n"
	"-o | --output		VP8 output [filename]\n"
	"-s | --VP8size		VP8 stream [width]x[height]\n"
	"-y | --YUVsize		YUV raw stream [width]x[height]\n"
	"-f | --fps		Framerate\n"
	"-b | --bitrate		Bitrate\n"
	"-g | --gop		GOP value\n"
	"-c | --count		Capture Counter\n"
	"-d | --demux		VP8 bad frame check\n"
	"-h | --help		Print this message\n"
	"",
	argv[0]);
}



static const char short_options [] = "o:s:y:f:b:g:c:hd";


static const struct option long_options [] =
{
	{ "output",		required_argument,	NULL,	'o' },
	{ "VP8size",		required_argument,	NULL,	's' },
	{ "YUVsize",		required_argument,	NULL,	'y' },
	{ "fps",		required_argument,	NULL,	'f' },
	{ "bitrate",		required_argument,	NULL,	'b' },
	{ "gop",		required_argument,	NULL,	'g' },
	{ "count",		required_argument,	NULL,	'c' },
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
	unsigned short vp8_width = 1280;
	unsigned short vp8_height = 720;
	unsigned short yuv_width = 320;
	unsigned short yuv_height = 240;
	unsigned short mjpeg_width=320;
	unsigned short  mjpeg_height = 240;
	unsigned char u_framerate = 30, demux=0;
	unsigned int u_frame_interval=333333;  //frame rate interval, unit:100nS ->0.1uS
	unsigned int u_bitrate = 3000000;
	unsigned short u_key_frame_interval = 0; //mS gop interval
	qic_module *my_qic = NULL;
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
			vp8_width = strtoul(sizestring, &separator, 10);
			if (*separator != 'x') {
				printf("Error in size use -s widthxheight\n");
				exit(1);
			} else {
				++separator;
				vp8_height = strtoul(separator, &separator, 10);
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
			} else {
				++separator;
				yuv_height = strtoul(separator, &separator, 10);
				mjpeg_height = yuv_height;
				if (*separator != 0)
					printf("hmm.. dont like that!! trying this height\n");
			}
			break;
			
		case 'f':
			u_framerate = atoi(optarg);
			u_frame_interval=10000000/u_framerate;
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

		case 'd':
			printf("Enable VP8 bad frame check...\n");
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

#ifdef USE_MJPEG
	/* set /dev/video0 as device 0(MJPEG) */
    if(strlen(video_name.dev_yuv)>0)
		my_qic->cam[0].dev_name = video_name.dev_yuv;
     else
        my_qic->cam[0].dev_name ="/dev/video0";
	my_qic->cam[0].format = V4L2_PIX_FMT_MJPEG;
	my_qic->cam[0].width = mjpeg_width;
	my_qic->cam[0].height = mjpeg_height;
	my_qic->cam[0].is_bind = 0; /* 2-way output from single QIC module */
	my_qic->cam[0].framerate= u_framerate;
	my_qic->cam[0].num_mmap_buffer = 6; /*  less memory */
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
#endif

	/*set /dev/video1 as device 1 (VP8) */
    if(strlen(video_name.dev_avc)>0)
		my_qic->cam[1].dev_name = video_name.dev_avc;
    else	
		my_qic->cam[1].dev_name = "/dev/video1";
	my_qic->cam[1].format = V4L2_PIX_FMT_MJPEG;
	my_qic->cam[1].width = vp8_width; 
	my_qic->cam[1].height = vp8_height;
	my_qic->cam[1].framerate= u_framerate;
	my_qic->cam[1].bitrate= u_bitrate;
	my_qic->cam[1].is_bind = 0; 
	my_qic->cam[1].num_mmap_buffer = 6; 
	my_qic->cam[1].codec_type=CODEC_VP8;
	my_qic->cam[1].is_encoding_video=1;
	my_qic->cam[1].key_frame_interval=u_key_frame_interval;
	my_qic->cam[1].frame_interval=u_frame_interval;
	if(demux){
		my_qic->cam[1].is_demux =1;  //Enable Encoding stream bad frame check
	}

	/* commit and init the video dev */
	ret = qic_config_commit();
	if (ret) {
		printf("qic_config_commit error\n");
		return 1;
	}


     qic_get_system_version(DEV_ID_1, &qic_info);
		printf("qic(system) VID:0x%.4lX PID:0x%.4lX REV:0x%.4lX svn(%d), fw_new_old_version (%d)\n", qic_info.vid, qic_info.pid,
				qic_info.revision, qic_info.svn,qic_info.fw_api_version);


/*EU control API for uvc1.0*/

 qic_config_codec_EU(DEV_ID_1,CODEC_VP8 );
 qic_change_resolution_EU(DEV_ID_1, SIMULCAST_STREAM0, vp8_width, vp8_height);
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
	add_vp8_file_header(vp8_width, vp8_height, u_framerate, frame_count);


/************************************************
 *
 * step 4: activate the camera before capture frames
 *
 *************************************************/
	ret = qic_start_capture(DEV_ID_0+DEV_ID_1); /* activate both video0 & video1 */
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
		
			printf("select error %d, %s\n", errno, strerror(errno));
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



/*V4L2 control testing */
#if 0
          if( frame_count <= 300 && test==0 )
               {
                   unsigned int tt;
                   test=1;
                   tt= my_qic->cam[1].bitrate;
                 //  ret=   qic_change_bitrate(DEV_ID_1, 2000000);
                  // printf("s1  %d %d\n",tt,my_qic->cam[1].bitrate);
                   if (ret) {
                       printf("S1-1 error\n");
                   }
			ret=qic_change_V4L2_WHITE_BALANCE( DEV_ID_1,1,test_value%2);
			  if (ret) {

                       printf("S1-2 error\n");
                   }
			ret=qic_change_V4L2_WHITE_BALANCE( DEV_ID_1,0,test_value+2800);
			  if (ret) {
                       printf("S1-3 error\n");
                   }
			ret=qic_change_V4L2_FOCUS_ABSOLUTE(DEV_ID_0 + DEV_ID_1,1, 1);
			  if (ret) {
                       printf("S1-4 error\n");
                   }
			  ret=qic_change_V4L2_EXPOSURE_AUTO_PRIORITY(DEV_ID_0 + DEV_ID_1,test_value%2);
			     if (ret) {
                       printf("S1-5 error\n");
                   } 
               }

              if( frame_count <=250 && test==1)
              {
                  unsigned int tt;
                  frame_count--;
                  test=2;
                  tt= my_qic->cam[1].width;
    
              //    ret =qic_change_resolution(DEV_ID_1, 320, 240);
               //   printf(" S2 %d %d\n",tt,my_qic->cam[1].width);
       
                  if (ret) {
                       printf("S2-1 error\n");
                  }
			ret=qic_change_V4L2_BRIGHTNESS( DEV_ID_1,(test_value%200)-100);
			if (ret) {
                       printf("S2-2 error\n");
                  }
 			ret=qic_change_V4L2_CONTRAST(DEV_ID_1, test_value%6);
			if (ret) {
                       printf("S2-3 error\n");
                  }

			ret=qic_change_V4L2_FOCUS_ABSOLUTE(DEV_ID_0 + DEV_ID_1,0, (test_value%5)+1);
			if (ret) {
                       printf("S2-4 error\n");
                  }
			

			
              }
 
              if( frame_count <= 200 && test ==2)
              {
                  unsigned int tt;
                  test=3;
                  tt= my_qic->cam[1].framerate;
              //    ret = qic_change_framerate(DEV_ID_1, 15);
               //   printf("S3  %d %d\n",tt,my_qic->cam[1].framerate);
       
                  if (ret) {
                      printf("S3-1 error\n");
                  }
			 ret=qic_change_V4L2_HUE( DEV_ID_1, test_value%6);
			  if (ret) {
                      printf("S3-2 error\n");
                  }
			ret=qic_change_V4L2_SATURATION( DEV_ID_1, (test_value+32)%63);	
			 if (ret) {
                      printf("S3-3 error\n");
                  }
		
			 	ret=qic_change_V4L2_EXPOSURE(DEV_ID_0 + DEV_ID_1,1,3);
				 if (ret) {
                      printf("S3-6 error\n");
			 	}
				ret=qic_change_V4L2_EXPOSURE(DEV_ID_0 + DEV_ID_1,0,(test_value%200)+12);
				if (ret) {
                      printf("S3-7 error\n");
			 	}
              }

              if( frame_count <= 100 && test==3  )
              {
                  unsigned int tt;
                  test=4;
                  tt= my_qic->cam[1].bitrate;
               //   ret= qic_change_bitrate(DEV_ID_1, 512000);
                //  printf(" S4 %d %d\n",tt,my_qic->cam[1].bitrate);
        
                  if (ret) {
                      printf("S4-1 error\n");
                  }
			ret=qic_change_V4L2_GAMMA( DEV_ID_1, (test_value%5)+1);
			if (ret) {
                      printf("S4-2 error\n");
                  }

			ret=qic_change_V4L2_BACKLIGHT_COMPENSATION(DEV_ID_1,test_value%5);
			if (ret) {
                      printf("S4-3 error\n");
                  }

			
              }

		 if( frame_count <= 50 && test==4  )
              {
                  test=5;

			printf(" S5\n"); 
			 ret=qic_change_V4L2_SHARPNESS(DEV_ID_1,test_value%5);			
                  if (ret) {
                      printf("S5-1 error\n");
                  }
			ret=qic_change_V4L2_POWER_LINE_FREQUENCY(DEV_ID_1,test_value%3);
			
                  if (ret) {
                      printf("S5-2 error\n");
                  }
                  /*
	                ret=qic_change_V4L2_ZOOM_ABSOLUTE(DEV_ID_1 , 5);
	                 if (ret) {
                      printf("S5-3 error\n");
                  }
                  */
              }	  

		

				
test_value++;
#endif

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
