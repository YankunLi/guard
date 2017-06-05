/*
 *
 *
 * Author: YankunLi <lioveni99@gmail.com>
 *
 */

#include "config.h"
#include "group.h"
#include "element.h"
#include "list.h"
#include "guard.h"
#include "utils.h"

static LIST_HEAD(titles_list);
static LIST_HEAD(group_list);

void group_foreach(void (*cb)(struct element_group *, void *), void *arg)
{
    struct element_group *g, *n;

    list_for_each_entry_safe(g, n, &group_list, g_list)
        cb(g, arg);
}



static struct group_hdr *group_lookup_hdr(const char *name)
{
    struct group_hdr *hdr;

    list_for_each_entry(hdr, &titles_list, gh_list)
        if (!strcmp(hdr->gh_name, name))
            return hdr;

    return NULL;
}

struct element_group *group_lookup(const char *name, int flags)
{
    struct element_group *g;
    struct group_hdr *hdr;

    list_for_each_entry(g, &group_list, g_list)
        if (!strcmp(g->g_name, name))
            return g;

    if (!(hdr = group_lookup_hdr(name))) {
        fprintf(stderr, "Cannot find title for group %s", name);
        return NULL;
    }

    g = xcalloc(1, sizeof(*g));

    init_list_head(&g->g_elements);
    g->g_name = hdr->gh_name;
    g->g_hdr = hdr;

    list_add_tail(&g->g_list, &group_list);
    //ngroups++
    return g;
}

static int group_new_hdr(const char *name, const char *title,
        const char *col1, const char *col2, const char *col3,
        const char *col4, const char *col5)
{
    struct group_hdr *hdr;
    if (group_lookup_hdr(name))
        return -EEXIST;

    hdr = xcalloc(1, sizeof(*hdr));

    init_list_head(&hdr->gh_list);

    hdr->gh_name = strdup(name);
    hdr->gh_title = strdup(title);

    hdr->gh_column[0] = strdup(col1);
    hdr->gh_column[1] = strdup(col2);
    hdr->gh_column[2] = strdup(col3);
    hdr->gh_column[3] = strdup(col4);
    hdr->gh_column[4] = strdup(col5);

    list_add_tail(&hdr->gh_list, &titles_list);

    DBG("New group title %s \"%s\"");
    return 0;
}

static void __attribute__ ((constructor)) group_init(void)
{
    DBG("init group");

    group_new_hdr("ceph-pools", "Pools", "total_size", "used_size",
            "avaible_size", "rd_kB", "wr_kB");
}
