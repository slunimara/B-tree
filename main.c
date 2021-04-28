#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int static test_var = 0;

//Node structure
typedef struct node {
    int *keys; // array of keys (size 2t-1)
    struct node** children; // array of pointers to children (size 2t)
    struct node* parent; // pointer to parent
    int n; // number of keys in node
    int leaf; // it node leaf? - values 0(false)/ 1(true)
}node;

//B-tree structure
typedef struct Btree{
    int t;
    int min; // minimal degree
    int max; // maximal degree
    node *root;
} btree;

//Constructor to create new node
node *node_constructor(btree *tree, int parent){
    node *n = malloc(sizeof(node));
    if(!tree->root)
        tree->root = n;
    n->keys = malloc((tree->max + 1) * sizeof(int));
    n->children = malloc(tree->max * sizeof(node));
    n->parent = parent;
    n->n = 0;
    n->leaf = 1;

    return n;
}

//Checking if node is root
int node_is_root(btree *tree, node *node){
    if(tree->root == node)
        return 1;
    return 0;
}

//Insert key into the node
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

//Print children of the B-tree
void node_print_children(btree *tree, node *n){
    node_print(n);
    if(!n->leaf){
        int j = 0;
        for(j; j < n->n; j++){
            node_print_children(tree, n->children[j]);
        }
        node_print_children(tree, n->children[j]);
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

//Insert key into the btree
void btree_insert(btree *tree, int key){
    node *n;
    btree_search(tree->root, &n, key);
    btree_insert_divide(tree, n, key);
}

//Insert key into the btree or divide node
void btree_insert_divide(btree *tree, node *n, int key){
    if(n->n == tree->max){
        int median = n->n / 2;
        int is_root = node_is_root(tree, n);

        if(is_root){
            int median_key = n->keys[median];
            node *new_root = node_constructor(tree, 0);
            node_insert(new_root, median_key);
            new_root->leaf = 0;

            node *left_node = tree->root;
            left_node->parent = new_root;

            node *right_node = node_constructor(tree, new_root);

            tree->root = new_root;
            new_root->children[0] = left_node;
            new_root->children[1] = right_node;

            node_transfer(left_node, right_node, median);


            if(median_key < key)
                btree_insert_divide(tree, right_node, key);
            else
                btree_insert_divide(tree, left_node, key);
        } else {
            int median_key = n->keys[median];
            btree_insert_divide(tree, n->parent, median_key);
            int insert_key_parent = get_key_index(n->parent, median_key);

            node *parent = n->parent;
            node *left_node = n;
            node *right_node = node_constructor(tree, parent);

            int temp = 0;
            for(int i = insert_key_parent + 1; i <= parent->n; i++){
                int t = parent->children[i];
                parent->children[i] = temp;
                temp = t;
            }
            parent->children[insert_key_parent] = left_node;
            parent->children[insert_key_parent + 1] = right_node;

            node_transfer(left_node, right_node, median);

            if(median_key < key)
                btree_insert_divide(tree, right_node, key);
            else
                btree_insert_divide(tree, left_node, key);
        }
    } else node_insert(n, key);
}

//Help function for transferring divided node
void node_transfer(node *left_node, node *right_node, int median){
    for(int j = median + 1; j < left_node->n; j++){
        node_insert(right_node, left_node->keys[j]);

        left_node->n -= 1;
        left_node->keys[j] = 0;

        if(!left_node->leaf){
            int n_index = right_node->n - 1;
            right_node->leaf = 0;

            right_node->children[n_index] = left_node->children[j];
            right_node->children[n_index]->parent = right_node;
            left_node->children[j] = 0;
            if((j + 1) >= left_node->n){
                right_node->children[n_index + 1] = left_node->children[j + 1];
                right_node->children[n_index + 1]->parent = right_node;
                left_node->children[j + 1] = 0;
            }
        }
    }

    left_node->n -= 1;
    left_node->keys[median] = 0;
}

int get_key_index(node *n, int key){
    for(int j = 0; j < n->n; j++)
        if(n->keys[j] == key)
            return j;
    return -1;
}

//Search tree inside btree
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

//Print all nodes of the tree
void btree_print_nodes(btree *tree){
    node_print_children(tree, tree->root);
}

void test(){
    int t = 2;

    btree strom;
    btree_constructor(&strom, t);

    node *uzel_root = node_constructor(&strom, 0);

    btree_insert(&strom, 26);
    btree_insert(&strom, 12);
    btree_insert(&strom, 42);
    btree_insert(&strom, 6);
    btree_insert(&strom, 51);
    btree_insert(&strom, 62);
    btree_insert(&strom, 5);
    btree_insert(&strom, 1);
    btree_insert(&strom, 70);
    btree_insert(&strom, 83);
    btree_insert(&strom, 97);
    btree_insert(&strom, 100);
    btree_insert(&strom, 103);
    btree_insert(&strom, 137);
    btree_insert(&strom, 159);


    btree_print(&strom);
    btree_print_nodes(&strom);
    /*
    node_insert(&uzel_root, 26);

    node uzel_1;
    node_constructor(&strom, &uzel_1, 0);
    node_insert(&uzel_1, 6);
    node_insert(&uzel_1, 12);

    node uzel_2;
    node_constructor(&strom, &uzel_2, 0);
    node_insert(&uzel_2, 42);
    node_insert(&uzel_2, 51);

    uzel_root.children[0] = &uzel_1;
    uzel_root.children[1] = &uzel_2;
    uzel_root.leaf = 0;

    btree_insert(&strom, 45);
    btree_insert(&strom, 46);

    btree_print(&strom);
    node_print(&uzel_root);
    node_print(&uzel_1);
    node_print(&uzel_2);


    node *test;
    int key = btree_search(&uzel_root, &test, 1);
    printf("\naddress:%i, key:%i", test, key);
    */
}

int main()
{
    test();
    return 0;
}
