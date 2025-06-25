/* server.c */
/*
    This focus is on the server software and network communication
    so that we can log into and interact with the DB server
*/

// if memory acts weird, try changing the use of the 'sizeof()' function to the actual number for the buffer size 
#include "server.h"

bool server_continuation;
bool child_continuation;


int32 handle_hello(Client *client, int8 *folder, int8 *args){
    if(*args){

    }
    dprintf(client->s, "hello back '%s\n", folder);

    return 0;
}

CmdHandler handlers[] = {
    {(int8 *)"hello" , handle_hello},
    {(int8 *)"no" , handle_hello},
};

Callback getcmd(int8 *cmd){
    Callback cb;
    int16 n, arrLen;

    arrLen = sizeof(handlers)/16;

    if(sizeof(handlers)<16){
        return 0;
    }

    cb = 0;
    for(n=0; n<arrLen; n++){
        if(!strcmp((char *) cmd, (char *)handlers[n].cmd)){
            cb = handlers[n].handler;
            break;
        }
    }

    return cb;
}

// Combo of assert and print error functions
// had to make a custom assert_perror() because compiler won't recognize it
void assert_perror(int err){
    if (err != 0) {
        fprintf(stderr, "Error: %s\n", strerror(err));
        assert(0);
    }
}

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

// Loop for child processes (forked from mainLoop()) that handles client connections 
void childLoop(Client *client){
    // Buffer to read from the client socket's file descriptor
    int8 buf[256];
    // Counter variable
    int16 n;
    // Char arrrays to hold the client's commands and arguments
    int8 cmd[256], folder[256], args[256];
    // Prepare buffers
    zero(cmd, sizeof(cmd));
    zero(folder, sizeof(folder));
    zero(args, sizeof(args));
    // Char pointer to point to the command section of client input
    int8 *p;
    // Char pointer to point to the folder section of client input
    int8 *f;

    // prepare the buffer
    zero(buf, sizeof(buf));
    // read the contents of the client's file descriptor into the buffer
    read(client->s, (char *)buf, sizeof(buf)-1);
    // Set the number of characters in the buffer to the counter
    n = (int16)strlen((char *)buf);
    // Backup
    if(n>254){
        n=254;
    }

    /*
        User input (buf) will be in the format:   
        'command folder argument'
    */ 

    // HANDLE 'command' INPUT
    /* While logic - continue if:
        'p' is positive
        'n' is positive
        'p' is not a space (delimitter)
        'p' is not a newline
        'p' is not a carriage return character
    */
    for(p=buf; 
            (*p) 
            && (n--)
            && (*p != ' ')
            && (*p != '\n')
            && (*p != '\r');
        p++){
    };

    // If 'p' or 'n' are not positive, 
    if(!(*p) || (!n)){
        // Copy over the user's command and finish
        strncpy((char *)cmd, (char *)buf, 255);
        goto done;
    }
    // Else if there is a delimitter
    else if(*p == ' '){
        *p = 0;
        strncpy((char *)cmd, (char *)buf, 255);
    }
    // Else if there is a new line or carriage return character
    else if((*p == '\n') || (*p == '\r')){
        *p = 0;
        // Copy over the command
        strncpy((char *)cmd, (char *)buf, 255);
        goto done;
    } 

    // HANDLE 'folder' INPUT
    for(p++, f=p; // Add one to get past the space or endline after the 'command', then 'f' points to the folder
            (*p) 
            && (n--)
            && (*p != ' ')
            && (*p != '\n')
            && (*p != '\r');
        p++){
    };

    // If 'p' is equal to zero, 
    if(!(*p) || (!n)){
        // Copy over the user's folder 
        strncpy((char *)folder, (char *)f, 255);
        goto done;
    }
    else if((*p == ' ') || (*p == '\n') || (*p == '\r')){
        *p = 0;
        // Copy over the folder
        strncpy((char *)folder, (char *)f, 255);
    } 

    // HANDLE 'argument' INPUT
    p++;
    if (*p) { // Only continue if 'p' is positive
        strncpy((char *)args, (char *)p, 255);
        for (p=args; 
                ((*p) && 
                (*p != '\n') && 
                (*p != '\r')); 
            p++){
        } 
        *p = 0;
    }
   
    // Print the parsed input
    done:
        dprintf(client->s, "cmd:\t%s\n", cmd);
        dprintf(client->s, "folder:\t%s\n", folder);
        dprintf(client->s, "args:\t%s\n", args);

    return; 
}

