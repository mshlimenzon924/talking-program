/* Mytalk Asgn 5
   Program that allows user on one machine
   to send and receive messages from another 
   user to another machine. 
   By Mira Shlimenzon */

/* 3 versions of v:
   1- basic getopts info 
   2- basic setup info of sockets
   3- chatting info
*/ 

#include "mytalk.h"
#include "talk.h"

int main(int argc, char *argv[]) {
  int client = 0, v = 0, a = 0, N = 0;
  int opt;
  struct hostent *host;
  int port_num = 0;
  char *end;
 
  if(argc < 2) {
    usage();
  }
  while((opt = getopt(argc, argv, ":vaN")) != -1) {
    switch(opt){
      case 'v':
	      v++;
	      break;
      case 'a':
        a = 1;
        break;
      case 'N':
        N = 1;
        break;
      case '?':
	      usage();
    }
  }

  /* if letftover arguments */
  if(optind <= argc) {
    while(optind < argc) {
      /* attempting to process port # */
      port_num = strtol(argv[optind], &end, 10);
      if(*end) { /* port_num isn't a num */
        /* if no other arguments after, then no port # given */
        if(optind + 1 == argc){
          usage();
        }
        else {  /* maybe this is hostname */
          if((host = gethostbyname(argv[optind])) == NULL) {
            usage();
            /* there shouldn't be an argument on the command
               line that is neither a hostname or a port */
          }
	        else { /* mytalk is client */
	          client = 1;
	        }
	      }  
      }  /* if port_num is #, let's check if out of port range */
      else if(!PORT_RANGE(port_num)) {
	      usage();
      }
      optind++;
    }
  }
  else { /* so no port # given */
    usage();
  }

  if(v >= 1) {
    printf("PORT_NUM %d\n", port_num); 
  }

  if(client) {
    if(v >= 1) {
      printf("HOST_NAME %s\n", host->h_name); 
    }
    isclient(host, port_num, v, a, N);
  }
  else {
    isserver(port_num, v, a, N);
  }
 return 0;
}


/* Usage print. Returns nothing. */
void usage() {
  printf("Usage: [ -v ] [ -a ] [ -N ] [ hostname ] port");
  exit(-1);
}

/* Hostname is not given so mytalk is server.
   Mytalk opens listening socket on local machine
   at given port. Client and server have conversation.  */
void isserver(int port, int v, int a, int N) {
  int serv_fd, client_fd;
  struct sockaddr_in servaddr, clientaddr;
  struct hostent *host;
  socklen_t len;
  char buff[MAXLEN + 1];
  memset(buff, 0, MAXLEN + 1);

  /* Creates socket on given port */
  if((serv_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("server socket");
    exit(-1);
  } 

  if(v >= 2){
    printf("socket\n");
  }

  servaddr.sin_family = AF_INET; /* IPV4 family */
  servaddr.sin_port = htons(port); /* Given port */
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY); /* All local */

  /* Attach address to a socket */
  if(bind(serv_fd, (struct sockaddr *)&servaddr, 
     sizeof(servaddr)) == -1){
    perror("server bind");
    exit(-1);
  }

  if(v >= 2){
    printf("bind\n");
  }

  /* Listening for connection attempts from client */
  if(listen(serv_fd, DEFAULT_BACKLOG) == -1) {
    perror("server listen");
    exit(-1);
  }

  if(v >= 2){
    printf("listen\n");
  }

  /* Accept connection and fill clientaddr with client's address */
  len = sizeof(clientaddr);
  if((client_fd = accept(serv_fd, (struct sockaddr *)&clientaddr, 
      &len)) == -1){
    perror("server accept");
    exit(-1);
  }

  if(v >= 2){
    printf("accept\n");
  }

  if(!a) { /* Connection established, accept discussion */
    /* Get hostname of client */
    host = gethostbyaddr((char *)&clientaddr.sin_addr, 
           sizeof(clientaddr.sin_addr), clientaddr.sin_family);
    if(host == NULL) {
      perror("get name info issue");
      exit(-1);
    }

    /* Grab package from client and get username of client */
    if(recv(client_fd, buff, sizeof(buff), 0) == -1) {
      perror("package username recv issue");
      exit(-1);
    }
  
    printf("Mytalk request from %s@%s. Accept (y/n)? ", buff, host->h_name);
    memset(buff, 0, MAXLEN + 1);

    /* getting server's response */
    scanf("%s", buff);

    /* if client respond with yes or y (not case sensitive) */
    if(!strcasecmp(buff, "yes") || !strcasecmp(buff, "y")) {
      /* If match then send to client "ok" */
      memset(buff, 0, MAXLEN + 1);
      sprintf(buff, "ok");
      if(send(client_fd, buff, MAXLEN + 1, 0) == -1){
        perror("send ok issue");
        exit(-1);
      }
    }
    else {
      /* If not a match send to client "nok" */
      memset(buff, 0, MAXLEN + 1);
      sprintf(buff, "nok");
      /* I think I'm funny because it's not okay combined */
      if(send(client_fd, buff, MAXLEN + 1, 0) == -1){
        perror("send ok issue");
        exit(-1);
      }
      close(serv_fd);
      close(client_fd);
      return;
    }
  }

  set_verbosity(v);
  if(!N) { /* Switches to graphic mode */
    start_windowing();
  }

  /* Conversation begins between */
  talk(client_fd, v);

  stop_windowing();
  close(serv_fd);
  close(client_fd);
}

