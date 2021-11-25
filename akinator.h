#ifndef AKINATOR_H

#define AKINATOR_H

#include "akinator_config.h"

#include "BinaryTree/bin_tree.h"

const size_t MAX_STRING_LEN = 100;

enum AkinStatus
{
    // statuses
    FUNC_IS_OK          = 0,
    READ_SUCCS          = 0,
    DEAD_INSIDE         = 0,

    // errors

    TREE_IS_NULL_                 = 1 << 10,
    TREE_ROOT_IS_NULL_            = 1 << 11,
    NODE_PTR_IS_NULL_             = 1 << 12,

    STREAM_IS_NULL                = 1 << 13,
    BUFFER_IS_NULLPTR             = 1 << 14,
    FILESIZE_IS_ZERO              = 1 << 15,
    BUFFER_CANT_BE_READ           = 1 << 16,
    EXPR_IS_INVALID               = 1 << 17,
    PTR_IS_NULL                   = 1 << 18,
    PTR_BIGGER_BUFF_END           = 1 << 19,
    READ_WAS_UNSUCCESSFUL         = 1 << 20,

    FUCK_MY_LIFE                  = 1 << 29,
    PIZDEC                        = 1 << 30,
    GABELLA                       = 1 << 31,
};

#endif
