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
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "../qic_include/qic_utility_for_demo.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

char * mem_usage_info(void)
{
    char buf[256];
    static char mem_print[256];
    FILE * file;
    unsigned int pages;
    unsigned long mem_size;
    char *ret = NULL;

    snprintf( buf, sizeof(buf), "/proc/%d/statm", (unsigned int) getpid() );

    if ((file = fopen( buf, "r" )) == NULL){
        perror( "open" );
        return 0;
    }

    ret = fgets( buf, sizeof(buf), file );

    if (ret == NULL)
        return 0;

    fclose( file );

    sscanf( buf, "%u", & pages );
    mem_size = ((unsigned long) pages) * ((unsigned long) getpagesize());

    memset(mem_print, 0, sizeof(mem_print));
    snprintf(mem_print, sizeof(mem_print), "memory usage = %lu bytes\n", mem_size);

    return mem_print;
}

unsigned int myatoi(char *str)
{
    char *cstr;
    char *p = str;
    unsigned int val;
    int i = 0;

    if ((*p == '0') && ((*(p+1) == 'x') || (*(p+1) == 'X')))
    {
        cstr = p+2;
        sscanf(cstr, "%x", &val);
    }
    else
    {
        val = atoi (str);
    }

    return val;
}
