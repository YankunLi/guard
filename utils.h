/*
 *
 * Author: Yankun Li <lioveni99@gmail.com>
 *
 */

#ifndef __GUARD_UTILS_H_
#define __GUARD_TUILS_H_

#include "guard.h"

extern void float_to_timestamp(timestamp_t *dst, float src);
extern void timestamp_add(timestamp_t *dst, timestamp_t *src1, timestamp_t *src2);
extern void timestamp_sub(timestamp_t *dst, timestamp_t *src1, timestamp_t *src3);
extern void update_timestamp(timestamp_t *dst);
extern int timestamp_le(timestamp_t *a, timestamp_t *b);
extern void copy_timestamp(timestamp_t *ts1, timestamp_t *ts2);

#endif
