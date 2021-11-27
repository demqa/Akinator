#include "akinator.h"
#include "debug_lib.h"

#include "Stack/print_func.cpp"

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

struct MemoryDefender
{
    size_t size;
    size_t capacity;
    char **allocs;
};

int DefenderCtor  (MemoryDefender *defender)
{
    if (defender == nullptr) return -1;

    defender->allocs   = (char **) calloc(MAX_ALLOCS, sizeof(char *));
    defender->capacity = MAX_ALLOCS;
    defender->size     = 0;

    return 0;
}

int DefenderPush  (MemoryDefender *defender, char *memory)
{
    if (defender == nullptr) return -1;
    
    if (memory   == nullptr) return -2;

    defender->allocs[defender->size++] = memory;

    return 0;
}

int DefenderDtor  (MemoryDefender *defender)
{
    if (defender == nullptr) return -1;

    defender->size = 0;
    defender->capacity = 0;

    for (size_t iter = 0; iter < defender->size; ++iter)
        free(defender->allocs[iter]);
    
    free(defender->allocs);

    return 0;
}

int DefenderIsFull(MemoryDefender *defender)
{
    if (defender == nullptr) return -1;

    return (defender->size >= defender->capacity) ? 1 : 0;
}

AkinStatus GetString(char **ptr, char *end_ptr, char **string)
{
    if (ptr == nullptr || *ptr == nullptr || end_ptr == nullptr) return PTR_IS_NULL;

    if (*ptr > end_ptr)
        return PTR_BIGGER_BUFF_END;

    *string    = *ptr;
    int number =  0;

    assert(**ptr != '}');

    while (*(*ptr + 1) != '}' && *(*ptr + 1) != '{')
    {
        if (*ptr + 1 >= end_ptr)
            return PIZDEC;

        if (number + 1 >= MAX_STRING_LEN)
            return MAX_STRING_LEN_REACHED;

        (*string)[number++] = **ptr;

        (*ptr)++;
    }

    assert(number < MAX_STRING_LEN);

    if (number == 0)
        return BASE_IS_EMPTY;

    if ((*string)[number] != ' ')
        fprintf(stderr, "WHY YOU CANT JUST PUT SPACE AFTER NAME\n");

    (*string)[number] = '\0';

    assert(*ptr < end_ptr);
    
    return DEAD_INSIDE;
}

