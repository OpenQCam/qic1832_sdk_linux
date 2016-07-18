/*
--       This software proprietary belong to Quanta computer Inc. and may be used        --
--        only as expressly authorized agreement from          --
--                            Quanta Computer Inc..                             --
--                   (C) COPYRIGHT 2010 Quanta Computer Inc.                    --
--                            ALL RIGHTS RESERVED                             --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
*/

#include "../qic_include/qic_api_fw_update.h"

// Common part
extern char debug_str[1024];
extern char debug_xuctrl_str[512];
extern int config_is_commit;
extern qic_module *dev_pt;
// End of common part

char usb_image[] = {};

static int qic_backup_firmware (char *pathinfo, char *usb_image, char *parm_image, char *audio_image, char *osd_font_image);
static unsigned long CalculateCKSum(int image_max_size);
static int qic_dump_flash_firmware (char *pathinfo, char *dump_image,  int dump_image_size );

int qic_get_system_version (unsigned int dev_id, version_info_t *version_info)
{
    unsigned int index;
    FirmwareVersion_t fm_status;
    int qic_ret = 0;
    int ret = 0;

    char *endptr;
    int base = 16;

    memset (version_info, 0, sizeof(version_info));

    /* device array loop */
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {
            /* set fd */
            qic_ret = QicSetDeviceHandle(dev_pt->cam[index].fd);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);

            /* get firmware info */
            CLEAR(fm_status);

            qic_ret = QicGetFirmwareVersion(&fm_status);
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if (qic_ret) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "can't get firmware version info from device %s\n", dev_pt->cam[index].dev_name);
                ret = qic_ret;
                break;
                /*return qic_ret;*/
            }
            else {
                LOG_PRINT(debug_str, DEBUG_INFO, "%s: get firmware info from device %s\n", __func__, dev_pt->cam[index].dev_name);
            }

            version_info->pid = strtol(fm_status.szPID, &endptr, base);
            version_info->vid = strtol(fm_status.szVID, &endptr, base);
            version_info->revision = strtol(fm_status.szREV, &endptr, base);

            qic_ret = QicGetSvnVersion(&(version_info->svn));
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if (qic_ret) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "can't get svn version info from device %s\n", dev_pt->cam[index].dev_name);
                ret = qic_ret;
                break;
                /*return qic_ret;*/
            }
            else {
                LOG_PRINT(debug_str, DEBUG_INFO, "%s: get svn info from device %s\n", __func__, dev_pt->cam[index].dev_name);
            }

            qic_ret = QicGetFwAPIVersion(&(version_info->fw_api_version));
            LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);
            if (qic_ret) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "can't get fw version info from device %s\n", dev_pt->cam[index].dev_name);
                ret = qic_ret;
                break;
                /*return qic_ret;*/
            }
            else {
                LOG_PRINT(debug_str, DEBUG_INFO, "%s: get fw version info from device %s\n", __func__, dev_pt->cam[index].dev_name);
            }
            ret = 0;
        }
    }
    //	LOG_PRINT(debug_str, DEBUG_DETAIL, "qic_get_system_version - OUT\n");

    return ret;
}

int qic_get_image_version (char *path_info, version_info_t *version_info)
{
    char usb_img_full [512];
    int img_fd = -1;

    int image_size = -1;
    char *usb_img_array = NULL;

    char *endptr;
    int base = 16;
    int usb_image_max_size;
    unsigned int image_max_size;

    int ret;

    if (path_info == NULL) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "no img directory is specified.\n");
        return 1;
    }
    memset (version_info, 0, sizeof(version_info));

    snprintf(usb_img_full, sizeof(usb_img_full), "%s/%s", path_info, QIC1822_USB_IMAGE_NAME);
    usb_image_max_size=QIC1822_FLASH_USB_MAX_SIZE;
    image_max_size =QIC1822_FLASH_MAX_SIZE;

    /* open file for reading */
    img_fd = open(usb_img_full, O_RDONLY);
    if ( -1 == img_fd) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "can't open file %s for reading f/w info: (%d)%s\n", usb_img_full, errno, strerror(errno));
        return 1;
    }
    else {
        LOG_PRINT(debug_str, DEBUG_INFO, "open file %s for reading f/w version\n", usb_img_full);
    }

    /* get the file size */
    image_size = lseek(img_fd, 0, SEEK_END);
    if (-1 == image_size) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "file %s size error: (%d)%s\n", usb_img_full, errno, strerror(errno));
        close(img_fd);
        return 1;
    }
    else {
        LOG_PRINT(debug_str, DEBUG_INFO, "file size = %d\n", image_size);
        lseek(img_fd, 0, SEEK_SET);
    }

    /* size check */
    if (image_size > usb_image_max_size) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "image size erro: too big, (%d is larger than %d bytes)\n", image_size, usb_image_max_size);
        close(img_fd);
        return 1;
    }

    /* map the image */
    usb_img_array = mmap (0, image_size, PROT_READ, MAP_SHARED, img_fd, 0);
    if (MAP_FAILED == usb_img_array) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "mmap error: (%d)%s\n", errno, strerror(errno));
        close(img_fd);
        return 1;
    } else {
        LOG_PRINT(debug_str, DEBUG_INFO, "mmap success\n");
    }

    /* get the f/w version info */

    char szTemp[0x18];
    memcpy (szTemp, usb_img_array + 0x240, sizeof(szTemp));

    char version_str[5];

    /*VID*/
    if (!strncmp(&szTemp[0], "VID", 3))
        strncpy(version_str, &szTemp[3], 5);
    else
        strncpy(version_str, "FFFF\0", 5);

    version_info->vid = strtol(version_str, &endptr, base);


    /*PID*/
    if (!strncmp(&szTemp[8], "PID", 3))
        strncpy(version_str, &szTemp[11], 5);
    else
        strncpy(version_str, "FFFF\0", 5);

    version_info->pid = strtol(version_str, &endptr, base);

    /*REV*/
    if (!strncmp(&szTemp[16], "REV", 3))
        strncpy(version_str, &szTemp[19], 5);
    else
        strncpy(version_str, "FFFF\0", 5);

    version_info->revision= strtol(version_str, &endptr, base);

    /*SVN - only number*/
    memset (version_str, 0, 5);
    ret = QicFlashRead (0x22C, (unsigned char*)version_str, 4,image_max_size);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, ret);
    version_info->svn = atoi(version_str);


    /* unmap the image */
    if (munmap(usb_img_array, image_size) == -1) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "munmap error: (%d)%s\n", errno, strerror(errno));
    } else {
        LOG_PRINT(debug_str, DEBUG_INFO, "munmap success\n");
    }

    close(img_fd);
    //	LOG_PRINT(debug_str, DEBUG_DETAIL, "qic_get_image_version - OUT\n");

    return 0;
}

int qic_get_image_version_by_filename (char *usb_path_info, version_info_t *version_info)
{
    char usb_img_full [512];
    int img_fd = -1;

    int image_size = -1;
    char *usb_img_array = NULL;

    char *endptr;
    int base = 16;
    int usb_image_max_size;
    unsigned int image_max_size;
    int ret;

    if (usb_path_info == NULL) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "no img directory is specified.\n");
        return 1;
    }

    memset(version_info, 0, sizeof(version_info));
    memset(usb_img_full, 0, sizeof(usb_img_full));

    snprintf(usb_img_full, sizeof(usb_img_full), "%s", usb_path_info);
    usb_image_max_size=QIC1822_FLASH_USB_MAX_SIZE;
    image_max_size=QIC1822_FLASH_MAX_SIZE;

    /* open file for reading */
    img_fd = open(usb_img_full, O_RDONLY);
    if ( -1 == img_fd) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "can't open file %s for reading f/w info: (%d)%s\n", usb_img_full, errno, strerror(errno));
        return 1;
    }
    else {
        LOG_PRINT(debug_str, DEBUG_INFO, "open file %s for reading f/w version\n", usb_img_full);
    }

    /* get the file size */
    image_size = lseek(img_fd, 0, SEEK_END);
    if (-1 == image_size) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "file %s size error: (%d)%s\n", usb_img_full, errno, strerror(errno));
        close(img_fd);
        return 1;
    }
    else {
        LOG_PRINT(debug_str, DEBUG_INFO, "file size = %d\n", image_size);
        lseek(img_fd, 0, SEEK_SET);
    }

    /* size check */
    if (image_size > usb_image_max_size) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "image size erro: too big, (%d is larger than %d bytes)\n", image_size, usb_image_max_size);
        close(img_fd);
        return 1;
    }

    /* map the image */
    usb_img_array = mmap (0, image_size, PROT_READ, MAP_SHARED, img_fd, 0);
    if (MAP_FAILED == usb_img_array) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "mmap error: (%d)%s\n", errno, strerror(errno));
        close(img_fd);
        return 1;
    } else {
        LOG_PRINT(debug_str, DEBUG_INFO, "mmap success\n");
    }

    /* get the f/w version info */
    char szTemp[0x18];
    memcpy (szTemp, usb_img_array + 0x240, sizeof(szTemp));

    char version_str[5];

    /*VID*/
    if (!strncmp(&szTemp[0], "VID", 3))
        strncpy(version_str, &szTemp[3], 5);
    else
        strncpy(version_str, "FFFF\0", 5);

    version_info->vid = strtol(version_str, &endptr, base);


    /*PID*/
    if (!strncmp(&szTemp[8], "PID", 3))
        strncpy(version_str, &szTemp[11], 5);
    else
        strncpy(version_str, "FFFF\0", 5);

    version_info->pid = strtol(version_str, &endptr, base);

    /*REV*/
    if (!strncmp(&szTemp[16], "REV", 3))
        strncpy(version_str, &szTemp[19], 5);
    else
        strncpy(version_str, "FFFF\0", 5);

    version_info->revision= strtol(version_str, &endptr, base);

    /*SVN - only number*/
    memset (version_str, 0, 5);
    ret = QicFlashRead (0x22C, (unsigned char*)version_str, 4,image_max_size);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, ret);
    version_info->svn = atoi(version_str);


    /* unmap the image */
    if (munmap(usb_img_array, image_size) == -1) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "munmap error: (%d)%s\n", errno, strerror(errno));
    } else {
        LOG_PRINT(debug_str, DEBUG_INFO, "munmap success\n");
    }

    close(img_fd);
    //	LOG_PRINT(debug_str, DEBUG_DETAIL, "qic_get_image_version - OUT\n");

    return 0;
}

