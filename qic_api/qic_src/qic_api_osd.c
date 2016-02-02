/*
--       This software proprietary belong to Quanta computer Inc. and may be used        --
--        only as expressly authorized agreement from          --
--                            Quanta Computer Inc..                             --
--                   (C) COPYRIGHT 2010 Quanta Computer Inc.                    --
--                            ALL RIGHTS RESERVED                             --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
*/

#include "../qic_include/qic_api_osd.h"

// Common part
extern char debug_str[1024];
extern char debug_xuctrl_str[512];
extern int config_is_commit;
extern qic_module *dev_pt;
// End of common part


#ifdef QIC_OSD_API

int qic_osd_change_status(unsigned int dev_id,unsigned char on)
{
    int ret=0;
    unsigned int index;

    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)){
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id){
            QicChangeFD(dev_pt->cam[index].fd);
            ret=QicOsdStatusSet(on);
#ifdef DEBUG_LOG
            unsigned char osd_status;
            QicOsdStatusGet (&osd_status);
            printf("QicOsdStatusGet=======%d\n",osd_status);
#endif
        }
    }
    return ret;
}

int qic_osd_change_transparency_blink(unsigned int dev_id,OsdMiscell_t set_misc)
{
    int ret=0;
    unsigned int index;
    
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)){
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id){
            QicChangeFD(dev_pt->cam[index].fd);
            ret= QicOsdMiscellSet (set_misc);
#ifdef DEBUG_LOG
            OsdMiscell_t osd_miscell;
            QicOsdMiscellGet (&osd_miscell);
            printf("QicOsdMiscellGet FG_alpha=%d, BG_alpha=%d, blink_on=%d, blink_off=%d=======%d\n",osd_miscell.fg_alpha,osd_miscell.bg_alpha,osd_miscell.blink_on_frame_count, osd_miscell.blink_off_frame_count );
#endif
        }
    }
    return ret;
}

int qic_osd_get_transparency_blink(unsigned int dev_id,OsdMiscell_t *set_misc)
{
    int ret=0;
    unsigned int index;

    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL))
    {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id){
            QicChangeFD(dev_pt->cam[index].fd);
            ret= QicOsdMiscellGet (set_misc);
        }
    }
    return ret;
}

int qic_osd_change_color(unsigned int dev_id,OsdColor_t osd_color[OSD_COLOR_MAX])
{
    int ret=0;
    unsigned int index;

    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)){
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id){
            QicChangeFD(dev_pt->cam[index].fd);
            ret=QicOsdColorSet(osd_color);
#ifdef DEBUG_LOG
            OsdColor_t osd_color_get[OSD_COLOR_MAX];
            QicOsdColorGet(osd_color_get);
            int i=0;
            for(i=0;i<OSD_COLOR_MAX;i++)
                LOG_PRINT(debug_str, DEBUG_INFO, "y=%d, u=%d, u=%d\n",osd_color_get[i].color_y,osd_color_get[i].color_u,osd_color_get[i].color_v);
#endif
        }
    }
    return ret;
}

int qic_osd_get_color(unsigned int dev_id,OsdColor_t *osd_color)
{
    int ret=0;
    unsigned int index;

    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)){
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id){
            QicChangeFD(dev_pt->cam[index].fd);
            ret=QicOsdColorGet(osd_color);
#ifdef DEBUG_LOG
            int i=0;
            for(i=0;i<OSD_COLOR_MAX;i++)
                LOG_PRINT(debug_str, DEBUG_INFO, "y=%d, u=%d, u=%d\n",osd_color[i].color_y,osd_color[i].color_u,osd_color[i].color_v);
#endif
        }
    }
    return ret;
}

int qic_osd_change_frame(unsigned int dev_id,OsdFrame_t osd_frame[OSD_FRAME_MAX])
{
    int ret=0;
    unsigned int index;

    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)){
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id){
            QicChangeFD(dev_pt->cam[index].fd);
            ret=QicOsdFrameSet(osd_frame);
#ifdef DEBUG_LOG
            OsdFrame_t osd_frame_get[OSD_FRAME_MAX];
            int i=0;
            QicOsdFrameGet(osd_frame_get);
            for(i=0;i<OSD_FRAME_MAX;i++)
                LOG_PRINT(debug_str, DEBUG_INFO, "frame_height=%d, frame_y_start=%d\n",osd_frame_get[i].frame_height, osd_frame_get[i].frame_y_start);
#endif
        }
    }
    return ret;
}

