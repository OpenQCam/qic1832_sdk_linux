/*
--       This software proprietary belong to Quanta computer Inc. and may be used        --
--        only as expressly authorized agreement from          --
--                            Quanta Computer Inc..                             --
--                   (C) COPYRIGHT 2010 Quanta Computer Inc.                    --
--                            ALL RIGHTS RESERVED                             --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
*/

#include "../qic_include/qic_api_video.h"

// Common part
extern char debug_str[1024];
extern char debug_xuctrl_str[512];
extern int config_is_commit;
extern qic_module *dev_pt;
// End of common part

#if defined(QIC1822)&& defined(QIC_SIMULCAST_API)
int qic_config_codec_EU(unsigned int dev_id,EuExSelectCodec_t tCodec)
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
        if (dev_pt->cam[index].dev_id & dev_id) {
            dev_pt->cam[index].codec_type=tCodec;

            QicChangeFD(dev_pt->cam[index].fd);
            ret=QicEuExSetSelectCodec(tCodec);

            if (dev_pt->cam[index].is_demux){
                if(tCodec==CODEC_VP8||tCodec==CODEC_VP8_SIMULCAST){
                    ret=QicMmioWrite(0x6F00009C, 1); //Enable VP8 debug mode
                }else if(tCodec==CODEC_H264||tCodec==CODEC_H264_SIMULCAST){
                    ret=QicMmioWrite(0x6F0000A0, 1); //Enable H264 debug mode
                }
                if(ret){
                    LOG_PRINT(debug_str, DEBUG_INFO, "Enable Encoding stream debug mode Error...\n");
                }else{
                    LOG_PRINT(debug_str, DEBUG_INFO, "Enable Encoding stream debug mode success...\n");
                }
            }else{
                if(tCodec==CODEC_VP8||tCodec==CODEC_VP8_SIMULCAST){
                    ret=QicMmioWrite(0x6F00009C, 0); //Disable VP8 debug mode
                }else if(tCodec==CODEC_H264||tCodec==CODEC_H264_SIMULCAST){
                    ret=QicMmioWrite(0x6F0000A0, 0); //Disable H264 debug mode
                }
                if(ret){
                    LOG_PRINT(debug_str, DEBUG_INFO, "Disable Encoding stream debug mode Error...\n");
                }else{
                    LOG_PRINT(debug_str, DEBUG_INFO, "Disable Encoding stream debug mode success...\n");
                }
            }
#ifdef DEBUG_LOG
            EuExSelectCodec_t bCodec;
            ret=QicEuExGetSelectCodec(&bCodec);
            printf("QicEuExGetSelectCodec=======%d\n",bCodec);
#endif
        }
    }
    return ret;
}

