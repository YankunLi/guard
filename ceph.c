/*
 *
 * Author: Yankun Li <lioveni99@gmail.com>
 *
 */

#include "list.h"
#include "ceph.h"
#include "config.h"
#include "guard.h"

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

void init_pools_ioctx()
{
    int ret = 0;
    struct rados_pool *p;
    list_for_each_entry(p, &cluster_pool.c_pools_list, p_list)
    {
       ret = rados_ioctx_create(cluster, p->p_name, &p->p_ioctx);
       if (ret < 0)
       {
           DBG("create pool io context fail %s", p->p_name);
       }
    }
}

int update_pool_stat(struct rados_pool* pool)
{
    int ret = 0;
    struct rados_pool_stat_t st;
    ret = rados_ioctx_pool_stat(pool->p_ioctx, &st);
    if (ret < 0)
    {
        DBG("get pool %s status failure", pool->p_name);
    }
    DBG("update pool %s status", pool->p_name);
    DBG("pool %s used read kb : %d", pool->p_name, pool->pool_stat.num_wr_kb);

    pool->pool_stat.num_used_kb = st.num_kb;
    pool->pool_stat.num_objects = st.num_objects;
    pool->pool_stat.num_object_clones = st.num_object_clones;
    pool->pool_stat.num_object_copies = st.num_object_copies;
    pool->pool_stat.num_rd = st.num_rd;
    pool->pool_stat.num_rd_kb = st.num_rd_kb;
    pool->pool_stat.num_wr = st.num_wr;
    pool->pool_stat.num_wr_kb =st.num_wr_kb;

    return ret;
}

void read_pools_stat()
{
    struct rados_pool *p;
    list_for_each_entry(p, &cluster_pool.c_pools_list, p_list)
        update_pool_stat(p);
}
