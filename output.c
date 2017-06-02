/*
 *
 * Author: Yankun Li <lioveni99@gmail.com>
 *
 */

#include "list.h"
#include "module.h"

static struct guard_subsys output_subsys;

void output_register(struct guard_module *m)
{
    module_register(&output_subsys, m);
}

static void activate_default(void)
{
    DBG("activate input subsystem");
}

void output_pre(void)
{
    module_foreach_run_enabled_pre(&output_subsys);
}

void output_draw(void)
{
    module_foreach_run_enabled(&output_subsys);
}

void output_post(void)
{
    module_foreach_run_enabled_post(&output_subsys);
}

static struct guard_subsys output_subsys = {
    .s_name = "output",
    .s_activate_default = &activate_default,
    .s_mod_list = LIST_SELF(output_subsys.s_mod_list),
};

static void __attribute__ ((constructor)) _output_init(void)
{
    return module_register_subsys(&output_subsys);
}
