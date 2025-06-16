/* tree.c */
#include "tree.h"

// The root "node" or the beginning node for the base of the tree
Tree root = { 
    // We want the type to be a node for root
    .n = {
        // It can either be a root or node
        .tag = (TagRoot | TagNode),
        // It should point to itself (typecasted as a Node)
        .north = (Node *)&root,
        .west = 0,
        .east = 0,
        .path = "/"
    }
};

/* 
struct s_node {
    Tag tag;
    // If the node is a root, *north points to the struct (itself),
    // else it points to the node above it
    struct s_node *north;
    // To point to the node on the "left"
    struct s_node *west;
    // Instead of a node, we have a leaf to the "rigtht"
    struct s_leaf *east;
    // Used to store the whole path to the node
    int8 path[256];
};
*/

// Intiialize a memmory block to zero
/* Why?
    Clear the buffer of the pointer
    Avoid garbage/unitialized data
    Prepare memory for fresh use
*/
// Same effect as "memset(str, 0, size)"
void zero(int8 *str, int16 size){
    int8 *p;
    int16 n;

    for (n=0, p=str; n < size; p++, n++){
        *p = 0;
    }

    return;
}

// Function to create a node (or root) and assign a parent to it
Node *create_node(Node *parent, int8 *path){
    // The node to be created
    Node *n;
    // Re-usable for the path to the node
    int16 size;

    errno = NoError;
    // Ensure (assert) that the parent isn't null
    assert(parent);
    // Set the size of the node (the default size of a node struct)
    size = sizeof(struct s_node);
    // Allocate memory for the node
    n = (Node *)malloc((int)size);
    // Zero out the structure
    zero((int8*)n, size);
    
    // Assign the parent (parent's left node is now 'n')
    parent->west = n;
    // Assign the tag as a node type
    n->tag = TagNode;
    // Assign the parent (n's up node is now the parent)
    n->north = parent;
    // copy the path that is being set for the node
    strncpy((char *)n->path, (char *)path, 255);

    return n;
}

// Returns the last leaf 
Leaf *find_last_linear(Node *parent){
    // Leaf to be returned when found
    Leaf *l;
    // Parent can't be null
    assert(parent);

    errno = NoError;

    // If the parent does not have a last node, return 0
    if(!parent->east){
        reterr(NoError);
    }
    /*
        Point 'l' to the parent's first leaf
        As long as 'l' has an east, keep going (parent has another leaf)
        Each iteration, set 'l' to l's east
    */ 
    for(l = parent->east; l->east; l = l->east);
    // Ensure that l is not 0
    assert(l);

    return l;
}

// Function to create a leaf and assign a parent to it
Leaf *create_leaf(Node *parent, int8 *key, int8 *value, int16 count){ 
    /*
        l = last leaf connected to the parent
        new = new leaf to be created
    */
    Leaf *l, *new;

    int16 size;

    // Ensure that the west Node/Tree is not null
    assert(parent);
    // Find the last leaf the parent has (will return 0 if parent has none)
    l = find_last(parent);

    size = sizeof(struct s_leaf);

    new = (Leaf *)malloc(size);
    assert(new);

    /* We need to find the leaves that are already connected (i.e. find the last leaf)
        The goal of the tree is to have a Node have many leaves:
        NODE -- leaf1 -- leaf2 -- leaf3 ...

        If there is no last leaf, connect it directly to the node
    */
    if (!l){
        // directly connected
        parent->east = new;
    } else {
        // l is a leaf
        l->east = new;
    }

    zero((int8 *)new, size);

    new->tag = TagLeaf;
    /* Ternary statment
        If there is no leaves connected, assign the left node as the parent
        Else, assign the left node as the farthest leaf to the parent
    */
    new->west = (!l) ?
        (Tree *)parent :
    (Tree *)l;
    // Copy the key being passed through
    strncpy((char *)new->key, (char *)key, 127);
    // Allocate memory for the value before setting it
    new->value = (int8 *)malloc(count);
    // Zero out the value
    zero(new->value, count);
    // Ensure that the value is not null and memory was allocated
    assert(new->value);
    // Copy over the value and the size is count
    strncpy((char *)new->value, (char *)value, count);
    // Size of the leaf's value is saved
    new->size = count;

    return new;
}

int main(){
    Node *n1, *n2;
    Leaf *l1, *l2;
    int8 *keyl1, *keyl2, *valuel1, *valuel2;
    int16 sizel1, sizel2;

    n1 = create_node((Node *)&root, (int8 *)"/Users");
    assert(n1);
    n2 = create_node(n1, (int8 *)"/Users/login");
    assert(n2);

    keyl1 = (int8 *)"kenneth";
    valuel1 = (int8 *)"xyz93323aa";
    sizel1 = (int16)strlen((char *)valuel1);
    l1 = create_leaf(n2, keyl1, valuel1, sizel1);

    printf("%s\n", l1->value);

    keyl2 = (int8 *)"kim";
    valuel2 = (int8 *)"kajana91881MM";
    sizel2 = (int16)strlen((char *)valuel2);
    l2 = create_leaf(n2, keyl2, valuel2, sizel2);
    assert(l2);

    printf("%s\n", l2->key);
    Leaf *left_Leaf = (Leaf *)l2->west;
    printf("%s\n", left_Leaf->key);

    printf("%p %p\n", n1, n2);
    free(n2);
    free(n1);
    free(l1);
    free(l2);

    return 0;
}