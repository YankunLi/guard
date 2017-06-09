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
#include "group.h"

static struct guard_module ceph_ops;

static int cluster_initialized = 0;
rados_t cluster;
static char cluster_name[] = "ceph", user_name[] = "client.admin";
static char * c_path = "/etc/ceph/ceph.conf";
uint64_t flags;

static struct element_group *grp;

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
        cluster_initialized = 1;
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
    if (!(grp = group_lookup("ceph-pools", 1)))
            BUG();
    list_pools();
    init_pools_ioctx();

    return 0;
}

static void ceph_read(void)
{
    read_info();
   // read_pools_stat();
   // send_mon_command();
    //command_osd_tree();
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

static void __attribute__ ((destructor)) distroy_handle(void)
{
    if (cluster_initialized)
        rados_shutdown(cluster);
}

#endif
