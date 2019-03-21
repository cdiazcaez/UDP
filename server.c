#include <sys/socket.h>       
#include <sys/types.h>        
#include <arpa/inet.h>        
#include <unistd.h>           
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/*  Global constants  */

#define PORT 8081
#define MAXLINE  1024
#define MAXCLIENTS 50

// Structure to maintain a list of active clients
typedef struct node
{
	struct sockaddr_in addr;
	time_t ts;
	struct node *next;
	struct node *previous;
} node;
