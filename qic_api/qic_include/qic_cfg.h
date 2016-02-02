/*
--       This software proprietary belong to Quanta computer Inc. and may be used        --
--        only as expressly authorized agreement from          --
--                            Quanta Computer Inc..                             --
--                   (C) COPYRIGHT 2010 Quanta Computer Inc.                    --
--                            ALL RIGHTS RESERVED                             --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
*/

#ifndef _QIC_CFG_H_
#define _QIC_CFG_H_

// Supported QIC chips and APIs
#define QIC1822

// Enable debug log
//#define DEBUG_LOG

// Option for supported function, enable it by removing comment mark
// 1.Simulcast
#define QIC_SIMULCAST_API

// 2.MD
#define QIC_MD_API

// 3.OSD
//#define QIC_OSD_API

// 4.2nd bootrom
//#define QIC_SUPPORT_2ND_BL

// 5.Audio calibration backup
//#define QIC_SUPPORT_AUDIO_CALIBRATION_BACKUP
// TODO:Remove it?

#endif
