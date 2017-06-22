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

#define CEPH_OSD_PERF      103

#define CEPH_OSD_METADATA  104

struct cmd_result_t {
    char * c_name;
    int c_type;

    char * c_json;
    char * c_status;
    size_t c_json_len, c_status_len;

    cJSON * c_root_object;
    struct list_head c_list;
};

struct ceph_cmd_t {
    char * c_name;
    int c_type;
    char * c_cmd[2];
    struct list_head c_list;

    struct cmd_result_t * c_result_ptr;
};

struct ceph_cmds_t {
    char * c_name;

    uint16_t c_count;
    struct list_head c_cmds;

    struct list_head c_list;

};

struct cmds_result_t {
    char * c_name;

    uint16_t c_count;
    struct list_head c_cmd_results;

    struct list_head c_list;
};

struct pool_info_t {
    uint64_t num_used_kb;
    uint64_t num_objects;
    uint64_t num_object_clones;
    uint64_t num_object_copies;

    uint64_t num_rd;
    uint64_t num_rd_kb;
    uint64_t num_wr;
    uint64_t num_wr_kb;
};

struct rados_pool_t {
    char * p_name;

    int ioctx_initialized;
    rados_ioctx_t p_ioctx;

    struct pool_info_t p_pool_info;

    struct list_head p_list;
};

struct rados_cluster_pool_t {
    char *c_name;

    uint16_t c_num_pools;
    struct list_head c_pools;

    int c_has_initialized;

    struct list_head c_list;
};

struct osd_perf_t {
    int commit_latency_ms;
    int apply_latency_ms;
};

struct osd_server_t {
    char * o_arch;
    char * o_ceph_version;
    char * o_filestore_backend;
    char * o_front_addr;
    char * o_hb_back_addr;
    char * o_hb_front_addr;
    char * o_hostname;
    char * o_osd_data;
    char * o_osd_journal;
    char * o_osd_objectstore;

    struct list_head o_list;
};

struct nodes_t {
    char * n_name;
    char * n_id;
    int n_reweight;
    int n_raw_kb;
    int n_raw_kb_used;
    int n_raw_kb_avail;
    double n_utilization;
    double n_var;

    char * n_osd_data;

    struct osd_perf_t n_perf_stats;

    struct osd_server_t * n_osd_server;

    struct list_head n_list;
};

struct rados_osds_t {
    struct list_head rados_osds;
};


extern int read_info();
extern int ceph_init();
extern void close_cluster();

#endif