int qic_simulcast_config_codec_EU(unsigned int dev_id,EuExSelectCodec_t tCodec,simulcast_configs_t config_setting)
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
        if (dev_pt->cam[index].dev_id & dev_id) {
            dev_pt->cam[index].codec_type=tCodec;
            dev_pt->cam[index].simulcast_configs_setting=config_setting;
            QicChangeFD(dev_pt->cam[index].fd);

            ret=QicEuExSetSelectCodec(tCodec);
            if(config_setting.config_all!=0){
                ret= QicEuSetSelectLayer(SIMULCAST_STREAM0);
                ret=QicEuSetVideoResolution(config_setting.configs[0].width,config_setting.configs[0].height);
                ret=QicEuSetMinimumFrameInterval(config_setting.configs[0].frame_interval);
                ret=QicEuSetAverageBitrateControl(config_setting.configs[0].bitrate);
                ret=QicEuSetSynchronizationAndLongTermReferenceFrame(1, config_setting.configs[0].key_frame_interval, 0);

                ret= QicEuSetSelectLayer(SIMULCAST_STREAM1);
                ret=QicEuSetVideoResolution(config_setting.configs[1].width,config_setting.configs[1].height);
                ret=QicEuSetMinimumFrameInterval(config_setting.configs[1].frame_interval);
                ret=QicEuSetAverageBitrateControl(config_setting.configs[1].bitrate);
                ret=QicEuSetSynchronizationAndLongTermReferenceFrame(1, config_setting.configs[1].key_frame_interval, 0);

                ret= QicEuSetSelectLayer(SIMULCAST_STREAM2);
                ret=QicEuSetVideoResolution(config_setting.configs[2].width,config_setting.configs[2].height);
                ret=QicEuSetMinimumFrameInterval(config_setting.configs[2].frame_interval);
                ret=QicEuSetAverageBitrateControl(config_setting.configs[2].bitrate);
                ret=QicEuSetSynchronizationAndLongTermReferenceFrame(1, config_setting.configs[2].key_frame_interval, 0);

                ret= QicEuSetSelectLayer(SIMULCAST_STREAM3);
                ret=QicEuSetVideoResolution(config_setting.configs[3].width,config_setting.configs[3].height);
                ret=QicEuSetMinimumFrameInterval(config_setting.configs[3].frame_interval);
                ret=QicEuSetAverageBitrateControl(config_setting.configs[3].bitrate);
                ret=QicEuSetSynchronizationAndLongTermReferenceFrame(1, config_setting.configs[3].key_frame_interval, 0);
            }else{
                ret= QicEuSetSelectLayer(SIMULCAST_STREAMALL);
                ret=QicEuSetVideoResolution(config_setting.configs[0].width,config_setting.configs[0].height);
                ret=QicEuSetMinimumFrameInterval(config_setting.configs[0].frame_interval);
                ret=QicEuSetAverageBitrateControl(config_setting.configs[0].bitrate);
                ret=QicEuSetSynchronizationAndLongTermReferenceFrame(1, config_setting.configs[0].key_frame_interval, 0);
            }
#ifdef DEBUG_LOG
            unsigned short wWidth, wHeight;
            unsigned int dwFrameInterval;
            unsigned int dwAverageBitRate;
            unsigned char bSyncFrameType;
            unsigned short wSyncFrameInterval;
            unsigned char bGradualDecoderRefresh;
            EuExSelectCodec_t bCodec;

            QicEuExGetSelectCodec(&bCodec);
            printf("QicEuExGetSelectCodec=======%d\n",bCodec);

            QicEuGetVideoResolution(&wWidth,&wHeight);
            printf("QicEuGetVideoResolution=======%d, %d\n",wWidth,wHeight);

            QicEuGetMinimumFrameInterval(&dwFrameInterval);
            printf("QicEuGetMinimumFrameInterval=======%u\n",dwFrameInterval);

            QicEuGetAverageBitrateControl(&dwAverageBitRate);
            printf("QicEuGetAverageBitrateControl=======%u\n",dwFrameInterval);

            QicEuGetSynchronizationAndLongTermReferenceFrame(&bSyncFrameType, &wSyncFrameInterval, &bGradualDecoderRefresh);
            printf("QicEuGetSynchronizationAndLongTermReferenceFrame=======%d, %d, %d\n",bSyncFrameType,wSyncFrameInterval,bGradualDecoderRefresh);
#endif
        }
    }
    return ret;
}

int qic_set_temporal_layer_number_EU(unsigned int dev_id,unsigned short stream_id,unsigned char temp_layer_num)
{
    unsigned int index;
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
            QicChangeFD(dev_pt->cam[index].fd);
            qic_ret=QicEuSetSelectLayer(stream_id);
            qic_ret= QicEuExSetTsvc( temp_layer_num);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuExSetTsvc success \n");
#ifdef DEBUG_LOG
            unsigned char number;
            qic_ret=QicEuExGetTsvc(&number);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuExGetTsvc=======%d\n",number);
#endif
        }
    }
    /* success */
    return qic_ret;
}

int qic_change_bitrate_EU(unsigned int dev_id,unsigned short stream_id, unsigned int ubitrate)
{
    unsigned int index, CPBsize;
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
            QicChangeFD(dev_pt->cam[index].fd);
            printf("%d, %d, %d, %d\n",dev_pt->cam[index].simulcast_configs_setting.configs[0].width,dev_pt->cam[index].simulcast_configs_setting.configs[1].width
                   ,dev_pt->cam[index].simulcast_configs_setting.configs[2].width,dev_pt->cam[index].simulcast_configs_setting.configs[3].width);
            qic_ret=QicEuSetSelectLayer(stream_id);
            if(qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetSelectLayer failed \n");

            dev_pt->cam[index].bitrate=ubitrate;
            qic_ret=QicEuSetAverageBitrateControl(ubitrate);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetAverageBitrateControl success \n");

            CPBsize = ubitrate >>1;
            qic_ret=QicEuSetCpbSizeControl(CPBsize);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetCpbSizeControl success \n");
#ifdef DEBUG_LOG
            unsigned int dwAverageBitRate;
            qic_ret=QicEuGetAverageBitrateControl(&dwAverageBitRate);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuGetAverageBitrateControl=======%u\n",dwAverageBitRate);
#endif
        }
    }
    /* success */
    return qic_ret;
}

