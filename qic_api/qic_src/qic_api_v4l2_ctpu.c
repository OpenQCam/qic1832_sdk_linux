/*
--       This software proprietary belong to Quanta computer Inc. and may be used        --
--        only as expressly authorized agreement from          --
--                            Quanta Computer Inc..                             --
--                   (C) COPYRIGHT 2010 Quanta Computer Inc.                    --
--                            ALL RIGHTS RESERVED                             --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
*/

#include "../qic_include/qic_api_v4l2_ctpu.h"

// Common part
extern char debug_str[1024];
extern char debug_xuctrl_str[512];
extern int config_is_commit;
extern qic_module *dev_pt;
// End of common part

int qic_V4L2_Control(int fd, unsigned long cmd, int Get,
                     signed long *value, signed long invalue)
{
    struct v4l2_queryctrl queryctrl;
    struct v4l2_control control;
    int qic_ret = 0;
    signed long* tempvalue=value;
    unsigned char h_flip2, v_flip2;

    memset (&queryctrl, 0, sizeof (queryctrl));
    queryctrl.id = cmd;

    if((cmd ==V4L2_CID_HFLIP) || (cmd ==V4L2_CID_VFLIP))
    {
        h_flip2 = 0;
        v_flip2 = 0;
        qic_ret = QicGetFlipMode(&v_flip2, &h_flip2);
        if (Get ==1){
            if(cmd == V4L2_CID_HFLIP){
                *tempvalue=h_flip2;
            }
            else if(cmd== V4L2_CID_VFLIP){
                *tempvalue=v_flip2;
            }
        }
        else {
            if(cmd == V4L2_CID_HFLIP){
                qic_ret = QicSetFlipMode(v_flip2,invalue);
            }
            else if(cmd== V4L2_CID_VFLIP){
                qic_ret = QicSetFlipMode(invalue,h_flip2);
            }
        }
        return 0;
    }

    if (-1 == ioctl (fd, VIDIOC_QUERYCTRL, &queryctrl)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "%u is not supported\n", queryctrl.id);
        return 1;
    }
    else if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "%u is not supported\n", queryctrl.id);
        return 1;
    }
    else {
        LOG_PRINT(debug_str, DEBUG_INFO, "%s is supported, %u\n", queryctrl.name, queryctrl.id);
    }

    memset (&control, 0, sizeof (control));
    control.id = cmd;
    if(Get==1) {
        if(0 == xioctl(fd, VIDIOC_G_CTRL, &control)) {
            *tempvalue=control.value;
        }
    }
    else {
        control.value=invalue;
        if(0 == xioctl(fd, VIDIOC_S_CTRL, &control)) {
            LOG_PRINT(debug_str, DEBUG_INFO, "%s set success, value=%u\n", queryctrl.name, control.value);
        }
        else{
            LOG_PRINT(debug_str, DEBUG_ERROR, "%s set fail, value=%u\n",  queryctrl.name, control.value);
            return 1;
        }
    }
    return 0;
}

int qic_change_V4L2_FOCUS_ABSOLUTE(unsigned int dev_id, unsigned int Auto, signed long absolute)
{
    unsigned int index;
    int ret=0;
#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR,"QIC module library config is not committed\n");
        return 1;
    }
#endif

    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {
            if(Auto)
                ret= qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_FOCUS_AUTO,0,0,absolute);
            else
            {
                ret= qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_FOCUS_AUTO,0,0,0);
                ret=qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_FOCUS_ABSOLUTE,0, 0,absolute);
            }
#ifdef DEBUG_LOG
            /*TIME_DELAY(1);*/
            signed long value=0;
            if(Auto)
                qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_FOCUS_ABSOLUTE,1,&value,0);
            else
                qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_FOCUS_AUTO,1,&value,0);
#endif
        }
    }
    /* success */
    return ret;
}

int qic_change_V4L2_BACKLIGHT_COMPENSATION(unsigned int dev_id, signed long  BC)
{
    unsigned int index;
    int ret=0;
#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR,"QIC module library config is not committed\n");
        return 1;
    }
#endif
    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {
            ret= qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_BACKLIGHT_COMPENSATION,0,0,BC);
#ifdef DEBUG_LOG
            /*TIME_DELAY(1);*/
            signed long value=0;
            qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_BACKLIGHT_COMPENSATION,1,&value,0);
#endif
        }
    }
    /* success */
    return ret;
}

