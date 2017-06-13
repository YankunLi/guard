/*
 *
 * Author: Yankun Li <lioveni99@gmail.com>
 *
 */

#include "config.h"
#include "output.h"
#include "utils.h"
#include "group.h"
#include "element.h"

#define LIST_COL_1 25
#define LIST_COL_2 75

static int row;
static int rows, cols;

static int initialized;

static int c_use_color;

static struct global_info_t *global_info = NULL;

static int c_disable_global_info = 0;

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
    attrset(COLOR_PAIR(1) | A_REVERSE);
    char *pad = xcalloc(1, cols);
//    move(row, 0);
    memset(pad, ' ', sizeof(pad));
    put_line("%s", pad);
    move(row, 0);
    put_line("%s %c%s%c", "curses", '(', "", ')');

    move(row, COLS - strlen("guard 1.0") - 1);
    put_line("%s", "guard 1.0");
    attrset(A_NORMAL);
    xfree(pad);
    move(row, 0);
}

static void draw_attr()
{
    char buf[32];

    move(row, LIST_COL_1);
    addch(ACS_VLINE);
    printw("%10s%-3s", "10000", "kB");
    printw("%10s%-3s", "6000", "kB");
    printw("%10s%-3s", "100", "kB");
    printw("%10s%-3s", "400", "kB");
    printw("%2.of%%", 1.2);
}

static void draw_element(struct element_group *g, struct element *e, void *arg)
{
    int *line = arg;

    if (line_visible(*line)) {
        NEXT_ROW();

        char pad[64];
        memset(pad, 0, sizeof(pad));
        memset(pad, ' ', 4);

        strncat(pad, "openstack", sizeof(pad) - strlen(pad) - 1);

        put_line("%-30.30%", pad);

        draw_attr();
    }
}

static void draw_global_info()
{
    int col_size = 3;
    int interval_size = cols / col_size;
    int half_interval = interval_size / 2;

    if (c_disable_global_info)
        return;

    if (!global_info)
        global_info = get_global_info();

    //draw header for global info
    mvprintw(row, half_interval - strlen("FSID")/2, "%s", "FSID");

    mvaddch(row,  interval_size * 1, ACS_VLINE);
    mvprintw(row, interval_size * 1 + half_interval - strlen("Status")/2,
            "%s", "Status");

    mvaddch(row, interval_size * 2, ACS_VLINE);
    mvprintw(row, interval_size * 2 + half_interval - strlen("Summary")/2,
            "%s", "Summary");
    //draw global info
    NEXT_ROW();
    mvprintw(row, half_interval - strlen(global_info->g_fsid)/2,"%s",
            global_info->g_fsid);

    mvaddch(row,  interval_size * 1, ACS_VLINE);
    mvprintw(row, interval_size * 1 + half_interval - strlen(global_info->g_status)/2,
            "%s", global_info->g_status);

    mvaddch(row, interval_size * 2, ACS_VLINE);
    mvprintw(row, interval_size * 2 + half_interval - strlen(global_info->g_summary)/2,
            "%s", global_info->g_summary);

    NEXT_ROW();
    hline(ACS_HLINE, cols);
    mvaddch(row,  interval_size * 1, ACS_BTEE);
    mvaddch(row,  interval_size * 2, ACS_BTEE);

}

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
        put_line("%10s    %10s    %10s    %%", g->g_hdr->gh_column[0],
                g->g_hdr->gh_column[1], g->g_hdr->gh_column[2]);

        attroff(A_BOLD);
        mvaddch(row, LIST_COL_2, ACS_VLINE);
        attron(A_BOLD);
        put_line("%10s    %10s    %%", g->g_hdr->gh_column[3], g->g_hdr->gh_column[4]);
        attroff(A_BOLD);
        NEXT_ROW();
        char pad[32];
        memset(pad, 0, sizeof(pad));
        memset(pad, 'w', 6);
        strncat(pad, "openstack", sizeof(pad) - strlen(pad) - 1);
        put_line("%-30.30s", pad);
    }
    (*line)++;

    group_foreach_element(g, draw_element, arg);
}

static void draw_element_list(void)
{
    int line = 0;

    group_foreach(draw_group, &line);

}


static void draw_context(void)
{
    draw_element_list();

    NEXT_ROW();
    hline(ACS_HLINE, cols);
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
    NEXT_ROW();

    put_line("rows : %d, cols : %d", rows, cols);
    NEXT_ROW();
    draw_global_info();

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

    if (has_colors())
        c_use_color = 1;

    if (c_use_color) {
        start_color();
        init_pair(1, COLOR_BLUE, COLOR_WHITE);
        init_pair(2, COLOR_WHITE, COLOR_BLUE);
    }

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
