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

static void __group_foreach_element(struct element_group *g,
                    struct list_head *list,
                    void (*cb)(struct element_group *,
                        struct element *, void *),
                    void *arg)
{
    struct element *e, *n;

    list_for_each_entry_safe(e, n, list, e_list) {
        cb(g, e, arg);
    }
}

void group_foreach_element(struct element_group *g,
                void (*cb)(struct element_group *,
                    struct element *, void *), void *arg)
{
    __group_foreach_element(g, &g->g_elements, cb, arg);
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
        const char *col4, const char *col5, const char *col6,
        const char *col7, const char *col8)
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
    hdr->gh_column[5] = strdup(col6);
    hdr->gh_column[6] = strdup(col7);
    hdr->gh_column[7] = strdup(col8);

    list_add_tail(&hdr->gh_list, &titles_list);

    DBG("New group title %s \"%s\"");
    return 0;
}

static void __attribute__ ((constructor)) group_init(void)
{
    DBG("init group");

    group_new_hdr("ceph-pools", "Pools", "Used Size(KB)", "Objects",
            "Object Clones", "Object copies", "RD_OP", "RD_KB", "WR_OP", "WR_KB");
}

static void group_hdr_free(struct group_hdr *hdr)
{
    xfree(&hdr->gh_name);
    xfree(&hdr->gh_title);
    xfree(&hdr->gh_column[0]);
    xfree(&hdr->gh_column[1]);
    xfree(&hdr->gh_column[2]);
    xfree(&hdr->gh_column[3]);
    xfree(&hdr->gh_column[4]);
    xfree(&hdr->gh_column[5]);
    xfree(&hdr->gh_column[6]);
    xfree(&hdr->gh_column[7]);
    xfree(&hdr);

}

static void group_free(struct element_group *g)
{
    struct element *e, *n;

    list_for_each_entry_safe(e, n, &g->g_elements, e_list)
        element_free(e);

    xfree(&g);
}

static void __attribute__ ((destructor)) group_exit(void)
{
    struct group_hdr *hdr, *hnext;
    struct element_group *g, *gnext;

    list_for_each_entry_safe(hdr, hnext, &titles_list, gh_list)
        group_hdr_free(hdr);

    list_for_each_entry_safe(g, gnext, &group_list, g_list)
        group_free(g);

}