/* Hostname is given so mytalk acts like client.
   Connects to remote host with given port and talks. */
void isclient(struct hostent *host, int port, int v, int a, int N) {
  int serv_fd;
  struct sockaddr_in servaddr;
  struct passwd *pw;
  char buff[MAXLEN + 1];
  memset(buff, 0, MAXLEN + 1);

  /* Creates socket on given port */
  if((serv_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("client socket");
    exit(-1);
  } 

  if(v >= 2){
    printf("socket\n");
  }

  /* Hostent structure - who we talking to */
  servaddr.sin_family = AF_INET; /* IPV4 family */
  servaddr.sin_port = htons(port); /* Given port */
  servaddr.sin_addr.s_addr = *(uint32_t *)host->h_addr_list[0];

  /* Connects to server */
  if(connect(serv_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
    perror("client connect");
    exit(-1);
  }

  if(v >= 2){
    printf("connect\n");
  }

  if(!a) {
    /* Create and send username package */
    if((pw = getpwuid(getuid())) == NULL) {
      perror("Issue with getpwuid of the uname.\n");
      exit(-1);
    }

    /* Sending null terminated */
    if(send(serv_fd, pw->pw_name, strlen(pw->pw_name) + 1, 0) 
       != (strlen(pw->pw_name) + 1)){
      perror("send username package");
      exit(-1);
    }
  
    printf("Waiting for response from %s\n", host->h_name);

    /* Waiting for response (ok or not ok) */
    if(recv(serv_fd, buff, MAXLEN + 1, 0) <= 0){
      perror("Erorr with receiving");
      exit(-1);
    }

    /* If client receives answer other than "ok", terminates */
    if(strcmp(buff, "ok")) {
      printf("%s declined connection.\n", host->h_name);
      close(serv_fd);
      return;
    }
  }

  set_verbosity(v);
  if(!N) { /* Switches to graphic mode */
    start_windowing();  
  }

  /* Communication begins */
  talk(serv_fd, v); 

  stop_windowing();
  close(serv_fd);
}

/* chatting function - got code help from
   your notes from Lecture 20 */
void talk(int sock_fd, int v) {
  int done = 0, len = 0, mlen = 0;
  struct pollfd fds[2];
  char buff[MAXLEN];

  memset(buff, 0, MAXLEN + 1);
  fds[LOCAL].fd = STDIN_FILENO;
  fds[LOCAL].events = POLLIN;
  fds[LOCAL].revents = 0;
  fds[REMOTE] = fds[LOCAL];
  fds[REMOTE].fd = sock_fd;

  do {
     if(poll(fds, sizeof(fds)/sizeof(struct pollfd), -1) == -1) {
      perror("polling");
      exit(-1);
    }
    /* negative –> wait forever */
    if(fds[LOCAL].revents & POLLIN ) {
      if(v >= 3) { 
        fprint_to_output("local\n");
      }
      update_input_buffer(); 
      if(has_whole_line() || has_hit_eof()) {
        if((len = read_from_input(buff, MAXLEN + 1)) == -1) {
          perror("read from input");
          exit(-1);
        }
         if((mlen = send(sock_fd, buff, len, 0)) == -1){
          perror("send");
          exit(-1);
         }

        memset(buff, 0, MAXLEN + 1);
      }

      if(has_hit_eof()){
        return;
      }
    }
    if (fds[REMOTE].revents & POLLIN ) {
       if(v >= 3) { 
        fprint_to_output("remote\n");
      }
      if((mlen = recv(sock_fd, buff, sizeof(buff), 0)) == -1) {
        perror("recv");
        exit(-1);
      }
      /* If other end terminated */
      if(mlen == 0) { /* end of file received! */
        if(v >= 3) {
          fprint_to_output("end of file received.\n");
        }
        fprint_to_output("Connection closed. ^C to terminate. ");
        pause();
        return;
      }
      else {
        if(write_to_output(buff, strlen(buff)) == -1) {
          perror("write to output issue");
          exit(-1);
        }
      }
      memset(buff, 0, MAXLEN + 1);
    }
  } while (!done);
}

