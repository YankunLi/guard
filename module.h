/*
 *
 * Author: Yankun Li <lioveni99@gmail.com>
 *
 */

#ifndef __GUARD_MODULE_H_
#define __GUARD_MODULE_H_

#include "guard.h"
#include "config.h"
#include "list.h"

#define GUARD_MODULE_ENABLED (1 << 0)
#define GUARD_MODULE_DEFAULT (1 << 1)
#define GUARD_MODULE_AUTO    (1 << 2)

struct guard_subsys;

struct guard_module
{
    char * m_name;

    int (*m_init)(void);
    int (*m_probe)(void);
    void (*m_shutdown)(void);

    void (*m_attach)(void);
    void (*m_detach)(void);

    void (*m_parse_opt)(const char *, const char *);
    void (*m_pre)(void);
    void (*m_do)(void);
    void (*m_post)(void);

    int m_flags;
    struct list_head m_list;
    struct guard_subsys *m_subsys;
};

struct guard_subsys {
    char * s_name;

    int s_nmod;
    struct list_head s_mod_list;

    void (*s_activate_default)();

    struct list_head s_list;
};

extern int module_register(struct guard_subsys *, struct guard_module *);
extern void module_init(void);
extern void module_register_subsys(struct guard_subsys *);
extern void module_foreach_run_enabled(struct guard_subsys *);
extern void module_foreach_run_enabled_pre(struct guard_subsys *);
extern void module_foreach_run_enabled_post(struct guard_subsys *);
extern void module_shutdown(void);

#endif
