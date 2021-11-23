#include "bin_tree.h"

static int Filesize(FILE *stream, size_t *filesize)
{
    if (stream == nullptr)
        return -1;

    struct stat buff = {};
        
    fstat(fileno(stream), &buff);
    
    *filesize = buff.st_size;

    return 0;
}

static int ReadBuffer(char **buffer, FILE *stream)
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

TreeStatus NodeCreate(Node_t **node, Node_t *parent, const Value_t value)
{
    if (node == nullptr)
        return NODE_PTR_IS_NULL;
    
    *node = (Node_t *) calloc(1, sizeof(Node_t));
    if (*node == nullptr)
        return BAD_ALLOC;

    (*node)->value  = value;
    (*node)->parent = parent;

    return NODE_IS_OK;
}

TreeStatus NodeVerify(Node_t *node)
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

static TreeStatus NodesVerify(Node_t *node)
{
    if (node == nullptr)
        return NODE_PTR_IS_NULL;
    
    int status = TREE_IS_OK;

    status |= NodeVerify(node);

    if (node->left  != nullptr) status |= NodesVerify(node->left);
    if (node->right != nullptr) status |= NodesVerify(node->right);

    return (TreeStatus) status;
}

TreeStatus TreeVerify(Node_t *tree_root)
{
    if (tree_root == nullptr)
        return TREE_ROOT_IS_NULL;

    int status = TREE_IS_OK;
    if (tree_root->parent != nullptr)
    {
        status |= TREE_ROOT_IS_RUINED;
    }

    if (tree_root->left  != nullptr)
        status |= NodesVerify(tree_root->left);

    if (tree_root->right != nullptr)
        status |= NodesVerify(tree_root->right);

    return (TreeStatus) status;
}

#define PRINT_ERROR(error_code)                  \
{                                                 \
    if (status & error_code)                       \
    {                                               \
        fprintf(TREE_LOG_FILE_DEFAULT, #error_code); \
        fprintf(TREE_LOG_FILE_DEFAULT, "\n");         \
    }                                                  \
};

FILE *TREE_LOG_FILE_DEFAULT = stderr;

static void PrintNode(const Node_t *node, const size_t *number_of_node, const char color[COLOR_MAX_LEN], FILE *dump_file)
{
    if (node == nullptr || number_of_node == nullptr || color == nullptr || dump_file == nullptr)
        return;
    
    fprintf(dump_file, "    node%lu  [fillcolor=\"%s\", ", *number_of_node, color);

    fprintf(dump_file, "label=\"%s\"", node->value);
    // TODO
    // FUNC PRINT_VALUE_DUMP

    fprintf(dump_file, "];\n");

    return;
}

static void PrintEdge(const size_t from, const size_t to, const char color[COLOR_MAX_LEN], FILE *dump_file)
{
    if (color == nullptr || dump_file == nullptr)
        return;
    
    fprintf(dump_file, "    node%lu -> node%lu;\n", from, to);

    return;
}


static TreeStatus NodeDump(const Node_t *node, size_t *number_of_node, FILE *dump_file)
{
    int status = NODE_IS_OK;

    if (*number_of_node == 0)
    {
        PrintNode(node, number_of_node, root_color, dump_file);
    }
    else
    if (node->left == nullptr && node->right == nullptr)
    {
        PrintNode(node, number_of_node, internal_color, dump_file);
    }
    else
    {
        PrintNode(node, number_of_node, default_color, dump_file);
    }

    size_t current_number_of_node = *number_of_node;

    if (node->left)
    {
        PrintEdge(current_number_of_node, ++(*number_of_node), left_color, dump_file);

        status |= NodeDump(node->left, number_of_node, dump_file);
    }

    if (node->right)
    {
        PrintEdge(current_number_of_node, ++(*number_of_node), right_color, dump_file);

        status |= NodeDump(node->right, number_of_node, dump_file);
    }

    return (TreeStatus) status;
}

TreeStatus TreeDump(Node_t *tree_root)
{
    int status = TreeVerify(tree_root);
    // if (status)
    // {
    //     fprintf(TREE_LOG_FILE_DEFAULT, "CANT DUMP, TREE IS RUINED\n");
        
    //     PRINT_ERROR(BAD_ALLOC);
    //     PRINT_ERROR(NODE_PTR_IS_NULL);
    //     PRINT_ERROR(TREE_ROOT_IS_NULL);

    //     PRINT_ERROR(NODE_PARENT_IS_INVALID);
    //     PRINT_ERROR(NODE_LEFT___IS_INVALID);
    //     PRINT_ERROR(NODE_RIGHT__IS_INVALID);

    //     PRINT_ERROR(TREE_ROOT_IS_RUINED);

    //     return (TreeStatus) status;
    // }

    size_t number_of_node = 0;
    FILE *dump_file = fopen("dump", "w");
    
    fputs("digraph structs {\n", dump_file);
    fputs("    node [color=black, shape=box, style=\"filled\"];\n", dump_file);

    status = NodeDump(tree_root, &number_of_node, dump_file);

    fputs("}\n", dump_file);

    fclose(dump_file);

    system("dot dump -T png -o dump.png");
    system("eog dump.png");

    return (TreeStatus) status;
}