int qic_change_ErrorResiliency_EU(unsigned int dev_id, unsigned short errorResiliency)
{
    unsigned int index;
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
            QicChangeFD(dev_pt->cam[index].fd);
            qic_ret=QicEuSetErrorResiliency(errorResiliency);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetErrorResiliency success\n");

#ifdef DEBUG_LOG
            unsigned short bmErrorResiliencyFeatures;
            QicEuGetErrorResiliency(&bmErrorResiliencyFeatures);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuGetErrorResiliency=======%d\n",bmErrorResiliencyFeatures);
#endif
        }
    }
    /* success */
    return qic_ret;
}

int qic_change_resolution_EU(unsigned int dev_id,unsigned short stream_id, unsigned short width, unsigned short height)
{
    unsigned int index;
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
            QicChangeFD(dev_pt->cam[index].fd);
            qic_ret=QicEuSetSelectLayer(stream_id);
            if(qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetSelectLayer failed \n");

            qic_ret=QicEuSetVideoResolution(width,height);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetVideoResolution success \n");
#ifdef DEBUG_LOG
            unsigned short wWidth, wHeight;
            QicEuGetVideoResolution(&wWidth,&wHeight);
            printf("QicEuGetVideoResolution=======%d, %d\n",wWidth,wHeight);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuGetVideoResolution=======%d, %d\n",wWidth,wHeight);
#endif
        }
    }
    /* success */
    return qic_ret;
}

int qic_start_stop_layer_EU(unsigned int dev_id,unsigned short stream_layer,unsigned char on)
{
    unsigned int index;
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
            QicChangeFD(dev_pt->cam[index].fd);
            qic_ret=QicEuSetSelectLayer(stream_layer);
            if(qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetSelectLayer failed \n");

            qic_ret=QicEuSetStartOrStopLayer(on);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetStartOrStopLayer success\n");
#ifdef DEBUG_LOG
            unsigned char bUpdate;
            qic_ret=QicEuGetStartOrStopLayer(&bUpdate);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuGetStartOrStopLayer=======%u\n",bUpdate);
#endif
        }
    }
    /* success */
    return qic_ret;
}

int qic_change_frame_interval_EU(unsigned int dev_id,unsigned short stream_id,unsigned int uframe_interval)
{
    unsigned int index;
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
            QicChangeFD(dev_pt->cam[index].fd);
            qic_ret=QicEuSetSelectLayer(stream_id);
            if(qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetSelectLayer failed \n");

            dev_pt->cam[index].frame_interval=uframe_interval;

            qic_ret=QicEuSetMinimumFrameInterval(uframe_interval);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetMinimumFrameInterval success \n");
#ifdef DEBUG_LOG
            unsigned int dwFrameInterval;
            qic_ret=QicEuGetMinimumFrameInterval(&dwFrameInterval);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetMinimumFrameInterval=======%u\n",dwFrameInterval);
#endif
        }
    }
    /* success */
    return qic_ret;
}

int qic_change_CPB_size_EU(unsigned int dev_id,unsigned short stream_layer,unsigned int CPBsize)
{
    unsigned int index;
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
            QicChangeFD(dev_pt->cam[index].fd);
            qic_ret=QicEuSetSelectLayer(stream_layer);
            if(qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetSelectLayer failed \n");

            qic_ret=QicEuSetCpbSizeControl(CPBsize);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetCpbSizeControl success \n");
#ifdef DEBUG_LOG
            unsigned int dwCPBsize;
            qic_ret=QicEuGetCpbSizeControl(&dwCPBsize);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuGetCpbSizeControl=======%u\n",dwCPBsize);
#endif
        }
    }
    /* success */
    return qic_ret;
}

int qic_generate_key_frame_EU(unsigned int dev_id, unsigned short stream_id,unsigned char cSyncFrameType, unsigned short sSyncFrameInterval, unsigned char cGradualDecoderRefresh)
{
    unsigned int index;
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
            QicChangeFD(dev_pt->cam[index].fd);
            qic_ret=QicEuSetSelectLayer(stream_id);
            if(qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetSelectLayer failed \n");

            dev_pt->cam[index].key_frame_interval=sSyncFrameInterval;

            qic_ret=QicEuSetSynchronizationAndLongTermReferenceFrame(cSyncFrameType, sSyncFrameInterval, cGradualDecoderRefresh);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetSynchronizationAndLongTermReferenceFrame success \n");
#ifdef DEBUG_LOG
            unsigned char bSyncFrameType;
            unsigned short wSyncFrameInterval;
            unsigned char bGradualDecoderRefresh;
            qic_ret=QicEuGetSynchronizationAndLongTermReferenceFrame(&bSyncFrameType, &wSyncFrameInterval, &bGradualDecoderRefresh);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuGetSynchronizationAndLongTermReferenceFrame==>bSyncFrameType=%d, wSyncFrameInterval=%d, bGradualDecoderRefresh=%d\n",bSyncFrameType
                                   ,wSyncFrameInterval, bGradualDecoderRefresh);
#endif
        }
    }
    /* success */
    return qic_ret;
}

