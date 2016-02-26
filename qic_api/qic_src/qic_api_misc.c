/*
--       This software proprietary belong to Quanta computer Inc. and may be used        --
--        only as expressly authorized agreement from          --
--                            Quanta Computer Inc..                             --
--                   (C) COPYRIGHT 2010 Quanta Computer Inc.                    --
--                            ALL RIGHTS RESERVED                             --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
*/

#include "../qic_include/qic_api_misc.h"

// Common part
extern char debug_str[1024];
extern char debug_xuctrl_str[512];
extern int config_is_commit;
extern qic_module *dev_pt;
// End of common part


// System information
int qic_get_sysinfo(int dev_id,SysInfo_t *sysinfo)
{
    int qic_ret=0;
    unsigned int index=0;

#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif

    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {
            QicChangeFD(dev_pt->cam[index].fd);
            qic_ret=	QicGetSysInfo(sysinfo);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicGetSysInfo success\n");
        }
    }

    return qic_ret;
}

// Get capability supported list
int qic_get_Capability_List(unsigned char *maximun_number, unsigned int *support_list)
{
    int qic_ret=0;

#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif

    qic_ret=QicGetCapabilityList(maximun_number, support_list);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
    if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicGetCapabilityList success\n");

    printf("maximun support number=%d, support list=%d\n",*maximun_number, *support_list);
    printf("ALS=%d, IR=%d, OSD=%d, PT=%d, Simulcast=%d\n",*support_list&0x0001, (*support_list>>1)&0x0001,(*support_list>>2)&0x0001,(*support_list>>3)&0x0001,(*support_list>>4)&0x0001);

    return qic_ret;
}

// Motor control
int qic_change_motor_control( unsigned char control, unsigned char direction)
{
    int qic_ret = 0;

#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif

    qic_ret = QicSetMotorControl(control, direction);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);

    if(!qic_ret){
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_change_motor_control success\n");
    }
    else{
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_change_motor_control failed\n");
        qic_ret=1;
    }

    return qic_ret;
}

int qic_get_motor_status( unsigned char *motor_status)
{
    int qic_ret = 0;

#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif

    qic_ret = QicGetMotorStatus(motor_status);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);

    if(!qic_ret){
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_get_motor_status success motor status=%d\n", *motor_status);
    }
    else{
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_get_motor_status failed\n");
        qic_ret=1;
    }

    return qic_ret;
}

