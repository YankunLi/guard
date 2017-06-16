/*
 *
 * Author: Yankun Li <lioveni99@gmail.com>
 *
 */

#include "list.h"
#include "ceph.h"
#include "config.h"
#include "guard.h"
#include "group.h"
#include "element.h"

static rados_t cluster;
static int cluster_initialized = 0;
static uint64_t flags;

static char cluster_name[] = "ceph", user_name[] = "client.admin";
static char * c_path = "/etc/ceph/ceph.conf";

static struct element_group *grp = NULL;

static struct ceph_commands_t ceph_commands =
{
    .c_name = "ceph",
    .c_count = 0,
    .c_commands_list = LIST_SELF(ceph_commands.c_commands_list),
};

static struct commands_result_t cmds_result =
{
    .c_name = "ceph",
    .c_count = 0,
    .c_cmd_result_list = LIST_SELF(cmds_result.c_cmd_result_list),
};

struct rados_cluster_pool_t cluster_pool =
{
    .c_name = "ceph",
    .c_num_pools = 0,
    .c_pools_list = LIST_SELF(cluster_pool.c_pools_list),
    .c_has_initialized = 0,
};

static void __destroy_ioctx(rados_ioctx_t io)
{
        rados_ioctx_destroy(io);
}

static void destroy_ioctxs(struct list_head *list)
{
    struct rados_pool_t *rados_pool;
    list_for_each_entry(rados_pool, list, p_list)
        if (rados_pool->ioctx_initialized)
        __destroy_ioctx(rados_pool->p_ioctx);

}

static int list_pools()
{
    const char *buf_ptr;
    struct rados_pool_t * pool_ptr;

    if(cluster_pool.c_has_initialized)
        return 0;

    int buf_sz = rados_pool_list(cluster, NULL, 0);
    if (buf_sz < 0)
    {
        return -1;
    }

    char buf[buf_sz];
    int ret = rados_pool_list(cluster, buf, buf_sz);
    if (ret != buf_sz)
    {
        return -1;
    }

    buf_ptr = buf;
    while (1){
        if ('\0' == buf_ptr[0])
            break;
        pool_ptr = (struct rados_pool_t *) malloc(sizeof(struct rados_pool_t));
        pool_ptr->p_name = (char *) malloc(sizeof(buf_ptr));
        strcpy(pool_ptr->p_name, buf_ptr);
        cluster_pool.c_num_pools++;
        list_add_head(&pool_ptr->p_list, &cluster_pool.c_pools_list);

        buf_ptr += strlen(buf_ptr) + 1;
    }

//    if (cluster_pool.c_num_pools)
//        cluster_pool.c_has_initialized = 1;

//    struct rados_pool_t *p;
//    list_for_each_entry(p, &cluster_pool.c_pools_list, p_list)
//        printf("'%s' \t", p->p_name);

    return 0;
}

static void init_pools_ioctx()
{
    int ret = 0;
    struct rados_pool_t *p;
    list_for_each_entry(p, &cluster_pool.c_pools_list, p_list)
    {
       ret = rados_ioctx_create(cluster, p->p_name, &p->p_ioctx);
       if (ret < 0)
       {
           DBG("create pool io context fail %s", p->p_name);
           goto error;
       }
       p->ioctx_initialized = 1;
    }
    cluster_pool.c_has_initialized = 1;

    return;

error:
    destroy_ioctxs(&cluster_pool.c_pools_list);

    return;
}

int ceph_init(void)
{
    DBG("init ceph module");

    int err;
    //create a cluster handle.
    err = rados_create2(&cluster, cluster_name, user_name, flags);
    if (err < 0) {
        DBG("Couldn't create the cluster handle!");
        BUG();
    } else {
        cluster_initialized = 1;
        DBG("Create a cluster handle.");
    }

    //read ceph config file.
    err = rados_conf_read_file(cluster, c_path);
    if (err < 0) {
        DBG("cannot read config file %s", c_path);
    } else {
        DBG("Read the config file.");
    }

    //connect to the cluster.
    err = rados_connect(cluster);
    if (err < 0) {
        DBG("cannot connect to cluster.");
    } else {
        DBG("Connected to the cluster.");
    }
    if (!(grp = group_lookup("ceph-pools", 1)))
            BUG();
    list_pools();
    init_pools_ioctx();

    return 0;
}