int qic_change_profile_toolset_EU(unsigned int dev_id, unsigned short stream_id, unsigned short wProfile, unsigned char bmSettings)
{
    unsigned int index;
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
            QicChangeFD(dev_pt->cam[index].fd);
            qic_ret=QicEuSetSelectLayer(stream_id);
            if(qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetSelectLayer failed \n");

            qic_ret=QicEuSetProfileAndToolset(wProfile,  0, bmSettings);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetProfileAndToolset success \n");
#ifdef DEBUG_LOG
            unsigned short wProfile;
            unsigned short wConstrainedToolset;
            unsigned char bmSettings;

            qic_ret=QicEuGetProfileAndToolset(&wProfile, &wConstrainedToolset, &bmSettings);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuGetProfileAndToolset==>wProfile=%d, wConstrainedToolset=%d, bmSettings=%d\n",wProfile
                                   ,wConstrainedToolset, bmSettings);
#endif
        }
    }
    /* success */
    return qic_ret;
}

int qic_change_slice_size_EU(unsigned int dev_id,unsigned short stream_id, unsigned short wSliceConfigSetting)
{
    unsigned int index;
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
            QicChangeFD(dev_pt->cam[index].fd);
            qic_ret=QicEuSetSelectLayer(stream_id);
            if(qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetSelectLayer failed \n");

            qic_ret=QicEuSetSliceMode(0x03, wSliceConfigSetting);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetSliceMode success \n");

#ifdef DEBUG_LOG
            unsigned short wSliceMode;
            unsigned short wSliceConfigSetting;

            qic_ret=QicEuGetSliceMode(&wSliceMode, &wSliceConfigSetting);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuGetSliceMode==>wSliceMode=%d, wSliceConfigSetting=%d\n",wSliceMode
                                   ,wSliceConfigSetting);
#endif
        }
    }
    /* success */
    return qic_ret;
}

int qic_change_rete_control_mode_EU(unsigned int dev_id,unsigned short stream_id, unsigned char bRateControlMode)
{
    unsigned int index;
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
            QicChangeFD(dev_pt->cam[index].fd);
            qic_ret=QicEuSetSelectLayer(stream_id);
            if(qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetSelectLayer failed \n");

            qic_ret=QicEuSetRateControlMode( bRateControlMode);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetRateControlMode success \n");
#ifdef DEBUG_LOG
            unsigned char bRateControlMode;

            qic_ret=QicEuGetRateControlMode(&bRateControlMode);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuGetRateControlMode==>bRateControlMode=%d\n",bRateControlMode);
#endif
        }
    }
    /* success */
    return qic_ret;
}

int qic_change_QP_EU(unsigned int dev_id ,unsigned short stream_layer, unsigned short QP_I, unsigned short QP_P, unsigned short QP_B)
{
    unsigned int index;
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
            QicChangeFD(dev_pt->cam[index].fd);
            qic_ret=QicEuSetSelectLayer(stream_layer);
            if(qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetSelectLayer failed \n");

            qic_ret=QicEuSetQuantizationParameter(QP_I, QP_P, QP_B);

            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetQuantizationParameter success \n");
#ifdef DEBUG_LOG
            unsigned short QpPrime_I;
            unsigned short QpPrime_P;
            unsigned short QpPrime_B;

            qic_ret=QicEuGetQuantizationParameter(&QpPrime_I, &QpPrime_P, &QpPrime_B);

            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuGetQuantizationParameter==>QpPrime_I=%d,QpPrime_P=%d,QpPrime_B=%d\n",QpPrime_I
                                   ,QpPrime_P,QpPrime_B);
#endif
        }
    }
    /* success */
    return qic_ret;
}

int qic_change_level_EU(unsigned int dev_id ,unsigned short stream_id,unsigned char bLevelIDC)
{
    unsigned int index;
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
            QicChangeFD(dev_pt->cam[index].fd);
            qic_ret=QicEuSetSelectLayer(stream_id);
            if(qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetSelectLayer failed \n");

            qic_ret=QicEuSetLevelIdc(bLevelIDC);

            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetLevelIdc success \n");

#ifdef DEBUG_LOG
            unsigned char LevelIDC;

            qic_ret=QicEuGetLevelIdc(&LevelIDC);

            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuGetLevelIdc==>LevelIDC=%d\n",LevelIDC);
#endif
        }
    }
    /* success */
    return qic_ret;
}

