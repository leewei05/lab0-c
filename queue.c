#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));

    if (head) {
        INIT_LIST_HEAD(head);
        return head;
    }

    return NULL;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;

    // list_empty Return: 0 - list is not empty !0 - list is empty
    while (!list_empty(l)) {
        struct list_head *del = l->next;
        list_del(del);
        q_release_element(list_entry(del, element_t, list));
    }

    free(l);
    return;
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;

    INIT_LIST_HEAD(&node->list);
    int size = sizeof(char) * (strlen(s) + 1);
    node->value = malloc(size);
    if (!node->value) {
        free(node);
        return false;
    }

    // strncpy will insert null byte at the end
    strncpy(node->value, s, size);
    node->value[size - 1] = '\0';
    list_add(&node->list, head);

    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;

    INIT_LIST_HEAD(&node->list);
    char *str = strdup(s);
    if (!str) {
        free(node);
        return false;
    }

    node->value = str;
    list_add_tail(&node->list, head);

    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *del = list_first_entry(head, element_t, list);
    list_del(&del->list);
    if (!sp)
        return del;

    strncpy(sp, del->value, bufsize);
    sp[bufsize - 1] = '\0';

    return del;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *del = list_last_entry(head, element_t, list);
    list_del(&del->list);
    if (!sp)
        return del;

    strncpy(sp, del->value, bufsize);
    sp[bufsize - 1] = '\0';

    return del;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
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

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head *rabbit = head->next->next;
    struct list_head *turtle = head->next;

    while ((rabbit != head) && (rabbit->next != head)) {
        rabbit = rabbit->next->next;
        turtle = turtle->next;
    }

    element_t *del = list_entry(turtle, element_t, list);
    list_del(&del->list);
    q_release_element(del);
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if ((!head) || list_empty(head))
        return;

    struct list_head *node = head->next;
    while (node != head && node->next != head) {
        // swap nodes
        struct list_head *first = node, *second = node->next;
        second->prev = first->prev;
        first->prev = second;
        second->next->prev = first;
        second->prev->next = second;
        first->next = second->next;
        second->next = first;
        // traverse to next after swapped
        node = node->next;
    }
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if ((!head) || list_empty(head))
        return;

    struct list_head *node = head->next;
    while (node != head) {
        struct list_head *tmp = node->prev;
        node->prev = node->next;
        node->next = tmp;
        node = node->prev;
    }

    struct list_head *tmp = head->prev;
    head->prev = head->next;
    head->next = tmp;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head) {}
