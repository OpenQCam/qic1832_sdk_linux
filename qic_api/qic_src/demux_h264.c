/*
--       This software proprietary belong to Quanta computer Inc. and may be used        --
--        only as expressly authorized agreement from          --
--                            Quanta Computer Inc..                             --
--                   (C) COPYRIGHT 2010 Quanta Computer Inc.                    --
--                            ALL RIGHTS RESERVED                             --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../qic_include/demux_h264.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

#ifdef MARVELL_VPRO
#define memcpy arm9_memcpy
#define memset arm9_memset
#endif

static char guid[] = {0x19, 0xA3, 0x39, 0x57, 0x4F, 0xBD, 0x4A, 0x0D, 0xB7, 0xCF, 0xE5, 0xD2, 0xC8, 0xB2, 0xDD, 0x5D};

#define Read_Byte(x) *(x)

#define Read_2Byte(x) ((*(x) << 8) + (*((x) + 1)))

#define Read_3Byte(x) ((*(x) << 16) + (*((x)+1) << 8) + (*((x)+2)))

#ifdef CHECK_FRAM_ENUM
static unsigned int prov_stream_num=0;
#endif

int last_frame_size;

/*----------------------------------------------------------------------------
 * Functions
 *--------------------------------------------------------------------------*/
void demux_VP8_H264_check_bad_frame_initial(void)
{
    last_frame_size=0;
}

int demux_VP8_check_bad_frame( char **src , unsigned int *size)
{
    int ret=0,expected_size,actual_size;
    unsigned int i;
    quanta_debug_header_t *quanta_debug_header=(quanta_debug_header_t *)(*src);

    //printf("src=0x%lx\n",src);
    //printf("*src=0x%lx\n",*src);
    //printf("**src=0x%lx\n",**src);
    /*
    for(i=0;i<quanta_debug_header->payload_length+8;i=i+8){
        printf("0x%x, 0x%x, 0x%x, 0x%x\n",*(*src+i),*(*src+i+1),*(*src+i+2),*(*src+i+3));
        printf("0x%x, 0x%x, 0x%x, 0x%x\n",*(*src+i+4),*(*src+i+5),*(*src+i+6),*(*src+i+7));
    }
*/
    //printf("quanta_debug_header->syncword=0x%x\n",quanta_debug_header->syncword);
    //printf("quanta_debug_header->payload_length=%d\n",quanta_debug_header->payload_length);
    if (quanta_debug_header->syncword == 0x30385056)
    {
        // Parse Data and print message
        for (i=0;i<quanta_debug_header->payload_length;i=i+8){

            switch(*((unsigned int *)(*src + sizeof(quanta_debug_header_t) + i))){

            case DBG_SEI_FRAME_SIZE:
                expected_size = *((unsigned int *)(*src + sizeof(quanta_debug_header_t) + i + 4));
                actual_size = *size - quanta_debug_header->payload_length - sizeof(quanta_debug_header_t);
                //printf("VP8 DBG_SEI_FRAME_SIZE = %d (actual %d)\n", expected_size, actual_size);

                if(!(actual_size >= expected_size && actual_size < expected_size+4)){
                    printf("\nError!! VP8 DBG_SEI_FRAME_SIZE check Fail...!!\n");
                    ret = 1;
                }
                break;

            case DBG_SEI_LAST_FRAME_SIZE:
                expected_size = *((unsigned int *)(*src + sizeof(quanta_debug_header_t) + i + 4));
                //	printf("VP8 DBG_SEI_LAST_FRAME_SIZE = %d (actual %d)\n", expected_size, last_frame_size);

                if(!(last_frame_size >= expected_size && last_frame_size < expected_size+4) && last_frame_size!=0){
                    printf("\nError!! VP8 DBG_SEI_LAST_FRAME_SIZE check Fail...!!\n");
                    ret = 1;
                }
                break;

            default:
                //printf("\nUnknown VP8 type (%d) = %d\n", *((unsigned int *)(*src + sizeof(quanta_debug_header_t) +i)),
                //							*((unsigned int *)(*src + sizeof(quanta_debug_header_t) +i + 4)));
                break;
            }
        }

        //printf("*src=0x%lx\n",*src);
        //printf("*size=%d\n",*size);
        *src += (quanta_debug_header->payload_length + sizeof(quanta_debug_header_t));
        *size -= (quanta_debug_header->payload_length  + sizeof(quanta_debug_header_t));
        last_frame_size = *size;
        //printf("*src=0x%lx\n",*src);
        //printf("*size=%d\n",*size);
    }
    else{
        printf("\nWarning!! VP8 Debug syncword was not found...!!\n");
        printf("Please make sure your camera firmware has support this VP8 bad frame check function!!\n\n");
        ret = 1;
    }

    return ret;
}

