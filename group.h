/*
 *
 *
 * Author: YankunLi <lioveni99@gmail.com>
 *
 */

#ifndef __GUARD_GROUP_H_
#define __GUARD_GROUP_H_

#include "list.h"
#include "element.h"

#define GROUP_COL_MAX 5

struct group_hdr
{
    char * gh_name;
    char * gh_title;
    char * gh_column[GROUP_COL_MAX];
    struct list_head gh_list;
};

struct element_group
{
    char * g_name;
    struct group_hdr * g_hdr;

    struct list_head g_elements;
    unsigned int g_nelements;

    struct element * g_current;

    struct list_head g_list;
};

extern void group_foreach(void (*cb)(struct element_group *, void *), void *arg);
extern struct element_group *group_lookup(const char *name, int flags);
#endif