int qic_change_V4L2_flip(unsigned int dev_id, unsigned char flip)
{
    unsigned int index;
    int ret = 0;
    unsigned char h_flip, v_flip;

#ifdef DEBUG_LOG
    int qic_ret;
#endif

#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR,"QIC module library config is not committed\n");
        return 1;
    }
#endif

    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {
            h_flip = flip & H_FLIP;
            v_flip = (flip & V_FLIP)>>1;
            ret=qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_HFLIP,0,0,h_flip);
            ret=qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_VFLIP,0,0,v_flip);
#ifdef DEBUG_LOG
            /*TIME_DELAY(1);*/
            h_flip = 0;
            v_flip = 0;
            qic_ret = QicGetFlipMode(&v_flip, &h_flip);
#endif
            LOG_PRINT(debug_str, DEBUG_INFO,  "QicGetFlipMode(%d) H_FLIP=%s, V_FLIP=%s\n", qic_ret,  h_flip?"on":"off", v_flip?"on":"off");
        }
    }
    /* success */
    return ret;
}

int qic_change_V4L2_WHITE_BALANCE(unsigned int dev_id, unsigned int  Auto,signed long WB)
{
    unsigned int index;
    int ret=0;
#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR,"QIC module library config is not committed\n");
        return 1;
    }
#endif
    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {
            if(Auto)
                ret= qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_AUTO_WHITE_BALANCE,0,0,WB);
            else
            {
                ret=qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_AUTO_WHITE_BALANCE,0,0,0);  //disable WBT,audo
                ret=qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_WHITE_BALANCE_TEMPERATURE,0,0,WB);
            }
#ifdef DEBUG_LOG
            /*TIME_DELAY(1);*/
            signed long value=0;
            if(Auto)
                qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_AUTO_WHITE_BALANCE,1,&value,0);
            else
                qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_WHITE_BALANCE_TEMPERATURE,1,&value,0);
#endif
        }
    }
    /* success */
    return ret;
}

int qic_change_V4L2_BRIGHTNESS(unsigned int dev_id,signed long BRIGHTNESS)
{
    unsigned int index;
    int ret=0;
#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR,"QIC module library config is not committed\n");
        return 1;
    }
#endif
    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {
            ret=qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_BRIGHTNESS,0,0,BRIGHTNESS);
#ifdef DEBUG_LOG
            /*TIME_DELAY(1);*/
            signed long value=0;
            qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_BRIGHTNESS,1,&value,0);
#endif
        }
    }
    /* success */
    return ret;
}

int qic_change_V4L2_CONTRAST(unsigned int dev_id,signed long  CONTRAST)
{
    unsigned int index;
    int ret=0;
#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR,"QIC module library config is not committed\n");
        return 1;
    }
#endif

    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {
            ret= qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_CONTRAST,0,0,CONTRAST);
#ifdef DEBUG_LOG
            /*TIME_DELAY(1);*/
            signed long value=0;
            qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_CONTRAST,1,&value,0);
#endif
        }
    }
    /* success */
    return ret;
}

int qic_change_V4L2_HUE(unsigned int dev_id, signed long HUE)
{
    unsigned int index;
    int ret=0;
#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR,"QIC module library config is not committed\n");
        return 1;
    }
#endif
    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {

            ret=qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_HUE,0,0,HUE);
#ifdef DEBUG_LOG
            /*TIME_DELAY(1);*/
            signed long value=0;
            qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_HUE,1,&value,0);
#endif
        }
    }
    /* success */
    return ret;
}

int qic_change_V4L2_SATURATION(unsigned int dev_id, signed long SATURATION)
{
    unsigned int index;
    int ret=0;
#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR,"QIC module library config is not committed\n");
        return 1;
    }
#endif
    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {

            ret=qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_SATURATION,0,0,SATURATION);

#ifdef DEBUG_LOG
            /*TIME_DELAY(1);*/
            signed long value=0;
            qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_SATURATION,1,&value,0);
#endif
        }
    }
    /* success */
    return ret;
}

int qic_change_V4L2_SHARPNESS(unsigned int dev_id,signed long SHARPNESS)
{
    unsigned int index;
    int ret=0;
#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR,"QIC module library config is not committed\n");
        return 1;
    }
#endif
    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {

            ret=qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_SHARPNESS,0,0,SHARPNESS);
#ifdef DEBUG_LOG
            /*TIME_DELAY(1);*/
            signed long value=0;
            qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_SHARPNESS,1,&value,0);
#endif
        }
    }
    /* success */
    return ret;
}

