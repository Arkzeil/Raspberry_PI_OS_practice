// source:https://github.com/jsandler18/raspi-kernel/blob/62f68bf494eefd372b28ef1ce54841e310f82295/include/kernel/list.h
// ref : https://jsandler18.github.io/explanations/list_h.html
/**
 * This implimentation of a list is done in macros so it may be used generically for any struct.
 *
 * To use, call DEFINE_LIST(typename) in the header file where you would want to define the struct for the list.  typename should be the NON-'typedef'ED name of the struct
 * without the keyword "struct".  If I wand to define a list of type "struct point", I would need to do "DEFINE_LIST(point)".  This will create the list type called
 * "typename_list_t", which may be reused as many times as you want.
 *
 * Next, you must call DEFINE_LINK(typename) inside the body of the struct.
 *
 * Finally, call IMPLEMENT_LIST(typename) where you would want the list to be implimented
 *
 * Before using an instance of the list, you should call INITIALIZE_LIST(list) to correctly it up
 *
 *
 * IMPLEMENT_LIST defines the following functions:
 * void append_nodeType_list(nodeType_list_t * list, struct nodeType *)
 *      appends to the back of the list
 *
 * void push_nodeType_list(nodeType_list_t * list, struct nodeType *)
 *      adds to the front of the list
 *
 * struct nodeType * peek_nodeType_list(nodeType_list_t * list)
 *      gets the first element from the list without removing it
 *
 * struct nodeType * pop_nodeType_list(nodeType_list_t * list)
 *      gets the first element from the list and removes it
 *
 * uint32_t size_nodeType_list(nodeType_list_t * list)
 *      returns the number of elements in the list
 *
 * struct nodeType * next_nodeType_list(struct nodeType * node)
 *      gets the next node in the list, null if none left
 */
#include <stddef.h>
#include <stdint.h>

#ifndef LIST_H
#define LIST_H

// ref :https://stackoverflow.com/questions/14570297/what-does-it-mean-to-have-a-block-of-c-code-with-a-backslash-after-each-semico 
// A backslash as last character in a line causes this line to be joined with the next for preprocessing.
// For regular C++ parsing newlines are simply whitespace, so this does not matter. But preprocessor directives, in particular macro definitions end at the end of line.
// Using a backslash for line continuation allows formatting long macro bodies across multiple source text lines.

// ref : https://ithelp.ithome.com.tw/articles/10207697
// ref : https://opensourcedoc.com/c-programming/preprocessor/
// "##" is used to recognize a macro function parameter and expand it in the macro function
#define DEFINE_LIST(nodeType) \
typedef struct nodeType##list { \
    struct nodeType * head; \
    struct nodeType * tail; \
    uint32_t size;\
} nodeType##_list_t;

#define DEFINE_LINK(nodeType) \
struct nodeType * next##nodeType;\
struct nodeType * prev##nodeType;

#define INITIALIZE_LIST(list) \
    list.head = list.tail = (void *)0;\
    list.size = 0;

#define IMPLEMENT_LIST(nodeType) \
void append_##nodeType##_list(nodeType##_list_t * list, struct nodeType * node) {  \
    list->tail->next##nodeType = node;                                       \
    node->prev##nodeType = list->tail;                                       \
    list->tail = node;                                                       \
    node->next##nodeType = NULL;                                             \
    list->size += 1;                                                         \
    if (list->head == NULL) {                                                \
        list->head = node;                                                   \
    }                                                                        \
}                                                                            \
                                                                             \
void push_##nodeType##_list(nodeType##_list_t * list, struct nodeType * node) {    \
    node->next##nodeType = list->head;                                       \
    node->prev##nodeType = NULL;                                             \
    list->head = node;                                                       \
    list->size += 1;                                                         \
    if (list->tail == NULL) {                                                \
        list->tail = node;                                                   \
    }                                                                        \
}                                                                            \
                                                                             \
struct nodeType * peek_##nodeType##_list(nodeType##_list_t * list) {         \
    return list->head;                                                       \
}                                                                            \
                                                                             \
struct nodeType * pop_##nodeType##_list(nodeType##_list_t * list) {          \
    struct nodeType * res = list->head;                                      \
    list->head = list->head->next##nodeType;                                 \
    list->head->prev##nodeType = NULL;                                                 \
    list->size -= 1;                                                         \
    if (list->head == NULL) {                                                \
        list->tail = NULL;                                                  \
    }                                                                        \
    return res;                                                              \
}                                                                            \
                                                                             \
uint32_t size_##nodeType##_list(nodeType##_list_t * list) {                  \
    return list->size;                                                       \
}                                                                            \
                                                                             \
struct nodeType * next_##nodeType##_list(struct nodeType * node) {           \
    return node->next##nodeType;                                             \
}                                                                            \

#endif