int qic_osd_get_frame(unsigned int dev_id,OsdFrame_t *osd_frame)
{
    int ret=0;
    unsigned int index;

    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)){
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id){
            QicChangeFD(dev_pt->cam[index].fd);
            ret=QicOsdFrameGet(osd_frame);
#ifdef DEBUG_LOG
            int i=0;
            for(i=0;i<OSD_FRAME_MAX;i++)
                LOG_PRINT(debug_str, DEBUG_INFO,"frame_height=%d, frame_y_start=%d\n",osd_frame[i].frame_height, osd_frame[i].frame_y_start);
#endif
        }
    }
    return ret;
}

int qic_osd_change_string(unsigned int dev_id,unsigned char line_id,
                          unsigned char start_char_index, //start char position 0~31
                          unsigned char *str, /*Display string*/
                          OsdCharAttr_t char_attr)
{
    int ret=0;
    unsigned int index;

    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)){
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id){
            QicChangeFD(dev_pt->cam[index].fd);
            ret= QicOsdLineStringSet(line_id,start_char_index,str, char_attr);
#ifdef DEBUG_LOG
            unsigned char str_get[OSD_CHAR_MAX];
            OsdCharAttr_t char_attr_get;
            QicOsdLineStringGet(line_id,start_char_index,str_get, &char_attr_get);
            LOG_PRINT(debug_str, DEBUG_INFO, "str_get=%s bg_color_on=%d, fg_color=%d, color_mode=%d, updated=%d\n",
                      str_get, char_attr_get.bg_color_on, char_attr_get.fg_color, char_attr_get.color_mode,char_attr_get.updated);
#endif
        }
    }
    return ret;
}

int qic_osd_change_char_of_string(unsigned int dev_id,unsigned char line_id, unsigned char *str, OsdCharAttr_t char_attr[OSD_CHAR_MAX])
{
    int ret=0;
    unsigned int index;

    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)){
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id){
            QicChangeFD(dev_pt->cam[index].fd);
            ret= QicOsdLineArraySet(line_id,str,char_attr);
#ifdef DEBUG_LOG
            unsigned char str_get[OSD_CHAR_MAX];
            OsdCharAttr_t char_attr_get[OSD_CHAR_MAX];
            QicOsdLineArrayGet(line_id,str_get, char_attr_get);
            LOG_PRINT(debug_str, DEBUG_INFO, "str_get=%s bg_color_on[0]=%d, fg_color[0]=%d, color_mode[0]=%d, updated[0]=%d\n",
                      str_get, char_attr_get[line_id].bg_color_on, char_attr_get[line_id].fg_color, char_attr_get[line_id].color_mode,char_attr_get[line_id].updated);
#endif
        }
    }
    return ret;
}

int qic_osd_get_char_of_string(unsigned int dev_id,unsigned char line_id, unsigned char *str, OsdCharAttr_t *char_attr)
{
    int ret=0;
    unsigned int index;

    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)){
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id){
            QicChangeFD(dev_pt->cam[index].fd);
            ret= QicOsdLineArrayGet(line_id,str, char_attr);
        }
    }
    return ret;
}

int qic_osd_change_line_attr(unsigned int dev_id,unsigned char line_id,OsdLineAttr_t line_attr)
{
    int ret=0;
    unsigned int index;

    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)){
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id){
            QicChangeFD(dev_pt->cam[index].fd);
            ret=QicOsdLineAttrSet(line_id, line_attr);
#ifdef DEBUG_LOG
            OsdLineAttr_t line_attr_get;
            QicOsdLineAttrGet(line_id, &line_attr_get);
            LOG_PRINT(debug_str, DEBUG_INFO, "char_count=%d,char_x_repeat=%d,char_y_repeat=%d,enabled=%d,spacing=%d, x_start=%d, y_start=%d\n",
                      line_attr_get.char_count,line_attr_get.char_x_repeat,line_attr_get.char_y_repeat,line_attr_get.enabled,
                      line_attr_get.spacing,line_attr_get.x_start,line_attr_get.y_start);

#endif
        }
    }
    return ret;
}

int qic_osd_get_line_attr(unsigned int dev_id,unsigned char line_id,OsdLineAttr_t *line_attr)
{
    int ret=0;
    unsigned int index;

    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)){
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id){
            QicChangeFD(dev_pt->cam[index].fd);
            ret= QicOsdLineAttrGet(line_id, line_attr);
        }
    }
    return ret;
}

int qic_osd_change_timer(unsigned int dev_id,unsigned char line_id,unsigned char enable,OsdTimer_t timer)
{
    int ret=0;
    unsigned int index;

    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)){
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id){
            QicChangeFD(dev_pt->cam[index].fd);
            ret=QicOsdTimerSet(line_id, enable,timer);
        }
    }
    return ret;
}
#endif // end of OSD

