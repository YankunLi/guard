/*
 *
 * Author: Yankun Li <lioveni99@gmail.com>
 *
 */

#include "config.h"
#include "output.h"

static int initialized;

static int handle_input(int ch)
{
    return 0;
}

static void curses_shutdown()
{}

static void curses_draw()
{}

static int curses_init()
{
    DBG("init curses");
    if (!initscr()) {
        fprintf(stderr, "Unable to initialize curses screen\n");
        return -EOPNOTSUPP;
    }

    initialized = 1;

    keypad(stdscr, TRUE);
    nonl();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    clear();
    curs_set(0);

    return 0;
}

static void curses_pre()
{
    static int init = 0;
    if (!init) {
        curses_init();
        init = 1;
    }

    for (;;) {
        int ch = getch();

        if (-1 == ch)
            break;

        if (handle_input(ch))
            curses_draw();
    }
}

static struct guard_module curses_ops = {
    .m_name = "curses",
    .m_shutdown = curses_shutdown,
    .m_pre = curses_pre,
    .m_do = curses_draw,
};

static void __attribute__ ((constructor)) do_curses_init(void)
{
    DBG("register curses module");
    output_register(&curses_ops);
}