static int update_pool_stat(struct rados_pool_t * pool)
{
    int ret = 0;
    struct rados_pool_stat_t st;
    ret = rados_ioctx_pool_stat(pool->p_ioctx, &st);
    if (ret < 0)
    {
        DBG("get pool %s status failure", pool->p_name);
    }
    DBG("update pool %s status", pool->p_name);
    DBG("pool %s used read kb : %d", pool->p_name, pool->p_pool_info.num_wr_kb);

    pool->p_pool_info.num_used_kb = st.num_kb;
    pool->p_pool_info.num_objects = st.num_objects;
    pool->p_pool_info.num_object_clones = st.num_object_clones;
    pool->p_pool_info.num_object_copies = st.num_object_copies;
    pool->p_pool_info.num_rd = st.num_rd;
    pool->p_pool_info.num_rd_kb = st.num_rd_kb;
    pool->p_pool_info.num_wr = st.num_wr;
    pool->p_pool_info.num_wr_kb =st.num_wr_kb;

    return ret;
}

static void read_pools_stat()
{
    struct rados_pool_t *p;
    list_for_each_entry(p, &cluster_pool.c_pools_list, p_list)
        update_pool_stat(p);
}

static int send_mon_command()
{
    int ret = 0;
    char *buf, *st;
    size_t buf_len, st_len;
    char *cmd[2];

    cmd[1] = NULL;
    //cmd[0] = (char *)"{\"prefix\":\"get_command_descriptions\"}";
    //cmd[0] = (char *)"{\"prefix\":\"status\", \"format\": \"html-pretty\"}";
    cmd[0] = (char *)"{\"prefix\":\"osd df\", \"format\": \"json\"}";
    ret = rados_mon_command(cluster, (const char **)cmd, 1, "", 0, &buf, &buf_len, &st, &st_len);
    if (ret < 0)
    {
        DBG("get command descriptions error");
    }
    printf("mon command deacriptions : %s \n", buf);

    printf("mon command stat : %s \n", st);
    rados_buffer_free(buf);
    rados_buffer_free(st);
}

int command_osd_tree()
{
    int ret = 0;
    char *buf, *st;
    size_t buf_len, st_len;
    char *cmd[2];

    cmd[1] = NULL;
    cmd[0] = "{\"prefix\": \"\"}";

    ret = rados_mon_command_target(cluster, "", (const char **)cmd, 1, "", 0, &buf, &buf_len, &st, &st_len);
    if (ret > 0)
    {
        DBG("get command descriptions error");
        return ret;
    }

    printf("command osd tree result: %s \n", buf);
    printf("command osd tree result status: %s \n", st);
    rados_buffer_free(buf);
    rados_buffer_free(st);

    return ret;
}

static void check_command()
{}

static void prepare_commands()
{
    DBG("prepare commands for reading cluster info");
    struct ceph_command_t * cmd = NULL;
    struct command_result_t * result = NULL;
    list_for_each_entry(cmd, &ceph_commands.c_commands_list, c_list)
    {
        result = (struct command_result_t *) malloc(sizeof(struct command_result_t));
        if (!result)
        {
            DBG("failure of malloc");
            assert("malloc fail" == 0);
        }
        result->c_json = NULL;
        result->c_status = NULL;
        result->c_object = NULL;
        cmd->c_result_ptr = result;

        list_add_tail(&result->c_list, &cmds_result.c_cmd_result_list);
        cmds_result.c_count++;
    }
}

static int read_ceph_status()
{}

static int read_ceph_osd_df()
{}

static int read_ceph_osd_tree()
{
}

static int execute_command(struct ceph_command_t * cmd)
{
    int ret = 0;
    char **buf = &cmd->c_result_ptr->c_json;
    char **st = &cmd->c_result_ptr->c_status;
    size_t * buf_len = &cmd->c_result_ptr->c_json_len;
    size_t * st_len = &cmd->c_result_ptr->c_status_len;
    DBG("ececute command %s", cmd->c_name);

    ret = rados_mon_command(cluster, (const char **)cmd->c_command, 1, "", 0, buf, buf_len, st, st_len);
    if (ret < 0)
    {
        DBG("executing command %s fail", cmd->c_name);
        return ret;
    }

    return 0;
}

static void submit_commands()
{
    struct ceph_command_t * cmd;
    list_for_each_entry(cmd, &ceph_commands.c_commands_list, c_list)
    {
//        switch (cmd->c_type)
//        {
//            case CEPH_STATUS:
//                read_ceph_status();
//                break;
//            case CEPH_OSD_DF:
//                read_ceph_osd_df();
//                break;
//            case CEPH_OSD_TREE:
//                read_ceph_osd_tree();
//                break;
//            default:
//                DBG("this command is invalid");
//        }
        execute_command(cmd);
    }
}

static void buffer_free(char * buf)
{
    if (buf)
        rados_buffer_free(buf);
}

