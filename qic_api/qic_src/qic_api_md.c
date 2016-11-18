/*
--       This software proprietary belong to Quanta computer Inc. and may be used        --
--        only as expressly authorized agreement from          --
--                            Quanta Computer Inc..                             --
--                   (C) COPYRIGHT 2010 Quanta Computer Inc.                    --
--                            ALL RIGHTS RESERVED                             --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
*/

#include "../qic_include/qic_api_md.h"

// Common part
extern char debug_str[1024];
extern char debug_xuctrl_str[512];
extern int config_is_commit;
extern qic_module *dev_pt;
// End of common part


#ifdef QIC_MD_API
static const unsigned char guid[] = {0x19, 0xA3, 0x39, 0x57, 0x4F, 0xBD, 0x4A, 0x0D, 0xB7, 0xCF, 0xE5, 0xD2, 0xC8, 0xB2, 0xDD, 0x5D};

unsigned int AnalyzeSEIMessages(unsigned char* buffer, unsigned int size, unsigned int* sei_begin_offset, md_status_t* md_status)
{
    unsigned int i;
    bool foundSEI = false;
    unsigned char* sei;
    unsigned int sei_size;
    int top = 0;

    /*DEBUG("0x%02x 0x%02x 0x%02x 0x%02x  0x%02x 0x%02x 0x%02x 0x%02x  0x%02x 0x%02x 0x%02x 0x%02x  0x%02x 0x%02x 0x%02x 0x%02x",
         buffer[0], buffer[1], buffer[2], buffer[3],
            buffer[4], buffer[5], buffer[6], buffer[7],
            buffer[8], buffer[9], buffer[10], buffer[11],
            buffer[12], buffer[13], buffer[14], buffer[15]
            );*/

    /* search for the SEI uuid */
    for(i=0;i<size-16;i++)
    {
        if(buffer[i] == guid[0])
        {
            if(memcmp(&buffer[i], guid, sizeof(guid)) == 0)
            {
                foundSEI = true;
                *sei_begin_offset = i - 7;
                sei = &buffer[i+16];
                sei_size =buffer[i-1] - 16;

                break;
            }
        }
    }
    //  printf("foundSEI=%d\n", foundSEI);
    if(foundSEI == false)
        return 0;

    memset(md_status, 0, sizeof(md_status_t));

    /* parse key/value */
    int codepage = 0;
    for(i=0;i<sei_size;i++)
    {
        int type = sei[i];
        int len = type >> 6;
        int value = 0;

        type = type & 0x3F;
        //      printf("type=0x%x\n", foundSEI);
        unsigned char* ptr = &sei[i+1];
        switch(len)
        {
        case 3:
            value = *ptr++;
            i++;
            // fall through
        case 2:
            value <<=8;
            value |= *ptr++;
            i++;
            // fall through
        case 1:
            value <<=8;
            value |= *ptr++;
            i++;
            // fall through
        case 0:
            value <<=8;
            value |= *ptr++;
            i++;
            // fall through
        }

        if(type == DBG_SEI_CODE_PAGE)
        {
            codepage = value;
        }
        else if(codepage == 1)
        {
            switch(type)
            {
            case DBG_SEI_MD_TIMESTAMP:
                md_status->timestamp = value;
                //			printf("timestamp=%d\n",value);
                break;
            case DBG_SEI_MD_NUM_OF_MOVING_OBJS:
                md_status->number_of_moving_objects = value;
                //			printf("OBJ=%d\n",value);
                break;
            case DBG_SEI_MD_X:
                md_status->moving_objects[top].x = value;
                //			printf("X=%d\n",value);
                break;
            case DBG_SEI_MD_Y:
                md_status->moving_objects[top].y = value;
                //			printf("Y=%d\n",value);
                break;
            case DBG_SEI_MD_WIDTH:
                md_status->moving_objects[top].width = value;
                //			printf("W=%d\n",value);
                break;
            case DBG_SEI_MD_HEIGHT:
                md_status->moving_objects[top].height = value;
                //			printf("H=%d\n",value);
                top++;
                break;
            default:
                printf("got unknown entry (%d, %d, %d)", codepage, type, value);
            }
        }
    }

    unsigned int sei_end_offset = (unsigned int)(sei - buffer) + sei_size + 1;
    return sei_end_offset;
}

