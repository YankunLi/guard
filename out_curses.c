/*
 *
 * Author: Yankun Li <lioveni99@gmail.com>
 *
 */

#include "config.h"
#include "output.h"
#include "utils.h"
#include "group.h"

#define LIST_COL_1 25
#define LIST_COL_2 75

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

#define NEXT_ROW()    \
    do {              \
        row++;        \
        if (row >= rows - 1) \
            return;          \
        move(row, 0);        \
    } while(0)

static inline int line_visible(int line)
{
    return 1;
}

static void draw_header(void)
{
    put_line("%s %c%s%c", "curses", '(', "", ')');

    move(row, COLS - strlen("guard 1.0") - 1);
    put_line("%s", "guard 1.0");
    move(row, 0);
}

static void draw_element()
{}

static void draw_group(struct element_group *g, void *arg)
{
    int *line = arg;

    if (line_visible(*line)) {
        NEXT_ROW();
        attron(A_BOLD);
        put_line("%s", g->g_hdr->gh_title);

        attroff(A_BOLD);
        mvaddch(row, LIST_COL_1, ACS_VLINE);
        attron(A_BOLD);
        put_line("%10s    %10s   %10s    %%", g->g_hdr->gh_column[0],
                g->g_hdr->gh_column[1], g->g_hdr->gh_column[2]);

        attroff(A_BOLD);
        mvaddch(row, LIST_COL_2, ACS_VLINE);
        attron(A_BOLD);
        put_line("%10s    %10s", g->g_hdr->gh_column[3], g->g_hdr->gh_column[4]);
    }
    (*line)++;

 //   group_foreach_element(g, draw_element, arg);
}

static void draw_element_list(void)
{
    int line = 0;

    group_foreach(draw_group, &line);

}


static void draw_context(void)
{
    draw_element_list();
}

static void draw_statusbar(void)
{
    static const char *help_text = "Press ? for help";
    char s[27];
    time_t t = time(NULL);

    asctime_r(localtime(&t), s);
    s[strlen(s) - 1] = '\0';

    row = rows - 1;
    move(row, 0);
    put_line(" %s", s);

    move(row, COLS - strlen(help_text) - 1);
    put_line("%s", help_text);

    move(row, 0);
}

static void curses_draw()
{
    row = 0;
    move(0, 0);

    getmaxyx(stdscr, rows, cols);

    DBG("Screen height is %d width is %d", rows, cols);

    draw_header();

    draw_context();

    draw_statusbar();

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
