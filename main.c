#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//Node struct
typedef struct node {
    int *keys; // array of keys (size 2t-1)
    int **children; // array of pointers to children (size 2t)
    int parent; // pointer to parent
    int n; // number of keys in node
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

void node_insert(node *n, int key){
    int insert_index = -1;
    for(int j = 0; j < n->n; j++){
        if(key < n->keys[j]){
            insert_index = j;
            break;
        }
    }

    if(insert_index == -1){
        n->keys[n->n] = key;
        printf("n: %i", n->keys[n->n]);
        n->n += 1;
    } else {
        n->n += 1;
        int temp = key;
        for(int i = insert_index; i < n->n; i++){
            int t = n->keys[i];
            n->keys[i] = temp;
            temp = t;
        }
    }
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

void btree_insert(btree *tree, int key){
    node *last_node;

}

int btree_search(node *n, node **last_node, int key){
    int i = 0;
    while(i < n->n && key > n->keys[i])
        i++;

    if(i < n->n && n->keys[i] == key){
        *last_node = n;
        return i;
    }

    if(n->leaf){
        *last_node = n;
        return -1;
    }

    return btree_search(n->children[i], last_node, key);
}

//Print informations of the B-tree
void btree_print(btree *tree){
    printf("\n###B-tree: t: %i, min: %i, max: %i, &root: %i###", tree->t, tree->min, tree->max, tree->root);
}

//Print informations of the node
void node_print(node *n){
    printf("\n#Node(&%i): keys: [ ", n);
    for(int j = 0; j < n->n; j++)
        printf("%i ", n->keys[j]);
    printf("], &children:[ ");
    if(!n->leaf)
        for(int j = 0; j <= n->n; j++)
            printf("%i ", n->children[j]);
    printf("], &parent: %i, n: %i, leaf: %i#", n->parent, n->n, n->leaf);
}

void test(){
    int t = 2;

    btree strom;
    btree_constructor(&strom, t);

    node uzel_root;
    node_constructor(&strom, &uzel_root, 0);
    node_insert(&uzel_root, 26);

    node uzel_1;
    node_constructor(&strom, &uzel_1, 0);
    node_insert(&uzel_1, 6);
    node_insert(&uzel_1, 12);

    node uzel_2;
    node_constructor(&strom, &uzel_2, 0);
    node_insert(&uzel_2, 42);
    node_insert(&uzel_2, 51);
    node_insert(&uzel_2, 62);

    strom.root = &uzel_root;
    uzel_root.children[0] = &uzel_1;
    uzel_root.children[1] = &uzel_2;
    uzel_root.leaf = 0;

    btree_print(&strom);
    node_print(&uzel_root);
    node_print(&uzel_1);
    node_print(&uzel_2);

    node *test;
    int key = btree_search(&uzel_root, &test, 62);

    printf("\naddress:%i, key:%i", test, key);
}

int main()
{
    test();
    return 0;
}