int qic_update_firmware_by_filename(unsigned int dev_id,
                                    char *update_usb_img_loc,
                                    char *update_param_img_loc,
                                    char *update_audio_img_loc,
                                    char *update_osd_font_img_loc,
                                    unsigned char update_flags)
{
    // read from system
    unsigned char *orig_usb_image = NULL;
    unsigned char *orig_parm_image = NULL;
    unsigned char *orig_audio_image = NULL;
    unsigned char *orig_osd_font_image = NULL;
    // read from file
    unsigned char *new_usb_image = NULL;
    unsigned char *new_parm_image = NULL;
    unsigned char *new_audio_image = NULL;
    unsigned char *new_osd_font_image = NULL;

    int ret = 0;
    int usb_img_fd = -1;
    int parm_img_fd = -1;
    int audio_img_fd = -1;
    int osd_font_img_fd = -1;

    int usb_img_size = 0;
    int parm_img_size = 0;
    int audio_img_size = 0;
    int osd_font_img_size = 0;

    unsigned char img_header[QIC_IMG_HEADER_SIZE];
    unsigned char header_verify[QIC_IMG_HEADER_SIZE];
    unsigned char usb_fail=0;
    unsigned char parm_fail=0;
    unsigned char audio_fail=0;
    unsigned char osd_font_fail=0;

    int image_max_size;
    int usb_image_max_size;
    int parm_image_max_size;
    int audio_image_max_size;
    int osd_font_image_max_size;

    image_max_size=QIC1822_FLASH_MAX_SIZE;
    usb_image_max_size=QIC1822_FLASH_USB_MAX_SIZE;
    parm_image_max_size=QIC1822_FLASH_PARAM_MAX_SIZE;
    audio_image_max_size=QIC1822_FLASH_AUDIO_PARAM_MAX_SIZE;
    osd_font_image_max_size=QIC1822_FLASH_OSD_FONT_MAX_SIZE;

    /* read data from cam */
    unsigned int index;
    int count = 0;
    for (index = 0; index < dev_pt->num_devices; index++)
    {
        if (dev_pt->cam[index].dev_id & dev_id)
        {
            count ++;
            QicChangeFD(dev_pt->cam[index].fd);
        }
    }

#ifdef QIC_SUPPORT_2ND_BL
    /*checking 2nd Boot Rom if available*/
    ret = QicFlashRead(QIC1822_FLASH_2ndBL_ADDR, header_verify, QIC_IMG_HEADER_SIZE,image_max_size);

    if (header_verify[0] != 0x18&&header_verify[1] != 0x22)
    {
        printf("QIC1822 2nd Boot Rom no Found\n");
        return FW_NO_2ND_BL;
    }
#endif

    orig_usb_image = calloc(1, usb_image_max_size);
    orig_parm_image = calloc (1, parm_image_max_size);
    orig_audio_image = calloc (1, audio_image_max_size);
    orig_osd_font_image = calloc (1, osd_font_image_max_size);

    new_usb_image = NULL;
    new_parm_image = NULL;
    new_audio_image = NULL;
    new_osd_font_image = NULL;

    if ((orig_usb_image == NULL) || (orig_parm_image == NULL)|| (orig_osd_font_image == NULL)|| (orig_audio_image == NULL))
    {
        LOG_PRINT(debug_str, DEBUG_ERROR, "%s: can't malloc\n", __func__);
        goto ERR_CLOSE;
    }

    ret = QicSetPll();

#ifdef QIC_SUPPORT_2ND_BL
    ret = QicFlashSetSpiConfig(0x10, MODE_FLASH_PROGRAM_PP);
#else
    ret = QicFlashSetSpiConfig(0x08, MODE_FLASH_PROGRAM_PP);
#endif
    /* read to ram */
    // USB
    ret = QicFlashRead(QIC1822_FLASH_USB_ADDR, orig_usb_image, usb_image_max_size,image_max_size);
    if (ret)
    {
        LOG_PRINT(debug_str,DEBUG_ERROR, "USB block read error = %d\n", ret);
    }
    else
    {
        LOG_PRINT(debug_str, 0, "USB block read from flash\n");
        new_usb_image = orig_usb_image;
        usb_img_size = usb_image_max_size;
    }

    // ISP param
    ret = QicFlashRead(QIC1822_FLASH_PARAM_ADDR, orig_parm_image, parm_image_max_size,image_max_size);
    if (ret)
    {
        LOG_PRINT(debug_str,DEBUG_ERROR, "ISP param block read error = %d\n", ret);
    }
    else
    {
        LOG_PRINT(debug_str, DEBUG_INFO, "ISP param block read from flash\n");
        new_parm_image = orig_parm_image;
        usb_img_size = image_max_size;
    }

    // audio param
    ret = QicFlashRead(QIC1822_FLASH_AUDIO_PARAM_ADDR, orig_audio_image, audio_image_max_size,image_max_size);
    if (ret)
    {
        LOG_PRINT(debug_str,DEBUG_ERROR, "AUDIO block read error = %d\n", ret);
    }
    else
    {
        LOG_PRINT(debug_str, DEBUG_INFO, "AUDIO block read from flash\n");
        new_audio_image = orig_audio_image;
        usb_img_size = audio_image_max_size;
    }

    // OSD font
    ret = QicFlashRead(QIC1822_FLASH_OSD_FONT_ADDR, orig_osd_font_image, osd_font_image_max_size,image_max_size);
    if (ret)
    {
        LOG_PRINT(debug_str,DEBUG_ERROR, "OSD FONT block read error = %d\n", ret);
    }
    else
    {
        LOG_PRINT(debug_str, DEBUG_INFO, "OSD FONT block read from flash\n");
        new_osd_font_image = orig_osd_font_image;
        usb_img_size = osd_font_image_max_size;
    }

    if (!ret)
    {

    }
    else
    {	//return and not forward
        goto ERR_CLOSE;
    }

    /* read image file */
    char image_name[512];

    if (update_usb_img_loc == NULL)
    {
        char tmp[10];
        update_usb_img_loc=tmp;
        strcpy(update_usb_img_loc,".");
    }

    /* open file for reading */
    if (update_flags & USB_FIRMWARE)
    {	/* USB image */
        snprintf(image_name, sizeof(image_name), "%s", update_usb_img_loc);
        usb_img_fd = open(image_name, O_RDONLY);

        if ( -1 == usb_img_fd)
        {
            LOG_PRINT(debug_str, DEBUG_ERROR, "can't open file %s for reading f/w info: %s\n", image_name, strerror(errno));
            new_usb_image = orig_usb_image;
            usb_img_size = usb_image_max_size;
        }
        else
        {
            LOG_PRINT(debug_str, DEBUG_INFO, "open file %s for reading f/w version\n", image_name);

            /* get the file size */
            usb_img_size = lseek(usb_img_fd, 0, SEEK_END);

            if (-1 == usb_img_size)
            {
                LOG_PRINT(debug_str, DEBUG_ERROR, "file %s size error: %s\n", image_name, strerror(errno));
                close(usb_img_fd);
                ret = 1;
            }
            else
            {
                lseek(usb_img_fd, 0, SEEK_SET);

                /* size check */
                if (usb_img_size > usb_image_max_size)
                {
                    LOG_PRINT(debug_str, DEBUG_ERROR, "image size erro: too big, (%d is larger than %d bytes)\n", usb_img_size, usb_image_max_size);
                    close(usb_img_fd);
                    ret = 1;
                }
                else
                {
                    LOG_PRINT(debug_str, DEBUG_INFO, "image size: %d\n", usb_img_size);

                    /* map the image */
                    new_usb_image= mmap (0, usb_img_size, PROT_READ, MAP_SHARED, usb_img_fd, 0);
                    if (MAP_FAILED == new_usb_image)
                    {
                        LOG_PRINT(debug_str, DEBUG_ERROR, "mmap error: %s\n", strerror(errno));
                        close(usb_img_fd);
                        ret = 1;
                    }
                }
            }
        }
    }
    else
    {
        new_usb_image = orig_usb_image;
        usb_img_size = usb_image_max_size;
    }

    if (ret)
    {
        goto ERR_CLOSE;
    }
    else
    {
        if (update_flags & PARAM_FIRMWARE)
        {	/* PARM image */
            snprintf(image_name, sizeof(image_name), "%s", update_param_img_loc);

            parm_img_fd = open(image_name, O_RDONLY);

            if ( -1 == parm_img_fd)
            {
                LOG_PRINT(debug_str, DEBUG_ERROR, "can't open file %s for reading f/w info: %s\n", image_name, strerror(errno));
                new_parm_image = orig_parm_image;
                parm_img_size = parm_image_max_size;
            }
            else
            {
                LOG_PRINT(debug_str, DEBUG_INFO, "open isp parm file %s for reading f/w version\n", image_name);

                /* get the file size */
                parm_img_size = lseek(parm_img_fd, 0, SEEK_END);
                if (-1 == parm_img_size)
                {
                    LOG_PRINT(debug_str, DEBUG_ERROR, "isp parm file %s size error: %s\n", image_name, strerror(errno));
                    close(parm_img_fd);
                    ret = 1;
                }
                else
                {
                    lseek(parm_img_fd, 0, SEEK_SET);

                    /* size check */
                    if (parm_img_size > parm_image_max_size)
                    {
                        LOG_PRINT(debug_str, DEBUG_ERROR, "isp parm image size erro: too big, (%d is larger than %d bytes)\n", parm_img_size, parm_image_max_size);
                        close(parm_img_fd);
                        ret = 1;
                    }
                    else
                    {
                        LOG_PRINT(debug_str, DEBUG_INFO, "isp parm image size: %d\n", parm_img_size);

                        /* map the image */
                        new_parm_image= mmap (0, parm_img_size, PROT_READ, MAP_SHARED, parm_img_fd, 0);
                        if (MAP_FAILED == new_parm_image)
                        {
                            LOG_PRINT(debug_str, DEBUG_ERROR, "mmap error: %s\n", strerror(errno));
                            close(parm_img_fd);
                            ret = 1;
                        }
                    }
                }
            }
        }
        else
        {
            new_parm_image = orig_parm_image;
            parm_img_size = parm_image_max_size;
        }
    }

    if (ret)
    {
        goto ERR_CLOSE;
    }
    else
    {
        if (update_flags & AUDIO_FIRMWARE)
        {	/* audio image */
            snprintf(image_name, sizeof(image_name), "%s", update_audio_img_loc);
            audio_img_fd = open(image_name, O_RDONLY);

            if ( -1 == parm_img_fd)
            {
                LOG_PRINT(debug_str, DEBUG_ERROR, "can't open file %s for reading f/w info: %s\n", image_name, strerror(errno));
                new_audio_image = orig_audio_image;
                audio_img_size = audio_image_max_size;
            }
            else
            {
                LOG_PRINT(debug_str, DEBUG_INFO, "open audio parm file %s for reading f/w version\n", image_name);

                /* get the file size */
                audio_img_size = lseek(audio_img_fd, 0, SEEK_END);
                if (-1 == audio_img_size) {
                    LOG_PRINT(debug_str, DEBUG_ERROR, "audio parm file %s size error: %s\n", image_name, strerror(errno));
                    close(audio_img_fd);
                    ret = 1;
                }
                else
                {
                    lseek(audio_img_fd, 0, SEEK_SET);

                    /* size check */
                    if (audio_img_size > audio_image_max_size)
                    {
                        LOG_PRINT(debug_str, DEBUG_ERROR, "audio parm image size erro: too big, (%d is larger than %d bytes)\n", audio_img_size, audio_image_max_size);
                        close(audio_img_fd);
                        ret = 1;
                    }
                    else
                    {
                        LOG_PRINT(debug_str, DEBUG_INFO, "audio parm image size: %d\n", audio_img_size);

                        /* map the image */
                        new_audio_image= mmap (0, audio_img_size, PROT_READ, MAP_SHARED, audio_img_fd, 0);
                        if (MAP_FAILED == new_audio_image)
                        {
                            LOG_PRINT(debug_str, DEBUG_ERROR, "mmap error: %s\n", strerror(errno));
                            close(audio_img_fd);
                            ret = 1;
                        }
                    }
                }
            }
        }
        else
        {
            new_audio_image = orig_audio_image;
            audio_img_size = audio_image_max_size;
        }
    }

    if (ret)
    {
        goto ERR_CLOSE;
    }
    else
    {
        if (update_flags & OSD_FONT_FIRMWARE)
        {	/* OSD FONT image */
            snprintf(image_name, sizeof(image_name), "%s", update_osd_font_img_loc);
            osd_font_img_fd = open(image_name, O_RDONLY);

            if ( -1 == osd_font_img_fd)
            {
                LOG_PRINT(debug_str, DEBUG_ERROR, "can't open osd font file %s for reading f/w info: %s\n", image_name, strerror(errno));
                new_osd_font_image = orig_osd_font_image;
                osd_font_img_size = osd_font_image_max_size;
            }
            else
            {
                LOG_PRINT(debug_str, DEBUG_INFO, "open osd font file %s for reading f/w version\n", image_name);

                /* get the file size */
                osd_font_img_size = lseek(osd_font_img_fd, 0, SEEK_END);
                if (-1 == osd_font_img_size)
                {
                    LOG_PRINT(debug_str, DEBUG_ERROR, "osd font file %s size error: %s\n", image_name, strerror(errno));
                    close(osd_font_img_fd);
                    ret = 1;
                }
                else
                {
                    lseek(parm_img_fd, 0, SEEK_SET);

                    /* size check */
                    if (osd_font_img_size > osd_font_image_max_size)
                    {
                        LOG_PRINT(debug_str, DEBUG_ERROR, "osd font image size erro: too big, (%d is larger than %d bytes)\n", osd_font_img_size, osd_font_image_max_size);
                        close(osd_font_img_fd);
                        ret = 1;
                    }
                    else
                    {
                        LOG_PRINT(debug_str, DEBUG_INFO, "osd font image size: %d\n", osd_font_img_size);

                        /* map the image */
                        new_osd_font_image= mmap (0, osd_font_img_size, PROT_READ, MAP_SHARED, osd_font_img_fd, 0);
                        if (MAP_FAILED == new_osd_font_image)
                        {
                            LOG_PRINT(debug_str, DEBUG_ERROR, "mmap error: %s\n", strerror(errno));
                            close(osd_font_img_fd);
                            ret = 1;
                        }
                    }
                }
            }
        }
        else
        {
            new_osd_font_image = orig_osd_font_image;
            osd_font_img_size = osd_font_image_max_size;
        }
    }

    if (ret)
    {
        goto ERR_CLOSE;
    }

    /* erase flash */
    int i;
    unsigned char *BufVerify = calloc (1, image_max_size);

    printf("\nFlash erasing...\n");
    /* erase */
#ifdef QIC_SUPPORT_2ND_BL
    for (i =QIC1822_FLASH_USB_ADDR; i < image_max_size; i += FLASH_SECTOR_SIZE)
    {
        ret = QicFlashSectorErase(i);
        printf("\r [size:%d/total:%d]",i,image_max_size);
        usleep(20*1000);
    }
    printf("\n [size:%d/total:%d]\n",image_max_size,image_max_size);
    printf("Flash erasing... done\n");
#else
    if(update_flags & USB_FIRMWARE)
    {
        QicFlashErase ();
        usleep(2000*1000);
    }
    else
    {
        if(update_flags & USB_FIRMWARE)
        {
            for (i =QIC1822_FLASH_USB_ADDR; i < image_max_size; i += FLASH_SECTOR_SIZE)
            {
                ret = QicFlashSectorErase(i);
                printf("\r [size:%d/total:%d]",i,image_max_size);
                usleep(20*1000);
            }
        }
        if(update_flags & PARAM_FIRMWARE)
        {
            for (i =QIC1822_FLASH_PARAM_ADDR; i < QIC1822_FLASH_PARAM_ADDR+parm_image_max_size; i += FLASH_SECTOR_SIZE)
            {
                ret = QicFlashSectorErase(i);
                printf("\r [size:%d/total:%d]",i,parm_image_max_size);
                usleep(20*1000);
            }
        }
        if(update_flags & AUDIO_FIRMWARE)
        {
            for (i =QIC1822_FLASH_AUDIO_PARAM_ADDR; i < QIC1822_FLASH_AUDIO_PARAM_ADDR+audio_image_max_size; i += FLASH_SECTOR_SIZE)
            {
                ret = QicFlashSectorErase(i);
                printf("\r [size:%d/total:%d]",i,audio_image_max_size);
                usleep(20*1000);
            }
        }
        if(update_flags & OSD_FONT_FIRMWARE)
        {
            for (i =QIC1822_FLASH_OSD_FONT_ADDR; i < QIC1822_FLASH_OSD_FONT_ADDR+osd_font_image_max_size; i += FLASH_SECTOR_SIZE)
            {
                ret = QicFlashSectorErase(i);
                printf("\r [size:%d/total:%d]",i,osd_font_image_max_size);
                usleep(20*1000);
            }
        }
    }
#endif

    /* check if erase complete */
#ifdef QIC_SUPPORT_2ND_BL
    ret = QicFlashRead(QIC1822_FLASH_USB_ADDR, BufVerify, image_max_size-QIC1822_FLASH_2ndBL_MAX_SIZE,image_max_size);
#else
    ret = QicFlashRead(QIC1822_FLASH_USB_ADDR, BufVerify, image_max_size,image_max_size);
#endif
    LOG_PRINT(debug_str, DEBUG_ERROR, "Check Flash USB sector erase ret=%d\n",ret);

#ifdef QIC_SUPPORT_2ND_BL
    for (i = /*QIC1822_FLASH_USB_ADDR*/0; i < image_max_size-QIC1822_FLASH_2ndBL_MAX_SIZE; i++)
    {
        if (BufVerify[i] != 0xff)
        {
            ret = 1;
            LOG_PRINT(debug_str, DEBUG_ERROR, "Flash USB erase error at sector %x %x\n", i,BufVerify[i] );
        }
}
#else
    if(update_flags & USB_FIRMWARE){
        for (i = 0; i < image_max_size; i++)
        {
            if (BufVerify[i] != 0xff)
            {
                ret = 1;
                LOG_PRINT(debug_str, DEBUG_ERROR, "Flash USB erase error at sector %x %x\n", i,BufVerify[i] );
            }
        }
    }
    else
    {
        if(update_flags & PARAM_FIRMWARE)
        {
            for (i = QIC1822_FLASH_PARAM_ADDR; i < parm_image_max_size; i++)
            {
                if (BufVerify[i] != 0xff)
                {
                    ret = 1;
                    LOG_PRINT(debug_str, DEBUG_ERROR, "Flash PARAM erase error at sector %x %x\n", i,BufVerify[i] );
                }
            }
        }
        if(update_flags & AUDIO_FIRMWARE)
        {
            for (i = QIC1822_FLASH_AUDIO_PARAM_ADDR; i < audio_image_max_size; i++)
            {
                if (BufVerify[i] != 0xff)
                {
                    ret = 1;
                    LOG_PRINT(debug_str, DEBUG_ERROR, "Flash AUDIO erase error at sector %x %x\n", i,BufVerify[i] );
                }
            }
        }
        if(update_flags & OSD_FONT_FIRMWARE)
        {
            for (i = QIC1822_FLASH_OSD_FONT_ADDR; i < osd_font_image_max_size; i++)
            {
                if (BufVerify[i] != 0xff)
                {
                    ret = 1;
                    LOG_PRINT(debug_str, DEBUG_ERROR, "Flash OSD font erase error at sector %x %x\n", i,BufVerify[i] );
                }
            }
        }
    }
#endif
    if (ret)
    {
        free(BufVerify);
        goto ERR_CLOSE_MMAP;
    }

    /* write to flash */
    if (update_flags & USB_FIRMWARE)
    {
        printf("\nUSB image flashing... \n");
        memcpy(&img_header[0],&new_usb_image[0],QIC_IMG_HEADER_SIZE);

        ret = QicFlashWriteUSBIMG (QIC1822_FLASH_USB_ADDR+QIC_IMG_HEADER_SIZE, new_usb_image+QIC_IMG_HEADER_SIZE, usb_img_size-QIC_IMG_HEADER_SIZE,image_max_size);

        LOG_PRINT(debug_str, DEBUG_INFO, "USB Sector Image Write to Flash\n");
        if (ret != 0)
        {
            ret = 1;
            LOG_PRINT(debug_str, DEBUG_ERROR, "USB Sector Image Write Failed\n");
        }
        printf("USB image flashing...done \n");
    }

    if (update_flags & PARAM_FIRMWARE)
    {
        printf("\nISP PARAM image flashing... \n");
        ret = QicFlashWriteUSBIMG (QIC1822_FLASH_PARAM_ADDR, new_parm_image, parm_image_max_size,image_max_size);
        LOG_PRINT(debug_str, DEBUG_INFO, "PARAM Sector Image Write to Flash\n");
        if (ret != 0){
            ret = 1;
            LOG_PRINT(debug_str, DEBUG_ERROR, "PARAM Sector Image Write Failed\n");
        }
        printf("ISP PARM image flashing...done \n");
    }

    if (update_flags & AUDIO_FIRMWARE)
    {
        printf("\nAUDIO PARAM image flashing... \n");
        ret = QicFlashWriteUSBIMG (QIC1822_FLASH_AUDIO_PARAM_ADDR, new_audio_image, audio_image_max_size,image_max_size);
        LOG_PRINT(debug_str, DEBUG_INFO, "AUDIO Sector Image Write to Flash\n");
        if (ret != 0)
        {
            ret = 1;
            LOG_PRINT(debug_str, DEBUG_ERROR, "AUDIO Sector Image Write Failed\n");
        }
        printf("AUDIO PARM image flashing...done \n");
    }

    if (update_flags & OSD_FONT_FIRMWARE)
    {
        printf("\nOSD FONT image flashing... \n");
        ret = QicFlashWriteUSBIMG (QIC1822_FLASH_OSD_FONT_ADDR, new_osd_font_image, osd_font_image_max_size,image_max_size);
        LOG_PRINT(debug_str, DEBUG_INFO, "OSD FONT Sector Image Write to Flash\n");
        if (ret != 0)
        {
            ret = 1;
            LOG_PRINT(debug_str, DEBUG_ERROR, "OSD FONT Sector Image Write Failed\n");
        }
        printf("OSD FONT image flashing...done \n");
    }

    /*verify*/
    if (update_flags & USB_FIRMWARE){
        ret = QicFlashRead(QIC1822_FLASH_USB_ADDR, BufVerify, usb_img_size,image_max_size);
        LOG_PRINT(debug_str, DEBUG_INFO, "Verify USB Sector Image\n");
        for (i = 0+QIC_IMG_HEADER_SIZE; i < usb_img_size-QIC_IMG_HEADER_SIZE; i++)
        {
            if (BufVerify[i] != new_usb_image[i])
            {
                ret = 1;
                usb_fail=1;
                LOG_PRINT(debug_str, DEBUG_ERROR, "USB Sector Image Verify Failed at %d, %x, %x\n", i, new_usb_image[i], BufVerify[i]);
            }
        }
        if(usb_fail)
        {
            printf("\nUSB Flash ERROR!!!!!!!!!!!!!\n");
        }
    }

    if (update_flags & PARAM_FIRMWARE)
    {
        ret = QicFlashRead(QIC1822_FLASH_PARAM_ADDR, BufVerify, parm_image_max_size,image_max_size);
        LOG_PRINT(debug_str, 0, "Verify PARAM Sector Image\n");
        for (i = 0; i < parm_image_max_size; i++)
        {
            if (BufVerify[i] != new_parm_image[i])
            {
                ret = 1;
                parm_fail=1;
                LOG_PRINT(debug_str, DEBUG_ERROR, "PARAM Sector Image Verify Failed at %d\n", i);
            }
        }
        if(parm_fail)
        {
            printf("\nISP PARM Flash ERROR!!!!!!!!!!!!!!\n");
        }
    }

    if (update_flags & AUDIO_FIRMWARE){
        ret = QicFlashRead(QIC1822_FLASH_AUDIO_PARAM_ADDR, BufVerify, parm_image_max_size,image_max_size);
        LOG_PRINT(debug_str, 0, "Verify AUDIO Sector Image\n");
        for (i = 0; i < audio_image_max_size; i++)
        {
            if (BufVerify[i] != new_audio_image[i])
            {
                ret = 1;
                audio_fail=1;
                LOG_PRINT(debug_str, DEBUG_ERROR, "AUDIO Sector Image Verify Failed at %d\n", i);
            }
        }
        if(audio_fail)
        {
            printf("\nAUDIO PARM Flash ERROR!!!!!!!!!!!!!!\n");
        }
    }

    if (update_flags & OSD_FONT_FIRMWARE)	{
        ret = QicFlashRead(QIC1822_FLASH_OSD_FONT_ADDR, BufVerify, osd_font_image_max_size,image_max_size);
        LOG_PRINT(debug_str, 0, "Verify OSD FONT Sector Image\n");
        for (i = 0; i < osd_font_image_max_size; i++)
        {
            if (BufVerify[i] != new_osd_font_image[i])
            {
                ret = 1;
                osd_font_fail=1;
                LOG_PRINT(debug_str, DEBUG_ERROR, "OSD FONT Sector Image Verify Failed at %d\n", i);
            }
        }
        if(osd_font_fail)
        {
            printf("\nOSD FONT Flash ERROR!!!!!!!!!!!!!!\n");
        }
    }

    if (update_flags & USB_FIRMWARE)
    {
        if(!usb_fail&&!parm_fail)
        {
            printf("\ndownload ok\n");
            memset(header_verify,0,sizeof(header_verify));

            ret = QicFlashWrite(QIC1822_FLASH_USB_ADDR, img_header, QIC_IMG_HEADER_SIZE, image_max_size);
            /*checking header*/
            ret = QicFlashRead(QIC1822_FLASH_USB_ADDR, header_verify, QIC_IMG_HEADER_SIZE,image_max_size);

            if (header_verify[0] != 0x18&&header_verify[1] != 0x22)
            {
                printf("QIC1822 header flash error\n");
                ret= 1;
            }
        }
    }

    if (ret)
    {
        free (BufVerify);
        goto ERR_CLOSE_MMAP;
    }
    free(BufVerify);

    /*release mem */
ERR_CLOSE_MMAP:
    if (new_usb_image != orig_usb_image)
    {
        munmap(new_usb_image, usb_img_size);
        close(usb_img_fd);
    }

    if (new_parm_image != orig_parm_image)
    {
        munmap (new_parm_image, parm_img_size);
        close(parm_img_fd);
    }

    if (new_audio_image != orig_audio_image)
    {
        munmap (new_audio_image, audio_img_size);
        close(parm_img_fd);
    }

    if (new_osd_font_image != orig_osd_font_image)
    {
        munmap (new_osd_font_image, osd_font_img_size);
        close(osd_font_img_fd);
    }

ERR_CLOSE:
    free(orig_usb_image);
    free(orig_parm_image);
    free(orig_audio_image);
    free(orig_osd_font_image);

    return ret;
}
// end of qic_update_firmware_by_filename()

