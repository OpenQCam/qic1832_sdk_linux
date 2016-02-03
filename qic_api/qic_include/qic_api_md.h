/*
--       This software proprietary belong to Quanta computer Inc. and may be used        --
--        only as expressly authorized agreement from          --
--                            Quanta Computer Inc..                             --
--                   (C) COPYRIGHT 2010 Quanta Computer Inc.                    --
--                            ALL RIGHTS RESERVED                             --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
*/

#ifndef _QIC_API_MD_H_
#define _QIC_API_MD_H_

#include "qic_api_common_define.h"

#ifdef QIC_MD_API

#define DBG_SEI_MD_TIMESTAMP            1
#define DBG_SEI_MD_NUM_OF_MOVING_OBJS   2
#define DBG_SEI_MD_X                    3
#define DBG_SEI_MD_Y                    4
#define DBG_SEI_MD_WIDTH                5
#define DBG_SEI_MD_HEIGHT               6
#define DBG_SEI_CODE_PAGE            0x3F

unsigned int AnalyzeSEIMessages(unsigned char* buffer,unsigned int size,unsigned int* sei_begin_offset,md_status_t* md_status);

/*MD*/
int qic_md_get_version(unsigned int dev_id,int *major_version, int *minor_version);
int qic_md_start_stop(unsigned int dev_id,unsigned char on);
int qic_md_change_config(unsigned int dev_id,md_config_t* config, unsigned char interrupt_mode);
int qic_md_get_config(unsigned int dev_id,md_config_t* config, unsigned char* interrupt_mode);
int qic_md_get_status(unsigned int dev_id,md_status_t* status);// no used

/*TD*/
int qic_td_get_version(unsigned int dev_id,int *major_version, int *minor_version);
int qic_td_start_stop(unsigned int dev_id,unsigned char on);
int qic_td_change_config(unsigned int dev_id,td_config_t* config);
int qic_td_get_config(unsigned int dev_id,td_config_t* config);
int qic_td_get_status(unsigned int dev_id,int* status);

#endif

#endif
