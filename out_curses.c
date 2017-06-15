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

static uint64_t scale[7] = {
    1, // B
    1 << 10, //1024  1024 KB
    1 << 20, // 1024 * 1024 MB
    1 << 30, //1024 * 1024 * 1024,  GB
//    1 << 40, //1024 * 1024 * 1024,  TB
//    1 << 50, // 1024 ^ 5,  PB
//    1 << 60 // 1024 ^ 6 EB
};

static char *unit_array[7] = {
    "B",
    "KB",
    "MB",
    "GB",
//    "TB",
//    "PB",
//    "EB",
};

enum {
    B,
    KB,
    MB,
    GB,
//    TB,
//    PB,
//    EB,
};


static void unit_value(uint64_t value, int unit, char *buf)
{
    int new_value;

    int i = 3; //ARRAY_SIZE(scale) - 1;
    for (; i > 0; i--)
    {
        if (value / scale[i])
        {
            new_value = value / scale[i];
            goto out;
        }
    }

    new_value = value;

out:
    if (i + unit > 6)
        exit(1);

    snprintf(buf, strlen(buf) - 1, "%d%s", new_value, unit_array[i + unit]);

    return;
}

static int handle_input(int ch)
{
    switch (ch)
    {
        case KEY_CLEAR:
            clear();
            return 1;

        case 'c':
            clear();
            return 1;
    }
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

//char * unit_value(uint64_t v)
//{
//    int val
//    if (val = v/(1024*1024*1024))
//    {
//
//    }
//}

static void __draw_global_mon(struct mon_t *mon, int interval)
{
    NEXT_ROW();
    mvprintw(row, interval/2 - 28, "%10s%24s%6d%18s",
            mon->m_name, mon->m_addr, mon->m_rank, mon->m_health);
    mvaddch(row,  interval * 1, ACS_VLINE);
    mvprintw(row, interval + interval/2 - 28, "%10d    %10d    %10d    %10d",
            mon->m_kb_total, mon->m_kb_used, mon->m_kb_avail, mon->m_avail_percent);
}

static void draw_global_mon()
{
    int col_size = 2;
    int interval_size = cols / col_size;
    int half_interval = interval_size / 2;

    struct mon_t *mon_ptr;

    mvprintw(row, half_interval - 28, "%10s%18s%13s%15s",
            "Monitor", "Address", "Rank", "Status");
    mvaddch(row,  interval_size * 1, ACS_VLINE);
    mvprintw(row, interval_size + half_interval - 28 , "%10s    %10s    %10s    %14s",
            "Total Size", "Used Size", "Avail Size", "Avail Percent");
    list_for_each_entry(mon_ptr, &global_info->g_mon_servers->g_mons, m_list)
        __draw_global_mon(mon_ptr, interval_size);

    NEXT_ROW();
    hline(ACS_HLINE, cols);
    mvaddch(row,  interval_size * 1, ACS_BTEE);
}

static void draw_resource_usage()
{
    char temp_value[100];
    char *ptr1, *ptr2, *ptr3, *ptr4, *ptr5, *ptr6;
    int col_size = 2;
    int interval_size = cols / col_size;
    int half_interval = interval_size / 2;

    mvprintw(row, interval_size * 1 / 2 - 8, "%s", "Storage Resource");
    mvaddch(row,  interval_size * 1, ACS_VLINE);

    mvprintw(row, interval_size * 1 +  half_interval - 4, "%s",
            "Client IO");

    NEXT_ROW();
    move(row, half_interval - 28);
    put_line("%10s%10.5s%10s%10s%10s", "PGS", "Data", "Used", "Avail", "Total");
    mvaddch(row,  interval_size * 1, ACS_VLINE);
    move(row, interval_size * 1 + half_interval - 18);
    put_line("%10s  %10s  %10s", "RD", "WR", "OP");

    memset(temp_value, '\0', sizeof(temp_value));
    ptr1 = temp_value;
    unit_value(global_info->g_usage.data_bytes, B, ptr1);
    ptr2 = ptr1 + strlen(ptr1) + 1;
    unit_value(global_info->g_usage.bytes_used, B, ptr2);
    ptr3 = ptr2 + strlen(ptr2) + 1;
    unit_value(global_info->g_usage.bytes_avail, B, ptr3);
    ptr4 = ptr3 + strlen(ptr3) + 1;
    unit_value(global_info->g_usage.bytes_total, B, ptr4);

    if (temp_value + 99 < ptr4 + strlen(ptr4))
        exit(1);

    NEXT_ROW();
    move(row, half_interval - 28);
//    put_line("%10d%10s%10s%10s%10s", global_info->g_usage.num_pgs,
//            unit_value(global_info->g_usage.data_bytes, B),
//            unit_value(global_info->g_usage.bytes_used, B),
//            unit_value(global_info->g_usage.bytes_avail, B),
//            unit_value(global_info->g_usage.bytes_total, B));
    put_line("%10d%10s%10s%10s%10s", global_info->g_usage.num_pgs,
            ptr1,
            ptr2,
            ptr3,
            ptr4);

    ptr5 = ptr4 + strlen(ptr4) + 1;
    unit_value(global_info->g_usage.usage_rate.read_bytes_sec, B, ptr5);
    ptr6 = ptr5 + strlen(ptr5) + 1;
    unit_value(global_info->g_usage.usage_rate.write_bytes_sec, B, ptr6);

    if (temp_value + 99 < ptr6 + strlen(ptr6))
        exit(1);

    mvaddch(row,  interval_size * 1, ACS_VLINE);
    move(row, interval_size * 1 + half_interval - 18);
    put_line("%10s/s%10s/s%10d/s", ptr5,
            ptr6,
            global_info->g_usage.usage_rate.op_per_sec);

    NEXT_ROW();
    hline(ACS_HLINE, cols);
    mvaddch(row, interval_size * 1, ACS_BTEE);
}

static void draw_global_info()
{
    int col_size = 3;
    int s_col_size = 2;
    int s_server_interval_size = cols / s_col_size;
    int s_half_interval = s_server_interval_size / 2;
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

    mvaddch(row, s_server_interval_size * 1, ACS_TTEE);
    //storage servers
    NEXT_ROW();
    //draw header
    /* int location = s_half_interval - strlen(" "); */
    move(row, s_half_interval - 20);
    put_line("%10s    %10s    %10s", "Total OSDs", "UP OSDs", "IN OSDs");
    /* put_line("%10s    %10s    %10s",global_info->g_storage_servers.g_num ); */
    mvaddch(row, s_server_interval_size * 1, ACS_VLINE);
    move(row, s_server_interval_size + s_half_interval - 20);
    put_line("%10s    %10s    %10s", "FULL", "NEARFULL", "Remapped Pgs");

    NEXT_ROW();
    move(row, s_half_interval - 22);
    put_line("%10d    %10d    %10d", global_info->g_storage_servers.g_num_osds,
            global_info->g_storage_servers.g_num_up_osds,
            global_info->g_storage_servers.g_num_in_osds);
    mvaddch(row, s_server_interval_size * 1, ACS_VLINE);
    move(row, s_server_interval_size + s_half_interval - 22);
    put_line("%10d    %10d    %10d", global_info->g_storage_servers.g_full,
            global_info->g_storage_servers.g_nearfull,
            global_info->g_storage_servers.g_num_remapped_pgs);

    NEXT_ROW();
    hline(ACS_HLINE, cols);
    mvaddch(row,  s_server_interval_size * 1, ACS_BTEE);

    NEXT_ROW();
    draw_global_mon();

    NEXT_ROW();
    draw_resource_usage();

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
    static int old_rows_and_cols = 0;
    row = 0;
    move(0, 0);

    getmaxyx(stdscr, rows, cols);

    if (old_rows_and_cols != rows + cols)
    {
        old_rows_and_cols = rows + cols;
        clear();
    }

    DBG("Screen height is %d width is %d", rows, cols);

    draw_header();
//    NEXT_ROW();
//
//    put_line("rows : %d, cols : %d", rows, cols);
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