/* Motion detection APIs */
int qic_md_get_version(unsigned int dev_id,int *major_version, int *minor_version)
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
            ret=QicMDGetVersion(major_version, minor_version);
        }
    }

    return ret;
}

int qic_md_start_stop(unsigned int dev_id, unsigned char on, unsigned short stream_id)
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
            ret=QicMmioWrite(0x6F0000A0, (unsigned int)on); //Enable MD info in SEI of H.264
            ret=QicMDSetEnable(on, stream_id);
#ifdef DEBUG_LOG
            unsigned char md_status;
            QicMDGetEnable (&md_status);
            printf("QicMDGetEnable=======%d\n", md_status);
#endif
        }
    }

    return ret;
}

int qic_md_change_config(unsigned int dev_id,md_config_t* config, unsigned char interrupt_mode)
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
            ret=QicMDSetInterruptMode(interrupt_mode);

            if(ret){
                LOG_PRINT(debug_str, DEBUG_ERROR, "QicMDSetInterruptMode failed\n");
                return 1;
            }
            ret=QicMDSetConfiguration(config);

            if(ret){
                LOG_PRINT(debug_str, DEBUG_ERROR, "QicMDSetConfiguration failed\n");
            }
        }
    }

    return ret;
}

int qic_md_get_config(unsigned int dev_id,md_config_t* config, unsigned char* interrupt_mode)
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
            ret= QicMDGetInterruptMode(interrupt_mode);
            if(ret){
                LOG_PRINT(debug_str, DEBUG_ERROR, "QicMDGetInterruptMode failed\n");
            }
            ret= QicMDGetConfiguration(config);
            if(ret){
                LOG_PRINT(debug_str, DEBUG_ERROR, "QicMDGetConfiguration failed\n");
            }
        }
    }

    return ret;
}

int qic_md_get_status(unsigned int dev_id,md_status_t* status)
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
            ret= QicMDGetStatus(status);
            if(ret){
                LOG_PRINT(debug_str, DEBUG_ERROR, "QicMDGetStatus failed\n");
            }
        }
    }

    return ret;
}

int qic_td_get_version(unsigned int dev_id,int *major_version, int *minor_version)
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
            ret=QicTDGetVersion(major_version, minor_version);
        }
    }

    return ret;
}

int qic_td_start_stop(unsigned int dev_id,unsigned char on)
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

            ret=QicTDSetEnable(on);
#ifdef DEBUG_LOG
            unsigned char td_status;
            QicTDGetEnable (&td_status);
            printf("QicTDGetEnable=======%d\n",td_status);
#endif
        }
    }

    return ret;
}

int qic_td_change_config(unsigned int dev_id,td_config_t* config)
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

            ret=QicTDSetConfiguration(config);

            if(ret)LOG_PRINT(debug_str, DEBUG_ERROR, "QicTDSetConfiguration failed\n");
        }
    }

    return ret;
}

int qic_td_get_config(unsigned int dev_id,td_config_t* config)
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

            ret= QicTDGetConfiguration(config);
            if(ret){
                LOG_PRINT(debug_str, DEBUG_ERROR, "QicTDGetConfiguration failed\n");

            }

        }
    }

    return ret;
}

int qic_td_get_status(unsigned int dev_id,int* status)
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

            ret= QicTDGetStatus(status);
            if(ret){
                LOG_PRINT(debug_str, DEBUG_ERROR, "QicTDGetStatus failed\n");

            }

        }
    }

    return ret;
}

#endif
