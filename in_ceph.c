/*
 *
 * Author: Yankun Li <lioveni99gmail.com>
 *
 */

#ifndef __GUARD_IN_CEPH_H_
#define __GUARD_IN_CEPH_H_

#include "config.h"
#include "list.h"
#include "input.h"
#include "module.h"
#include "ceph.h"

static struct guard_module ceph_ops;

static rados_t cluster;
static char cluster_name[] = "ceph", user_name[] = "client.admin";
static char * c_path = "/etc/ceph/ceph.conf";
uint64_t flags;

struct rados_cluster_pool cluster_pool =
{
    .c_name = "ceph",
    .c_num_pools = 0,
    .c_pools_list = LIST_SELF(cluster_pool.c_pools_list),
    .c_has_initialized = 0,
};

static int ceph_probe(void)
{
    printf("ceph module is ok\n");

    return 0;
}

static void ceph_parse_opt(const char * type, const char * value)
{}

static int ceph_do_init(void)
{
    DBG("init ceph module");

    int err;
    //create a cluster handle.
    err = rados_create2(&cluster, cluster_name, user_name, flags);
    if (err < 0) {
        DBG("Couldn't create the cluster handle!");
        BUG();
    } else {
        DBG("Create a cluster handle.");
    }

    //read ceph config file.
    err = rados_conf_read_file(cluster, c_path);
    if (err < 0) {
        DBG("cannot read config file %s", c_path);
    } else {
        DBG("Read the config file.");
    }

    //connect to the cluster.
    err = rados_connect(cluster);
    if (err < 0) {
        DBG("cannot connect to cluster.");
    } else {
        DBG("Connected to the cluster.");
    }
    return 0;
}

static int __show_pools()
{
    if(cluster_pool.c_has_initialized)
        return 0;

    int buf_sz = rados_pool_list(cluster, NULL, 0);
    char buf[buf_sz];
    int ret = rados_pool_list(cluster, buf, buf_sz);
    if (ret != buf_sz)
    {
        return -1;
    }
    const char *b = buf;
    struct rados_pool * pool_ptr;
    while (1){
        if ('\0' == b[0])
            break;
        pool_ptr = (struct rados_pool *) malloc(sizeof(struct rados_pool));
        pool_ptr->p_name = (char *) malloc(sizeof(b));
        strcpy(pool_ptr->p_name, b);
        cluster_pool.c_num_pools++;
        list_add_head(&pool_ptr->p_list, &cluster_pool.c_pools_list);

        b += strlen(b) + 1;
    }

    if (cluster_pool.c_num_pools)
        cluster_pool.c_has_initialized = 1;

    struct rados_pool *p;
    list_for_each_entry(p, &cluster_pool.c_pools_list, p_list)
        printf("'%s' \t", p->p_name);

    return 0;
}

static void ceph_read(void)
{
    __show_pools();
//    DBG("I am ceph module");
}

static struct guard_module ceph_ops = {
    .m_name = "ceph",
    .m_do = ceph_read,
    .m_parse_opt = ceph_parse_opt,
    .m_probe = ceph_probe,
    .m_init = ceph_do_init,
};

static void __attribute__ ((constructor)) ceph_init(void)
{
    DBG("register input module %s", ceph_ops.m_name);

    input_register(&ceph_ops);
}

#endif