int demux_H264_check_bad_frame( char *src , unsigned int size)
{
    int ret=0,expected_size,actual_size;
    unsigned int i;
    char found_sei = 0;

    if(size < 16) return 1;

    /* search for guid */
    for(i=0;i<size-16;i++){
        //printf("src[%d]=0x%x\n",i,src[i]);
        if(src[i] == guid[0]){
            if(memcmp(&src[i], guid, sizeof(guid)) == 0){
                //printf("\nH264 SEI was found...!!\n");
                found_sei = 1;
                break;
            }
        }

        if(src[i+0] == 0x00 &&
                src[i+1] == 0x00 &&
                src[i+2] == 0x00 &&
                src[i+3] == 0x01 &&
                ((src[i+4]&0x1F) == 0x05 || (src[i+4]&0x1F) == 0x01)){

            printf("\nWarning!! H264 Debug GUID was not found...!!\n");
            printf("Please make sure your camera firmware has support this H264 bad frame check function!!\n");
            return 1;
        }
    }

    if(found_sei == 0){
        printf("\nError!! H264 SEI was not found...!!\n");
        return 1;
    }

    /* get payload size */
    int payload_size = src[i-1];
    //printf("payload_size=%d, i=%d\n",payload_size,i);
    if(payload_size < 16){
        printf("\nInvalid debug sei payload\n");
        return 1;
    }
    if(payload_size > size){
        printf("\ndebug sei payload too large\n");
        return 1;
    }

    /* decode each fields */
    char *ptr = &src[i+16];
    for(i=0;i<payload_size-16;i++){

        int type = *ptr++;
        int len = type >> 6;
        type = type & 0x3F;

        unsigned int value = 0;
        //printf("len=%d\n",len);
        switch(len)
        {
        case 3:
            value = *ptr++;
            //printf("value3=0x%x\n",value);
            i++;
            // fall through
        case 2:
            value <<=8;
            value |= *ptr++;
            //printf("value2=0x%x\n",value);
            i++;
            // fall through
        case 1:
            value <<=8;
            value |= *ptr++;
            //printf("value1=0x%x\n",value);
            i++;
            // fall through
        case 0:
            value <<=8;
            value |= *ptr++;
            //printf("value0=0x%x\n",value);
            i++;
            // fall through
        }

        switch(type){

        case DBG_SEI_FRAME_SIZE:
            actual_size = size - payload_size - 8;
            expected_size = value;
            //printf("H264 DBG_SEI_FRAME_SIZE = %d (actual %d)\n", expected_size, actual_size);

            if(!(actual_size >= expected_size && actual_size < expected_size+4)){
                printf("\nError!! H264 DBG_SEI_FRAME_SIZE check Fail...!!\n");
                ret = 1;
            }
            break;

        case DBG_SEI_LAST_FRAME_SIZE:
            expected_size = value;
            //printf("H264 DBG_SEI_LAST_FRAME_SIZE = %d (actual %d)\n", expected_size, last_frame_size);
            if(!(last_frame_size >= expected_size && last_frame_size < expected_size+4) && last_frame_size!=0){
                printf("\nError!! H264 DBG_SEI_LAST_FRAME_SIZE check Fail...!!\n");
                ret = 1;
            }
            break;

        default:
            if(!type==0x37||!type==1||!type==2||!type==3||!type==4||!type==5||!type==6){
                printf("\nUnknown H264 type (%d) = %d\n", type, value);
                ret = 1;
            }
            break;
        }
    }

    last_frame_size = size - payload_size - 8;

    return ret;
}

