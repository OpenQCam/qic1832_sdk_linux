/*
--       This software proprietary belong to Quanta computer Inc. and may be used        --
--        only as expressly authorized agreement from          --
--                            Quanta Computer Inc..                             --
--                   (C) COPYRIGHT 2010 Quanta Computer Inc.                    --
--                            ALL RIGHTS RESERVED                             --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
*/

#ifndef _QIC_API_VIDEO_H_
#define _QIC_API_VIDEO_H_

#include "qic_api_common_define.h"

/*EU control API for QIC1822*/
#if defined(QIC1822) && defined(QIC_SIMULCAST_API)
int qic_config_codec_EU(unsigned int dev_id,EuExSelectCodec_t tCodec );
int qic_simulcast_config_codec_EU(unsigned int dev_id,EuExSelectCodec_t tCodec,simulcast_configs_t config_setting);
int qic_set_temporal_layer_number_EU(unsigned int dev_id,unsigned short stream_id,unsigned char temp_layer_num);
int qic_change_bitrate_EU( unsigned int dev_id,unsigned short stream_id,unsigned int ubitrate);
int qic_change_frame_interval_EU(unsigned int dev_id,unsigned short stream_id,unsigned int uframe_interval);
int qic_generate_key_frame_EU(unsigned int dev_id,unsigned short stream_id,unsigned char cSyncFrameType, unsigned short sSyncFrameInterval, unsigned char cGradualDecoderRefresh);
int qic_change_resolution_EU(unsigned int dev_id,unsigned short stream_id, unsigned short width, unsigned short height);
int qic_start_stop_layer_EU(unsigned int dev_id,unsigned short stream_layer,unsigned char on);
int qic_change_ErrorResiliency_EU(unsigned int dev_id, unsigned short errorResiliency);
int qic_change_CPB_size_EU(unsigned int dev_id,unsigned short stream_layer,unsigned int CPBsize);
int qic_change_profile_toolset_EU(unsigned int dev_id,  unsigned short stream_id,unsigned short wProfile, unsigned char bmSettings);
int qic_change_slice_size_EU(unsigned int dev_id,unsigned short stream_id, unsigned short wSliceConfigSetting);
int qic_change_rete_control_mode_EU(unsigned int dev_id,unsigned short stream_id, unsigned char bRateControlMode);
int qic_change_level_EU(unsigned int dev_id , unsigned short stream_id,unsigned char bLevelIDC);
int qic_change_QP_range_EU(unsigned int dev_id ,unsigned short stream_id,unsigned char bMinQp,unsigned char bMaxQp);
int qic_change_QP_EU(unsigned int dev_id ,unsigned short stream_layer, unsigned short value);
#endif

int qic_set_burst_mode_peak_bitrate(unsigned int dev_id,unsigned int stream_id, unsigned int peakBitrate);
int qic_get_burst_mode_peak_bitrate(unsigned int dev_id, unsigned short stream_id, unsigned int *peakBitrate);

// Get H.264/VP8 Encoder Number of Capabilities
int qic_get_frame_number_of_encoder(unsigned char *count);
// Get H.264/VP8 Encoder Stream Capabilities (width, height, bitrate, framerate)
int qic_get_descriptor_of_encoder(unsigned char index, EncoderCapability_t *capability);

// MJPEG QP control
int qic_get_MJPEG_QP_Value(unsigned char *value);
int qic_change_MJPEG_QP_Value(unsigned char value);

#endif