int qic_check_firmware(unsigned int dev_id,unsigned long checksum)
{
    unsigned long sum=0;
    char* current_path=".";
    int ret = 0;
    int image_max_size;
    unsigned char *BufVerify=NULL;

    /* read data from cam */
    unsigned int index;
    int count = 0;
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {
            count ++;
            QicChangeFD(dev_pt->cam[index].fd);
        }
    }

    image_max_size=QIC1822_FLASH_MAX_SIZE;

    ret = QicSetPll();
    ret = QicFlashSetSpiConfig(0x10, MODE_FLASH_PROGRAM_PP);

    sum=CalculateCKSum(image_max_size);
    if(checksum==sum){
        printf("\nchecksum verify...ok    (%lu=%lu)\n",sum,checksum);
    }
    else{
        printf("\nchecksum verify...failed    (%lu!=%lu)\n",sum,checksum);
        printf("dump error image in current folder...\n");

        BufVerify = calloc (1, image_max_size);

        ret = QicFlashRead(QIC1822_FLASH_USB_ADDR, BufVerify, image_max_size,image_max_size);

        if(!qic_dump_flash_firmware(current_path, (char*)BufVerify,image_max_size ))
            printf("dump error image success\n");
        else
            printf("dump error image error\n");

        ret=1;
    }
    if(BufVerify!=NULL)free (BufVerify);

    return ret;
}

