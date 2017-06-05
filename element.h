/*
 *
 *
 * Author: YankunLi <lioveni99@gmail.com>
 *
 */

#ifndef __GUARD_ELEMENT_H_
#define __GUARD_ELEMENT_H_

#include "list.h"

struct element
{
    char * e_name;
    char * e_description;
    uint32_t e_id;
    uint32_t e_flags;
    unsigned int e_lifecycles;
    unsigned int e_level;

    struct element * e_parent;
    struct element_group * e_group;

    struct list_head e_list;
    struct list_head e_childs;

    unsigned int e_nattrs;
    struct list_head e_attrhash[32];
    struct list_head e_attr_sorted;

    unsigned int e_ninfo;
    struct list_head e_info_list;

    float e_rd_kB, e_wr_kB;
};

#endif
