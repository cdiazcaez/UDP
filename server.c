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
	
	if (argc == 2)
	{
		port = strtol(argv[1], &endptr, 0);
		if (*endptr)
		{
			fprintf(stderr, "UDPSERV: Invalid port number.\n");
			exit(EXIT_FAILURE);
		}
	}

	else if (argc < 2) 
	{
		port = PORT;
	}
		
	else 
	{
		fprintf(stderr, "UDPSERV: Invalid arguments.\n");
		exit(EXIT_FAILURE);
	}


	struct sockaddr_in servaddr, cliaddr;
	int clients = 0;
	
	// Create an unbound socket
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) 
	{
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	// Filling server information
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(port);
	
	// Assign address to the unbound socket.
	if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	
	// Initialize buffer
	memset(&buffer, 0, MAXLINE);
	
	//Enter an infinite loop to respond	to client requests and echo input
		
	while (1)
	{
		int len, n;
		n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &cliaddr,&len);
		buffer[n] = '\0';

		node * temp = find_node(cliaddr);
		if (temp == NULL) insert_at_head(cliaddr);
		else time(&(temp->ts));
		
		
		if (strcmp(buffer, "\n") == 0)
		{
			// Test keep_alive signal is receive by the server.
			printf("KEEP_ALIVE\n");
			continue;
		}
				else
		{
			printf("Client : %s", buffer);
			send_to_active_clients(sockfd, buffer, n, len);
		}
	}
}



/* Helper Function Definitions */

int sock_cmp(struct sockaddr_in a, struct sockaddr_in b)
{
	if((a.sin_addr.s_addr == b.sin_addr.s_addr) && (a.sin_port == b.sin_port)) return 1;
	return 0;
}

// create a new node for the list
node* getNewNode(struct sockaddr_in c)
{
    node *temp_node;
    temp_node = (node *) malloc(sizeof(node));
    temp_node->addr=c;
    time(&(temp_node->ts));
    temp_node->next=NULL;
    temp_node->previous=NULL;

    return temp_node;
}

// insert a node with given socket address to the head of list
void insert_at_head(struct sockaddr_in c)
{
    node *newNode = getNewNode(c);

    if(head==NULL)
    {
    	head=newNode;
        tail=newNode;
        return;
    }

    newNode->next = head;
    head->previous = newNode; 
    head = newNode; 
}

// find the location of node with given socket address
// Returns NULL if not found, else pointer to that node
node * find_node(struct sockaddr_in c)
{
	if (head == NULL) return head;

	node * current = head;
	while (current != NULL)
	{
		if (sock_cmp(current->addr, c)) return current;
		current = current->next;
	}
	return current;
}

// Delete a node from the list pointed by the given node pointer
void delete_node(node *pos)
{
	if (pos == NULL || head == NULL) return;
	if (pos == head) head = pos->next;

	if (pos->previous != NULL) pos->previous->next = pos->next;
	if (pos->next != NULL) pos->next->previous = pos->previous;
	pos->next = NULL;
	pos->previous = NULL;
	free(pos);
}

// send the given string to all known active clients
void send_to_active_clients(int sockfd, char *str, int n, int len)
{
	node * temp = head;
	double diff_t;

	// traverse the list
	while (temp != NULL)
	{
		diff_t = difftime(time(NULL), temp->ts);
		
		// if given difference is less than 15 sec, the node is an active client
		if (diff_t < 15.0)
		{
			struct sockaddr_in c = temp->addr;
			sendto(sockfd, (char *)str, n, MSG_CONFIRM, (const struct sockaddr *) &c, len);
			// printf("Message sent %s", str);
			temp = temp->next;
		}

		// else delete the node from the list
		else
		{
			// printf("IN DELETE MODE\n");
			if (temp->next != NULL) 
			{
				temp = temp->next;
				delete_node(temp->previous);
			}
			else
			{
				delete_node(temp);
				temp = NULL;
			}
		}
	}
}

		



