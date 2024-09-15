/**
 * \file list.h
 * \brief Header file for linked list operations.
 *
 * This file contains the declarations of functions and structures related
 * to linked list management.
 *
 * \author Sylvain Labopin
 * \date 2024-09-09
 */

#ifndef LIST_H
#define LIST_H

/**
 * \struct cons
 * \brief Represents a node in a linked list.
 *
 * This structure is used to store an element in the linked list (`car`) and a pointer
 * to the next node (`cdr`).
 */
typedef struct cons {
  void *car; /**< Pointer to the element stored in the list. */
  struct cons *cdr; /**< Pointer to the next node in the list. */
} cons, *list;

/** \defgroup LinkedList Linked List Operations
 *  \brief Functions to manage linked lists.
 *  @{
 */

/**
 * \brief Adds a new element to the beginning of the linked list.
 *
 * This function allocates a new node for the linked list,
 * initializes its values, and inserts it at the head of the list.
 *
 * \param l Pointer to the linked list where the element should be added.
 * \param element Pointer to the element to be inserted into the list.
 *
 * \par Example
 * \code
 * list l = NULL;
 * int *value1 = malloc(sizeof(int));
 * int *value2 = malloc(sizeof(int));
 *
 * *value1 = 42;
 * *value2 = 99;
 *
 * // Add two elements to the list
 * push_list(&l, value1);
 * push_list(&l, value2);

 * // Clean up the list
 * cleanup_list(&l);
 * \endcode
 *
 * \par Details
 * If memory allocation fails, an error message is printed
 * and the program terminates with an error code.
 */
list push_list(list *l, void *element); // to do return

//doc to do
list to_list(int length, ...);

/**
 * \brief Cleans up and frees all nodes in the linked list.
 *
 * This function traverses a linked list, frees the memory of each element
 * and node, and resets the pointers to prevent memory leaks.
 *
 * \param lp Pointer to the linked list to clean up.
 *
 * \see push_list for an example of how to use cleanup_list together with push_list.
 *
 * \par Details
 * After this function is called, the linked list will be emptied, and all the memory used by the nodes and elements will be freed.
 * It is important to note that this function assumes that `car` (the elements) were dynamically allocated, and will free them.
 * If the elements are not dynamically allocated, this function could cause errors or undefined behavior.
 */
void cleanup_list(list *);

//doc to do
void reverse(list *lp);

/** @} */

#endif //LIST_H
