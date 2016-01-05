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


#ifdef DMALLOC
#include <dmalloc.h>
#endif



char g_runing=1;

void debug_log(int level, char *string) {

	/* debug mesg level */
	char *debug_level[] = {"INFO", "WARN", "CRIT", "FATL", "DEAD"};
	
	printf("QIC module debug_print (%s):%s", debug_level[level], string);
	
}

static void usage(FILE * fp, int argc, char **argv)
{
	fprintf(fp,
	"Usage: %s [options]\n\n"
	"Options:\n"
	"-c | --checksum set firmware checksum value\n"
	"-d | --checkfile set firmware check file\n"	 
	"-h | --help	 Print this message\n"
	"",
	argv[0]);
}


static const char short_options [] = "hc:d:";


static const struct option long_options [] =
{

	{ "checksum",		required_argument,	NULL,	'c' },
	{ "checkfile",		required_argument,	NULL,	'd' },	
	{ "help",		no_argument,		NULL,	'h' },

	{ 0, 0, 0, 0 }
};





int main(int argc,char ** argv)
{	
	
	unsigned short yuv_width = 320;
	unsigned short yuv_height = 240;
       	char *check_bin_file=NULL;
        char b_checksum=0;
	char b_checkbin=0;	
	qic_module *my_qic = NULL;
	int ret;
	SysInfo_t qicSysInfo;
        unsigned long checksum=0;
	  static struct timespec start_ts,end_ts;
	  long diff_sec,diff_nsec;
	  double diff_msec;
			  

	
	qic_dev_name_s video_name;



	if (getuid() != 0){
		printf("please run as root\n");
		exit(0);
	}


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
			
		case 'c':
			checksum =(unsigned long) atoi(optarg);
                        b_checksum=1;
			break;
		case 'd':
			check_bin_file= optarg;
			b_checkbin=1;
			break;	
		case 'h':
			usage(stdout, argc, argv);
			exit(EXIT_SUCCESS);
			
		default:
			usage(stderr, argc, argv);
			exit(EXIT_FAILURE);
		}
	}


	memset(&video_name,0, sizeof(video_name));
	ret=qic_enum_device_formats(&video_name);
//	printf("\n QIC1822 encdoing video=%s, raw video=%s\n",video_name.dev_avc, video_name.dev_yuv);
	if(ret){
               printf("\033[1;31m Not supported camera !\033[m\n");
			   return 0;

	}	
		
	
	if(b_checksum&& !checksum){
            printf("pleaes set checksum value\n");
            return 1;
	}else if(b_checkbin&& !check_bin_file){
	   printf("pleaes set check bin file\n");
            return 1;	
    }else if(b_checkbin==0&b_checksum==0){
		printf("pleaes set check bin file or set checksum value\n");
            return 1;
	}

	
/************************************************
 *
 * first step, init the qic module capture library
 * two devices, /dev/video0 as YUV raw
 *                   /dev/video1 as H.264/AVC
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

	/*  set scheduler */
	my_qic->high_prio = 1;


	
     if(strlen(video_name.dev_yuv)>0)
	my_qic->cam[0].dev_name = video_name.dev_yuv;
     else	
        my_qic->cam[0].dev_name ="/dev/video0";
	
	my_qic->cam[0].format = V4L2_PIX_FMT_YUYV;
	my_qic->cam[0].width = yuv_width; 
	my_qic->cam[0].height = yuv_height;
	my_qic->cam[0].is_bind = 1; 
	my_qic->cam[0].num_mmap_buffer = 0; /*  less memory */
	
	ret=qic_force_config();
	if (ret) {
		printf("\033[1;31m qic cmaera configure failed \033[m\n");
		return 1;
	}





/************************************************
 *
 * step 3: update the firmware
 *
 *************************************************/

	version_info_t qic_info;



	clock_gettime(0,&start_ts);
             printf("\n");
             printf("\nStart check firmware ....\n");


	    if(b_checksum){		  
	      ret=qic_check_firmware(DEV_ID_0,checksum);
	    }else if(b_checkbin){
                ret=qic_check_firmware_by_file(DEV_ID_0,check_bin_file);
	    }

	
		  g_runing=0;
		  clock_gettime(0,&end_ts);
             printf("\nRESULT:");
		if(!ret)
			printf("\n\033[1;32mfirmware check success!\033[m\n");
		else{
			printf("\n\033[1;31mfirmware check failed!\033[m\n");

			return 1;
		}	


			diff_sec= end_ts.tv_sec-start_ts.tv_sec;
			diff_nsec=end_ts.tv_nsec-start_ts.tv_nsec;
			diff_msec=diff_nsec/1000000;
			printf("elapsed time= %u.%u sec \n", (unsigned int)diff_sec,(unsigned char)diff_msec);
			

	ret = qic_release();
	if (ret) {
		printf("qic_release error\n");
		return 1;
	}




	return 0;
}