// Advanced motor control
int qic_set_advance_motor_postions( unsigned char set_numbers,MotorPosition_t Pos[] )
{
    int qic_ret = 0;
    AdvMotorCtl_t PosSetting;
    unsigned char i=0;
    unsigned char temp_number,temp_start_index;

#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif

    if(set_numbers<=5){
        PosSetting.TotalPositionNumbers=set_numbers;
        PosSetting.StartElementIndex=1;

        memcpy(&PosSetting.PosArray[0],Pos,sizeof(MotorPosition_t)*set_numbers);
        printf("0: %d, %d, 1:%d, %d, 2:%d, %d,3:%d, %d,4:%d, %d,",PosSetting.PosArray[0].pan, PosSetting.PosArray[0].tilt
               ,PosSetting.PosArray[1].pan, PosSetting.PosArray[1].tilt,PosSetting.PosArray[2].pan, PosSetting.PosArray[2].tilt
               ,PosSetting.PosArray[3].pan, PosSetting.PosArray[3].tilt,PosSetting.PosArray[4].pan, PosSetting.PosArray[4].tilt);
        qic_ret =QicSetAdvMotorControl(ADV_MOTOR_NAVIGATE_POSITION_SET,set_numbers,PosSetting);

    }
    else{
        temp_number=set_numbers/5;
        temp_start_index=set_numbers%5;

        printf("temp_number=%d, temp_start_index=%d\n",temp_number,temp_start_index);

        for(i=0;i<temp_number*5;i+=5)
        {
            PosSetting.TotalPositionNumbers=5;
            PosSetting.StartElementIndex=i+1;

            memcpy(&PosSetting.PosArray[0],Pos+i,sizeof(MotorPosition_t)*5);
            qic_ret =QicSetAdvMotorControl(ADV_MOTOR_NAVIGATE_POSITION_SET,5,PosSetting);

            printf("i=%d, end=%d\n",i,temp_number*5);
            printf("TotalPositionNumbers=%d\n",PosSetting.TotalPositionNumbers);
            printf("StartElementIndex=%d\n",PosSetting.StartElementIndex);
            printf("0: %d, %d, 1:%d, %d, 2:%d, %d,3:%d, %d,4:%d, %d,",PosSetting.PosArray[0].pan, PosSetting.PosArray[0].tilt
                   ,PosSetting.PosArray[1].pan, PosSetting.PosArray[1].tilt,PosSetting.PosArray[2].pan, PosSetting.PosArray[2].tilt
                   ,PosSetting.PosArray[3].pan, PosSetting.PosArray[3].tilt,PosSetting.PosArray[4].pan, PosSetting.PosArray[4].tilt);

            printf("i=%d, end=%d\n",i,temp_number*5);
        }

        if(temp_start_index!=0)
        {

            PosSetting.TotalPositionNumbers=temp_start_index;
            PosSetting.StartElementIndex=(temp_number*5)+1;
            memcpy(&PosSetting.PosArray[0],Pos+temp_number*5,sizeof(MotorPosition_t)*temp_start_index);
            qic_ret =QicSetAdvMotorControl(ADV_MOTOR_NAVIGATE_POSITION_SET,temp_start_index,PosSetting);
            printf("TotalPositionNumbers=%d\n",PosSetting.TotalPositionNumbers);
            printf("StartElementIndex=%d\n",PosSetting.StartElementIndex);
            printf("0: %d, %d, 1:%d, %d, 2:%d, %d,3:%d, %d,4:%d, %d,",PosSetting.PosArray[0].pan, PosSetting.PosArray[0].tilt
                   ,PosSetting.PosArray[1].pan, PosSetting.PosArray[1].tilt,PosSetting.PosArray[2].pan, PosSetting.PosArray[2].tilt
                   ,PosSetting.PosArray[3].pan, PosSetting.PosArray[3].tilt,PosSetting.PosArray[4].pan, PosSetting.PosArray[4].tilt);

        }
    }

    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);

    if(!qic_ret){
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_set_advance_motor_postions success\n");
    }
    else{
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_set_advance_motor_postions failed\n");
        qic_ret=1;
    }

    return qic_ret;
}

int qic_get_advance_motor_current_postion( MotorPosition_t *Pos )
{
    int qic_ret = 0;

#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif

    qic_ret=QicGetAdvMotorPosition(Pos);

    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);

    if(!qic_ret){
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_get_advance_motor_current_postion success\n");
    }
    else{
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_get_advance_motor_current_postion failed\n");
        qic_ret=1;
    }

    return qic_ret;
}

int qic_set_advance_motor_postion_single( MotorPosition_t Pos )
{
    int qic_ret = 0;
    AdvMotorCtl_t PosSetting;
#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif

    PosSetting.PosArray[0].pan=Pos.pan;
    PosSetting.PosArray[0].tilt=Pos.tilt;
    PosSetting.TotalPositionNumbers=1;
    PosSetting.StartElementIndex=1;
    PosSetting.PosArray[0].pan=Pos.pan;
    PosSetting.PosArray[0].tilt=Pos.tilt;

    qic_ret =QicSetAdvMotorControl(ADV_MOTOR_SINGLE_SET,1,PosSetting);

    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);

    if(!qic_ret){
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_set_advance_motor_postion_singl success\n");
    }
    else{
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_set_advance_motor_postion_singl failed\n");
        qic_ret=1;
    }

    return qic_ret;
}

int qic_start_advance_motor( void )
{
    int qic_ret = 0;
    AdvMotorCtl_t PosSetting;
#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif

    PosSetting.TotalPositionNumbers=0;
    PosSetting.StartElementIndex=1;

    qic_ret =QicSetAdvMotorControl(ADV_MOTOR_NAVIGATE_SET,0,PosSetting);

    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);

    if(!qic_ret){
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_start_advance_motor success\n");
    }
    else{
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_start_advance_motor failed\n");
        qic_ret=1;
    }

    return qic_ret;
}

