/*
 *
 * Author: Yankun Li <lioveni99@gmail.com>
 *
 */

#ifndef __GUARD_CEPH_H_
#define __GUARD_CEPH_H_

#include "config.h"
#include "list.h"
#include "cJSON.h"

//typedef unsigned __int64 uint64_t;
//typedef unsigned __int32 uint32_t;

#define CEPH_STATUS        100

#define CEPH_OSD_TREE      101

#define CEPH_OSD_DF        102

struct command_result_t {
    int c_type;
    char * c_json;
    char * c_status;
    size_t c_json_len, c_status_len;

    cJSON * c_object;
    struct list_head c_list;
};

struct ceph_command_t {
    char * c_name;
    int c_type;
    char * c_command[2];
    struct list_head c_list;

    struct command_result_t * c_result_ptr;
};

struct rados_pool_stat {
    uint64_t num_used_kb;
    uint64_t num_objects;
    uint64_t num_object_clones;
    uint64_t num_object_copies;

    uint64_t num_rd;
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

struct rados_cluster_pool_t {
    char *c_name;

    uint16_t c_num_pools;
    struct list_head c_pools_list;

    int c_has_initialized;

    struct list_head c_list;
};

struct ceph_commands_t {
    char * c_name;

    uint16_t c_count;
    struct list_head c_commands_list;

    struct list_head c_list;

};

struct commands_result_t {
    char * c_name;

    uint16_t c_count;
    struct list_head c_cmd_result_list;

    struct list_head c_list;
};

extern int list_pools();
extern void init_pool_ioctx();
extern int update_pool_stat();
extern void read_pools_stat();
extern int send_mon_command();
extern void read_cluster_info();

#endif
