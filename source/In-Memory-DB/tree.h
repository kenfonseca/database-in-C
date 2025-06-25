/* tree.h */
// A file simply to include all headers
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include <errno.h>

// Used for the Tag variable to assign the value type
#define TagRoot     1 /* 00 01 */
#define TagNode     2 /* 00 10 */
#define TagLeaf     4 /* 01 00 */

#define NoError     0

// Custom void pointer variable
typedef void* Nullptr;
// I renamed "nullPtr" to "myNullPtr" because "nullPtr" is a reserved keyword
Nullptr myNullPtr = 0;

#define find_last(x)    find_last_linear(x)
#define reterr(x) \
    errno = (x); \
    return myNullPtr

typedef unsigned int int32;
typedef unsigned short int int16;
typedef unsigned char int8;
typedef unsigned char Tag;

// Node strucutre in the tree
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
typedef struct s_node Node;

// Leaf structure in the tree
struct s_leaf {
    Tag tag;
    // *west points to the node (or tree) it came from 
    union u_tree *west;
    // If more values are to the right, *east will represent it 
    struct s_leaf *east;
    // key will be used to distinguish the leaf 
    int8 key[128];
    // The count of characters
    int8 *value;
    int16 size;
};
typedef struct s_leaf Leaf;

// union to represent a tree that contains either a Node or Lead
union u_tree {
    Node n;
    Leaf l;
};
typedef union u_tree Tree;