int qic_stop_advance_motor( void )
{
    int qic_ret = 0;
    AdvMotorCtl_t PosSetting;
#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif

    PosSetting.TotalPositionNumbers=0;
    PosSetting.StartElementIndex=1;
    PosSetting.PosArray[0].pan=0;
    PosSetting.PosArray[0].tilt=0;

    qic_ret =QicSetAdvMotorControl(ADV_MOTOR_STOP,0,PosSetting);

    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);

    if(!qic_ret){
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_stop_advance_motor success\n");
    }
    else{
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_stop_advance_motor failed\n");
        qic_ret=1;
    }

    return qic_ret;
}

int qic_reset_position_advance_motor( void )
{
    int qic_ret = 0;
    AdvMotorCtl_t PosSetting;
#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif
    PosSetting.TotalPositionNumbers=0;
    PosSetting.StartElementIndex=1;

    qic_ret =QicSetAdvMotorControl(ADV_MOTOR_RESET,0,PosSetting);

    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);

    if(!qic_ret){
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_reset_position_advance_motor success\n");
    }
    else{
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_reset_position_advance_motor failed\n");
        qic_ret=1;
    }

    return qic_ret;
}

int qic_set_advance_motor_speed( unsigned short pan_speed, unsigned short tilt_speed )
{
    int qic_ret = 0;
    AdvMotorCtl_t PosSetting;
#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif

    PosSetting.StartElementIndex=1;
    PosSetting.TotalPositionNumbers=1;
    PosSetting.PosArray[0].pan=pan_speed;
    PosSetting.PosArray[0].tilt=tilt_speed;

    qic_ret =QicSetAdvMotorControl(ADV_MOTOR_SPEED,1,PosSetting);

    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);

    if(!qic_ret){
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_set_advance_motor_speed success\n");
    }
    else{
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_set_advance_motor_speed failed\n");
        qic_ret=1;
    }

    return qic_ret;
}

// Shutter mode control
int qic_change_Shutter_Mode( unsigned char mode)
{
    int ret = 0;
    int qic_ret = 0;

#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif

    qic_ret = QicSetShutterMode(mode);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
    if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicSetShutterMode success\n");

#ifdef DEBUG_LOG
    unsigned char shutter_mode;
    shutter_mode = 0;
    qic_ret = QicGetShutterMode(&shutter_mode);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
    if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicGetShutterMode success,  Shutter_Mode=%d, \n", shutter_mode);
#endif

    /* success */
    return ret;
}

// Get ALS value
int qic_get_ALS(unsigned int dev_id, unsigned short *ALS)
{
    int qic_ret=0;
    unsigned int index=0;

#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif

    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {
            QicChangeFD(dev_pt->cam[index].fd);
            qic_ret=QicGetALS(ALS);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicGetALS success\n");
        }
    }

    return qic_ret;
}

// IR control
int qic_set_IR(unsigned char als_mode, unsigned char ir_status)
{
    int qic_ret = 0;
    unsigned char als_mode_and_ir_status;

#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif

    qic_ret = QicSetIR((als_mode<<1) && ir_status);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);

    if(!qic_ret){
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_set_IR success\n");
    }
    else{
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_set_IR failed\n");
        qic_ret=1;
    }

    QicGetIR(&als_mode_and_ir_status);
    LOG_PRINT(debug_str, DEBUG_INFO, "ALS mode=%d, IR status=%d\n", (als_mode_and_ir_status>>1) & 0x1, als_mode_and_ir_status & 0x1);

    return qic_ret;
}

int qic_get_IR(unsigned char *als_mode, unsigned char *ir_status)
{
    int qic_ret = 0;
    unsigned char als_mode_and_ir_status;

#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif

    qic_ret = QicGetIR(als_mode_and_ir_status);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);

    if(!qic_ret){
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_get_IR success motor status=%d\n", *ir_status);
    }
    else{
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_get_IR failed\n");
        qic_ret=1;
    }

    *als_mode = (als_mode_and_ir_status>>1) & 0x1;
    *ir_status = als_mode_and_ir_status & 0x1;

    return qic_ret;
}

