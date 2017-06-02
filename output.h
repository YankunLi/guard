/*
 *
 * Author: Yankun Li <lioveni99@gmail.com>
 *
 */

#ifndef __GUARD_OUTPUT_H_
#define __GUARD_OUTPUT_H_

#include "guard.h"
#include "module.h"

extern void output_register(struct guard_module *);
//extern int output_set(const char *);
extern void output_pre(void);
extern void output_draw(void);
extern void output_post(void);

#endif
