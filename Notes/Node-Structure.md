A Node is a structure that contains:
* An uplink to the node above it
* A downlink to the node below it (left node)
* An arm to the right to connect all the leaves
```
struct s_node{
    struct s_node *up;
    struct s_node *left;
    struct s_leaf *right;
} Tag tag; 

// When a user connects to the TCP port, the database should create a path and link a structure to the path 
char path[255];
```