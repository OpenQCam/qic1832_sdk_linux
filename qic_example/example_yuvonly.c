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


#define CALC_FPS



/* capture 100 frames for demo */
int frame_count = 500;

/* debug mesg level */
char *debug_level[] = {"INFO", "WARN", "CRIT", "FATL", "DEAD"};


void debug_log(int level, char *string) {

		printf("QIC module debug_print (%s):%s", debug_level[level], string);
}

//void frame_process(unsigned int dev_id, unsigned int length, char *data,unsigned long timestamp) {
void frame_process(unsigned int dev_id, out_frame_t frame) {

	static unsigned int yuv_count = 0;

/* { calculate current frame rate*/
#ifdef CALC_FPS
static struct timespec start_ts,end_ts;
static unsigned int frame_num=0;
long diff_sec,diff_nsec;
double diff_msec;
float fps;

#endif
/*  calculate current frame rate}*/

	switch (dev_id) {

		case DEV_ID_0:
			/* is YUV stream, log to screen */
			if(frame_count > 0)
			{

			yuv_count++;
			write_yuv(frame.frame_data,frame.frame_len);
			putchar('R');fflush(stdout);
			}

/*  {calculate current frame rate*/
#ifdef CALC_FPS

	frame_num++;
if(frame_num>=30){
	clock_gettime(0,&start_ts);
	diff_sec=start_ts.tv_sec- end_ts.tv_sec;
	diff_nsec=start_ts.tv_nsec-end_ts.tv_nsec;
	diff_msec=(diff_sec*1000.)+(diff_nsec/1000000.);
	fps=(frame_num*1000.)/diff_msec;
	printf("(YUYV) frame rate= %2.2f fps \n", fps);

	frame_num=0;

	clock_gettime(0,&end_ts);
	
}
#endif
/*  calculate current frame rate}*/	

			break;

	}

	frame_count--;
	if((yuv_count)%100 == 0)
		printf("\n");	
}


static void usage(FILE * fp, int argc, char **argv)
{
	fprintf(fp,
	"Usage: %s [options]\n\n"
	"Options:\n"
	"-o | --output		YUV output [filename]\n"
	"-y | --YUVsize		YUV raw stream [width]x[height]\n"
	"-f | --fps		Framerate\n"
	"-c | --count		Capture Counter\n"
	"-d | --demux		YUV bad frame check\n"
	"-h | --help		Print this message\n"
	"",
	argv[0]);
}



static const char short_options [] = "o:y:f:c:hd";


static const struct option long_options [] =
{
	{ "output",		required_argument,	NULL,	'o' },
	{ "YUVsize",		required_argument,	NULL,	'y' },
	{ "fps",		required_argument,	NULL,	'f' },
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

	unsigned short yuv_width = 640;
	unsigned short yuv_height = 480;
	unsigned char u_framerate = 30, demux=0;
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
		
	        case 'y':
			sizestring = strdup(optarg);
			yuv_width = strtoul(sizestring, &separator, 10);
			if (*separator != 'x') {
				printf("Error in size use -s widthxheight\n");
				exit(1);
			} else {
				++separator;
				yuv_height = strtoul(separator, &separator, 10);
				if (*separator != 0)
					printf("hmm.. dont like that!! trying this height\n");
			}
			break;
			
		case 'f':
			u_framerate = atoi(optarg);
			break;

		case 'c':
			frame_count= atoi(optarg);
			break;

		case 'd':
			printf("Enable YUV bad frame check...\n");
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
	my_qic = qic_initialize(1);

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

	/* set /dev/video0 as device 0(YUV) */
    if(strlen( video_name.dev_yuv)>0)
		my_qic->cam[0].dev_name =  video_name.dev_yuv;
    else	
		my_qic->cam[0].dev_name = "/dev/video0";
	my_qic->cam[0].format = V4L2_PIX_FMT_YUYV;
	my_qic->cam[0].width = yuv_width; 
	my_qic->cam[0].height = yuv_height;
	my_qic->cam[0].framerate= u_framerate;
	my_qic->cam[0].is_bind = 0; /* 2-way output from single QIC module */
	my_qic->cam[0].num_mmap_buffer = 4; /*  less memory */
	if(demux){
		my_qic->cam[0].is_demux =1;  //Enable YUYV bad frame check
	}


	/* commit and init the video dev */
	ret = qic_config_commit();
	if (ret) {
		printf("qic_config_commit error\n");
		return 1;
	}



/************************************************
 *
 * step 3: config print debug can be called at any time
 *
 *************************************************/
	char *log_str = NULL;
	log_str = qic_print_config_param(DEV_ID_0); /* print both interface config */
	printf("%s", log_str);

	/* open file for dump */
	open_file_dump(filename);


/************************************************
 *
 * step 4: activate the camera before capture frames
 *
 *************************************************/
	ret = qic_start_capture(DEV_ID_0); /* activate both video0 & video1 */
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
	int max_fd = fd0+1;

	/* custom select function for capture frames without pthread */
	fd_set fds;
	
	struct timeval tv;
	int r;

	while (frame_count > 0) {

		FD_ZERO(&fds);
		FD_SET(fd0, &fds);

		/* Timeout. */
		tv.tv_sec = 5;
		tv.tv_usec = 0;

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

		if (FD_ISSET(fd0, &fds)) {
			ret = qic_getframe2(DEV_ID_0);

			if (ret) {
				printf("qic_getframe error\n");
				return 1;
			}

		}

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
	ret = qic_stop_capture(DEV_ID_0); /* deactivate both video0 & video1 */
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
