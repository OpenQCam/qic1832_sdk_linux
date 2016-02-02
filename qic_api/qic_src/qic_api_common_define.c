/*
--       This software proprietary belong to Quanta computer Inc. and may be used        --
--        only as expressly authorized agreement from          --
--                            Quanta Computer Inc..                             --
--                   (C) COPYRIGHT 2010 Quanta Computer Inc.                    --
--                            ALL RIGHTS RESERVED                             --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
*/

#include "../qic_include/qic_api_common_define.h"

// Common part, define
char debug_str[1024] = {0}; /* debug string output */
char debug_xuctrl_str[512] = {0}; /* debug string for xuctrl */

int config_is_commit = 0; /* easy check whether config is commit */
qic_module *dev_pt = NULL; /* global device ref pt */
// End of common part

int xioctl(int fd, int request, void * arg)
{
    int r;

    do r = ioctl(fd, request, arg);
    while (-1 == r && EINTR == errno);

    return r;
}
