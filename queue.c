#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (head == NULL)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l || list_empty(l)) {
        free(l);
        return;
    }
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, l, list) {
        q_release_element(entry);
    }
    free(l);
    return;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *tmp = malloc(sizeof(element_t));
    if (!tmp)
        return false;
    size_t s_len = strlen(s);
    tmp->value = malloc(s_len + 1);
    if (!(tmp->value)) {
        free(tmp);
        return false;
    }
    strncpy(tmp->value, s, s_len);
    tmp->value[s_len] = '\0';
    list_add(&(tmp->list), head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *tmp = malloc(sizeof(element_t));
    if (!tmp)
        return false;
    size_t s_len = strlen(s);
    tmp->value = malloc(s_len + 1);
    if (!(tmp->value)) {
        free(tmp);
        return false;
    }
    strncpy(tmp->value, s, s_len);
    tmp->value[s_len] = '\0';
    list_add_tail(&(tmp->list), head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *target = list_first_entry(head, element_t, list);
    list_del(head->next);
    if (sp) {
        strncpy(sp, target->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return target;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *target = list_last_entry(head, element_t, list);
    list_del(head->prev);
    if (sp) {
        strncpy(sp, target->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return target;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *left = head->prev, *right = head->next;
    while (!(left == right) && !(right->prev == left)) {
        right = right->next;
        left = left->prev;
    }
    list_del(right);
    q_release_element(list_entry(right, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *curr_node = head->next, *next_node = (head->next)->next;
    bool last_cmp = 0;
    while (next_node != head) {
        if (strcmp((list_entry(curr_node, element_t, list))->value,
                   (list_entry(next_node, element_t, list))
                       ->value)) {  // curr and next elements don't have the
                                    // same value.
            if (last_cmp) {
                list_del(curr_node);
                q_release_element(list_entry(curr_node, element_t, list));
            }
            last_cmp = 0;
        } else {  // curr and next elements have the same value;
            last_cmp = 1;
            list_del(curr_node);
            q_release_element(list_entry(curr_node, element_t, list));
        }
        curr_node = next_node;
        next_node = next_node->next;
    }
    if (last_cmp) {
        list_del(curr_node);
        q_release_element(list_entry(curr_node, element_t, list));
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *curr_node;
    list_for_each (curr_node, head) {
        if (curr_node->next == head)
            break;
        list_move(curr_node, curr_node->next);
    }
    return;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *node = NULL, *safe = NULL;
    list_for_each_safe (node, safe, head)
        list_move(node, head);
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || k <= 1)
        return;
    struct list_head *node = head->next;
    while (1) {
        struct list_head *tmp = node, *curr_head = node->prev;
        for (int i = 0; i < k; i++, node = node->next)
            if (node == head)
                return;
        node = tmp;
        tmp = node->next;
        for (int i = 0; i < k; i++, node = tmp, tmp = tmp->next)
            list_move(node, curr_head);
    }
}

/* Merge two list */
struct list_head *merge_two_queue(struct list_head *a, struct list_head *b)
{
    struct list_head *head, **tail = &head;

    while (1) {
        int cmp = strcmp(container_of(a, element_t, list)->value,
                         container_of(b, element_t, list)->value);
        if (cmp <= 0) {
            *tail = a;
            tail = &(a->next);
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            tail = &(b->next);
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    return head;
}

/* Merge final two list and restore the doubly-linked list structure */
void merge_final(struct list_head *head,
                 struct list_head *a,
                 struct list_head *b)
{
    struct list_head *tail = head;
    int count = 0;

    while (1) {
        int cmp = strcmp(container_of(a, element_t, list)->value,
                         container_of(b, element_t, list)->value);
        if (cmp <= 0) {
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;
            if (!a) {
                break;
            }
        } else {
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
            if (!b) {
                b = a;
                break;
            }
        }
    }

    tail->next = b;
    do {
        if (!++count)
            strcmp(container_of(a, element_t, list)->value,
                   container_of(b, element_t, list)->value);
        b->prev = tail;
        tail = b;
        b = b->next;
    } while (b);

    /* And the final links to make a circular doubly-linked list */
    tail->next = head;
    head->prev = tail;
    return;
}


/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *queue = head->next, *pending = NULL;
    size_t count = 0;

    /* Convert to a null-terminated singly-linked list. */
    head->prev->next = NULL;
    do {
        size_t bits;
        struct list_head **tail = &pending;

        /* Find the least-significant clear bit in count */
        for (bits = count; bits & 1; bits >>= 1)
            tail = &(*tail)->prev;
        /* Do the indicated merge */
        if (bits) {
            struct list_head *a = *tail, *b = a->prev;

            a = merge_two_queue(b, a);
            /* Install the merged result in place of the inputs */
            a->prev = b->prev;
            *tail = a;
        }

        /* Move one element from input list to pending */
        queue->prev = pending;
        pending = queue;
        queue = queue->next;
        pending->next = NULL;
        count++;
    } while (queue);
    queue = pending;
    pending = pending->prev;
    while (1) {
        struct list_head *next = pending->prev;

        if (!next)
            break;
        queue = merge_two_queue(pending, queue);
        pending = next;
    }
    merge_final(head, pending, queue);
    return;
}


/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    struct list_head *curr_node = head->prev, *prev_node = curr_node->prev,
                     *tmp;
    while (prev_node != head) {
        if (strcmp(container_of(prev_node, element_t, list)->value,
                   container_of(curr_node, element_t, list)->value) < 0) {
            tmp = prev_node->prev;
            list_del(prev_node);
            q_release_element(container_of(prev_node, element_t, list));
            prev_node = tmp;
        } else {
            curr_node = prev_node;
            prev_node = prev_node->prev;
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    queue_contex_t *first_queue_container =
        container_of(head->next, queue_contex_t, chain);
    if (list_is_singular(head))
        return first_queue_container->size;
    for (struct list_head *curr_node = head->next->next; curr_node != head;
         curr_node = curr_node->next) {
        queue_contex_t *que = container_of(curr_node, queue_contex_t, chain);
        list_splice_init(que->q, first_queue_container->q);
        que->size = 0;
    }
    q_sort(first_queue_container->q);
    first_queue_container->size = q_size(first_queue_container->q);
    return first_queue_container->size;
}