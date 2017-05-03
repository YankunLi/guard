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

static struct guard_module ceph_ops;

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
    return 0;
}

static void ceph_read(void)
{
    DBG("I am ceph module");
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