int decode_pps_id(unsigned char* data)
{
    unsigned int buffer;
    int leading_zero;

    buffer = (data[0]<<24) | (data[1]<<16) | (data[2]<<8) | (data[3]<<0);

    /* skip first_mb_in_slice */
    leading_zero = 0;
    while(!(buffer & 0x80000000))
    {
        leading_zero++;
        buffer <<=1;
    }
    buffer <<= (1+leading_zero);

    /* skip slice_type */
    leading_zero = 0;
    while(!(buffer & 0x80000000))
    {
        leading_zero++;
        buffer <<=1;
    }
    buffer <<= (1+leading_zero);

    /* extract pps_id */
    leading_zero = 0;
    while(!(buffer & 0x80000000))
    {
        leading_zero++;
        buffer <<=1;
    }
    buffer <<= 1;
    if(leading_zero == 0)
        buffer = 0;
    else
        buffer >>= (32-leading_zero);

    return (1<<leading_zero) + buffer - 1;
}

int get_avc_stream_id(unsigned char* data, unsigned int data_size)
{
    /* search for slice header */
    int pps_id = 0;
    unsigned int i;

    for(i=0;(i+9)<data_size;i++)
    {
        if(data[i+0] == 0x00 &&
                data[i+1] == 0x00 &&
                data[i+2] == 0x00 &&
                data[i+3] == 0x01 &&
                ((data[i+4]&0x1F) == 0x05 || (data[i+4]&0x1F) == 0x01))
        {
            /* decode pps id from slice header */
            pps_id = decode_pps_id(&data[i+5]);
            break;
        }
    }

    return pps_id;
}

int get_stream_temporal_id(unsigned char* data, unsigned int data_size){
    /* search for slice header */
    int temporal_id=0;
    int i;
    for(i=0;i<data_size-16;i++){
        if(data[i+0] == 0x00 &&
                data[i+1] == 0x00 &&
                data[i+2] == 0x00 &&
                data[i+3] == 0x01 &&
                (data[i+4]&0x0F) == 0x0E &&
                (data[i+5]&0x80) == 0x80 )
        {
            temporal_id =((data[i+7]&0xE0))>>5;
            break;
        }
    }

    return temporal_id;
}

int check_for_P_frame(unsigned char* data, unsigned int data_size)
{
    /* search for slice header */
    int is_p_frame=0;
    int i;
    char found_i=0;

    for(i=0;i<data_size-5;i++)
    {
        if(data[i+0] == 0x00 &&
                data[i+1] == 0x00 &&
                data[i+2] == 0x00 &&
                data[i+3] == 0x01 &&
                (((data[i+4]&0x1F) == 0x05)||((data[i+4]&0x1F) == 0x07)||((data[i+4]&0x1F) == 0x08)))
        {
            if(data[i+0] != 0x00 ||data[i+1] != 0x00 ||data[i+2] != 0x00 )
                printf("I=0x%x,0x%x,0x%x,0x%x, i=%d, size=%d\n",data[1],data[i+1],data[i+2],data[i+4],i, data_size);
            //    printf("I=0x%x\n",data[i+4]);
            is_p_frame=0;
            found_i=1;
            break;
        }

        if(found_i==0 &&
                data[i+0] == 0x00 &&
                data[i+1] == 0x00 &&
                data[i+2] == 0x00 &&
                data[i+3] == 0x01 &&
                (((data[i+4]&0x1F) == 0x01 )))
        {
            if(data[i+0] != 0x00 ||data[i+1] != 0x00 ||data[i+2] != 0x00 )
                printf("P=0x%x,0x%x,0x%x,0x%x, i=%d, size=%d\n",data[1],data[i+1],data[i+2],data[i+4],i, data_size);

            //	printf("P=0x%x",data[i+4]);
            is_p_frame=1;
            break;
        }
    }

    if(!found_i && !is_p_frame) {
        is_p_frame=2;
        printf("Unknow Type:: data_size=%d",data_size);

    }

    return is_p_frame;
}
