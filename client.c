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

    /* print out a selection menu so the user knows whats going on */
    printf("-- Client Input --\n");
    printf("s - Echo string to all active clients\n");
    printf("q - exit\n");

    signal( SIGALRM, handle_alarm ); 
    alarm(5);
    
    int status = 1;
	while (status)
    {
        int c;

        set_mode(1);
        while (!(c = get_key())) 
        {
            int n, len;
            n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr,&len);
            buffer[n] = '\0';
            if (n > 0) printf("Server: %s", buffer);
            
            // send a keep_alive signal every 5 sec
            if (send_flag)
            {
                char *keep_alive = "\n";
                sendto(sockfd, (const char *)keep_alive, strlen(keep_alive),MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));           
            
                send_flag = false;
                alarm(5);
            }
        }
        
        // if user pressed s/S, prompt for string input.
        if (c == 83 || c == 115)
        {
            printf("Enter string to echo: ");
            char user_input[MAXLINE];
            fgets(user_input, MAXLINE-1, stdin);
             // strcat(user_input, "\n");
            sendto(sockfd, (const char *)user_input, strlen(user_input),MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        }
         
        // if user pressed q/Q, quit!
        else if (c == 81 || c == 113) status = 0;
    }

    return 0;
}

/* Helper Function Definitions */

void handle_alarm( int sig )
{
    send_flag = true;
}

void set_mode(int want_key)
{
    static struct termios old, new;
    if (!want_key) {
        tcsetattr(STDIN_FILENO, TCSANOW, &old);
        return;
    }
 
    tcgetattr(STDIN_FILENO, &old);
    new = old;
    new.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new);
}

int get_key()
{
    int c = 0;
    struct timeval tv;
    fd_set fs;
    tv.tv_usec = tv.tv_sec = 0;
 
    FD_ZERO(&fs);
    FD_SET(STDIN_FILENO, &fs);
    select(STDIN_FILENO + 1, &fs, 0, 0, &tv);
 
    if (FD_ISSET(STDIN_FILENO, &fs)) {
        c = getchar();
        set_mode(0);
    }
    return c;
}