int qic_change_V4L2_GAMMA(unsigned int dev_id, signed long GAMMA)
{
    unsigned int index;
    int ret=0;
#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR,"QIC module library config is not committed\n");
        return 1;
    }
#endif
    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {

            ret=qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_GAMMA,0,0,GAMMA);
#ifdef DEBUG_LOG
            /*TIME_DELAY(1);*/
            signed long value=0;
            qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_GAMMA,1,&value,0);

#endif
        }
    }
    /* success */
    return ret;
}

int qic_change_V4L2_GAIN(unsigned int dev_id, signed long GAIN)
{
    unsigned int index;
    int ret=0;
#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR,"QIC module library config is not committed\n");
        return 1;
    }
#endif

    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {
            ret= qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_GAIN,0,0,GAIN);
#ifdef DEBUG_LOG
            /*TIME_DELAY(1);*/
            signed long value=0;
            qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_GAIN,1,&value,0);
#endif
        }
    }
    /* success */
    return ret;
}

int qic_change_V4L2_POWER_LINE_FREQUENCY(unsigned int dev_id, signed long PLF)
{
    unsigned int index;
    int ret=0;
#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR,"QIC module library config is not committed\n");
        return 1;
    }
#endif

    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {
            ret=qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_POWER_LINE_FREQUENCY,0,0,PLF);
#ifdef DEBUG_LOG
            /*TIME_DELAY(1);*/
            signed long value=0;
            qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_POWER_LINE_FREQUENCY,1,&value,0);
#endif
        }
    }
    /* success */
    return ret;
}

int qic_change_V4L2_EXPOSURE(unsigned int dev_id, unsigned int Auto, signed long absolute_or_mode)
{
    unsigned int index;
    int ret=0;
#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR,"QIC module library config is not committed\n");
        return 1;
    }
#endif

    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {

            if(Auto)
                ret=qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_EXPOSURE_AUTO,0,0,absolute_or_mode);// note: 1:manual, 3:auto
            else
            {
                ret=qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_EXPOSURE_AUTO,0,0,1);
                ret=qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_EXPOSURE_ABSOLUTE,0,0,absolute_or_mode);
            }
#ifdef DEBUG_LOG
            /*TIME_DELAY(1);*/
            signed long value=0;
            if(Auto)
                ret=qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_EXPOSURE_AUTO,1,&value,0);
            else
            {
                ret=qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_EXPOSURE_ABSOLUTE,1,&value,0);
            }
#endif
        }
    }
    /* success */
    return ret;
}

int qic_change_V4L2_EXPOSURE_AUTO_PRIORITY(unsigned int dev_id, signed long priority)
{
    unsigned int index;
    int ret=0;
#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR,"QIC module library config is not committed\n");
        return 1;
    }
#endif
    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {
            ret=qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_EXPOSURE_AUTO_PRIORITY,0,0,priority);
#ifdef DEBUG_LOG
            /*TIME_DELAY(1);*/
            signed long value=0;
            ret=qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_EXPOSURE_AUTO_PRIORITY,1,&value,0);
#endif
        }

    }
    /* success */
    return ret;
}

int qic_change_V4L2_ZOOM_ABSOLUTE(unsigned int dev_id, signed long absolute)
{
    unsigned int index;
    int ret=0;
#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR,"QIC module library config is not committed\n");
        return 1;
    }
#endif

    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {
            ret=qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_ZOOM_ABSOLUTE,0,0,absolute);
#ifdef DEBUG_LOG
            /*TIME_DELAY(1);*/
            signed long value=0;
            ret=qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_ZOOM_ABSOLUTE,1,&value,0);
#endif
        }
    }
    /* success */
    return ret;
}

int qic_change_V4L2_Pan_Tilt2(unsigned int dev_id,signed short pan_value, signed short tilt_value)
{
    int ret=0;
    signed long pos=0;
    unsigned int index;
#ifdef DEBUG_LOG
    signed long value=0;
#endif

#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR,"QIC module library config is not committed\n");
        return 1;
    }
#endif

    /*
       if(10 <tilt_value||-10> tilt_value)
        {
            printf( " Tilt out of range!!\n");
        return -1;
        }

      if(10 <pan_value||-10> pan_value)
        {
            printf( " Pan out of range!!\n");
        return -1;
        }
*/
    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {
            /* min:-36000 max:36000 step:3600  def:0 */

            pos=(tilt_value*PTZ_TILT_STEP);
            ret=qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_TILT_ABSOLUTE,0,0,pos);
