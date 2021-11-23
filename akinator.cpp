#include "akinator.h"

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

int main()
{
    Tree_t tree = {};

    TreeCtor(&tree);

    NodeInsert(&tree, tree.root, L_CHILD, "eblan?");
    // TreeDump(&tree);
    NodeInsert(&tree, tree.root, L_CHILD, "vovka");
    // TreeDump(&tree);
    NodeInsert(&tree, tree.root, R_CHILD, "dimka");
    // TreeDump(&tree);

    TreeDump(&tree);

    TreeDtor(&tree);

    TreeDump(&tree);

    return 0;
}

int test()
{
    // Node_t *root = nullptr;

    // NodeCreate(&root, nullptr, "1234");
    
    // TreeDump(root);

    

    // Node_t *topNode = (Node_t *)calloc (1, sizeof (Node_t));
    // scanf ("%ms", &(topNode->value));

    // Node_t *leftNode = (Node_t *)calloc (1, sizeof (Node_t));
    // scanf ("%ms", &(leftNode->value));

    // Node_t *rightNode = (Node_t *)calloc (1, sizeof (Node_t));
    // scanf ("%ms", &(rightNode->value));

    // topNode->left = leftNode;

    // topNode->right = rightNode;

    // Node_t *leftleftNode = (Node_t *)calloc (1, sizeof (Node_t));
    // scanf ("%ms", &(leftleftNode->value));

    // Node_t *leftrightNode = (Node_t *)calloc (1, sizeof (Node_t));
    // scanf ("%ms", &(leftrightNode->value));

    // topNode->left->left = leftleftNode;
    // topNode->left->right = leftrightNode;

    // Node_t *rightleftNode = (Node_t *)calloc (1, sizeof (Node_t));
    // scanf ("%ms", &(rightleftNode->value));

    // Node_t *rightrightNode = (Node_t *)calloc (1, sizeof (Node_t));
    // scanf ("%ms", &(rightrightNode->value));

    // topNode->right->left = rightleftNode;
    // topNode->right->right = rightrightNode;

    // TreeStatus status = TreeDump(topNode);
    // printf("status = %d\n", status);
    return 0;
}
