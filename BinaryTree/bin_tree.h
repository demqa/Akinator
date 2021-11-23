#ifndef BIN_TREE_H

#define BIN_TREE_H

#include <stdio.h>
#include "bin_tree_config.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <sys/stat.h>

struct Node_t
{
    Value_t value;

    Node_t  *parent;
    Node_t  *left;
    Node_t  *right;

};

enum TreeStatus
{
    // statuses
    TREE_IS_OK              = 1 << 0,
    NODE_IS_OK              = 1 << 0,
       
    // errors       
    BAD_ALLOC               = 1 << 9,
    NODE_PTR_IS_NULL        = 1 << 10,
    TREE_ROOT_IS_NULL       = 1 << 11,

    NODE_PARENT_IS_INVALID  = 1 << 12,
    NODE_LEFT___IS_INVALID  = 1 << 13,
    NODE_RIGHT__IS_INVALID  = 1 << 14,

    TREE_ROOT_IS_RUINED     = 1 << 15,
    
};

TreeStatus NodeCreate(Node_t **node, Node_t *parent, const Value_t value);
TreeStatus NodeVerify(Node_t *node);
TreeStatus TreeVerify(Node_t *tree_root);

TreeStatus TreeDump(Node_t *tree_root);

#endif