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

/* HELPER FUNCTIONS */

// compare two socket address
int sock_cmp(struct sockaddr_in a, struct sockaddr_in b);

// Related LL functions
node* getNewNode(struct sockaddr_in c);
void insert_at_head(struct sockaddr_in c);
node * find_node(struct sockaddr_in c);
void delete_node(node *pos);


// Global variables for our List
node *head = NULL, *tail = NULL;

// Send given string to all active clients
void send_to_active_clients(int sockfd, char *str, int n, int len);

/*   -------    */

int main(int argc, char* argv[])
{
	int sockfd;
	char buffer[MAXLINE];
	short int port;
	
	char* endptr;




