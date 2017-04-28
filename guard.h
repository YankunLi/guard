#ifndef __GUARD_GUARD_H_
#define __GUARD_GUARD_H_

#include "config.h"



typedef struct timestamp_s
{
    int64_t tv_sec;
    int64_t tv_usec;
} timestamp_t;

struct reader_timing
{
    timestamp_t rt_last_read;
    timestamp_t rt_next_read;

    struct {
        float v_error;
        float v_max;
        float v_min;
        float v_total;
    } rt_variance;
};

#define BUG()                  \
    do {                      \
        fprintf(stderr, "BUG: %s:%d\n", __FILE__, __LINE__);    \
        assert(0);     \
        exit(EINVAL);  \
    } while(0)

#define ARRAY_SIZE(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))

#define DEBUG 1

#ifdef DEBUG
#define DBG(FMT, ARG...) \
    do {                     \
        fprintf(stderr,           \
                "[DBG] %20s:%-4u %s: " FMT "\n",    \
                __FILE__, __LINE__, __PRETTY_FUNCTION__, ##ARG); \
    } while(0)
#else
#define DBG(FMT, ARG...)    \
    do {    \
    } while(0)
#endif

#endif
