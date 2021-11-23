#include "akinator.h"

int main()
{
    // Node_t *root = nullptr;

    // NodeCreate(&root, nullptr, "1234");
    
    // TreeDump(root);

    Node_t *topNode = (Node_t *)calloc (1, sizeof (Node_t));
    scanf ("%ms", &(topNode->value));

    Node_t *leftNode = (Node_t *)calloc (1, sizeof (Node_t));
    scanf ("%ms", &(leftNode->value));

    Node_t *rightNode = (Node_t *)calloc (1, sizeof (Node_t));
    scanf ("%ms", &(rightNode->value));

    topNode->left = leftNode;

    topNode->right = rightNode;

    Node_t *leftleftNode = (Node_t *)calloc (1, sizeof (Node_t));
    scanf ("%ms", &(leftleftNode->value));

    Node_t *leftrightNode = (Node_t *)calloc (1, sizeof (Node_t));
    scanf ("%ms", &(leftrightNode->value));

    topNode->left->left = leftleftNode;
    topNode->left->right = leftrightNode;

    Node_t *rightleftNode = (Node_t *)calloc (1, sizeof (Node_t));
    scanf ("%ms", &(rightleftNode->value));

    Node_t *rightrightNode = (Node_t *)calloc (1, sizeof (Node_t));
    scanf ("%ms", &(rightrightNode->value));

    topNode->right->left = rightleftNode;
    topNode->right->right = rightrightNode;

    TreeStatus status = TreeDump(topNode);
    printf("status = %d\n", status);

    return 0;
}