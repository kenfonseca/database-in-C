/* IMDB.c */
/*
    This focus is on the server software and network communication
    so that we can log into and interact with the DB server
*/
#include "IMDB.h"

bool server_continuation;
bool child_continuation;

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
void childLoop(Client *cli){
    sleep(1);

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
        dprintf(s2, "[100] - Connected to IMDB server\n"); //Like regular printf but sends to a file descriptor (open file, socket, etc.)
        // Continue connection
        child_continuation = true;
        while (child_continuation){
            childLoop(client);
        }
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
    printf("IMDB server listening on %s:%d\n", HOST, port);

    return s;
}

int main(int argc, char *argv[]){
    char *s_port;
    int16 port;
    // Socket descriptor to accept connections
    int s;

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
    printf("Shutting down IMDB server");
    // Shut off connections
    close(s);

    return 0;
}