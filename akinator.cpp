#include "akinator.h"
#include "debug_lib.h"

#define HELLO do{ fprintf(stderr, "i'm at line №%d.\n", __LINE__); }while(0);

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
    if (stream == nullptr)
        return -1;

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

    (*buffer)[filesize] = '\0';

    fclose(stream);

    return 0;
}

AkinStatus GetString(char **ptr, char *end_ptr, char **string)
{
    if (ptr == nullptr || *ptr == nullptr || end_ptr == nullptr) return PTR_IS_NULL;

    if (*ptr > end_ptr)
        return PTR_BIGGER_BUFF_END;

    *string = *ptr;
    int number = 0;

    assert(**ptr != '}');

    while (*(*ptr + 1) != '}' && *(*ptr + 1) != '{')
    {
        if (*ptr + 1 >= end_ptr)
            return GABELLA;

        if (number + 1 >= MAX_STRING_LEN)
            return MAX_STRING_LEN_REACHED;

        (*string)[number++] = **ptr;

        (*ptr)++;
    }

    assert(number < MAX_STRING_LEN);

    if ((*string)[number] != ' ')
        fprintf(stderr, "WHY YOU CANT JUST PUT SPACE AFTER NAME\n");

    (*string)[number] = '\0';

    assert(*ptr < end_ptr);
    
    return DEAD_INSIDE;
}

#define GO_NEXT_CYCLE(NODE)  \
do{                           \
    status = TreeReadProcessing(tree, (NODE), ptr, end_ptr); \
    if (status)                 \
        return status;           \
}while(0);

AkinStatus TreeReadProcessing(Tree_t *tree, Node_t *node, char **ptr, char *end_ptr)
{
    if (tree == nullptr)   return TREE_IS_NULL_;
    
    if (ptr  == nullptr || *ptr == nullptr || end_ptr == nullptr) return PTR_IS_NULL;

    if (*ptr > end_ptr)    return PTR_BIGGER_BUFF_END;

    AkinStatus status = FUNC_IS_OK;

    if (**ptr == '{')
    {        
        (*ptr)++;

        GO_NEXT_CYCLE(node);
    }

    if (*ptr < end_ptr && **ptr == '}')
    {
        if (node == nullptr)
            return FUCK_MY_LIFE;
        
        (*ptr)++;

        if (node->parent != nullptr)
            GO_NEXT_CYCLE(node->parent);
    }
    
    if (*ptr < end_ptr && **ptr != '{' && **ptr != '}')
    {
        char *string = nullptr;

        status = GetString(ptr, end_ptr, &string);
        if (status)
            return status;

        (*ptr)++;

        if (node == nullptr)
        {
            assert(tree->root == node && tree->size == 0);

            NodeInsert(tree, node, L_CHILD, string);
            
            GO_NEXT_CYCLE(tree->root);
        }
        else
        if (node->left == nullptr)
        {
            NodeInsert(tree, node, L_CHILD, string);

            GO_NEXT_CYCLE(node->left);
        }
        else
        {
            NodeInsert(tree, node, R_CHILD, string);

            GO_NEXT_CYCLE(node->right);
        }
    }

    if (*ptr != end_ptr) return READ_WAS_UNSUCCESSFUL;

    return READ_SUCCS;
}

AkinStatus TreeRead(Tree_t *tree, char *buffer, size_t buff_size)
{
    if (tree == nullptr)   return TREE_IS_NULL_;

    if (buffer == nullptr) return BUFFER_IS_NULLPTR;

    if (buff_size == 0)    return FILESIZE_IS_ZERO;

    char *ptr     = buffer;
    char *end_ptr = buffer + buff_size - 1;

    int counter = 0;
    while (ptr <= end_ptr)
    {
        if (*ptr == '{')
            counter++;

        if (*ptr == '}')
            counter--;
        
        if (counter < 0)
        {
            return EXPR_IS_INVALID;
        }

        ptr++;
    }

    ptr = buffer;
    AkinStatus status = TreeReadProcessing(tree, tree->root, &ptr, end_ptr);
    if (status)
        return status;
    
    return FUNC_IS_OK;
}

AkinStatus TreeFill(Tree_t *tree, FILE *stream)
{
    if (tree == nullptr)
        return TREE_IS_NULL_;

    if (stream == nullptr)
        return STREAM_IS_NULL;
    
    size_t buff_size = 0;
    if (Filesize(stream, &buff_size))
        return FILESIZE_IS_ZERO;

    char *buffer = nullptr;

    if (ReadBuffer(&buffer, stream))
        return BUFFER_CANT_BE_READ;
    
    AkinStatus status = TreeRead(tree, buffer, buff_size);
    free(buffer);

    if (status)
        return status;

    return FUNC_IS_OK;
}

AkinStatus NodesOut(Node_t *node, FILE *stream)
{
    if (stream == nullptr)
        return STREAM_IS_NULL;

    if (node == nullptr)
        return NODE_PTR_IS_NULL_;

    int status = FUNC_IS_OK;
    fprintf(stream, "{");
    fprintf(stream, node->value);
    fprintf(stream, " ");

    if (node->left  != nullptr) status |= NodesOut(node->left,  stream);
    if (node->right != nullptr) status |= NodesOut(node->right, stream);

    fprintf(stream, "}");

    return (AkinStatus) status;
}

AkinStatus TreeOut(Tree_t *tree, const char *out_filename)
{
    if (TreeVerify(tree))
    {
        // TreeDump(tree);
        return TREE_IS_DEAD;
    }

    if (out_filename == nullptr) return FILENAME_PTR_IS_NULL;

    FILE *out = fopen(out_filename, "w");
    if (out == nullptr)
        return CANT_OPEN_FILE;

    int status = FUNC_IS_OK;
    if (tree->root == nullptr)
        status |= CANT_WRITE_EMPTY_TREE;
    
    status |= NodesOut(tree->root, out);

    fclose(out);

    return (AkinStatus) status;
}

int main()
{
    FILE *stream = fopen("tree", "rb");
    if (stream == nullptr)
        return -1;

    Tree_t tree = {};

    TreeCtor(&tree);

    AkinStatus status = TreeFill(&tree, stream);

    TreeDump(&tree);

    status = TreeOut(&tree, "tree_new");

    fprintf(stderr, "status = %d\n", status);

    TreeDtor(&tree);

    return 0;
}
