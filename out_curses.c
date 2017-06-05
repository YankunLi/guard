/*
 *
 * Author: Yankun Li <lioveni99@gmail.com>
 *
 */

#include "config.h"
#include "output.h"
#include "utils.h"

static int row;
static int rows, cols;

static int initialized;

static int handle_input(int ch)
{
    return 0;
}

static void curses_shutdown()
{}

static void put_line(const char *fmt, ...)
{
    va_list args;
    char *buf;
    int len;
    int x, y;

    getyx(stdscr, y, x);

    len = cols -x;
    buf = xcalloc(len+1, 1);

    va_start(args, fmt);
    vsnprintf(buf, len+1, fmt, args);
    va_end(args);

    if (strlen(buf) < len)
    {
        memset(&buf[strlen(buf)], ' ', len - strlen(buf));
    }

    addstr(buf);
    xfree(buf);
}

static void draw_header(void)
{
    put_line("%s %c%s%c", "curses", '(', "", ')');

    move(row, COLS - strlen("guard 1.0") - 1);
    put_line("%s", "guard 1.0");
    move(row, 0);
}

static void curses_draw()
{
    row = 0;
    move(0, 0);

    getmaxyx(stdscr, rows, cols);

    DBG("Screen height is %d width is %d", rows, cols);

    draw_header();

out:
    attrset(0);
    refresh();
}

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