// Sensor Information
int qic_get_Sensor_AEInfo(unsigned short *AE_statistic)
{
    int qic_ret=0;

#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif

    qic_ret=	QicGetSensorAEInfo(AE_statistic);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
    if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicGetSensorAEInfo success\n");

    return qic_ret;
}

// LED mode
int qic_change_LED_Mode( unsigned char mode)
{
    int ret = 0;
    int qic_ret = 0;

#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif

    qic_ret = QicSetLedMode(mode);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);

#ifdef DEBUG_LOG
    unsigned char led_mode;
    led_mode = 0;
    qic_ret = QicGetLedMode(&led_mode);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
    if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicGetLedMode success,  LED_Mode=%d, \n", led_mode);
#endif

    /* success */
    return ret;
}

int qic_change_LED_Brightness( unsigned char brightness)
{
    int ret = 0;
    int qic_ret = 0;

#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif

    qic_ret =  QicSetLEDBrightness(brightness);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);

#ifdef DEBUG_LOG
    unsigned char led_brightness;
    led_brightness = 0;
    qic_ret = QicGetLEDBrightness(&led_brightness);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
    if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicGetLEDBrightness success,  LED_brightness=%d, \n", led_brightness);
#endif

    /* success */
    return ret;
}

// Audio Function control
int qic_change_NR_mode(unsigned char onoff)
{
    int qic_ret;

    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL))
    {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }

    qic_ret = QicSetANRStatus(onoff);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
    if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicSetANRStatus success...\n");

#ifdef DEBUG_LOG
    unsigned char mode=0;
    qic_ret = QicGetANRStatus(&mode);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
    if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicGetANRStatus success,  OnOff=%d ..., \n", mode);
#endif

    return qic_ret;
}

int qic_change_EQ_mode(unsigned char onoff, unsigned char type)
{
    int qic_ret;

    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL))
    {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }

    qic_ret = QicSetEQStatus(onoff, type);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
    if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicSetEQStatus success...\n");

#ifdef DEBUG_LOG
	unsigned char mode=0, mode_type=0;
    qic_ret = QicGetEQStatus(&mode, &mode_type);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
    if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicGetEQStatus success, OnOff=%d, Type=%d ..., \n", mode, mode_type);
#endif

    return qic_ret;
}

int qic_change_BF_mode(unsigned char onoff)
{
    int qic_ret;

    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL))
    {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }

    qic_ret = QicSetBFStatus(onoff);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
    if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicSetBFStatus success...\n");

#ifdef DEBUG_LOG
    unsigned char mode=0;
    qic_ret = QicGetBFStatus(&mode);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
    if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicGetBFStatus success,  OnOff=%d ..., \n", mode);
#endif

    return qic_ret;
}

// Audio ADC mode control
int qic_change_ADC_mode(ADCMode_t mode)
{
    int qic_ret;

    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL))
    {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }

    qic_ret = QicSetADCMode(mode);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
    if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicSetADCMode success...\n");

#ifdef DEBUG_LOG
	ADCMode_t cur_mode;
    qic_ret = QicGetADCMode(&cur_mode);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
    if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicGetADCMode success,  current mode=%d ..., \n", cur_mode);
#endif

    return qic_ret;
}

// H/V flip
int qic_change_flip(unsigned int dev_id, unsigned char flip)
{
    unsigned int index;
    int ret = 0;
    unsigned char h_flip, v_flip;
    int qic_ret = 0;

#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif

    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {
            {
                h_flip = flip & H_FLIP;
                v_flip = (flip & V_FLIP)>>1;

                QicChangeFD(dev_pt->cam[index].fd);
                qic_ret = QicSetFlipMode(v_flip, h_flip);
                LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);

#ifdef DEBUG_LOG
                h_flip = 0;
                v_flip = 0;
                qic_ret = QicGetFlipMode(&v_flip, &h_flip);
                LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
                if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicGetFlipMode success,  H_FLIP=%s, V_FLIP=%s\n", h_flip?"on":"off", v_flip?"on":"off");
#endif
            }
        }
    }

    /* success */
    return ret;
}