int qic_change_QP_range_EU(unsigned int dev_id ,unsigned short stream_id,unsigned char bMinQp,unsigned char bMaxQp)
{
    unsigned int index;
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
            QicChangeFD(dev_pt->cam[index].fd);
            qic_ret=QicEuSetSelectLayer(stream_id);
            if(qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetSelectLayer failed \n");

            qic_ret=QicEuSetQpRange(bMinQp, bMaxQp);

            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetQpRange success \n");
#ifdef DEBUG_LOG
            unsigned char MinQp;
            unsigned char MaxQp;
            qic_ret=QicEuGetQpRange(&MinQp, &MaxQp);

            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicEuGetQpRange==>MinQp=%d,MaxQp=%d\n",MinQp,MaxQp);
#endif
        }
    }
    /* success */
    return qic_ret;
}
#endif


#ifdef QIC_MD_API
int qic_set_burst_mode_peak_bitrate(unsigned int dev_id, unsigned int stream_id, unsigned int peakBitrate)
{
    int ret=0;
    unsigned int index;
    unsigned int bitrate;

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

            ret=QicEuSetSelectLayer(stream_id);
            if(ret){
                LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetSelectLayer failed \n");
                return 1;
            }
            ret=QicSetPeakBitrate(peakBitrate);

            if(ret){
                LOG_PRINT(debug_str, DEBUG_ERROR, "QicSetPeakBitrate failed\n");
                return 1;
            }
            ret=QicGetPeakBitrate(&bitrate);

            if(ret){
                LOG_PRINT(debug_str, DEBUG_ERROR, "QicGetPeakBitrate failed\n");
            }
        }
    }
    return ret;
}

int qic_get_burst_mode_peak_bitrate(unsigned int dev_id, unsigned short stream_id, unsigned int *peakBitrate)
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

            ret=QicEuSetSelectLayer(stream_id);
            if(ret){
                LOG_PRINT(debug_str, DEBUG_INFO, "QicEuSetSelectLayer failed \n");
                return 1;
            }
            ret=QicGetPeakBitrate(peakBitrate);

            if(ret){
                LOG_PRINT(debug_str, DEBUG_ERROR, "QicGetPeakBitrate failed\n");
            }
        }
    }
    return ret;
}
#endif

// Get H.264/VP8 Encoder Number of Capabilitiess
int qic_get_frame_number_of_encoder(unsigned char *count)
{
    int qic_ret = 0;

#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif
    qic_ret=QicEncoderGetNumberOfCapabilities( count);

    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);

    if(!qic_ret){
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_get_frame_number_of_encoder success\n");
    }
    else{
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_get_frame_number_of_encoder failed\n");
        qic_ret=1;
    }

    return qic_ret;
}

// Get H.264/VP8 Encoder Stream Capabilities (width, height, bitrate, framerate)
int qic_get_descriptor_of_encoder(unsigned char index, EncoderCapability_t *capability)
{
    int qic_ret = 0;

#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif
    qic_ret=QicEncoderGetStreamCaps (index, capability);

    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);

    if(!qic_ret){
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_get_descriptor_of_encoder success\n");
    }
    else{
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_get_descriptor_of_encoder failed\n");
        qic_ret=1;
    }

    return qic_ret;
}

// MJPEG QP control
int qic_get_MJPEG_QP_Value(unsigned char *value)
{
    int qic_ret;

    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL))
    {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }

    qic_ret = QicGet_MJPEG_QP_Value(value);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
    if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicGet_MJPEG_QP_Value success, MJPEG_QP_Value=%d ...\n", *value);

    return qic_ret;
}

int qic_change_MJPEG_QP_Value(unsigned char value)
{
    int qic_ret;

    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL))
    {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }

    if(value==0 || value>100)
    {
        LOG_PRINT(debug_str, DEBUG_INFO, "Error!! MJPEG QP value over range=%d, correct rage 1~100...\n",value);
        return 1;
    }

    qic_ret = QicSet_MJPEG_QP_Value(value);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
    if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicSet_MJPEG_QP_Value success...\n");

#ifdef DEBUG_LOG
	unsigned char retValue=0;
    qic_ret = QicGet_MJPEG_QP_Value(&retValue);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
    if(!qic_ret) LOG_PRINT(debug_str, DEBUG_INFO, "QicGet_MJPEG_QP_Value success, MJPEG_QP_Value=%d ...\n", retValue);
#endif

    return qic_ret;
}
