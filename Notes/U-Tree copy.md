Union or U-Tree has:
* Either a struct "node"
* Or a strcut "leaf"
* Each one takes up a different part of the tree structure's memory
* In a union tree, we can only have either a "leaf" or "node" structure
    * This makes it suitable for a binary tree
```
union u_tree{
    struct s_node n;
    struct s_leaf l;
};
```

