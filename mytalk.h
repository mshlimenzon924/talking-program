/* Mytalk Asgn 5 Header 
   By Mira Shlimenzon */

#ifndef MYTALK
#define MYTALK

#include <netdb.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <limits.h>
#include <netdb.h>
#include <poll.h> 
#include <getopt.h>
#include <sys/select.h>
#include <sys/types.h>

#define DEFAULT_BACKLOG 100
#define MAXLEN 1000
#define LOCAL 0
#define REMOTE 1
#define PORT_RANGE(num) ((1024 < (num)) && ((num) <= 65535))

void usage();
void isserver(int port, int v, int a, int N);
void isclient(struct hostent *host, int port, int v, int a, int N);
void talk(int sock_fd, int v);

#endif
