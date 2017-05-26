/*
 *
 * Author: Yankun Li <lioveni99@gmail.com>
 *
 */

#include "list.h"
#include "ceph.h"
#include "config.h"

extern rados_t cluster;

struct rados_cluster_pool cluster_pool =
{
    .c_name = "ceph",
    .c_num_pools = 0,
    .c_pools_list = LIST_SELF(cluster_pool.c_pools_list),
    .c_has_initialized = 0,
};

int list_pools()
{
    if(cluster_pool.c_has_initialized)
        return 0;

    int buf_sz = rados_pool_list(cluster, NULL, 0);
    char buf[buf_sz];
    int ret = rados_pool_list(cluster, buf, buf_sz);
    if (ret != buf_sz)
    {
        return -1;
    }
    const char *b = buf;
    struct rados_pool * pool_ptr;
    while (1){
        if ('\0' == b[0])
            break;
        pool_ptr = (struct rados_pool *) malloc(sizeof(struct rados_pool));
        pool_ptr->p_name = (char *) malloc(sizeof(b));
        strcpy(pool_ptr->p_name, b);
        cluster_pool.c_num_pools++;
        list_add_head(&pool_ptr->p_list, &cluster_pool.c_pools_list);

        b += strlen(b) + 1;
    }

    if (cluster_pool.c_num_pools)
        cluster_pool.c_has_initialized = 1;

    struct rados_pool *p;
    list_for_each_entry(p, &cluster_pool.c_pools_list, p_list)
        printf("'%s' \t", p->p_name);

    return 0;
}
