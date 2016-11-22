/*
--       This software proprietary belong to Quanta computer Inc. and may be used        --
--        only as expressly authorized agreement from          --
--                            Quanta Computer Inc..                             --
--                   (C) COPYRIGHT 2010 Quanta Computer Inc.                    --
--                            ALL RIGHTS RESERVED                             --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
*/

#ifndef _QIC_API_OSD_H_
#define _QIC_API_OSD_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "qic_api_common_define.h"

#ifdef QIC_OSD_API
int qic_osd_change_status(unsigned int dev_id,unsigned char on);
int qic_osd_change_color(unsigned int dev_id,OsdColor_t osd_color[OSD_COLOR_MAX]);
int qic_osd_change_transparency_blink(unsigned int dev_id,OsdMiscell_t set_misc);
int qic_osd_get_transparency_blink(unsigned int dev_id,OsdMiscell_t *set_misc);
int qic_osd_get_color(unsigned int dev_id,OsdColor_t *osd_color);
int qic_osd_change_frame(unsigned int dev_id,OsdFrame_t osd_frame[OSD_FRAME_MAX]);
int qic_osd_get_frame(unsigned int dev_id,OsdFrame_t *osd_frame);
int qic_osd_change_string(unsigned int dev_id,unsigned char line_id,unsigned char start_char_index,unsigned char *str, OsdCharAttr_t char_attr);
int qic_osd_change_char_of_string(unsigned int dev_id,unsigned char line_id, unsigned char *str, OsdCharAttr_t char_attr[OSD_CHAR_MAX]);
int qic_osd_get_char_of_string(unsigned int dev_id,unsigned char line_id, unsigned char *str, OsdCharAttr_t *char_attr);
int qic_osd_change_line_attr(unsigned int dev_id,unsigned char line_id,OsdLineAttr_t line_attr);
int qic_osd_get_line_attr(unsigned int dev_id,unsigned char line_id,OsdLineAttr_t *line_attr);
int qic_osd_change_timer(unsigned int dev_id,unsigned char line_id,unsigned char enable,OsdTimer_t timer);
#endif

#ifdef __cplusplus
}
#endif

#endif
