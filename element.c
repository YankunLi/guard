/*
 *
 *
 * Author: Yankun Li <lioveni99@gmail.com>
 *
 */

#include "config.h"
#include "list.h"
#include "element.h"

static struct global_info_t global_info;

static struct global_mon_t global_mon =
{
    .g_mons = LIST_SELF(global_mon.g_mons),
};

struct global_info_t * get_global_info()
{
    return &global_info;
}

struct global_mon_t *get_global_mons()
{
    return &global_mon;
}

static void __attribute__  ((constructor)) bind_global_mons()
{
    int count = 3;
    global_info.g_mon_servers = &global_mon;

    //add test data for global info to test curses
    //global 
    strcpy(global_info.g_fsid, "xxxxx-xxxxxx-xxxxxxx-xxxxxxxx");
    strcpy(global_info.g_status, "HEALTH_WARN");
    strcpy(global_info.g_summary, "noout flag(s) set");

    //monitor
    struct mon_t * mon;
    while (count--)
    {
        mon = (struct mon_t*) malloc(sizeof(struct mon_t));
        strcpy(mon->m_name, "mon-1");
        mon->m_kb_total = 98544244;
        mon->m_kb_used = 8039344;
        mon->m_kb_avail = 85492472;
        mon->m_avail_percent = 86;

        strcpy(mon->m_addr, "10.100.13.180:6789/0");
        mon->m_rank = 1;
        strcpy(mon->m_health, "HEALTH_OK");
        list_add_tail(&mon->m_list, &global_mon.g_mons);
    }

    //osdmap
    global_info.g_storage_servers.g_epoch = 2497;
    global_info.g_storage_servers.g_num_osds = 18;
    global_info.g_storage_servers.g_num_up_osds = 18;
    global_info.g_storage_servers.g_num_in_osds = 18;
    global_info.g_storage_servers.g_full = 0;
    global_info.g_storage_servers.g_nearfull = 0;
    global_info.g_storage_servers.g_num_remapped_pgs = 0;

    //global_usage
    global_info.g_usage.num_pgs = 1144;
    global_info.g_usage.data_bytes = 5784683876643;
    global_info.g_usage.bytes_used = 11530439438336;
    global_info.g_usage.bytes_avail = 42253956984832;
    global_info.g_usage.bytes_total = 53784396423168;
    global_info.g_usage.usage_rate.read_bytes_sec = 23;
    global_info.g_usage.usage_rate.write_bytes_sec = 396444;
    global_info.g_usage.usage_rate.op_per_sec = 9;


}
