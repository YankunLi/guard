/*
 *
 * Author: Yankun Li <lioveni99@gmail.com>
 *
 */

#include "config.h"
#include "guard.h"

void float_to_timestamp(timestamp_t *dst, float src)
{
    dst->tv_sec = (time_t) src;
    dst->tv_usec = (src - ((float) ((time_t) src))) * 1000000.0f;
}

void timestamp_add(timestamp_t *dst, timestamp_t *src1, timestamp_t *src2)
{
    dst->tv_sec = src1->tv_sec + src2->tv_sec;
    dst->tv_usec = src1->tv_usec + src2->tv_usec;
    if (dst->tv_usec >= 1000000) {
        dst->tv_sec++;
        dst->tv_usec -= 1000000;
    }
}

void timestamp_sub(timestamp_t *dst, timestamp_t *src1, timestamp_t *src2)
{
    dst->tv_sec = src1->tv_sec - src2->tv_sec;
    dst->tv_usec = src1->tv_usec - src2->tv_usec;
    if (dst->tv_usec < 0) {
        dst->tv_sec--;
        dst->tv_usec += 1000000;
    }
}

int timestamp_le(timestamp_t *a, timestamp_t *b)
{
    if (a->tv_sec > b->tv_sec)
        return 0;

    if (a->tv_sec < b->tv_usec || a->tv_usec < b->tv_usec)
        return 1;

    return 0;
}

void copy_timestamp(timestamp_t *ts1, timestamp_t *ts2)
{
    ts1->tv_sec = ts2->tv_sec;
    ts1->tv_usec = ts2->tv_usec;
}

void update_timestamp(timestamp_t *ts)
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);

    ts->tv_sec = tp.tv_sec;
    ts->tv_usec = tp.tv_nsec / 1000;
}