int qic_check_firmware_by_file(unsigned int dev_id,char *check_img_loc)
{
    char* current_path=".";
    int ret = 0;
    int image_max_size;
    unsigned char *BufVerify=NULL;
    int check_img_fd = -1;
    int check_img_size = 0;
    int i=0;
    int firmware_check_fail=0;
    unsigned char *readed_check_image=NULL;

    if (check_img_loc == NULL) {
        printf("No bin file be found!!");
        return 1;
    }

    /* read data from cam */
    unsigned int index;
    int count = 0;
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {
            count ++;
            QicChangeFD(dev_pt->cam[index].fd);
        }
    }

    image_max_size=QIC1822_FLASH_MAX_SIZE;
    ret = QicSetPll();
    ret = QicFlashSetSpiConfig(0x10, MODE_FLASH_PROGRAM_PP);

    /* open file for reading */
    check_img_fd = open(check_img_loc, O_RDONLY);

    if ( -1 == check_img_fd) {
        printf("\ncan't open file %s for reading f/w info: %s\n", check_img_loc, strerror(errno));
        return 1;
    }
    else {
        LOG_PRINT(debug_str, DEBUG_INFO, "open file %s for reading f/w version\n", check_img_loc);

        /* get the file size */
        check_img_size = lseek(check_img_fd, 0, SEEK_END);

        if (-1 == check_img_size) {
            LOG_PRINT(debug_str, DEBUG_ERROR, "file %s size error: %s\n", check_img_loc, strerror(errno));
            close(check_img_fd);
            ret = 1;
        }
        else {
            lseek(check_img_fd, 0, SEEK_SET);

            /* size check */
            if (check_img_size > image_max_size) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "image size erro: too big, (%d is larger than %d bytes)\n", check_img_size, image_max_size);
                close(check_img_fd);
                ret = 1;
            }
            else {
                LOG_PRINT(debug_str, DEBUG_INFO, "image size: %d\n", check_img_size);

                /* map the image */
                readed_check_image= mmap (0, check_img_size, PROT_READ, MAP_SHARED, check_img_fd, 0);
                if (MAP_FAILED == check_img_fd) {
                    LOG_PRINT(debug_str, DEBUG_ERROR, "mmap error: %s\n", strerror(errno));
                    munmap(readed_check_image, check_img_size);
                    ret = 1;
                }
            }
        }
    }

    if(ret){
        printf( "\nRead check bin file failed\n");
        close(check_img_fd);
        return 1;
    }

    BufVerify = calloc (1, image_max_size);

    ret = QicFlashRead(QIC1822_FLASH_USB_ADDR, BufVerify, image_max_size,image_max_size);

    LOG_PRINT(debug_str, DEBUG_INFO, "Verify USB Sector Image\n");
    for (i = 0; i < image_max_size; i++)
    {
        if (BufVerify[i] != readed_check_image[i])
        {
            ret = 1;
            firmware_check_fail=1;
            printf("Image Verify Failed at %d, file is %x, camera is %x\n", i, readed_check_image[i], BufVerify[i]);
        }

        //LOG_PRINT(debug_str, DEBUG_ERROR, "Image Verify OK at %d, %x, %x\n", i, readed_check_image[i], BufVerify[i]);
    }

    if(!firmware_check_fail){
        printf("\nfirmware  verify...ok	\n");
    }
    else{
        printf("\nfirmware verify...failed\n");
        printf("dump error image in current folder...\n");

        if(!qic_dump_flash_firmware(current_path, (char*)BufVerify,image_max_size ))
            printf("dump error image success\n");
        else
            printf("dump error image error\n");

        ret=1;
    }

    if(BufVerify!=NULL)free (BufVerify);

    if (readed_check_image != NULL) {
        munmap (readed_check_image, check_img_size);
        close(check_img_fd);
    }
    return ret;
}

