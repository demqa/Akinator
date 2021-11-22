#include "bin_tree.h"

int Filesize(FILE *stream, size_t *filesize)
{
    if (stream == nullptr)
        return -1;

    struct stat buff = {};
        
    fstat(fileno(stream), &buff);
    
    *filesize = buff.st_size;

    return 0;
}

int ReadBuffer(char **buffer, FILE *stream)
{
    if (stream == nullptr){
        return -1;
    }

    size_t filesize = 0;
    if (Filesize(stream, &filesize))
    {
        fclose(stream);
        return -2;
    }

    *buffer = (char *) calloc(filesize + 1, sizeof(char));
    if (buffer == nullptr)
    {
        fclose(stream);
        return -3;
    }

    size_t count = fread(*buffer, sizeof(char), filesize, stream);
    if (count != filesize)
    {
        fclose(stream);
        return -4;
    }

    *buffer[filesize] = '\0';

    fclose(stream);

    return 0;
}

// read tree
// write tree

TreeStatus CreateNode(Node_t **node)
{
    if (node == nullptr)
        return NODE_PTR_IS_NULL;
    
    *node = (Node_t *) calloc(1, sizeof(Node_t));
    if (*node == nullptr)
        return BAD_ALLOC;

    return NODE_IS_OK;
}

TreeStatus VerifyNode(Node_t *node)
{
    if (node == nullptr)
        return NODE_PTR_IS_NULL;

    int status = NODE_IS_OK;

    if (node->parent        != nullptr &&
        node->parent->left  != node    &&
        node->parent->right != node)
        status |= NODE_PARENT_IS_INVALID;

    if (node->left          != nullptr &&
        node->left->parent  != node)
        status |= NODE_LEFT___IS_INVALID;
    
    if (node->right         != nullptr &&
        node->right->parent != node)
        status |= NODE_RIGHT__IS_INVALID;

    return (TreeStatus) status;   
}

static TreeStatus VerifyNodes(Node_t *node)
{
    if (node == nullptr)
        return NODE_PTR_IS_NULL;
    
    int status = TREE_IS_OK;

    status |= VerifyNode(node);

    if (node->left  != nullptr) status |= VerifyNodes(node->left);
    if (node->right != nullptr) status |= VerifyNodes(node->right);

    return (TreeStatus) status;
}

TreeStatus VerifyTree(Node_t *tree_root)
{
    if (tree_root == nullptr)
        return TREE_ROOT_IS_NULL;

    int status = TREE_IS_OK;
    if (tree_root->parent != nullptr)
    {
        status |= TREE_ROOT_IS_RUINED;
    }

    if (tree_root->left  != nullptr)
        status |= VerifyNodes(tree_root->left);

    if (tree_root->right != nullptr)
        status |= VerifyNodes(tree_root->right);

    return (TreeStatus) status;
}