#ifdef DEBUG_LOG
            ret=qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_TILT_ABSOLUTE,1,&value,0);
#endif
            pos=(pan_value*PTZ_TILT_STEP);
            ret=qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_PAN_ABSOLUTE,0,0,pos);
#ifdef DEBUG_LOG
            ret=qic_V4L2_Control(dev_pt->cam[index].fd,V4L2_CID_PAN_ABSOLUTE,1,&value,0);
#endif
        }
    }
    return ret;
}

// Function for retrieving CT/PU setting
int qic_get_ctpu_setting(int dev_id, sqicV4L2 *camerav4l2)
{
    struct v4l2_queryctrl queryctrl;
    struct v4l2_querymenu querymenu;
    struct v4l2_control   control_s;
    struct v4l2_input*    getinput;
    unsigned int ret=0;
    unsigned int index=0;
    //Name of the device
    signed int menu_index;
    int dev_found = 0;

    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id){// 1 ,2, 4 map
            getinput=(struct v4l2_input *) calloc(1, sizeof(struct v4l2_input));
            memset(getinput, 0, sizeof(struct v4l2_input));
            getinput->index=0;
            ret = ioctl(dev_pt->cam[index].fd, VIDIOC_ENUMINPUT, getinput);
            if(ret!=0){
                goto fatal_controls;
            }
            else{
                dev_found = 1;
            }

            //  printf (" Available controls of device '%s' (Type 1=Integer 2=Boolean 3=Menu 4=Button 5=Integer64 6=class)\n",getinput->name);
            LOG_PRINT(debug_str, DEBUG_INFO, " Available controls of device '%s' (Type 1=Integer 2=Boolean 3=Menu 4=Button 5=Integer64 6=class)\n",getinput->name);

            //subroutine to read menu items of controls with type 3
            void enumerate_menu (void)
            {
                LOG_PRINT(debug_str, DEBUG_INFO,"  Menu items:\n");
                memset (&querymenu, 0, sizeof (querymenu));
                querymenu.id = queryctrl.id;
                menu_index=(unsigned int)querymenu.index;
                for (menu_index= queryctrl.minimum;
                     menu_index <= queryctrl.maximum;
                     menu_index++) {
                    if (0 == ioctl (dev_pt->cam[index].fd, VIDIOC_QUERYMENU, &querymenu)) {
                        LOG_PRINT(debug_str, DEBUG_INFO, "index:%d name:%s\n", menu_index, querymenu.name);
                        TIME_DELAY(1);
                    } else {
                        LOG_PRINT(debug_str, DEBUG_INFO, "error getting control menu\n");
                        break;
                    }
                }
            }

            //predefined controls
            LOG_PRINT(debug_str, DEBUG_INFO,"V4L2_CID_BASE         (predefined controls):\n");
            memset (&queryctrl, 0, sizeof (queryctrl));
            for (queryctrl.id = V4L2_CID_BASE;
                 queryctrl.id < V4L2_CID_LASTP1;
                 queryctrl.id++) {
                if (0 == ioctl (dev_pt->cam[index].fd, VIDIOC_QUERYCTRL, &queryctrl)) {
                    if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
                        continue;
                    control_s.id=queryctrl.id;
                    ioctl(dev_pt->cam[index].fd, VIDIOC_G_CTRL, &control_s);
                    TIME_DELAY(1);
                    LOG_PRINT(debug_str, DEBUG_INFO," index:%-10d name:%-32s type:%d min:%-5d max:%-5d step:%-5d def:%-5d now:%d\n",
                              queryctrl.id, queryctrl.name, queryctrl.type, queryctrl.minimum,
                              queryctrl.maximum, queryctrl.step, queryctrl.default_value, control_s.value);
                    if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
                        enumerate_menu ();

                    if(V4L2_CID_BRIGHTNESS==queryctrl.id)
                    {
                        camerav4l2->Brightness.def=queryctrl.default_value;
                        camerav4l2->Brightness.max=queryctrl.maximum;
                        camerav4l2->Brightness.min=queryctrl.minimum;
                        camerav4l2->Brightness.step=queryctrl.step;
                        camerav4l2->Brightness.now=control_s.value;
                    }

                    if(V4L2_CID_CONTRAST==queryctrl.id)
                    {
                        camerav4l2->Contrast.def=queryctrl.default_value;
                        camerav4l2->Contrast.max=queryctrl.maximum;
                        camerav4l2->Contrast.min=queryctrl.minimum;
                        camerav4l2->Contrast.step=queryctrl.step;
                        camerav4l2->Contrast.now=control_s.value;
                    }

                    if(V4L2_CID_SATURATION==queryctrl.id)
                    {
                        camerav4l2->Saturation.def=queryctrl.default_value;
                        camerav4l2->Saturation.max=queryctrl.maximum;
                        camerav4l2->Saturation.min=queryctrl.minimum;
                        camerav4l2->Saturation.step=queryctrl.step;
                        camerav4l2->Saturation.now=control_s.value;
                    }

                    if(V4L2_CID_HUE==queryctrl.id)
                    {
                        camerav4l2->Hue.def=queryctrl.default_value;
                        camerav4l2->Hue.max=queryctrl.maximum;
                        camerav4l2->Hue.min=queryctrl.minimum;
                        camerav4l2->Hue.step=queryctrl.step;
                        camerav4l2->Hue.now=control_s.value;
                    }

                    if(V4L2_CID_GAMMA==queryctrl.id)
                    {
                        camerav4l2->Gamma.def=queryctrl.default_value;
                        camerav4l2->Gamma.max=queryctrl.maximum;
                        camerav4l2->Gamma.min=queryctrl.minimum;
                        camerav4l2->Gamma.step=queryctrl.step;
                        camerav4l2->Gamma.now=control_s.value;
                    }

                    if(V4L2_CID_GAIN==queryctrl.id)
                    {
                        camerav4l2->Gain.def=queryctrl.default_value;
                        camerav4l2->Gain.max=queryctrl.maximum;
                        camerav4l2->Gain.min=queryctrl.minimum;
                        camerav4l2->Gain.step=queryctrl.step;
                        camerav4l2->Gain.now=control_s.value;
                    }


                    if(V4L2_CID_POWER_LINE_FREQUENCY==queryctrl.id)
                    {
                        camerav4l2->Plf.def=queryctrl.default_value;
                        camerav4l2->Plf.max=queryctrl.maximum;
                        camerav4l2->Plf.min=queryctrl.minimum;
                        camerav4l2->Plf.step=queryctrl.step;
                        camerav4l2->Plf.now=control_s.value;
                    }

                    if(V4L2_CID_WHITE_BALANCE_TEMPERATURE==queryctrl.id)
                    {
                        camerav4l2->WB.def=queryctrl.default_value;
                        camerav4l2->WB.max=queryctrl.maximum;
                        camerav4l2->WB.min=queryctrl.minimum;
                        camerav4l2->WB.step=queryctrl.step;
                        camerav4l2->WB.now=control_s.value;
                    }

                    if(V4L2_CID_SHARPNESS==queryctrl.id)
                    {
                        camerav4l2->Sharpness.def=queryctrl.default_value;
                        camerav4l2->Sharpness.max=queryctrl.maximum;
                        camerav4l2->Sharpness.min=queryctrl.minimum;
                        camerav4l2->Sharpness.step=queryctrl.step;
                        camerav4l2->Sharpness.now=control_s.value;
                    }

                    if(V4L2_CID_BACKLIGHT_COMPENSATION==queryctrl.id)
                    {
                        camerav4l2->BC.def=queryctrl.default_value;
                        camerav4l2->BC.max=queryctrl.maximum;
                        camerav4l2->BC.min=queryctrl.minimum;
                        camerav4l2->BC.step=queryctrl.step;
                        camerav4l2->BC.now=control_s.value;
                    }

                } else {
                    if (errno == EINVAL)
                        continue;
                    perror ("error getting base controls");
                    goto fatal_controls;
                }
            }

            //predefined controls
            LOG_PRINT(debug_str, DEBUG_INFO,"V4L2_CID_CAMERA_CLASS_BASE         (predefined controls):\n");
            memset (&queryctrl, 0, sizeof (queryctrl));
            for (queryctrl.id = V4L2_CID_CAMERA_CLASS_BASE;
                 queryctrl.id < V4L2_CID_ZOOM_ABSOLUTE+1;
                 queryctrl.id++) {
                if (0 == ioctl (dev_pt->cam[index].fd, VIDIOC_QUERYCTRL, &queryctrl)) {
                    if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
                        continue;
                    control_s.id=queryctrl.id;
                    ioctl(dev_pt->cam[index].fd, VIDIOC_G_CTRL, &control_s);
                    TIME_DELAY(1);
                    LOG_PRINT(debug_str, DEBUG_INFO," index:%-10d name:%-32s type:%d min:%-5d max:%-5d step:%-5d def:%-5d now:%d\n",
                              queryctrl.id, queryctrl.name, queryctrl.type, queryctrl.minimum,
                              queryctrl.maximum, queryctrl.step, queryctrl.default_value, control_s.value);
                    if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
                        enumerate_menu ();

                    if(V4L2_CID_EXPOSURE_ABSOLUTE==queryctrl.id)
                    {
                        camerav4l2->Exposure.def=queryctrl.default_value;
                        camerav4l2->Exposure.max=queryctrl.maximum;
                        camerav4l2->Exposure.min=queryctrl.minimum;
                        camerav4l2->Exposure.step=queryctrl.step;
                        camerav4l2->Exposure.now=control_s.value;
                    }

                    if(V4L2_CID_EXPOSURE_AUTO_PRIORITY==queryctrl.id)
                    {
                        camerav4l2->E_priority.def=queryctrl.default_value;
                        camerav4l2->E_priority.max=queryctrl.maximum;
                        camerav4l2->E_priority.min=queryctrl.minimum;
                        camerav4l2->E_priority.step=queryctrl.step;
                        camerav4l2->E_priority.now=control_s.value;
                    }

                    if(V4L2_CID_FOCUS_ABSOLUTE==queryctrl.id)
                    {
                        camerav4l2->Focus.def=queryctrl.default_value;
                        camerav4l2->Focus.max=queryctrl.maximum;
                        camerav4l2->Focus.min=queryctrl.minimum;
                        camerav4l2->Focus.step=queryctrl.step;
                        camerav4l2->Focus.now=control_s.value;
                    }
                    if(V4L2_CID_PAN_ABSOLUTE==queryctrl.id)
                    {
                        camerav4l2->Pan.def=queryctrl.default_value;
                        camerav4l2->Pan.max=queryctrl.maximum/queryctrl.step;
                        camerav4l2->Pan.min=queryctrl.minimum/queryctrl.step;
                        camerav4l2->Pan.step=queryctrl.step/queryctrl.step;
                        camerav4l2->Pan.now=control_s.value/queryctrl.step;
                    }
                    if(V4L2_CID_TILT_ABSOLUTE==queryctrl.id)
                    {
                        camerav4l2->Tilt.def=queryctrl.default_value;
                        camerav4l2->Tilt.max=queryctrl.maximum/queryctrl.step;
                        camerav4l2->Tilt.min=queryctrl.minimum/queryctrl.step;
                        camerav4l2->Tilt.step=queryctrl.step/queryctrl.step;
                        camerav4l2->Tilt.now=control_s.value/queryctrl.step;
                    }

                    if(V4L2_CID_ZOOM_ABSOLUTE==queryctrl.id)
                    {
                        camerav4l2->Zoom.def=queryctrl.default_value;
                        camerav4l2->Zoom.max=queryctrl.maximum;
                        camerav4l2->Zoom.min=queryctrl.minimum;
                        camerav4l2->Zoom.step=queryctrl.step;
                        camerav4l2->Zoom.now=control_s.value;
                    }

                } else {
                    if (errno == EINVAL)
                        continue;
                    perror ("error getting base controls");
                    goto fatal_controls;
                }
            }

            //driver specific controls
            LOG_PRINT(debug_str, DEBUG_INFO,"V4L2_CID_PRIVATE_BASE (driver specific controls):\n");
            for (queryctrl.id = V4L2_CID_PRIVATE_BASE;;
                 queryctrl.id++) {
                if (0 == ioctl (dev_pt->cam[index].fd, VIDIOC_QUERYCTRL, &queryctrl)) {
                    if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
                        continue;
                    control_s.id=queryctrl.id;
                    ioctl(dev_pt->cam[index].fd, VIDIOC_G_CTRL, &control_s);
                    TIME_DELAY(20);
                    LOG_PRINT(debug_str, DEBUG_INFO," index:%-10d name:%-32s type:%d min:%-5d max:%-5d step:%-5d def:%-5d now:%d\n",
                              queryctrl.id, queryctrl.name, queryctrl.type, queryctrl.minimum,
                              queryctrl.maximum, queryctrl.step, queryctrl.default_value, control_s.value);
                    if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
                        enumerate_menu ();
                } else {
                    if (errno == EINVAL)
                        break;
                    perror ("error getting private base controls");
                    goto fatal_controls;
                }
            }

        }
    }

    if(dev_found==0){
        goto fatal_controls;
    }

    return 0;

fatal_controls:
    return -1;
}
