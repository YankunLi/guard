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
//    uint32_t e_flags;
//    unsigned int e_lifecycles;
//    unsigned int e_level;

    struct element_group * e_group;

    struct list_head e_list;

    unsigned int e_nattrs;
    struct list_head e_attrs;
//    struct list_head e_attrhash[32];

    unsigned int e_ninfo;
//    struct list_head e_info_list;

    uint64_t e_num_used_kb;
    uint64_t e_num_objects;
    uint64_t e_num_object_clones;
    uint64_t e_num_object_copies;

    uint64_t e_num_rd;
    uint64_t e_num_rd_kb;
    uint64_t e_num_wr;
    uint64_t e_num_wr_kb;
};

struct global_osdmap_t {
    unsigned long g_epoch;
    uint16_t g_num_osds;
    uint16_t g_num_up_osds;
    uint16_t g_num_in_osds;
    int g_full;
    int g_nearfull;
    int g_num_remapped_pgs;
};

struct global_usage_t {
    int num_pgs;
    uint64_t data_bytes;
    uint64_t bytes_used;
    uint64_t bytes_avail;
    uint64_t bytes_total;

    struct rate_t {
        uint64_t read_bytes_sec;
        uint64_t write_bytes_sec;
        uint32_t op_per_sec;
    } usage_rate;
};

struct mon_t {
    char m_name[40];
    uint64_t m_kb_total;
    uint64_t m_kb_used;
    uint64_t m_kb_avail;
    int m_avail_percent;

    char m_addr[40];
    int m_rank;
    char m_health[20];

    struct list_head m_list;
};

struct global_mon_t {
    int g_mon_size;
    struct list_head g_mons;
};

struct global_info_t {
    //osdmap
    struct global_osdmap_t g_storage_servers;

    //pgmap
    struct global_usage_t g_usage;

    //monmap
    struct global_mon_t *g_mon_servers;

    char g_fsid[40];
    char g_status[20];
    char g_summary[64];
};

extern struct global_info_t * get_global_info(void);
extern struct element *element_lookup(struct element_group *group, const char *name, int flags);
extern struct global_mon_t *get_global_mons();
#endif