int qic_update_osd_font_by_filename(unsigned int dev_id, char *update_osd_font_img_loc)
{
    unsigned char *orig_osd_font_image; /* read from system */
    unsigned char *new_osd_font_image; /*read from file */
    int ret = 0;
    int osd_font_img_fd = -1;
    int osd_font_img_size = 0;
    unsigned char audio_fail=0;
    int osd_font_image_max_size;
    int image_max_size;
    unsigned char *BufVerify=NULL;
    /* read data from cam */
    unsigned int index;
    int count = 0;
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {
            count ++;
            QicChangeFD(dev_pt->cam[index].fd);
        }
    }

    osd_font_image_max_size=QIC1822_FLASH_OSD_FONT_MAX_SIZE;
    image_max_size=QIC1822_FLASH_MAX_SIZE;

    orig_osd_font_image = calloc( 1, osd_font_image_max_size);

    new_osd_font_image = NULL;

    if ((orig_osd_font_image == NULL) )	{
        LOG_PRINT(debug_str, DEBUG_ERROR, "%s: can't malloc\n", __func__);
        goto ERR_CLOSE;
    }

    ret = QicSetPll();
    ret = QicFlashSetSpiConfig(0x10, MODE_FLASH_PROGRAM_PP);

    /* read to ram */
    ret = QicFlashRead(QIC1822_FLASH_OSD_FONT_ADDR, orig_osd_font_image, osd_font_image_max_size,image_max_size);

    new_osd_font_image = orig_osd_font_image;
    osd_font_img_size = osd_font_image_max_size;


    /* read image file */
    char image_name[512];

    if (update_osd_font_img_loc == NULL) {
        char tmp[10];
        update_osd_font_img_loc=tmp;
        strcpy(update_osd_font_img_loc,".");
    }

    /* open file for reading */
    if (1) {	/* osd font image */

        snprintf(image_name, sizeof(image_name), "%s", update_osd_font_img_loc);

        osd_font_img_fd = open(image_name, O_RDONLY);

        if ( -1 == osd_font_img_fd) {
            LOG_PRINT(debug_str, DEBUG_ERROR, "can't open file %s for reading f/w info: %s\n", image_name, strerror(errno));
            new_osd_font_image = orig_osd_font_image;
            osd_font_img_size = osd_font_image_max_size;
        }
        else {
            LOG_PRINT(debug_str, DEBUG_INFO, "open file %s for reading f/w version\n", image_name);

            /* get the file size */
            osd_font_img_size = lseek(osd_font_img_fd, 0, SEEK_END);

            if (-1 == osd_font_img_size) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "file %s size error: %s\n", image_name, strerror(errno));
                close(osd_font_img_fd);
                ret = 1;
            }
            else {
                lseek(osd_font_img_fd, 0, SEEK_SET);

                /* size check */
                if (osd_font_img_size > osd_font_image_max_size) {
                    LOG_PRINT(debug_str, DEBUG_ERROR, "image size erro: too big, (%d is larger than %d bytes)\n", osd_font_img_size, osd_font_image_max_size);
                    close(osd_font_img_fd);
                    ret = 1;
                }
                else {
                    LOG_PRINT(debug_str, DEBUG_INFO, "image size: %d\n", osd_font_img_size);

                    /* map the image */
                    new_osd_font_image= mmap (0, osd_font_img_size, PROT_READ, MAP_SHARED, osd_font_img_fd, 0);
                    if (MAP_FAILED == new_osd_font_image) {
                        LOG_PRINT(debug_str, DEBUG_ERROR, "mmap error: %s\n", strerror(errno));
                        close(osd_font_img_fd);
                        ret = 1;
                    }
                }
            }
        }
    }
    else {
        new_osd_font_image = orig_osd_font_image;
        osd_font_img_size =osd_font_image_max_size;
    }

    if (ret) {
        goto ERR_CLOSE;
    }

    /* erase osd font flash */
    int i;
    BufVerify = calloc (1, osd_font_image_max_size);

    printf("\nFlash erasing...\n");

    /* erase */
    for (i = QIC1822_FLASH_OSD_FONT_ADDR; i < QIC1822_FLASH_OSD_FONT_ADDR+osd_font_image_max_size; i += FLASH_SECTOR_SIZE) {
        ret = QicFlashSectorErase(i);
        printf("\r [size:%d/total:%d]",i,osd_font_image_max_size);
        usleep(50*1000);

    }
    usleep(100*1000);

    /* check if erase complete */
    ret = QicFlashRead(QIC1822_FLASH_OSD_FONT_ADDR, BufVerify, osd_font_image_max_size,image_max_size);

    LOG_PRINT(debug_str, DEBUG_ERROR, "\nCheck Flash osd font sector erase ret=%d\n",ret);


    for (i = 0; i < osd_font_image_max_size; i++)
    {
        if (BufVerify[i] != 0xff)
        {
            ret = 1;
            LOG_PRINT(debug_str, DEBUG_ERROR, "Flash osd font erase error at sector %x %x\n", i,BufVerify[i] );
        }
    }

    free(BufVerify);

    if (ret) {
        goto ERR_CLOSE_MMAP;
    }

    /* write to flash */
    if (1) {
        printf("\n osd font image flashing... \n");

        ret = QicFlashWriteUSBIMG (QIC1822_FLASH_OSD_FONT_ADDR, new_osd_font_image, osd_font_img_size,image_max_size);

        LOG_PRINT(debug_str, DEBUG_INFO, "osd font Sector Image Write to Flash\n");
        if (ret != 0)
        {
            ret = 1;
            LOG_PRINT(debug_str, DEBUG_ERROR, "osd font Sector Image Write Failed\n");
        }
        printf("osd font image flashing...done \n");
    }

    /*verify*/
    if (1) {
        BufVerify = calloc (1, osd_font_img_size);
        ret = QicFlashRead(QIC1822_FLASH_OSD_FONT_ADDR, BufVerify,osd_font_img_size,image_max_size);

        LOG_PRINT(debug_str, DEBUG_INFO, "Verify osd font Sector Image ret=%d\n", ret);

        for (i = 0; i < osd_font_img_size; i++)
        {
            if (BufVerify[i] != new_osd_font_image[i])
            {
                ret = 1;
                audio_fail=1;
                LOG_PRINT(debug_str, DEBUG_ERROR, "osd font Sector Image Verify Failed at %d, %x, %x\n", i, new_osd_font_image[i], BufVerify[i]);
            }
        }

        free (BufVerify);

        if(audio_fail){
            printf("\nosd font Flash ERROR!!!!!!!!!!!!!\n");
        }
    }

    if(!audio_fail){
        printf("\ndownload osd font ok\n");
    }
    else{
        printf("\ndownload osd font error\n");
        ret=1;


    }
    if (ret) {

        goto ERR_CLOSE_MMAP;
    }

    /*release mem */
ERR_CLOSE_MMAP:
    if (new_osd_font_image != orig_osd_font_image) {
        munmap(new_osd_font_image, osd_font_img_size);
        close(osd_font_img_fd);
    }

ERR_CLOSE:
    free(orig_osd_font_image);

    return ret;
}

