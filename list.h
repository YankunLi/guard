/*
 *
 * Author: Yankun Li <lioveni99@gmail.com>
 *
 */

#ifndef __GUARD_LIST_H_
#define __GUARD_LIST_H_

struct list_head
{
    struct list_head * prev;
    struct list_head * next;
};

#define offsetoff(type, member) ((size_t)&(((type*)0)->member))

#define container_of(ptr, type, member) ({              \
    const typeof(((type *)0)->member) * __mptr = (ptr); \
    (type *) ( (char*) __mptr - offsetoff(type, member) ); })
//(type *) ((char *) __mptr - (size_t) &(((type *)0)->member));})

#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

#define list_at_tail(pos, head, member) \
    (pos)->member.next == (head)

#define list_at_head(pos, head, member) \
    (pos)->member.prev == (head)

#define LIST_SELF(name) { &(name), &(name) }

#define LIST_HEAD(name)  \
struct list_head name = { &(name), &(name) }

#define list_for_each_entry(pos, head, member)  \
    for (pos = list_entry((head)->next, typeof(*(pos)), member); \
        &(pos)->member != (head); \
        (pos) = list_entry((pos)->member.next, typeof(*(pos)), member))

#define list_for_each_entry_revers(pos, head, member) \
    for (pos = list_entry((head)->prev, typeof(*pos), member); \
        &(pos)->member != head; \
        pos = list_entry((pos)->member.prev, typeof(*pos), member))

#define init_list_head(head) \
    do { (head)->next = (head); (head)->prev = (head); } while (0)

static inline void __list_add(struct list_head * obj, struct list_head *prev,
        struct list_head *next)
{
    prev->next = obj;
    obj->prev = prev;
    next->prev = obj;
    obj->next = next;
}

static inline int list_empty(struct list_head *head)
{
    return head->next == head;
}

static inline void list_add_head(struct list_head *obj, struct list_head *head)
{
    __list_add(obj, head, head->next);
}

static inline void list_add_tail(struct list_head *obj, struct list_head *head)
{
    __list_add(obj, head->prev, head);
}

static inline void list_del(struct list_head *obj)
{
   obj->prev->next = obj->next;
   obj->next->prev = obj->prev;
}

#endif