// Loop to set up the connections between server and client
void mainLoop(int s){
    struct sockaddr_in cli;
    int s2; // client side's file descriptor 
    int32 len; // Will be used to hold the length of the address that is returned by accept()
    char *ip;
    int16 port;
    Client *client; // Struct used as an instance of a client
    pid_t pid; // Used to keep tracking of processes during forking

    // Extract the first connection request on queue of connections for the listening socket
    s2 = accept(s, (struct sockaddr *)&cli, (unsigned int *)&len);
    if(s2 < 0){ // accept() returns fd on success, and -1 on error
        // Don't want to quit program incase the flow of connections is only temporarily full
        // usleep(1); 
        return;
    }

    // Return the port 
    port = (int16)htons((int)cli.sin_port);
    // Return the address
    ip = inet_ntoa(cli.sin_addr);

    printf("Connection from %s:%d\n", ip, port);

    // Allocate memory for a Client struct
    client = (Client *)malloc(sizeof(struct s_client));
    // Ensure that the memory was allocated
    assert(client);

    // Prepare the structure to recieve data (JIC)
    zero((int8 *)client, sizeof(struct s_client));
    // Assign client struct fields
    client->s = s2; // file descriptor
    client->port = port; // port
    strncpy(client->ip, ip, 15); // Copy IP address

    // Fork a process
    pid = fork(); 
    if(pid){ // fork() only returns PID if the current process is a parent process
        free(client);

        return;
    } else { // fork() returns 0 is the process is a child
        dprintf(s2, "[100] - Connected to server\n"); //Like regular printf but sends to a file descriptor (open file, socket, etc.)
        // Continue connection
        child_continuation = true;
        // Keep reading commands from the user while the connection is active
        while (child_continuation){
            childLoop(client);
        }
        // Clean-up
        close(s2);
        free(client);

        return;
    }
    
    return;
}

// Function to start a listening server
int initServer(int16 port){
    // Struct to hold socket connection info
    struct sockaddr_in sock;
    // Used to hold the file descriptor returned by socket()
    int s; 

    // Fill in the sock structure
    /*
        AF = Address Family: used to designate the type of addresses the socket can communicate w/
            - We are communicating w/ INET (IPv4) addresses
        Other communication examples:
            - AF_INET6: IPv6 address 
            - AF_UNIX: UNIX socket 
            - AF_IRDA: Infrared Data Association 
            - AF_BLUETOOTH: Bluetooth
    */
    sock.sin_family = AF_INET; 
    // Port number
    sock.sin_port = htons((int)port);
    // Host address
    sock.sin_addr.s_addr = inet_addr(HOST);

    // Initialize socket: create communication endpoint 
    /* int socket(int domain, int type, int protocol);

        domain: Defining the communication type
            AF_INET: Address Family IPv4
        type: Defining the data flow (stream):
            SOCK_STREAM: TCP
            SOCK_DGRAM: UDP
        protocol: Normally a single protocol type exists per AF
            0
    */
    s = socket(AF_INET, SOCK_STREAM, 0); // returns a file descriptor
    // Check the socket init is successfule (returns -1 on error)
    assert(s > 0);


    // Reset errno
    errno = 0;
    // Bind socket descriptor with structure to specify where the OS where to send incoming network traffic
    if(bind(s, (struct sockaddr *)&sock, sizeof(sock)) != 0){
        // If binding fails, check the error number and quit the program w/ the reason
        assert_perror(errno);
        // Can only bind once, if we attempt to bind more, the address will already be in use
    }

    errno = 0;
    // Listen with the socket descriptor
    if(listen(s, 20)){ // Allows 20 connections in queue
        // On error, listen() returns -1
        // On success, listen() returns 0
        assert_perror(errno);
    } 
    printf("Server listening on %s:%d\n", HOST, port);

    return s;
}

int main(int argc, char *argv[]){
    char *s_port;
    int16 port;
    // Socket descriptor to accept connections
    int s;

    Callback x;
    x = getcmd((int8 *)"hello");
    printf("%p\n", x);

    x = getcmd((int8 *)"no");
    printf("%p\n", x);
    return 0;

    // If there are less than 2 arguments, a port has not been specified by user
    if(argc < 2){
        // Set the default port
        s_port = PORT;
    } else {
        // Set the port to user specified port
        s_port = argv[1];
    }
    // Convert the port to an int
    port = (int16)atoi(s_port);

    // Assign the server to the socket descriptor
    s = initServer(port);

    // While this is true, it will keep running mainLoop
    server_continuation = true;
    while(server_continuation){
        mainLoop(s);
    }
    printf("Shutting down server");
    // Shut off connections
    close(s);

    return 0;
}