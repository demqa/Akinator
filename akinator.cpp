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

void sleep(int milliseconds)
{
    clock_t time_end = clock() + milliseconds * CLOCKS_PER_SEC / 1000;
    while (clock() < time_end) ;
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

AkinStatus TreeFill(Tree_t *tree, FILE *stream, char **buff)
{
    if (tree == nullptr)   return TREE_IS_NULL_;

    if (stream == nullptr) return STREAM_IS_NULL;
    
    size_t buff_size = 0;
    if (Filesize(stream, &buff_size))
        return FILESIZE_IS_ZERO;

    char *buffer = nullptr;

    if (ReadBuffer(&buffer, stream))
        return BUFFER_CANT_BE_READ;

    *buff = buffer;
    
    AkinStatus status = TreeRead(tree, buffer, buff_size);

    if (status)
        return status;

    return FUNC_IS_OK;
}

AkinStatus NodesOut(Node_t *node, FILE *stream)
{
    if (stream == nullptr) return STREAM_IS_NULL;

    if (node == nullptr)   return NODE_PTR_IS_NULL_;

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
        TreeDump(tree);
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
    fprintf(out, "\n");

    fclose(out);

    return (AkinStatus) status;
}

AkinStatus LineGet(char **string)
{
    if (string  == nullptr) return PTR_IS_NULL;

    if (*string != nullptr) return CANT_ALLOC_USED_PTR;

    *string = (char *) calloc(MAX_STRING_LEN, sizeof(char));
    if (*string == nullptr) return BAD_ALLOC_;

    char *ptr = fgets(*string, MAX_STRING_LEN, stdin);
    if (ptr == nullptr)
    {
        free(*string);
        return READ_WAS_UNSUCCESSFUL;
    };

    // HAHAHHAHAHAHAH
    if (strchr(ptr, '\n') == nullptr)
    {
        free(*string);
        return READ_WAS_UNSUCCESSFUL;
    }

    *(strchr(ptr, '\n')) = '\0';

    *string = ptr;

    return DEAD_INSIDE;
}

AkinStatus GuessWho/*is back*/(Tree_t *tree)
{
    if (TreeVerify(tree))
    {
        TreeDump(tree);
        return TREE_IS_DEAD;
    }

    int status = FUNC_IS_OK;

    Node_t *node = tree->root;

    if (node == nullptr)
    {
        assert(tree->size == 0 && tree->root == nullptr);

        NodeInsert(tree, tree->root, L_CHILD, "Неизвестно кто");
    }

    while (NodeIsTerminal(node) != NODE_IS_TERMINAL)
    {
        printf("Ваш персонаж %s?([y]es/[*]no)\n", node->value);

        char c = getchar();
        getchar();

        if (c == 'y')
            node = node->left;
        else
            node = node->right;
    }

    assert(NodeIsTerminal(node) == NODE_IS_TERMINAL);

    printf("Не мешай...\n");
    sleep(500);

    printf("Это %s?([y]es/[*]no)\n", node->value);

    char c = getchar();
    getchar();

    if (c == 'y')
    {
        printf("Пфф...");
        sleep(500);
        printf(" очевидно.\n");
    }
    else
    {
        printf("Чел, ты...\n");
        sleep(100);

        printf("Ладно, рассказывай...\n");
        sleep(300);

        printf("Кто это?\n");

        char *node_left = nullptr;
        status |= LineGet(&node_left);

        printf("И чем же %s отличается от %s?\n", node_left, node->value);

        printf("На размышление дается 5 секунд.\n");

        char *characteristic = nullptr;
        status |= LineGet(&characteristic);

        // addition
        char *node_right = node->value;

        node->value = characteristic;

        NodeInsert(tree, node, L_CHILD, node_left);
        NodeInsert(tree, node, R_CHILD, node_right);

        printf("Так уж и быть... даю тебе еще попытку...\n");
        printf("Воспользоваться?(y/n)\n");

        c = getchar();
        getchar();

        if (c == 'y')
        {
            return GuessWho(tree);
        }
        else
        {
            printf("Ну и отстань!(((\n");
        }
    }

    return (AkinStatus) status;
}

void Play()
{
    FILE *stream = fopen("tree", "rb");
    if (stream == nullptr)
        return;

    Tree_t tree = {};

    TreeCtor(&tree);

    char *buffer = nullptr;
    int   status = TreeFill(&tree, stream, &buffer);

    char c = 'p';
    while (c != 'e')
    {
        printf("Чё нада? [s]ave/[d]ump/[e]xit/play\n");
        c = getchar();
        getchar();

        switch (c)
        {
            case 's':
                TreeOut(&tree, "tree");
                break;

            case 'd':
                TreeDump(&tree);
                break;

            case 'e':
                printf("Ну и катись отсюда...");
                fflush(stdout);
                sleep(1000);
                printf(" лох\n");
                break;

            default:
                status |= GuessWho(&tree);
        }
    }

    // TreeDump(&tree);

    // status = TreeOut(&tree, "tree_");

    // fprintf(stderr, "status = %d\n", status);

    TreeDtor(&tree);
    free(buffer);
}

int main()
{
    Play();

    return 0;
}