AkinStatus TreeReadProcessing(Tree_t *tree, Node_t *node, char **ptr, char *end_ptr)
{
    if (tree == nullptr)   return TREE_IS_NULL_;
    
    if (ptr  == nullptr || *ptr == nullptr || end_ptr == nullptr) return PTR_IS_NULL;

    if (*ptr > end_ptr)    return PTR_BIGGER_BUFF_END;

    int status = FUNC_IS_OK;

    while (*ptr <= end_ptr)
    {
        if (**ptr == '{')
        {
            (*ptr)++;

            char *string = nullptr;
            status |= GetString(ptr, end_ptr, &string);
            if (status) return (AkinStatus) status;
            
            (*ptr)++;

            if (node == nullptr)
            {
                NodeInsert(tree, node, L_CHILD, string);
                node = tree->root;
            }
            else
            if (node->left == nullptr)
            {
                NodeInsert(tree, node, L_CHILD, string);
                node = node->left;
            }
            else
            {
                NodeInsert(tree, node, R_CHILD, string);
                node = node->right;
            }
        }
        else
        if (**ptr == '}')
        {
            if (node == nullptr)   return FUCK_MY_LIFE;

            (*ptr)++;

            if (node == tree->root)
            {
                // (*ptr) = end_ptr + 1;   // soft
                break;                    // horny
            }

            node = node->parent;
        }
    }
    
    return DEAD_INSIDE;
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

    if (node   == nullptr) return NODE_PTR_IS_NULL_;

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

AkinStatus GetLine(char **string)
{
    if (string  == nullptr) return PTR_IS_NULL;

    if (*string != nullptr) return CANT_ALLOC_USED_PTR;

    *string     = (char *) calloc(MAX_STRING_LEN, sizeof(char));
    if (*string == nullptr) return BAD_ALLOC_;

    char   c    = 'd';
    size_t ptr  =  0;

    while (true)
    {
        if (ptr == MAX_STRING_LEN)
        {
            free(*string);
            return READ_WAS_UNSUCCESSFUL;
        }

        c = getchar();
        if (c == '\n')
        {
            (*string)[ptr] = '\0';
            return FUNC_IS_OK;
        }

        (*string)[ptr++] = c;
    }

    assert(false);
    return DEAD_INSIDE;
}

AkinStatus GuessWho/*is back*/(Tree_t *tree, MemoryDefender *defender)
{
    if (TreeVerify(tree))
    {
        TreeDump(tree);
        return TREE_IS_DEAD;
    }

    if (defender == nullptr) return DEFENDER_IS_NULLPTR;

    int status = FUNC_IS_OK;

    Node_t *node = tree->root;

    if (node == nullptr)
    {
        assert(tree->size == 0 && tree->root == nullptr);

        NodeInsert(tree, node, L_CHILD, initial_elem);
        node = tree->root;
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
        printf("Чеeeл...");
        sleep(500);

        printf("ты...\n");
        sleep(300);

        printf("Ладно, рассказывай...\n");
        sleep(300);

        printf("Кто это?\n");

        char *node_left_string = nullptr;
        status |= GetLine(&node_left_string);

        if (DefenderIsFull(defender))
        {
            free(node_left_string);
            return MEMORY_IS_FULL;
        }

        DefenderPush(defender, node_left_string);

        printf("И чем же %s отличается от %s?\n", node_left_string, node->value);
                                           // \n
        printf("На размышление дается 5 секунд.\n");

        char *characteristic = nullptr;
        status |= GetLine(&characteristic);
        
        if (DefenderIsFull(defender))
        {
            free(characteristic);
            printf("Ну ты либо сохраняйся, либо иди лесом! Я не могу запомнить сразу так много!");

            return MEMORY_IS_FULL;
        }

        DefenderPush(defender, characteristic);

        // addition
        char *node_right_string = node->value;

        node->value = characteristic;

        NodeInsert(tree, node, L_CHILD, node_left_string);
        NodeInsert(tree, node, R_CHILD, node_right_string);

        printf("Так уж и быть... даю тебе еще попытку...\n");
        printf("Воспользоваться?(y/n)\n");

        c = getchar();
        getchar();

        if (c == 'y')
        {
            return GuessWho(tree, defender);
        }
        else
        {
            printf("Ну и отстань!(((\n");
        }
    }

    return (AkinStatus) status;
}

// #define ASSERT_(smth, do) if (smth){ do; };

AkinStatus FindHeroNode(Node_t *node, char *hero, Node_t **dest)
{
    if (node  == nullptr) return NODE_PTR_IS_NULL_;
    if (hero  == nullptr) return PTR_IS_NULL;
    if (dest  == nullptr) return INVALID_NODE_PTR;

    if (*dest != nullptr) return NODE_IS_FOUNDED;

    int status = FUNC_IS_OK;
    if (strcmp(node->value, hero) == 0)
    {
        *dest = node;
        return NODE_IS_FOUNDED;
    }
    else
    {
        if (node->left)  status |= FindHeroNode(node->left,  hero, dest);
        if (node->right) status |= FindHeroNode(node->right, hero, dest);
        
        if (status == NODE_IS_FOUNDED)
            return (AkinStatus) status;
    }
    
    return (AkinStatus) status;
}

AkinStatus FindHero    (Tree_t *tree, char *hero, Node_t **dest)
{
    if (TreeVerify(tree))
    {
        TreeDump(tree);
        return TREE_IS_DEAD;
    }

    if (hero == nullptr) return PTR_IS_NULL;

    if (dest == nullptr || *dest != nullptr) return INVALID_NODE_PTR;

    if (tree->size == 0) return BASE_IS_EMPTY;

    return FindHeroNode(tree->root, hero, dest);
}

AkinStatus DescribeHeroNode(Node_t *node, Stack_t *stack)
{
    if (node == nullptr) return NODE_PTR_IS_NULL_;

    if (StackVerify(stack))
    {
        return STACK_IS_RUINED;
    }

    while (node->parent != nullptr)
    {
        StackPush(stack, (char *) node);
        node = node->parent;
    }

    StackPush(stack, (char *) node);

    return FUNC_IS_OK;
}

AkinStatus DescribeHero(Tree_t *tree, MemoryDefender *defender)
{
    if (TreeVerify(tree))
    {
        TreeDump(tree);
        return TREE_IS_DEAD;
    }

    printf("Чё те там надо было? ");
    sleep(500);
    printf("Кого найти?\n");

    char *hero = nullptr;

    int status = GetLine(&hero);
    if (status) return (AkinStatus) status;

    if (DefenderIsFull(defender))
    {
        free(hero);
        return MEMORY_IS_FULL;
    }
    
    DefenderPush(defender, hero);

    Node_t *node = nullptr;

    Stack_t stack = {};
    StackCtor(&stack, 8, PrintString);

    printf("Да дай ты подумать-то, то ёпта...\n");
    fflush(stdout);
    sleep(1500);
    printf("Да отвали, не буду я ничего делать...\n");
    fflush(stdout);
    sleep(500);
    printf("Ладно, жди...");
    fflush(stdout);
    sleep(1000);
    printf("\n");

    if (FindHero(tree, hero, &node) == NODE_IS_FOUNDED)
    {
        assert(node != nullptr);

        status |= DescribeHeroNode(node, &stack);

        assert(stack.size != 0);

        printf("%s нашёлся! он ", node->value);

        Node_t **nodes     = (Node_t **) stack.data;
        size_t   elem      = stack.size - 1;
        Node_t  *tmp_node  = nullptr;

        do{
            tmp_node = nodes[elem];
            if (tmp_node->right == nodes[elem - 1])
                printf("не ");
            printf("%s, ", tmp_node->value);
            
            elem--;
        }while (elem > 0 && elem != (size_t)(-1));

        printf("а также просто пусечка!\n", node->value);
    }
    else
    {
        printf("У нас нет такого бойца!\n");
    }

    return (AkinStatus) status;
}

AkinStatus CompareHeroes(Tree_t *tree, MemoryDefender *defender)
{
    if (TreeVerify(tree))
    {
        TreeDump(tree);
        return TREE_IS_DEAD;
    }

    printf("Чё те там надо было?");
    sleep (500);
    printf(" Кого сравнить?\n");

    char *heroes[2] = {};

    for (int i = 0; i < 2; i++)
    {
        printf("Персонаж %d:\n", i + 1);

        int status = GetLine(&heroes[i]);
        if (status) return (AkinStatus) status;

        if (DefenderIsFull(defender))
        {
            free(heroes[i]);
            return MEMORY_IS_FULL;
        }

        DefenderPush(defender, heroes[i]);
    }

    Node_t *nodes[2] = {};

    Stack_t stacks[2] = {};
    for (int i = 0; i < 2; i++)
        StackCtor(&stacks[i], 8, PrintString);

    printf("\n");

    int status = NODE_IS_FOUNDED;
    for (int i = 0; i < 2; i++)
        status &= FindHero(tree, heroes[i], &nodes[i]);

    if (status == NODE_IS_FOUNDED)
    {
        assert(nodes != nullptr);

        for (int i = 0; i < 2; i++)
            assert(nodes[i] != nullptr);

        for (int i = 0; i < 2; i++)
            status |= DescribeHeroNode(nodes[i], &stacks[i]);

        for (int i = 0; i < 2; i++)
            assert(stacks[i].size);

        Node_t **nodess  [2]  = {};
        size_t   elem    [2]  = {};
        Node_t  *tmp_node[2]  = {};
        for (int i = 0; i < 2; i++)
        {
            nodess  [i] = (Node_t **) stacks[i].data;
            elem    [i] = stacks[i].size - 1;
            tmp_node[i] = nodess[i][elem[i]];
        }

        int flag = 0;
        while (tmp_node[0] == tmp_node[1] && tmp_node[0]->right == nodess[0][elem[0]] && tmp_node[1]->right == nodess[1][elem[1]])
        {   
            if (flag++ == 0) printf("%s и %s похожи тем, что они ", nodes[0]->value, nodes[1]->value);
            if (tmp_node[0]->right == nodess[0][elem[0]])
            {
                printf("не ");
            }
            printf("%s, ", tmp_node[0]->value);
            
            if (elem[0] == 0 || elem[1] == 0)
            {
                return FUCK_MY_LIFE;
            }

            for (int i = 0; i < 2; i++)
                tmp_node[i] = nodess[i][--elem[i]];
        }
        
        if (flag == 0) printf("%s и %s так близки, \n", nodes[0]->value, nodes[1]->value);

        for (int i = 0; i < 2; i++)
        {
            if (i == 0)
                printf("но %s ", nodes[i]->value);
            else
                printf("a %s ",  nodes[i]->value);

            do{
                tmp_node[i] = nodess[i][elem[i]];
                if (tmp_node[i]->right == nodess[i][elem[i] - 1])
                    printf("не ");
                printf("%s, ", tmp_node[i]->value);
                
                elem[i]--;
            }while (elem[i] > 0 && elem[i] != (size_t)(-1));

            printf("а также просто пусечка!\n");
        }
    }
    else
    {
        printf("У нас нет таких бойцов!\n");
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

    MemoryDefender defender = {};

    DefenderCtor(&defender);

    char c = 'p';
    while (c != 'e')
    {
        if (status == NODE_IS_FOUNDED) status = FUNC_IS_OK;

        if (status)
        {
            TreeDump(&tree);
            fprintf(stderr, "status = %d\n", status);
            break;
        }

        printf("Чё нада? [s]ave/[d]ump/[e]xit/desc[r]ibe/[c]ompare/play\n");
        c = getchar();
        getchar();

        switch (c)
        {
            case 's':
                TreeOut(&tree, "tree");
                DefenderDtor(&defender);
                TreeFill(&tree, stream, &buffer);
                DefenderCtor(&defender);
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

            case 'r':
                status |= DescribeHero(&tree, &defender);
                break;

            case 'c':
                status |= CompareHeroes(&tree, &defender);
                break;

            default:
                status |= GuessWho(&tree, &defender);
                break;
        }
    }

    // TreeDump(&tree);

    // status = TreeOut(&tree, "tree_");

    // fprintf(stderr, "status = %d\n", status);

    DefenderDtor(&defender);

    TreeDtor(&tree);
    free(buffer);
}

int main()
{
    Play();

    return 0;
}