int qic_update_audio_data_by_filename(unsigned int dev_id, char *update_audio_img_loc)
{
    unsigned char *orig_audio_image; /* read from system */
    unsigned char *new_audio_image; /*read from file */
    int ret = 0;
    int audio_img_fd = -1;
    int audio_img_size = 0;
    unsigned char audio_fail=0;
    int audio_image_max_size;
    int image_max_size;
    unsigned char *BufVerify=NULL;
    /* read data from cam */
    unsigned int index;
    int count = 0;
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {
            count ++;
            QicChangeFD(dev_pt->cam[index].fd);
        }
    }
    audio_image_max_size=QIC1822_FLASH_AUDIO_PARAM_MAX_SIZE;
    image_max_size=QIC1822_FLASH_MAX_SIZE;

    orig_audio_image = calloc( 1, audio_image_max_size);

    new_audio_image = NULL;

    if ((orig_audio_image == NULL) )	{
        LOG_PRINT(debug_str, DEBUG_ERROR, "%s: can't malloc\n", __func__);
        goto ERR_CLOSE;
    }

    ret = QicSetPll();
    ret = QicFlashSetSpiConfig(0x10, MODE_FLASH_PROGRAM_PP);

    /* read to ram */
    ret = QicFlashRead(QIC1822_FLASH_AUDIO_PARAM_ADDR, orig_audio_image, audio_image_max_size,image_max_size);

    new_audio_image = orig_audio_image;
    audio_img_size = audio_image_max_size;

    /* read image file */
    char image_name[512];

    if (update_audio_img_loc == NULL) {
        char tmp[10];
        update_audio_img_loc=tmp;
        strcpy(update_audio_img_loc,".");
    }

    /* open file for reading */
    if (1) {	/* audio image */
        snprintf(image_name, sizeof(image_name), "%s", update_audio_img_loc);

        audio_img_fd = open(image_name, O_RDONLY);

        if ( -1 == audio_img_fd) {
            LOG_PRINT(debug_str, DEBUG_ERROR, "can't open file %s for reading f/w info: %s\n", image_name, strerror(errno));
            new_audio_image = orig_audio_image;
            audio_img_size = audio_image_max_size;
        }
        else {
            LOG_PRINT(debug_str, DEBUG_INFO, "open file %s for reading f/w version\n", image_name);

            /* get the file size */
            audio_img_size = lseek(audio_img_fd, 0, SEEK_END);

            if (-1 == audio_img_size) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "file %s size error: %s\n", image_name, strerror(errno));
                close(audio_img_fd);
                ret = 1;
            }
            else {
                lseek(audio_img_fd, 0, SEEK_SET);

                /* size check */
                if (audio_img_size > audio_image_max_size) {
                    LOG_PRINT(debug_str, DEBUG_ERROR, "image size erro: too big, (%d is larger than %d bytes)\n", audio_img_size, audio_image_max_size);
                    close(audio_img_fd);
                    ret = 1;
                }
                else {
                    LOG_PRINT(debug_str, DEBUG_INFO, "image size: %d\n", audio_img_size);

                    /* map the image */
                    new_audio_image= mmap (0, audio_img_size, PROT_READ, MAP_SHARED, audio_img_fd, 0);
                    if (MAP_FAILED == new_audio_image) {
                        LOG_PRINT(debug_str, DEBUG_ERROR, "mmap error: %s\n", strerror(errno));
                        close(audio_img_fd);
                        ret = 1;
                    }
                }
            }
        }
    }
    else {
        new_audio_image = orig_audio_image;
        audio_img_size =audio_image_max_size;
    }

    if (ret) {
        goto ERR_CLOSE;
    }

    /* erase audio flash */
    int i;
    BufVerify = calloc (1, audio_image_max_size);

    printf("\nFlash erasing...\n");

    /* erase */
    ret = QicFlashSectorErase(QIC1822_FLASH_AUDIO_PARAM_ADDR);
    usleep(100*1000);

    /* check if erase complete */
    ret = QicFlashRead(QIC1822_FLASH_AUDIO_PARAM_ADDR, BufVerify, audio_image_max_size,image_max_size);

    LOG_PRINT(debug_str, DEBUG_ERROR, "Check Flash audio sector erase ret=%d\n",ret);

    for (i = 0; i < audio_image_max_size; i++)
    {
        if (BufVerify[i] != 0xff)
        {
            ret = 1;
            LOG_PRINT(debug_str, DEBUG_ERROR, "Flash audio erase error at sector %x %x\n", i,BufVerify[i] );
        }
    }

    free(BufVerify);

    if (ret) {
        goto ERR_CLOSE_MMAP;
    }

    /* write to flash */
    if (1) {
        printf("\n audio image flashing... \n");

        ret = QicFlashWriteUSBIMG (QIC1822_FLASH_AUDIO_PARAM_ADDR, new_audio_image, audio_img_size,image_max_size);

        LOG_PRINT(debug_str, DEBUG_INFO, "audio Sector Image Write to Flash\n");
        if (ret != 0)
        {
            ret = 1;
            LOG_PRINT(debug_str, DEBUG_ERROR, "audio Sector Image Write Failed\n");
        }
        printf("audio image flashing...done \n");
    }

    /*verify*/
    if (1) {
        BufVerify = calloc (1, audio_img_size);
        ret = QicFlashRead(QIC1822_FLASH_AUDIO_PARAM_ADDR, BufVerify,audio_img_size,image_max_size);

        LOG_PRINT(debug_str, DEBUG_INFO, "Verify audio Sector Image ret=%d\n", ret);

        for (i = 0; i < audio_img_size; i++)
        {
            if (BufVerify[i] != new_audio_image[i])
            {
                ret = 1;
                audio_fail=1;
                LOG_PRINT(debug_str, DEBUG_ERROR, "audio Sector Image Verify Failed at %d, %x, %x\n", i, new_audio_image[i], BufVerify[i]);
            }
        }

        free (BufVerify);

        if(audio_fail){
            printf("\audio Flash ERROR!!!!!!!!!!!!!\n");
        }
    }
    if(!audio_fail){
        printf("\ndownload audio ok\n");
    }
    else{
        printf("\ndownload audio error\n");
        ret=1;
    }
    if (ret) {

        goto ERR_CLOSE_MMAP;
    }

    /*release mem */
ERR_CLOSE_MMAP:
    if (new_audio_image != orig_audio_image) {
        munmap(new_audio_image, audio_img_size);
        close(audio_img_fd);
    }

ERR_CLOSE:
    free(orig_audio_image);

    return ret;
}

int qic_dump_all_flash(unsigned int dev_id)
{
    char* current_path=".";
    int ret = 0;
    int image_max_size;
    unsigned char *BufVerify=NULL;

    /* read data from cam */
    unsigned int index;
    int count = 0;
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {
            count ++;
            QicChangeFD(dev_pt->cam[index].fd);
        }
    }
    image_max_size=QIC1822_FLASH_MAX_SIZE;

    ret = QicSetPll();
    ret = QicFlashSetSpiConfig(0x10, MODE_FLASH_PROGRAM_PP);
    BufVerify = calloc (1, image_max_size);

#ifdef QIC_SUPPORT_2ND_BL
    ret = QicFlashRead(QIC1822_FLASH_2ndBL_ADDR, BufVerify, image_max_size,image_max_size);
#else
    ret = QicFlashRead(QIC1822_FLASH_USB_ADDR, BufVerify, image_max_size,image_max_size);
#endif
    if(!qic_dump_flash_firmware(current_path, (char*)BufVerify,image_max_size )){
        printf("dump  image success\n");
    }
    else{
        printf("dump  image error\n");
        ret=1;
    }
    if(BufVerify!=NULL)free (BufVerify);

    return ret;
}

int qic_backup_firmware_to_file(unsigned int dev_id, char *update_img_loc, unsigned char update_flags, char *backup_img_loc, unsigned char backup_flags)
{
    // read from system
    unsigned char *orig_usb_image = NULL;
    unsigned char *orig_parm_image = NULL;
    unsigned char *orig_audio_image = NULL;
    unsigned char *orig_osd_font_image = NULL;
    // read from file
    unsigned char *new_usb_image = NULL;
    unsigned char *new_parm_image = NULL;
    unsigned char *new_audio_image = NULL;
    unsigned char *new_osd_font_image = NULL;

    int ret = 0;

    int usb_img_size = 0;

    unsigned char *usb_w= NULL;
    unsigned char *param_w = NULL;
    unsigned char *audio_w = NULL;
    unsigned char *osd_font_w = NULL;

    int image_max_size;
    int usb_image_max_size;
    int parm_image_max_size;
    int audio_image_max_size;
    int osd_font_image_max_size;

    /* read data from cam */
    unsigned int index;
    int count = 0;

    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {
            count ++;
            QicChangeFD(dev_pt->cam[index].fd);
        }
    }

    image_max_size=QIC1822_FLASH_MAX_SIZE;
    usb_image_max_size=QIC1822_FLASH_USB_MAX_SIZE;
    parm_image_max_size=QIC1822_FLASH_PARAM_MAX_SIZE;
    audio_image_max_size=QIC1822_FLASH_AUDIO_PARAM_MAX_SIZE;
    osd_font_image_max_size=QIC1822_FLASH_OSD_FONT_MAX_SIZE;

    orig_usb_image = calloc( 1, usb_image_max_size);
    orig_parm_image = calloc (1, parm_image_max_size);
    orig_audio_image = calloc (1, audio_image_max_size);
    orig_osd_font_image = calloc (1, osd_font_image_max_size);

    if ((orig_usb_image == NULL) || (orig_parm_image == NULL)|| (orig_audio_image == NULL))	{
        LOG_PRINT(debug_str, DEBUG_ERROR, "%s: can't malloc\n", __func__);
        goto ERR_CLOSE;
    }

    ret = QicSetPll();
    ret = QicFlashSetSpiConfig(0x10, MODE_FLASH_PROGRAM_PP);

    /* read to ram */
    ret = QicFlashRead(QIC1822_FLASH_USB_ADDR, orig_usb_image, usb_image_max_size, image_max_size);

    if (ret) {
        LOG_PRINT(debug_str,DEBUG_ERROR, "USB block read error = %d\n", ret);
    }
    else {
        LOG_PRINT(debug_str, 0, "USB block read from flash\n");
        if ( backup_flags & USB_FIRMWARE){
            usb_w = orig_usb_image;
        }
        new_usb_image = orig_usb_image;
        usb_img_size = usb_image_max_size;
    }
    LOG_PRINT(debug_str,DEBUG_INFO,"QIC1822 don't need to flash ISP image\n");

    ret = QicFlashRead(QIC1822_FLASH_PARAM_ADDR, orig_parm_image, parm_image_max_size, image_max_size);
    if (ret) {
        LOG_PRINT(debug_str,DEBUG_ERROR, "PARAM block read error = %d\n", ret);
    }
    else {
        LOG_PRINT(debug_str, DEBUG_INFO, "PARAM block read from flash\n");
        if (backup_flags & PARAM_FIRMWARE){
            param_w = orig_parm_image;
        }
        new_parm_image = orig_parm_image;
        usb_img_size = parm_image_max_size;
    }

    /*audio data*/
    if (backup_flags & AUDIO_FIRMWARE){
        ret = QicFlashRead(QIC1822_FLASH_AUDIO_PARAM_ADDR, orig_audio_image, audio_image_max_size, image_max_size);
        if (ret) {
            LOG_PRINT(debug_str,DEBUG_ERROR, "AUDIO block read error = %d\n", ret);
        }
        else {
            LOG_PRINT(debug_str, DEBUG_INFO, "AUDIO block read from flash\n");
            if (backup_flags & AUDIO_FIRMWARE){
                audio_w = orig_audio_image;
            }
            new_audio_image = orig_audio_image;
            usb_img_size = audio_image_max_size;
        }
    }

    /*osd font data*/
    if (backup_flags & OSD_FONT_FIRMWARE){
        printf("dddd %p\n",orig_osd_font_image);
        ret = QicFlashRead(QIC1822_FLASH_OSD_FONT_ADDR, orig_osd_font_image, osd_font_image_max_size, image_max_size);

        if (ret) {
            LOG_PRINT(debug_str,DEBUG_ERROR, "OSD font block read error = %d\n", ret);
        }
        else {
            LOG_PRINT(debug_str, DEBUG_INFO, "OSD font block read from flash\n");
            if (backup_flags & OSD_FONT_FIRMWARE){
                osd_font_w = orig_osd_font_image;
            }
            new_osd_font_image = orig_osd_font_image;
            usb_img_size = osd_font_image_max_size;
        }
    }

    if (!ret) {
        //backup
        ret = qic_backup_firmware(backup_img_loc, (char*)usb_w, (char*)param_w, (char*)audio_w, (char*)osd_font_w);
        if (ret) {
            LOG_PRINT(debug_str, DEBUG_ERROR, "%s: firmware backup error", __func__);
        }

        goto ERR_CLOSE;

    }
    else {	//return and not forward
        goto ERR_CLOSE;
    }

ERR_CLOSE:
    free(orig_usb_image);
    free(orig_parm_image);
    free(orig_audio_image);
    free(orig_osd_font_image);

    return ret;
}