int qic_change_pan_tilt(unsigned int dev_id, signed short pan, signed short tilt)
{
    unsigned int index;
    int ret = 0;
    int qic_ret = 0;
    //	unsigned char fw_api_verson;
#ifdef DEBUG_LOG
    signed short got_pan, got_tilt;
#endif

#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC library config is not committed\n");
        return 1;
    }
#endif
    if(pan<0)
    {
        pan=(~pan)+0x8001;
    }

    if(tilt<0)
    {
        tilt=(~tilt)+0x8001;
    }
    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {

            QicChangeFD(dev_pt->cam[index].fd);

            qic_ret = QicSetPanTilt( pan,tilt);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "Set: Pan=%x, Tilt=%x\n", pan,tilt);
#ifdef DEBUG_LOG

            qic_ret = QicGetPanTilt(&got_pan,&got_tilt);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "Get: Pan=%x, Tilt=%x\n", got_pan,got_tilt);
#endif
        }
    }

    /* success */
    return ret;
}


int qic_change_mirror_mode(int dev_id, unsigned char mode)
{
    unsigned char mirror_mode;
    int qic_ret = 0;
    unsigned int index;

#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif
    mode=mode&0x01;

    for (index = 0; index < dev_pt->num_devices; index++) {
        if(dev_pt->cam[index].dev_id & dev_id){
            QicChangeFD(dev_pt->cam[index].fd);
            qic_ret = QicGetMirror(&mirror_mode);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicGetMirror success,  Mirror_Mode=%d, \n", mirror_mode);

            if(dev_pt->cam[index].format == V4L2_PIX_FMT_MJPEG&&dev_pt->cam[index].is_encoding_video) {
                mode= ((mode << 1) & 0x03)|(mirror_mode&0x01);
            }else
                mode= mode|(mirror_mode&0x02);

            qic_ret = QicSetMirror(mode);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicSetMirror success mode=%d\n",mode);

#ifdef DEBUG_LOG
            mirror_mode = 0;
            qic_ret = QicGetMirror(&mirror_mode);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicGetMirror success,  Mirror_Mode=%d, \n", mirror_mode);
#endif
        }
    }
    /* success */
    return qic_ret;
}

int qic_set_cropped_image(unsigned int dev_id,unsigned int crop_switch,CROPRECT crop_window)
{
    unsigned int index;
    int ret = 0;
    int qic_ret = 0;
    CROPRECT window;
    //	int boundary_x, boundary_y;

#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif

    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {

            if (dev_pt->cam[index].format == V4L2_PIX_FMT_YUYV) {

                /*QIC1816 can't support width of cropped window over 360 */
                if(crop_window.Wx > 360 && crop_switch ){
                    ret=1;
                    LOG_PRINT(debug_str, DEBUG_ERROR, "out of range, can't set cropped image\n");
                    goto ERR_SETTING;
                }

                QicChangeFD(dev_pt->cam[index].fd);

                if(crop_switch) {
                    qic_ret = QicSetCropped(1, crop_window);
                }
                else {
                    window.x=0;
                    window.y=0;
                    window.Wx=0;
                    window.Wy=0;
                    qic_ret = QicSetCropped(0, window);
                }
                LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);

                if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "corpped enable= %d\n", crop_switch);

                if(qic_ret) {
                    ret=1;
                    LOG_PRINT(debug_str, DEBUG_ERROR, "qic_set_cropped_image failed!!\n");
                }
            }
            else { /* N/A */
                LOG_PRINT(debug_str, DEBUG_ERROR, "cam %s isn't Preview device, can't set cropped image\n", dev_pt->cam[index].dev_name);
                ret = 1;
                continue;
            }
        }
    }

