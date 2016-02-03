/*
--       This software proprietary belong to Quanta computer Inc. and may be used        --
--        only as expressly authorized agreement from          --
--                            Quanta Computer Inc..                             --
--                   (C) COPYRIGHT 2010 Quanta Computer Inc.                    --
--                            ALL RIGHTS RESERVED                             --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
*/

#ifndef _QIC_API_FW_UPDATE_H_
#define _QIC_API_FW_UPDATE_H_

#include "qic_api_common_define.h"

/* firmware image file name default */
#define QIC1822_2nd_BOOTROM_IMAGE_NAME  "QIC1822A_2ndBL.bin"
#define QIC1822_USB_IMAGE_NAME          "QIC1822A_USB.bin"
#define QIC1822_ISP_IMAGE_NAME          "QIC1822A_ISP.bin"
#define QIC1822_PARAM_IMAGE_NAME        "QIC1822A_ISP_LSC_XXX.bin"
#define QIC1822_AUDIO_IMAGE_NAME        "QIC1822A_AUDIO_DATA.bin"
#define QIC1822_OSD_FONT_IMAGE_NAME     "QIC1822A_OSD_FONT_DATA.bin"
#define QIC_FLASH_DUMP_IMAGE_NAME       "_QIC_FLASH_DUMP.bin"

#define USB_FIRMWARE        0x01
#define ISP_FIRMWARE        0x02
#define PARAM_FIRMWARE      0x04
#define AUDIO_FIRMWARE      0x08
#define OSD_FONT_FIRMWARE   0x10


int qic_get_system_version (unsigned int dev_id, version_info_t *version_info);
int qic_get_image_version (char *path_info, version_info_t *version_info);
int qic_get_image_version_by_filename (char *usb_path_info, version_info_t *version_info);
int qic_update_firmware_by_filename(unsigned int dev_id,
                                    char *update_usb_img_loc,
                                    char *update_isp_img_loc,
                                    char *update_param_img_loc,
                                    char *update_audio_img_loc,
                                    char *update_osd_font_img_loc,
                                    unsigned char update_flags);
int qic_check_firmware(unsigned int dev_id,unsigned long checksum);
int qic_check_firmware_by_file(unsigned int dev_id,char *check_img_loc);
int qic_update_audio_data_by_filename(unsigned int dev_id, char *update_audio_img_loc);
int qic_update_osd_font_by_filename(unsigned int dev_id, char *update_osd_font_img_loc);

int qic_dump_all_flash(unsigned int dev_id);
int qic_backup_firmware_to_file(unsigned int dev_id, char *update_img_loc, unsigned char update_flags, char *backup_img_loc, unsigned char backup_flags);

#ifdef QIC_SUPPORT_2ND_BL
int qic_update_2nd_bootrom_by_filename(unsigned int dev_id, char eraseAll, char *update_BL2nd_img_loc);
int qic_backup_2nd_Boot_Rom(unsigned int dev_id, char *backup_BL2nd_img_loc);
#endif

/*Check Lock Streaming control*/
int qic_set_lock_steam_control( unsigned char lock);
int qic_check_lock_stream_status( unsigned char *isLock, unsigned char *isStream);

#endif
