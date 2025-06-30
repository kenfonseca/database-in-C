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
        .path = "/"}};

// Function that accepts a number and returns that amount of spaces in string format
int8 *indent(int8 spaces)
{
    // Will be filled with the amount of spaces
    static int8 buf[256]; // Making it static makes the contents accesible outside the function
    // Used to point and iterate through the buffer
    int8 *p;
    // counter variable
    int16 i;

    // Ensure that we don't go over or under the limit
    if(spaces < 1){
        return (int8 *)"";
    }
    assert(spaces < 120);
    // Prepare buffer
    zero(buf, 256);

    // Point 'p' to the begining of the buffer, copy the spaces, and iterate through it
    for (i = 0, p = buf; i < spaces; p++, i++)
    {
        strncpy((char *)p, "  ", 2);
    }

    return buf;
}

// Function to print out the tree of an entire database
/*
    int fd - Takes in a file desciptor to print out as standard output or as a file
    Tree root - The starting point that we want the function to print from (root || node)
*/
void print_tree(int fd, Tree *_root){
    int8 indentation;
    // Buffer to print
    int8 buf[256];
    int16 size;
    Node *n;
    Leaf *l, *last;

    indentation = 0;
    // As long as the current node has a west (left node), keep exploring
    for(n = (Node *)_root; n; n = n->west){
        // refer to "Print" macro in tree.h
        Print(indent(indentation++));
        Print(n->path);
        Print("\n");

        // Find 'n's last leaf, find leaves to print while we haven't gone back to the node
        if(n->east){
            last = find_last(n);
            if(last){
                for(l = last; (Node *)l != n; l = (Leaf *)l->west){
                    Print(indent(indentation));
                    Print(n->path);
                    Print("/");
                    Print(l->key);
                    Print(" -> \'");
                    write(fd, (char *)l->value, (int)l->size);
                    Print("'\n");
                }
            }
        }
    }

    return;
}

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
void zero(int8 *str, int16 size)
{
    int8 *p;
    int16 n;

    for (n = 0, p = str; n < size; p++, n++)
    {
        *p = 0;
    }

    return;
}

// Function to create a node (or root) and assign a parent to it
Node *create_node(Node *parent, int8 *path)
{
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
    zero((int8 *)n, size);

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

// Find the node we're using from the path
Node *find_node_linear(int8 *path){
    Node *n, *ret;
    // If the desired path is the given node return it
    for(ret = (Node *)0, n = (Node *)&root; n; n = n->west){
        if(!strcmp((char *)n->path, (char *)path)){
            ret = n;
            break;
        }
    }   

    return ret;
}

// Searching for leaves of a node using a path, key, and starting point
Leaf *find_leaf_linear(int8 *path, int8 *key){
    Node *n;
    n = find_node_linear(path);
    Leaf *l, *ret;
    ret = (Leaf *)0;
     // Make sure the start node is valid
    if(!(n)){
        return ret; 
    }

    // Find the leaf with the key and return it
    for(l = n->east; l; l = l->east){
        if(!strcmp((char *)l->key, (char *)key)){
            ret = l;
            break;
        }
    }

    return ret; 
}

int8 *lookup_linear(int8 *path, int8 *key){
    Leaf *p;

    p = find_leaf_linear(path, key);
    
    return (p) ?
        p->value :
    (int8 *)0;
}

// Returns the last leaf of a node
Leaf *find_last_linear(Node *parent)
{
    // Leaf to be returned when found
    Leaf *l;
    // Parent can't be null
    assert(parent);

    errno = NoError;

    // If the parent does not have a last node, return 0
    if (!parent->east)
        return (Leaf *)0;
    /*
        Point 'l' to the parent's first leaf
        As long as 'l' has an east, keep going (parent has another leaf)
        Each iteration, set 'l' to l's east
    */
    for (l = parent->east; l->east; l = l->east);
    // Ensure that l is not 0
    assert(l);

    return l;
}

// Function to create a leaf and assign a parent to it
Leaf *create_leaf(Node *parent, int8 *key, int8 *value, int16 count)
{
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
    if (!l)
    {
        // directly connected
        parent->east = new;
    }
    else
    {
        // l is a leaf
        l->east = new;
    }

    zero((int8 *)new, size);

    new->tag = TagLeaf;
    /* Ternary statment
        If there is no leaves connected, assign the left node as the parent
        Else, assign the left node as the farthest leaf to the parent
    */
    new->west = (!l) ? (Tree *)parent : (Tree *)l;
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
    Leaf *l1, *l2, *l3;
    int8 *keyl1, *keyl2, *keyl3, *valuel1, *valuel2, *valuel3;
    int16 sizel1, sizel2, sizel3;

    n1 = create_node((Node *)&root, (int8 *)"/Users");
    assert(n1);
    n2 = create_node(n1, (int8 *)"/Users/login");
    assert(n2);

    keyl1 = (int8 *)"kenneth";
    valuel1 = (int8 *)"xyz93323aa";
    sizel1 = (int16)strlen((char *)valuel1);
    l1 = create_leaf(n2, keyl1, valuel1, sizel1);
    assert(l1);

    keyl2 = (int8 *)"kim";
    valuel2 = (int8 *)"kajana91881MM";
    sizel2 = (int16)strlen((char *)valuel2);
    l2 = create_leaf(n2, keyl2, valuel2, sizel2);
    assert(l2);

    keyl3 = (int8 *)"scarlet";
    valuel3 = (int8 *)"kasdcis118281";
    sizel3 = (int16)strlen((char *)valuel3);
    l3 = create_leaf(n2, keyl3, valuel3, sizel3);
    assert(l3);

    int fd = 1;
    print_tree(fd, &root);

    // Node *found = find_node_linear(n1->path);
    // printf("%s\n", found->path);
    int8 *find = lookup_linear(n1->path, l1->key);
    if(find){
        printf("%s\n", find);
    } else {
        printf("no\n");
    }
   
    // printf("%p %p\n", n1, n2);
    free(n2);
    free(n1);
    free(l1);
    free(l2);
    free(l3);

    return 0;
}