#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//Node struct
typedef struct node {
    int *keys; // array of keys (size 2t-1)
    int *children; // array of pointers to children (size 2t)
    int parent; // pointer to parent
    int n; // number of key in node
    int leaf; // it node leaf? - values 0(false)/ 1(true)
}node;

//Btree struct
typedef struct Btree{
    int t;
    int min; // minimal degree
    int max; // maximal degree
    node *root;
} btree;

//Constructor to create new node
void node_constructor(btree *tree, node *n, int parent){
    n->keys = malloc((tree->max + 1) * sizeof(int));
    n->children = malloc(tree->max * sizeof(int));
    n->parent = parent;
    n->n = 0;
    n->leaf = 1;
}

//Checking if node is root
int node_is_root(btree *tree, node *node){
    if(tree->root == node)
        return 1;
    return 0;
}

//Constructor to create new B-tree
void btree_constructor(btree *tree, int t){
    if(t < 2){
        printf("Argument t must be grater or equal than 2. You entered '%i'.\n", t);
        assert(0);
    }

    tree->t = t;
    tree->min = t - 1;
    tree->max = 2 * t - 1;
    tree->root = 0;
}

//Print informations of the B-tree
void btree_print(btree *tree){
    printf("\n###B-tree: t: %i, min: %i, max: %i, &root: %i###", tree->t, tree->min, tree->max, tree->root);
}

//Print informations of the node
void node_print(node *node){
    printf("\n#Node: &keys: [");
    for(int j = 0; j < node->n; j++)
        printf("%i ", &node->keys[j]);
    printf("], &children:[");
    if(node->n > 0)
        for(int j = 0; j <= node->n; j++)
            printf("%i ", &node->children[j]);
    printf("], &parent: %i, n: %i, leaf: %i#", node->parent, node->n, node->leaf);
}

int main()
{
    btree strom;
    int t = 2;
    btree_constructor(&strom, t);
    btree_print(&strom);

    node uzel;
    node_constructor(&strom, &uzel, 0);
    node_print(&uzel);
    return 0;
}
