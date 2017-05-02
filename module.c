/*
 *
 * Author: Yankun Li <lioveni99@gmail.com>
 *
 */

#include "guard.h"
#include "config.h"
#include "utils.h"
#include "module.h"
#include "list.h"

static LIST_HEAD(subsys_list);

void module_register_subsys(struct guard_subsys *ss)
{
    DBG("Module %s registered", ss->s_name);

    list_add_tail(&ss->s_list, &subsys_list);
}

int module_register(struct guard_subsys *ss, struct guard_module *m)
{
    if (m->m_subsys)
        return -EBUSY;

    list_add_tail(&m->m_list, &ss->s_mod_list);
    m->m_subsys = ss;
    return 0;
}

void module_foreach_run_enabled(struct guard_subsys *ss)
{
    struct guard_module *m;

    list_for_each_entry(m, &ss->s_mod_list, m_list)
        if(m->m_do)
            m->m_do();
}

void module_init()
{
    struct guard_subsys *ss;

    DBG("Initializing modules");

    list_for_each_entry(ss, &subsys_list, s_list)
    {
        if (ss->s_activate_default)
            ss->s_activate_default();

 //       module_foreach(ss, __module_init);
    }
}
