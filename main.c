#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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

//Help function for transferring divided node
void node_transfer(node *left_node, node *right_node, int median){
    int end = left_node->n;
    for(int j = median + 1; j < end; j++){
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

//Find biggest key in node
int node_biggest_key(btree *tree, node *n, node **last_node){
    if(n->leaf){
        *last_node = n;
        int last_index = n->n - 1;
        return last_index;
    }
    return node_biggest_key(tree, n->children[n->n] ,last_node);
}

//Get sibling of the node
node *node_sibling(node *n, int left, int *index_key){
    node *parent = n->parent;
    int index = 0;
    for(int i = 0; i <= parent->n; i++){
        if(parent->children[i] == n){
            index = i;
            break;
        }
    }

    if(left){
        if(index > 0){
            *index_key = index - 1;
            return parent->children[index - 1];
        }
        *index_key = -1;
        return 0;
    } else {
        if(index != parent->n){
            *index_key = index;
            return parent->children[index + 1];
        }
        *index_key = -1;
        return 0;
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
void node_print_children(node *n){
    node_print(n);
    if(!n->leaf){
        int j = 0;
        for(j; j < n->n; j++){
            node_print_children(n->children[j]);
        }
        node_print_children(n->children[j]);
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

//Get index of the key in node n
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
    node_print_children(tree->root);
}

//Delete key from tree
void btree_delete(btree *tree, int key){
    node *last_node;
    int skey = btree_search(tree->root, &last_node, key);
    if(skey < 0){
        return;
    }

    if(last_node->leaf){
        btree_delete_from_leaf(tree, last_node, skey);
    } else {
        node *n;
        int sub_index = node_biggest_key(tree, last_node->children[skey], &n);
        last_node->keys[skey] = n->keys[sub_index];
        btree_delete_from_leaf(tree, n, sub_index);
    }
}

//Delete key from leaf (auxiliary function)
void btree_delete_from_leaf(btree *tree, node *last_node, int skey){
    int start_indes = (last_node->n - 2);
    int t = last_node->keys[start_indes + 1];
    for(int j = start_indes; j >= skey; j--){
        int temp = last_node->keys[j];
        last_node->keys[j] = t;
        t = temp;
    }
    last_node->n -= 1;

    btree_delete_second(tree, last_node);
}

//Delete second faze (auxiliary function)
void btree_delete_second(btree *tree, node *n){
    if(tree->root == n || n->n >= tree->min){
        if(tree->root == n && n->n < tree->min){
            node *root = tree->root;
            if(!root->leaf){
                tree->root = root->children[0];
                root->leaf = 1;
            }
        }
        return;
    }


    node *parent = n->parent;

    int *par;
    node *sibling = node_sibling(n, 1, &par);
    int parent_key_index = par;

    if(parent_key_index != -1){
        if(sibling->n > tree->min){
            int sibl_key = sibling->keys[sibling->n - 1];
            int parent_key = parent->keys[parent_key_index];
            parent->keys[parent_key_index] = sibl_key;

            node_insert(n, parent_key);

            if(!sibling->leaf){
                int sibl_child = sibling->children[sibling->n];
                n->children[n->n] = sibl_child;

                int t = 0;
                for(int j = 0; j <= n->n; j++){
                    int temp = n->children[j];
                    n->children[j] = t;
                    t = temp;
                }

                n->children[0] = sibl_child;
            }

            sibling->keys[sibling->n - 1] = 0;
            sibling->n -= 1;
            return;
        }
    }

    sibling = node_sibling(n, 0, &par);
    parent_key_index = par;

    if(parent_key_index != -1){
        if(sibling->n > tree->min){
            int sibl_key = sibling->keys[0];
            int parent_key = parent->keys[parent_key_index];
            parent->keys[parent_key_index] = sibl_key;

            int t = 0;
            for(int j = sibling->n - 1; j >= 0; j--){
                int temp = sibling->keys[j];
                sibling->keys[j] = t;
                t = temp;
            }

            node_insert(n, parent_key);

            if(!sibling->leaf){
                int sibl_child = sibling->children[0];
                n->children[n->n] = sibl_child;

                t = 0;
                for(int j = sibling->n; j >= 0; j--){
                    int temp = sibling->children[j];
                    sibling->children[j] = t;
                    t = temp;
                }
            }
            sibling->n -= 1;

            return;
        }

        btree_delete_merge(tree, n, sibling, parent_key_index);
        btree_delete_second(tree, n->parent);
        return;
    }

    sibling = node_sibling(n, 1, &parent_key_index);

    btree_delete_merge(tree, sibling, n, parent_key_index);
    btree_delete_second(tree, n->parent);
    return;
}

//Merge nodes (auxiliary function)
void btree_delete_merge(btree *tree, node *n, node *sibling, int parent_index){
    node *parent = n->parent;
    node_insert(n, parent->keys[parent_index]);

    if(!sibling->leaf)
        n->children[n->n] = sibling->children[0];

    for(int j = 0; j >= sibling->n - 1; j--){
        node_insert(n, sibling->keys[j]);
        if(!sibling->leaf)
            n->children[n->n] = sibling->children[j + 1];
    }

    free(sibling);

    int t = 0;
    for(int j = parent->n - 1; j >= parent_index; j--){
        int temp = parent->keys[j];
        parent->keys[
        j] = t;
        t = temp;
    }

    t = 0;
    for(int j = parent->n; j > parent_index; j--){
        int temp = parent->children[j];
        parent->children[j] = t;
        t = temp;
    }

    parent->n -= 1;
}

//-----
void line(){
 printf("\n------------------------------------------------------------------------------------------------------------");
}

void test(){
    int t = 2;

    btree strom;
    btree_constructor(&strom, t);

    node *uzel_root = node_constructor(&strom, 0);

    int length = 17;
    int array[] = {26,12,42,6,51,62,5,1,70,83,97,100,103,137,159,13,15};

    for(int i = 0; i < length; i++)
        btree_insert(&strom, array[i]);

    btree_print(&strom);
    btree_print_nodes(&strom);
    line();

    line();
    btree_print_nodes(&strom);
}

int main()
{
    test();
    return 0;
}
