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
static int initilized;

static int ceph_probe(void)
{
    printf("ceph module is ok\n");

    return 0;
}

static void ceph_parse_opt(const char * type, const char * value)
{}

static int ceph_do_init(void)
{
    int ret;
    ret = ceph_init();
    if (0 != ret)
    {
        BUG();
    }

    initilized = 1;

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

static void close_connection(void)
{
    close_cluster();
}

static void ceph_shutdown(void)
{
    if (initilized) {
        close_connection();
    }
}

static struct guard_module ceph_ops = {
    .m_name = "ceph",
    .m_do = ceph_read,
    .m_shutdown = ceph_shutdown,
    .m_parse_opt = ceph_parse_opt,
    .m_probe = ceph_probe,
    .m_init = ceph_do_init,
};

static void __attribute__ ((constructor)) ceph_module_init(void)
{
    DBG("register input module %s", ceph_ops.m_name);

    input_register(&ceph_ops);
}

static void __attribute__ ((destructor)) distroy_handle(void)
{
}

#endif
