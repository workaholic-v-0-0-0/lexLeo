#ifndef TREE_H
#define TREE_H

#include "data_structures/list.h"

/**
 * @brief Structure representing a tree with children stored in a list.
 *
 * This structure is used to represent a tree where each node can have multiple children.
 * The children of each node are stored in a list.
 *
 * @typedef tree
 * @struct tree_struct
 *
 * @var tree::root
 * Generic pointer to the root of the tree. This pointer can be cast to the appropriate data type
 * according to the specific needs of the implementation.
 *
 * @var tree::children
 * List of children of the node. Each child is also an instance of this `tree` structure.
 * The management of this list should ensure data integrity and proper memory release.
 */
typedef struct tree_node {
    void *root;    /**< Pointer to the root of the tree. */
    list children; /**< List containing the children of this node. */
} tree_node, *tree;

//doc to do
tree create_tree(void * root, list children);

#endif //TREE_H
