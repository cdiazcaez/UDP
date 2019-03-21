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
