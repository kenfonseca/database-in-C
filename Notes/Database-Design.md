You need to know how to store the data inside of the memory
* Not "disk space" memory
* How to store everything "in memory"
* We'll use a Binary Tree

We can store and find values quickly using a Modified Binary (Union) Tree with:
* Node Struct
* Leaf Struct

The goal is to create fast structs based on CPU speed
* Memory wastage is of no concern unless it proves inefficient
* It's not designed to have the best or quickest network protocol
    * It will use ASCII
    * Will be able to do: "telnet localhost (some port number)"
    * Will be able to respond with something like: "001 Hello, running IMDB v0"
    * Example interaction:
        * CREATE /Users/
        * 200 OK
        * INSERT /Users/key-value
        * SELECT /Users/key
* It will not be the most memory preserved one
* It will focus on CPU optimization
    * It will use very few CPU instructions when information is being grabbed