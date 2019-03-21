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
