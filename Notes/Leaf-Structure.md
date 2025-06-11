A Leaf is a strucutre that contains:
* A link to the left data structure
    * The element they contain is a string
    * The strings will be allocated on the heap
* A key to distingush the Leaf
    * This is a standard null terminated string
* Value
    * Pointer allocated on the heap
    * This should be able to be dynamically bigger or smaller than 255 (hence it's a pointer)
* Count 
    * Used to count the amount of charatcers the value will be
```
struct s_leaf {
    struct s_tree *left;
    char key[255];
    char *value;
    int count;
} Tag tag;
```

\* We are using a "tag" to differentiate between a node and leaf
* 1 = node
* 2 = leaf
