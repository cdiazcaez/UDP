#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <termios.h>
#include <fcntl.h>
#include <sys/time.h>

#include <time.h>
#include <signal.h>
#include <stdbool.h>

#define MAXLINE 1024

volatile sig_atomic_t send_flag = false;

// signal handler
void handle_alarm( int sig );

// Helper functions for nonblocking input
void set_mode(int want_key);
int get_key();

int main(int argc, char* argv[]) 
{
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[MAXLINE];
    short int port;

    char *remote_address;             
    char *remote_port;                
    char *endptr;                
    
        // Ensure correct usage
    if (argc != 3)
    {
        printf("usage:\n");
        printf("\t%s <server ip> <server port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    remote_address = argv[1];
    remote_port = argv[2];

    port = strtol(remote_port, &endptr, 0);
    if ( *endptr ) 
    {
        perror("Invalid port supplied");
        exit(EXIT_FAILURE);
    }
    
    // filling server information
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = INADDR_ANY;
    
    // set remote address
    if ( inet_aton(remote_address, &servaddr.sin_addr) <= 0 ) 
    {
        perror("Invalid remote IP address");
        exit(EXIT_FAILURE);
    }

    // Create an unbound socket
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set read timeout from server to 10 usec
    struct timeval read_timeout;
    read_timeout.tv_sec = 0;
    read_timeout.tv_usec = 10;
    if ( setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout) < 0)
    {
        perror("Error setting socket options");
    }



