/* IMDB.h */

#ifndef IMDB
#define IMDB

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdarg.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Listen for localhost
#define HOST       "127.0.0.1"
// Random port to broadcast
#define PORT       "12049"

typedef unsigned int int32;
typedef unsigned short int int16;
typedef unsigned char int8;

// Struct for client side connctions
struct s_client {
    int s; // socket file descriptor
    char ip[16]; // IP address
    int16 port; // Port number
};
typedef struct s_client Client;

void assert_perror(int);
void zero(int8*,int16);
void mainloop(int);
void childLoop(Client*);
int initServer(int16);
int main(int, char**);

#endif