/*
 *
 * Author: Yankun Li <lioveni99@gmail.com>
 *
 */

#include "guard.h"
#include "config.h"
#include "module.h"
#include "list.h"
#include "input.h"

static struct guard_subsys input_subsys;

void input_register(struct guard_module *m)
{
    module_register(&input_subsys, m);
}

void activate_default(void)
{
    DBG("activate input subsystem");
}

void input_read(void)
{
    module_foreach_run_enabled(&input_subsys);
}

static struct guard_subsys input_subsys = {
    .s_name = "input",
    .s_activate_default = &activate_default,
    .s_mod_list = LIST_SELF(input_subsys.s_mod_list),
};

static void __attribute__ ((constructor)) __input_init(void)
{
    DBG("register input subsystem %s", input_subsys.s_name);
    module_register_subsys(&input_subsys);
}
