/*
 *
 * Author: Yankun Li <lioveni99@gmail.com>
 *
 */

#ifndef __GUARD_CEPH_H_
#define __GUARD_CEPH_H_

#include "config.h"
#include "list.h"

//typedef unsigned __int64 uint64_t;
//typedef unsigned __int32 uint32_t;

struct rados_pool_stat {
    uint64_t num_used_kb;
    uint64_t num_objects;
    uint64_t num_object_clones;
    uint64_t num_object_copies;

    uint64_t num_rb;
    uint64_t num_rd_kb;
    uint64_t num_wr;
    uint64_t num_wr_kb;
};

struct rados_pool {
    char * p_name;
    rados_ioctx_t p_ioctx;

    struct rados_pool_stat pool_stat;

    struct list_head p_list;
};

struct rados_cluster_pool {
    char *c_name;

    uint16_t c_num_pools;
    struct list_head c_pools_list;

    int c_has_initialized;

    struct list_head c_list;
};

extern int list_pools();
extern void init_pool_ioctx();

#endif