#ifdef QIC_SUPPORT_2ND_BL
static int qic_dump_2nd_BootRom (char *pathinfo, char *dump_image,  int dump_image_size )
{
    char cur_path[512];
    char *default_path = NULL;
    int cur_fd = -1;
    int ret = 0;
    //	  struct tm *tblock;
    //	  time_t curtime;
    //	  curtime = time(NULL);

    //	  tblock = localtime(&curtime);
    //	  printf("Local time is: %s/n",asctime(tblock));

    /* set the backup folder */
    if (pathinfo == NULL) {
        LOG_PRINT(debug_str, DEBUG_INFO, "%s: no default path, do not backup\n", __func__);
        return 0; /* just no backup... */
    }
    else {
        LOG_PRINT(debug_str, DEBUG_INFO, "%s: use file path %s for backup\n", __func__, pathinfo);
        default_path= pathinfo;
    }

    /* dump flash image */
    if (dump_image != NULL) {
        //snprintf(cur_path, sizeof(cur_path), "%s/%s", default_path, QIC1822_2nd_BOOTROM_IMAGE_NAME);
        snprintf(cur_path, sizeof(cur_path), "%s", default_path);

        LOG_PRINT(debug_str, DEBUG_INFO, "dump flash image to %s\n", cur_path);

        cur_fd = open(cur_path, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO);
        if (cur_fd != -1) {
            if ( -1 == write (cur_fd, dump_image, dump_image_size)) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "can't write FLASH image: %s\n", strerror(errno));
                ret = 1;
            }

            if ( -1 == fsync(cur_fd)) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "can't fsync FLASH image: %s\n", strerror(errno));
                ret = 1;
            }

            if ( -1 == close(cur_fd)) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "can't close FLASH image: %s\n", strerror(errno));
                ret = 1;
            }

            cur_fd = -1;
        }
        else {
            LOG_PRINT(debug_str, DEBUG_ERROR, "Can't open the FLASH image block : %s\n", strerror(errno));
            ret = 1;
        }
    }

    return ret;
}

int qic_backup_2nd_Boot_Rom(unsigned int dev_id, char *backup_BL2nd_img_loc)
{
    //unsigned char* current_path=".";
    int ret = 0;
    int image_max_size;
    unsigned char *BufVerify=NULL;
    /* read image file */
    //char image_name[512];
    int i=0;
    int firmware_check_fail=0;

    /* set the backup folder */
    if (backup_BL2nd_img_loc == NULL) {
        LOG_PRINT(debug_str, DEBUG_INFO, "%s: no backup file path, do not backup\n", __func__);
        return 2; /* just no backup... */
    }

    /* read data from cam */
    unsigned int index;
    int count = 0;
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {
            count ++;
            QicChangeFD(dev_pt->cam[index].fd);
        }
    }

    image_max_size=QIC1822_FLASH_2ndBL_MAX_SIZE;
    ret = QicSetPll();
    ret = QicFlashSetSpiConfig(0x10, MODE_FLASH_PROGRAM_PP);

    BufVerify = calloc (1, image_max_size);

    ret = QicFlashRead(QIC1822_FLASH_2ndBL_ADDR, BufVerify, image_max_size,image_max_size);

    if(!qic_dump_2nd_BootRom(backup_BL2nd_img_loc, (char*)BufVerify,image_max_size )){
        printf("dump  2nd Boot Rom image success\n");
    }
    else{
        printf("dump  2nd Boot Rom image error\n");
        ret=1;
    }

    if(BufVerify!=NULL)free (BufVerify);

    return ret;
}

int qic_update_2nd_bootrom_by_filename(unsigned int dev_id,char eraseAll,  char *update_BL2nd_img_loc)
{
    unsigned char *orig_BL2nd_image; /* read from system */
    unsigned char *new_BL2nd_image; /*read from file */
    int ret = 0;
    int BL2nd_img_fd = -1;
    int BL2nd_img_size = 0;
    unsigned char img_header[QIC_IMG_HEADER_SIZE];
    unsigned char header_verify[QIC_IMG_HEADER_SIZE];
    unsigned char BL2nd_fail=0;
    int BL2nd_image_max_size;
    int image_max_size;
    unsigned char *BufVerify=NULL;
    /* read data from cam */
    unsigned int index;
    int count = 0;
    for (index = 0; index < dev_pt->num_devices; index++) {
        if (dev_pt->cam[index].dev_id & dev_id) {
            count ++;
            QicChangeFD(dev_pt->cam[index].fd);
        }
    }

    BL2nd_image_max_size=QIC1822_FLASH_2ndBL_MAX_SIZE;
    image_max_size=QIC1822_FLASH_MAX_SIZE;

    orig_BL2nd_image = calloc( 1, BL2nd_image_max_size);

    new_BL2nd_image = NULL;

    if ((orig_BL2nd_image == NULL) )	{
        LOG_PRINT(debug_str, DEBUG_ERROR, "%s: can't malloc\n", __func__);
        goto ERR_CLOSE;
    }

    ret = QicSetPll();
    ret = QicFlashSetSpiConfig(0x08, MODE_FLASH_PROGRAM_PP);

    /* read to ram */
    //		ret = QicFlashRead(QIC1822_FLASH_2ndBL_ADDR, orig_BL2nd_image, BL2nd_image_max_size,image_max_size);

    new_BL2nd_image = orig_BL2nd_image;
    BL2nd_img_size = BL2nd_image_max_size;

    /* read image file */
    char image_name[512];

    if (update_BL2nd_img_loc == NULL) {
        char tmp[10];
        update_BL2nd_img_loc=tmp;
        strcpy(update_BL2nd_img_loc,".");
    }

    /* open file for reading */
    if (1) {	/* 2nd Boot Rom image */

        snprintf(image_name, sizeof(image_name), "%s", update_BL2nd_img_loc);

        BL2nd_img_fd = open(image_name, O_RDONLY);

        if ( -1 == BL2nd_img_fd) {
            LOG_PRINT(debug_str, DEBUG_ERROR, "can't open file %s for reading f/w info: %s\n", image_name, strerror(errno));
            new_BL2nd_image = orig_BL2nd_image;
            BL2nd_img_size = BL2nd_image_max_size;
        }
        else {
            LOG_PRINT(debug_str, DEBUG_INFO, "open file %s for reading f/w version\n", image_name);

            /* get the file size */
            BL2nd_img_size = lseek(BL2nd_img_fd, 0, SEEK_END);

            if (-1 == BL2nd_img_size) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "file %s size error: %s\n", image_name, strerror(errno));
                close(BL2nd_img_fd);
                ret = 1;
            }
            else {
                lseek(BL2nd_img_fd, 0, SEEK_SET);

                /* size check */
                if (BL2nd_img_size > BL2nd_image_max_size) {
                    LOG_PRINT(debug_str, DEBUG_ERROR, "image size erro: too big, (%d is larger than %d bytes)\n", BL2nd_img_size, BL2nd_image_max_size);
                    close(BL2nd_img_fd);
                    ret = 1;
                }
                else {
                    LOG_PRINT(debug_str, DEBUG_INFO, "image size: %d\n", BL2nd_img_size);

                    /* map the image */
                    new_BL2nd_image= mmap (0, BL2nd_img_size, PROT_READ, MAP_SHARED, BL2nd_img_fd, 0);
                    if (MAP_FAILED == new_BL2nd_image) {
                        LOG_PRINT(debug_str, DEBUG_ERROR, "mmap error: %s\n", strerror(errno));
                        close(BL2nd_img_fd);
                        ret = 1;
                    }
                }
            }
        }
    }
    else {
        new_BL2nd_image = orig_BL2nd_image;
        BL2nd_img_size =BL2nd_image_max_size;
    }

    if (ret) {
        goto ERR_CLOSE;
    }

    /* erase 2nd BL flash */
    int i;
    BufVerify = calloc (1, image_max_size);

    printf("\nFlash erasing...\n");

    /* erase */
    if(eraseAll){
        BufVerify = calloc (1, image_max_size);

        QicFlashErase ();
        printf("\nFlash erase all ...\n");
        usleep(2000*1000);
        /* check if erase complete */
        ret = QicFlashRead(QIC1822_FLASH_2ndBL_ADDR, BufVerify, image_max_size,image_max_size);

        LOG_PRINT(debug_str, DEBUG_ERROR, "Check Flash 2nd Boot Rom sector erase ret=%d\n",ret);

        for (i = 0; i < image_max_size; i++)
        {
            if (BufVerify[i] != 0xff)
            {
                ret = 1;
                LOG_PRINT(debug_str, DEBUG_ERROR, "Flash 2nd Boot Rom erase error at sector %x %x\n", i,BufVerify[i] );
            }
        }
    }
    else{
        BufVerify = calloc (1, QIC1822_FLASH_2ndBL_MAX_SIZE);

        printf("\nFlash erasing ...\n");
        for (i = 0; i < QIC1822_FLASH_2ndBL_MAX_SIZE; i += FLASH_SECTOR_SIZE) {
            ret = QicFlashSectorErase(i);
            printf("\r [size:%d/total:%d]",i,image_max_size);
            usleep(50*1000);

        }
        /* check if erase complete */
        ret = QicFlashRead(QIC1822_FLASH_2ndBL_ADDR, BufVerify, QIC1822_FLASH_2ndBL_MAX_SIZE,image_max_size);

        LOG_PRINT(debug_str, DEBUG_ERROR, "Check Flash 2nd Boot Rom sector erase ret=%d\n",ret);


        for (i = 0; i < QIC1822_FLASH_2ndBL_MAX_SIZE; i++)
        {
            if (BufVerify[i] != 0xff)
            {
                ret = 1;
                LOG_PRINT(debug_str, DEBUG_ERROR, "Flash 2nd Boot Rom erase error at sector %x %x\n", i,BufVerify[i] );
            }
        }
    }

    free(BufVerify);

    if (ret) {
        goto ERR_CLOSE_MMAP;
    }

    /* write to flash */
    if (1) {
        printf("\n 2nd Boot Rom image flashing... \n");
        memcpy(&img_header[0],&new_BL2nd_image[0],QIC_IMG_HEADER_SIZE);

        ret = QicFlashWriteUSBIMG (QIC1822_FLASH_2ndBL_ADDR+QIC_IMG_HEADER_SIZE, new_BL2nd_image+QIC_IMG_HEADER_SIZE, BL2nd_img_size-QIC_IMG_HEADER_SIZE,image_max_size);
        //QicFlashWriteUSBIMG
        LOG_PRINT(debug_str, DEBUG_INFO, "2nd Boot Rom Sector Image Write to Flash\n");
        if (ret != 0)
        {
            ret = 1;
            LOG_PRINT(debug_str, DEBUG_ERROR, "2nd Boot Rom Sector Image Write Failed\n");
        }
        printf("2nd Boot Rom image flashing...done \n");
    }

    /*verify*/
    if (1) {
        BufVerify = calloc (1, BL2nd_img_size);
        ret = QicFlashRead(QIC1822_FLASH_2ndBL_ADDR, BufVerify, BL2nd_img_size,image_max_size);

        LOG_PRINT(debug_str, DEBUG_INFO, "Verify 2nd boot rom Sector Image ret=%d\n", ret);

        for (i = 0+QIC_IMG_HEADER_SIZE; i < BL2nd_img_size-QIC_IMG_HEADER_SIZE; i++)
        {
            if (BufVerify[i] != new_BL2nd_image[i])
            {
                ret = 1;
                BL2nd_fail=1;
                LOG_PRINT(debug_str, DEBUG_ERROR, "2nd boot rom Sector Image Verify Failed at %d, %x, %x\n", i, new_BL2nd_image[i], BufVerify[i]);
            }
        }

        free (BufVerify);

        if(BL2nd_fail){
            printf("\BL2nd Flash ERROR!!!!!!!!!!!!!\n");
        }
    }

    if(!BL2nd_fail){

        ret = QicFlashWrite(QIC1822_FLASH_2ndBL_ADDR, img_header, QIC_IMG_HEADER_SIZE, image_max_size);

        /*checking header*/
        ret = QicFlashRead(QIC1822_FLASH_2ndBL_ADDR, header_verify, QIC_IMG_HEADER_SIZE,image_max_size);

        if (header_verify[0] != 0x18&&header_verify[1] != 0x22){
            printf("QIC1822 header flash error\n");
            ret= 1;
        }

        printf("\ndownload 2nd Boot Rom ok\n");
    }
    else{
        printf("\ndownload 2nd Boot Rom error\n");
        ret=1;
    }
    if (ret) {
        goto ERR_CLOSE_MMAP;
    }

    /*release mem */
