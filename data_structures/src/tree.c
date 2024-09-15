#include "data_structures/tree.h"

#include <stdlib.h>

tree create_tree(void * root, list children) {
    tree tree = malloc(sizeof(tree_node));
    tree->root = root;
    tree->children = children;
    return tree;
}