ERR_SETTING:
    LOG_PRINT(debug_str, DEBUG_INFO,"ret=%d\n",ret);
    return ret;
}

//  Utilities functions
int qic_get_fd_from_devid(unsigned int dev_id)
{
    unsigned int index;
    int fd = 0;

    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id)
        {
            fd = dev_pt->cam[index].fd;
            break;
            /*return dev_pt->cam[index].fd;*/
        }
    }
    return !fd ? -1 : fd;
}

char* qic_print_config_param(unsigned int dev_id)
{
    static char report[4096];
    unsigned int index;
    int print_offset = 0;
    unsigned int buf_index;

    memset (report, 0, sizeof(report));

    for ( index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) { /* setup the report and print */
            /* name */
            print_offset += snprintf(report+print_offset, sizeof(report)-print_offset, "\ndevice %s config state:", dev_pt->cam[index].dev_name);

            /* basic parameters */
            print_offset += snprintf(report+print_offset, sizeof(report)-print_offset, "\n\t dev_id: 0x%X", dev_pt->cam[index].dev_id);
            print_offset += snprintf(report+print_offset, sizeof(report)-print_offset, "\n\t fd: %d", dev_pt->cam[index].fd);
            print_offset += snprintf(report+print_offset, sizeof(report)-print_offset, "\n\t format: "FOURCC_FORMAT,FOURCC_ARGS(dev_pt->cam[index].format));
            print_offset += snprintf(report+print_offset, sizeof(report)-print_offset, "\n\t resolution : %dx%d", dev_pt->cam[index].width, dev_pt->cam[index].height);

            /* MJPEG parameters */
            if (dev_pt->cam[index].format == V4L2_PIX_FMT_MJPEG) {
                if (dev_pt->cam[index].is_bind == 0)
                    print_offset += snprintf(report+print_offset, sizeof(report)-print_offset, "\n\t framerate: %dfps", dev_pt->cam[index].framerate);
                print_offset += snprintf(report+print_offset, sizeof(report)-print_offset, "\n\t is bind: %s", dev_pt->cam[index].is_bind== 1?"Yes":"No");
                if(dev_pt->cam[index].is_encoding_video==1)
                {
                    print_offset += snprintf(report+print_offset, sizeof(report)-print_offset, "\n\t frame rate interval: %dfps", dev_pt->cam[index].frame_interval);
                    print_offset += snprintf(report+print_offset, sizeof(report)-print_offset, "\n\t bitrate: %dbps", dev_pt->cam[index].bitrate);
                    print_offset += snprintf(report+print_offset, sizeof(report)-print_offset, "\n\t key frame rate interval: %dfps", dev_pt->cam[index].key_frame_interval);
                }
            }
            /* YUV parameters */
            else if (dev_pt->cam[index].format == V4L2_PIX_FMT_YUYV) {
                if (dev_pt->cam[index].is_bind == 0)
                    print_offset += snprintf(report+print_offset, sizeof(report)-print_offset, "\n\t framerate: %dfps", dev_pt->cam[index].framerate);
                print_offset += snprintf(report+print_offset, sizeof(report)-print_offset, "\n\t is bind: %s", dev_pt->cam[index].is_bind== 1?"Yes":"No");
            }
            else /* error setting */
                continue;

            /* system parameters */
            print_offset += snprintf(report+print_offset, sizeof(report)-print_offset, "\n\t capturing? %s", dev_pt->cam[index].is_on == 1?"Yes":"No");
            print_offset += snprintf(report+print_offset, sizeof(report)-print_offset, "\n\t buffers start addr: %d", dev_pt->cam[index].buffer_start_address);
            print_offset += snprintf(report+print_offset, sizeof(report)-print_offset, "\n\t number of MMAP buffers: %d", dev_pt->cam[index].num_mmap_buffer);

            for (buf_index = 0; buf_index < dev_pt->cam[index].num_mmap_buffer; buf_index++) {
                if (dev_pt->cam[index].buffers[buf_index].start != NULL)
                    print_offset += snprintf(report+print_offset, sizeof(report)-print_offset, "\n\t\t buffer #%d length %d at %p", \
                                             buf_index, \
                                             (unsigned int)dev_pt->cam[index].buffers[buf_index].length, \
                                             dev_pt->cam[index].buffers[buf_index].start);
            }
        }
    }

    print_offset += snprintf(report+print_offset, sizeof(report)-print_offset, "\n");
    //	LOG_PRINT(debug_str, DEBUG_DETAIL, "qic_print_config_param - OUT\n");

    return report;
}