static void guard_buf_free()
{
    DBG("free memory");
    struct command_result_t * cmd_ret;
    list_for_each_entry(cmd_ret, &cmds_result.c_cmd_result_list, c_list)
    {
        if (cmd_ret->c_json)
        {
            DBG("rados free");
            buffer_free(cmd_ret->c_json);
            cmd_ret->c_json = NULL;
        }

        if (cmd_ret->c_status)
        {
            buffer_free(cmd_ret->c_status);
            cmd_ret->c_status = NULL;
        }

        if (cmd_ret->c_object)
        {
            cJSON_Delete(cmd_ret->c_object);
            cmd_ret->c_object = NULL;
        }
    }
}

static void parse_json_format()
{
    struct command_result_t * cmd_ret;
    list_for_each_entry(cmd_ret, &cmds_result.c_cmd_result_list, c_list)
    {
        if (!cmd_ret->c_json)
            return;
        cmd_ret->c_object = cJSON_Parse(cmd_ret->c_json);
    }
}

static void update_elements()
{
    struct rados_pool_t * pool_ptr = NULL;
    const char *pool_name;
    struct element *e_ptr;

    if (!grp)
        grp = group_lookup("ceph-pools", 1);
    if (!grp)
        exit(1);

    list_for_each_entry(pool_ptr, &cluster_pool.c_pools_list, p_list)
    {
        pool_name = pool_ptr->p_name;
        e_ptr = element_lookup(grp, pool_name, 1);
        e_ptr->e_num_used_kb = pool_ptr->p_pool_info.num_used_kb;
        e_ptr->e_num_objects  = pool_ptr->p_pool_info.num_objects;
        e_ptr->e_num_object_clones = pool_ptr->p_pool_info.num_object_clones;
        e_ptr->e_num_object_copies = pool_ptr->p_pool_info.num_object_copies;
        e_ptr->e_num_rd = pool_ptr->p_pool_info.num_rd;
        e_ptr->e_num_rd_kb = pool_ptr->p_pool_info.num_rd_kb;
        e_ptr->e_num_wr = pool_ptr->p_pool_info.num_wr;
        e_ptr->e_num_wr_kb = pool_ptr->p_pool_info.num_wr_kb;
    }
}

static int transform()
{
    update_elements();
}

int read_info()
{
    if (ceph_commands.c_count <= 0)
    {
        DBG("don't found any command");
        return -1;
    }
    read_pools_stat();
    check_command();
    submit_commands();
    parse_json_format();
    transform();

    guard_buf_free();

//
//    parse_cmd_result();
//
}

static void parse_json()
{}

static void free_json_space()
{}

static struct  ceph_command_t ceph_status =
{
    .c_name = "ceph status",
    .c_type = CEPH_STATUS,
    .c_command = {"{\"prefix\": \"status\", \"format\": \"json\"}", NULL},
    .c_result_ptr = NULL,
};

static struct ceph_command_t ceph_osd_df =
{
    .c_name = "ceph osd df",
    .c_type = CEPH_OSD_DF,
    .c_command = {"{\"prefix\":\"osd df\", \"format\": \"json\"}", NULL},
    .c_result_ptr = NULL,
};

static struct ceph_command_t ceph_osd_tree =
{
    .c_name = "ceph osd tree",
    .c_type = CEPH_OSD_TREE,
    .c_command = {"{\"prefix\":\"status\", \"format\": \"json\"}", NULL},
    .c_result_ptr = NULL,
};

static struct ceph_command_t ceph_osd_perf =
{
    .c_name = "ceph osd perf",
    .c_type = CEPH_OSD_PERF,
    .c_command = {"{\"prefix\":\"osd perf\", \"format\": \"json\"}", NULL},
    .c_result_ptr = NULL,
};

//static struct ceph_command_t ceph_osd_metadata =
//{
//    .c_name = "ceph osd metadata",
//    .c_type = CEPH_OSD_METADATA,
//    .c_command = NULL,
//    .c_result_ptr = NULL,
//};

static int add_command(struct ceph_command_t * cmd)
{
    if (!cmd)
        return -EBUSY;

    DBG("add command %s into ceph_cmds %s", cmd->c_name, ceph_commands.c_name);
    list_add_tail(&cmd->c_list, &ceph_commands.c_commands_list);
    ceph_commands.c_count++;

    return 0;
}

static int commands_register()
{
 //   DBG("add command %s into ceph_cmds %s", cmd.c_name, ceph_cmds.c_name);
    add_command(&ceph_status);
    add_command(&ceph_osd_df);
    add_command(&ceph_osd_tree);
    add_command(&ceph_osd_perf);

    return 0;
}

static void __attribute__ ((constructor)) __init_ceph_commands(void)
{
    DBG("init commands, add command to %s", ceph_commands.c_name);
    int ret = 0;
    ret = commands_register();
    if (ret < 0)
    {
        DBG("don't found ceph commands %s", ceph_commands.c_name);
        assert("fail" == 0);
    }

    prepare_commands();
}

void destroy_handle(void)
{
    if (cluster_initialized)
        rados_shutdown(cluster);
}
