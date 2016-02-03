/*
--       This software proprietary belong to Quanta computer Inc. and may be used        --
--        only as expressly authorized agreement from          --
--                            Quanta Computer Inc..                             --
--                   (C) COPYRIGHT 2010 Quanta Computer Inc.                    --
--                            ALL RIGHTS RESERVED                             --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
*/

#ifndef _QIC_API_MISC_H_
#define _QIC_API_MISC_H_

#include "qic_api_common_define.h"

// System information
int qic_get_sysinfo(int dev_id,SysInfo_t *sysinfo);

// Get capability supported list
int qic_get_Capability_List( unsigned char *maximun_number, unsigned int *support_list);

// Motor control
int qic_change_motor_control( unsigned char control, unsigned char direction);
int qic_get_motor_status( unsigned char *motor_status);

// Advanced motor control
int qic_set_advance_motor_postions( unsigned char set_numbers,MotorPosition_t Pos[] );
int qic_get_advance_motor_current_postion( MotorPosition_t *Pos );
int qic_start_advance_motor( void );
int qic_stop_advance_motor( void );
int qic_reset_position_advance_motor( void ) ;
int qic_set_advance_motor_speed( unsigned short pan_speed, unsigned short tilt_speed );

// Shutter mode control
int qic_change_Shutter_Mode( unsigned char mode);

// Get ALS value
int qic_get_ALS(unsigned int dev_id, unsigned short *ALS);

// IR control
int qic_change_IR_control( unsigned char mode);
int qic_get_IR_ALS_status( unsigned char *ir_status, unsigned char *ALS_status);

// Sensor Information
int qic_get_Sensor_AEInfo(unsigned short *AE_statistic);

// LED mode
int qic_change_LED_Mode( unsigned char mode);
int qic_change_LED_Brightness( unsigned char brightness);

// Audio Function control
int qic_change_NR_mode(unsigned char onoff);
int qic_change_EQ_mode(unsigned char onoff, unsigned char type);
int qic_change_BF_mode(unsigned char onoff);

// Audio ADC mode control
int qic_change_ADC_mode(ADCMode_t mode);

// H/V flip
int qic_change_flip(unsigned int dev_id, unsigned char flip) ;

// Pan tilt control
int qic_change_pan_tilt(unsigned int dev_id, signed short pan, signed short tilt);

// Mirror control
int qic_change_mirror_mode(int dev_id, unsigned char mode);

// Cropped image control
int qic_set_cropped_image(unsigned int dev_id,unsigned int crop_switch,CROPRECT crop_window);

// Utility/debug functions
int qic_get_fd_from_devid(unsigned int dev_id); /* if multiple fd, return first, -1 as not found */
char* qic_print_config_param(unsigned int dev_id);

int qic_set_lock_steam_control( unsigned char lock);



#endif