ERR_CLOSE_MMAP:
    if (new_BL2nd_image != orig_BL2nd_image) {
        munmap(new_BL2nd_image, BL2nd_img_size);
        close(BL2nd_img_fd);
    }

ERR_CLOSE:
    free(orig_BL2nd_image);

    return ret;
}

#endif

int qic_check_lock_stream_status( unsigned char *isLock, unsigned char *isStream)
{
    int qic_ret = 0;

#ifdef COMMIT_CHECK
    /* check if committed */
    if ((!config_is_commit) &&(dev_pt == NULL)) {
        LOG_PRINT(debug_str, DEBUG_ERROR, "QIC module library config is not committed\n");
        return 1;
    }
#endif
    qic_ret = QicGetCheckLockStreamStatus(isLock, isStream);
    LOG_XU_PRINT(debug_str, debug_xuctrl_str, qic_ret);

    if(!qic_ret){
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_check_lock_stream_status => lock %d, stream=%d\n", *isLock, *isStream);
    }else{
        LOG_PRINT(debug_str, DEBUG_INFO, "qic_check_lock_stream_status failed\n");
        qic_ret=1;
    }

    return qic_ret;
}

/* internal usage */
static int qic_backup_firmware (char *pathinfo, char *usb_image, char *parm_image, char *audio_image, char *osd_font_image)
{
    char cur_path[512];
    char *default_path = NULL;
    int cur_fd = -1;
    int ret = 0;
    int usb_image_max_size;
    int parm_image_max_size;
    int audio_image_max_size;
    int osd_font_image_max_size;
    /* set the backup folder */
    if (pathinfo == NULL) {
        LOG_PRINT(debug_str, DEBUG_INFO, "%s: no default path, do not backup\n", __func__);
        return 0; /* just no backup... */

    }
    else {
        LOG_PRINT(debug_str, DEBUG_INFO, "%s: use path %s for backup\n", __func__, pathinfo);
        default_path= pathinfo;
    }

    /* backup usb image */
    if (usb_image != NULL) {
        snprintf(cur_path, sizeof(cur_path), "%s/%s", default_path, QIC1822_USB_IMAGE_NAME);
        usb_image_max_size=QIC1822_FLASH_USB_MAX_SIZE;

        LOG_PRINT(debug_str, DEBUG_INFO, "backup USB image to %s\n", cur_path);

        cur_fd = open(cur_path, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO);
        if (cur_fd != -1) {
            if ( -1 == write (cur_fd, usb_image, usb_image_max_size)) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "can't write USB image: %s\n", strerror(errno));
                ret = 1;
            }

            if ( -1 == fsync(cur_fd)) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "can't fsync USB image: %s\n", strerror(errno));
                ret = 1;
            }

            if ( -1 == close(cur_fd)) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "can't close USB image: %s\n", strerror(errno));
                ret = 1;
            }

            cur_fd = -1;
        }
        else {
            LOG_PRINT(debug_str, DEBUG_ERROR, "Can't open the USB image block : %s\n", strerror(errno));
            ret = 1;
        }
    }

    LOG_PRINT(debug_str, DEBUG_INFO,"QIC1822 don't need to backup ISP image\n");

    /* backup parm image */
    if (parm_image != NULL) {
        snprintf(cur_path, sizeof(cur_path), "%s/%s", default_path, QIC1822_PARAM_IMAGE_NAME);
        parm_image_max_size=QIC1822_FLASH_PARAM_MAX_SIZE;

        LOG_PRINT(debug_str, DEBUG_INFO, "backup PARAM image to %s\n", cur_path);

        cur_fd = open(cur_path, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO);
        if (cur_fd != -1) {
            if ( -1 == write (cur_fd, parm_image, parm_image_max_size)) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "can't write PARAM image: %s\n", strerror(errno));
                ret = 1;
            }

            if ( -1 == fsync(cur_fd)) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "can't fsync PARAM image: %s\n", strerror(errno));
                ret = 1;
            }

            if ( -1 == close(cur_fd)) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "can't close PARAM image: %s\n", strerror(errno));
                ret = 1;
            }

            cur_fd = -1;
        }
        else {
            LOG_PRINT(debug_str, DEBUG_ERROR, "Can't open the PARAM image block : %s\n", strerror(errno));
            ret = 1;
        }
    }

    /* backup audio image */
    if (audio_image != NULL) {
        snprintf(cur_path, sizeof(cur_path), "%s/%s", default_path, QIC1822_AUDIO_IMAGE_NAME);
        audio_image_max_size=QIC1822_FLASH_AUDIO_PARAM_MAX_SIZE;

        LOG_PRINT(debug_str, DEBUG_INFO, "backup AUDIO image to %s\n", cur_path);

        cur_fd = open(cur_path, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO);
        if (cur_fd != -1) {
            if ( -1 == write (cur_fd, audio_image, audio_image_max_size)) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "can't write AUDIO image: %s\n", strerror(errno));
                ret = 1;
            }

            if ( -1 == fsync(cur_fd)) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "can't fsync AUDIO image: %s\n", strerror(errno));
                ret = 1;
            }

            if ( -1 == close(cur_fd)) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "can't close AUDIO image: %s\n", strerror(errno));
                ret = 1;
            }

            cur_fd = -1;
        }
        else {
            LOG_PRINT(debug_str, DEBUG_ERROR, "Can't open the AUDIO image block : %s\n", strerror(errno));
            ret = 1;
        }
    }

    /* backup osd font image */
    if (osd_font_image != NULL) {
        snprintf(cur_path, sizeof(cur_path), "%s/%s", default_path, QIC1822_OSD_FONT_IMAGE_NAME);
        osd_font_image_max_size=QIC1822_FLASH_OSD_FONT_MAX_SIZE;

        LOG_PRINT(debug_str, DEBUG_INFO, "backup OSD Font image to %s\n", cur_path);

        cur_fd = open(cur_path, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO);
        if (cur_fd != -1) {
            if ( -1 == write(cur_fd, osd_font_image, osd_font_image_max_size)) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "can't write OSD font image: %s\n", strerror(errno));
                ret = 1;
            }

            if ( -1 == fsync(cur_fd)) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "can't fsync OSD font image: %s\n", strerror(errno));
                ret = 1;
            }

            if ( -1 == close(cur_fd)) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "can't close OSD font image: %s\n", strerror(errno));
                ret = 1;
            }
            cur_fd = -1;
        }
        else
        {
            LOG_PRINT(debug_str, DEBUG_ERROR, "Can't open the OSD font image block : %s\n", strerror(errno));
            ret = 1;
        }
    }
    return ret;
}

static unsigned long CalculateCKSum(int image_max_size)
{
    int Section[] = { 0x00000, 0x3C000, 0x3D000, 0x3000 };//bypass UVC setting section
    unsigned long Sum = 0;
    int ret=0;
    unsigned char pTemp[0x40000];
    int i,idx;

    for ( i=0; i<4; i+=2)
    {
        ret=QicFlashRead( Section[ i ], pTemp, Section[ i + 1 ],image_max_size );

        for(  idx = 0; idx < Section[ i + 1 ]; idx++ )
            Sum += (unsigned int)pTemp[ idx ];
    }

    if(ret)Sum=0;
    //    printf( "SUM=%u\n", Sum );

    return Sum;
}

static int qic_dump_flash_firmware (char *pathinfo, char *dump_image,  int dump_image_size )
{
    char cur_path[512];
    char *default_path = NULL;
    int cur_fd = -1;
    int ret = 0;
    //	  struct tm *tblock;
    time_t curtime;
    curtime = time(NULL);

    //	  tblock = localtime(&curtime);
    //	  printf("Local time is: %s/n",asctime(tblock));

    /* set the backup folder */
    if (pathinfo == NULL) {
        LOG_PRINT(debug_str, DEBUG_INFO, "%s: no default path, do not backup\n", __func__);
        return 0; /* just no backup... */

    }
    else {
        LOG_PRINT(debug_str, DEBUG_INFO, "%s: use path %s for backup\n", __func__, pathinfo);
        default_path= pathinfo;
    }

    /* dump flash image */
    if (dump_image != NULL) {
        snprintf(cur_path, sizeof(cur_path), "%s/%ld%s", default_path,curtime, QIC_FLASH_DUMP_IMAGE_NAME);

        LOG_PRINT(debug_str, DEBUG_INFO, "dump flash image to %s\n", cur_path);

        cur_fd = open(cur_path, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO);
        if (cur_fd != -1) {
            if ( -1 == write (cur_fd, dump_image, dump_image_size)) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "can't write FLASH image: %s\n", strerror(errno));
                ret = 1;
            }

            if ( -1 == fsync(cur_fd)) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "can't fsync FLASH image: %s\n", strerror(errno));
                ret = 1;
            }

            if ( -1 == close(cur_fd)) {
                LOG_PRINT(debug_str, DEBUG_ERROR, "can't close FLASH image: %s\n", strerror(errno));
                ret = 1;
            }

            cur_fd = -1;
        }
        else {
            LOG_PRINT(debug_str, DEBUG_ERROR, "Can't open the FLASH image block : %s\n", strerror(errno));
            ret = 1;
        }
    }
    return ret;
}