int qic_set_lock_steam_control( unsigned char lock)
{
    int qic_ret = 0;

#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif

    qic_ret = QicSetLockStream(lock);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);

    if(!qic_ret){
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_Set_lock_steam_control success\n");
    }
    else{
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_Set_lock_steam_control failed\n");
        qic_ret=1;
    }

    return qic_ret;
}

int qic_get_control_setting(int dev_id, sqicV4L2 *camerav4l2)
{
    struct v4l2_queryctrl queryctrl;
    struct v4l2_querymenu querymenu;
    struct v4l2_control   control_s;
    struct v4l2_input*    getinput;
    unsigned int ret=0;
    unsigned int index=0;
    //Name of the device
    signed int menu_index;


    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id){
            getinput=(struct v4l2_input *) calloc(1, sizeof(struct v4l2_input));
            memset(getinput, 0, sizeof(struct v4l2_input));
            getinput->index=0;
            ret= ioctl(dev_pt->cam[index].fd,VIDIOC_ENUMINPUT , getinput);
            //  printf (" Available controls of device '%s' (Type 1=Integer 2=Boolean 3=Menu 4=Button 5=Integer64 6=class)\n",getinput->name);
            LOG_PRINT(debug_str, DEBUG_INFO, " Available controls of device '%s' (Type 1=Integer 2=Boolean 3=Menu 4=Button 5=Integer64 6=class)\n",getinput->name);
            //subroutine to read menu items of controls with type 3
            void enumerate_menu (void) {
                LOG_PRINT(debug_str, DEBUG_INFO,"  Menu items:\n");
                memset (&querymenu, 0, sizeof (querymenu));
                querymenu.id = queryctrl.id;
                menu_index=(unsigned int)querymenu.index;
                for (menu_index= queryctrl.minimum;
                     menu_index <= queryctrl.maximum;
                     menu_index++) {
                    if (0 == ioctl (dev_pt->cam[index].fd, VIDIOC_QUERYMENU, &querymenu)) {
                        LOG_PRINT(debug_str, DEBUG_INFO,"  index:%d name:%s\n", menu_index, querymenu.name);
                        TIME_DELAY(1);
                    } else {
                        printf ("error getting control menu");
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
                        camerav4l2->Brightness.now=control_s.value;
                    }

                    if(V4L2_CID_CONTRAST==queryctrl.id)
                    {
                        camerav4l2->Contrast.def=queryctrl.default_value;
                        camerav4l2->Contrast.max=queryctrl.maximum;
                        camerav4l2->Contrast.min=queryctrl.minimum;
                        camerav4l2->Contrast.now=control_s.value;
                    }

                    if(V4L2_CID_SATURATION==queryctrl.id)
                    {
                        camerav4l2->Saturation.def=queryctrl.default_value;
                        camerav4l2->Saturation.max=queryctrl.maximum;
                        camerav4l2->Saturation.min=queryctrl.minimum;
                        camerav4l2->Saturation.now=control_s.value;
                    }

                    if(V4L2_CID_HUE==queryctrl.id)
                    {
                        camerav4l2->Hue.def=queryctrl.default_value;
                        camerav4l2->Hue.max=queryctrl.maximum;
                        camerav4l2->Hue.min=queryctrl.minimum;
                        camerav4l2->Hue.now=control_s.value;
                    }

                    if(V4L2_CID_GAMMA==queryctrl.id)
                    {
                        camerav4l2->Gamma.def=queryctrl.default_value;
                        camerav4l2->Gamma.max=queryctrl.maximum;
                        camerav4l2->Gamma.min=queryctrl.minimum;
                        camerav4l2->Gamma.now=control_s.value;
                    }

                    if(V4L2_CID_GAIN==queryctrl.id)
                    {
                        camerav4l2->Gain.def=queryctrl.default_value;
                        camerav4l2->Gain.max=queryctrl.maximum;
                        camerav4l2->Gain.min=queryctrl.minimum;
                        camerav4l2->Gain.now=control_s.value;
                    }


                    if(V4L2_CID_POWER_LINE_FREQUENCY==queryctrl.id)
                    {
                        camerav4l2->Plf.def=queryctrl.default_value;
                        camerav4l2->Plf.max=queryctrl.maximum;
                        camerav4l2->Plf.min=queryctrl.minimum;
                        camerav4l2->Plf.now=control_s.value;
                    }

                    if(V4L2_CID_WHITE_BALANCE_TEMPERATURE==queryctrl.id)
                    {
                        camerav4l2->WB.def=queryctrl.default_value;
                        camerav4l2->WB.max=queryctrl.maximum;
                        camerav4l2->WB.min=queryctrl.minimum;
                        camerav4l2->WB.now=control_s.value;
                    }

                    if(V4L2_CID_SHARPNESS==queryctrl.id)
                    {
                        camerav4l2->Sharpness.def=queryctrl.default_value;
                        camerav4l2->Sharpness.max=queryctrl.maximum;
                        camerav4l2->Sharpness.min=queryctrl.minimum;
                        camerav4l2->Sharpness.now=control_s.value;
                    }

                    if(V4L2_CID_BACKLIGHT_COMPENSATION==queryctrl.id)
                    {
                        camerav4l2->BC.def=queryctrl.default_value;
                        camerav4l2->BC.max=queryctrl.maximum;
                        camerav4l2->BC.min=queryctrl.minimum;
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
                        camerav4l2->Exposure.now=control_s.value;
                    }
                    if(V4L2_CID_EXPOSURE_AUTO_PRIORITY==queryctrl.id)
                    {
                        camerav4l2->E_priority.def=queryctrl.default_value;
                        camerav4l2->E_priority.max=queryctrl.maximum;
                        camerav4l2->E_priority.min=queryctrl.minimum;
                        camerav4l2->E_priority.now=control_s.value;
                    }
                    if(V4L2_CID_FOCUS_ABSOLUTE==queryctrl.id)
                    {
                        camerav4l2->Focus.def=queryctrl.default_value;
                        camerav4l2->Focus.max=queryctrl.maximum;
                        camerav4l2->Focus.min=queryctrl.minimum;
                        camerav4l2->Focus.now=control_s.value;
                    }
                    if(V4L2_CID_PAN_ABSOLUTE==queryctrl.id)
                    {
                        camerav4l2->Pan.def=queryctrl.default_value;
                        camerav4l2->Pan.max=queryctrl.maximum/queryctrl.step;
                        camerav4l2->Pan.min=queryctrl.minimum/queryctrl.step;
                        camerav4l2->Pan.now=control_s.value/queryctrl.step;
                    }
                    if(V4L2_CID_TILT_ABSOLUTE==queryctrl.id)
                    {
                        camerav4l2->Tilt.def=queryctrl.default_value;
                        camerav4l2->Tilt.max=queryctrl.maximum/queryctrl.step;
                        camerav4l2->Tilt.min=queryctrl.minimum/queryctrl.step;
                        camerav4l2->Tilt.now=control_s.value/queryctrl.step;
                    }

                    if(V4L2_CID_ZOOM_ABSOLUTE==queryctrl.id)
                    {
                        camerav4l2->Zoom.def=queryctrl.default_value;
                        camerav4l2->Zoom.max=queryctrl.maximum;
                        camerav4l2->Zoom.min=queryctrl.minimum;
                        camerav4l2->Zoom.now=control_s.value;
                    }

                }
                else {
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
                }
                else {
                    if (errno == EINVAL)
                        break;
                    perror ("error getting private base controls");
                    goto fatal_controls;
                }
            }

        }
    }

    return 0;
fatal_controls:
    return -1;
}

