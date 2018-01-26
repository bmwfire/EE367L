/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold
#define MAXDATASIZE 256
void error(char *s);// for pipe (ring)
//char *data="Some input data\n"; // for pipe (ring)
char data[MAXDATASIZE];
char *cmdbuf;


/*--------------isolate socket for pipe---------------------------------------------------*/
void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
struct addrinfo hints, *servinfo, *p;
struct sockaddr_storage their_addr; // connector's address information
socklen_t sin_size;
struct sigaction sa;
int yes=1;
char s[INET6_ADDRSTRLEN];
int rv;
int numbytes;  
char buf[255];
char fname[MAXDATASIZE];
char * fnametoken;
int flag0,flag1,flag2;

memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
hints.ai_flags = AI_PASSIVE; // use my IP

if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) 
{
	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
	return 1;
}
    // loop through all the results and bind to the first we can
for(p = servinfo; p != NULL; p = p->ai_next) 
{
	if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) 
	{
		perror("server: socket");
		continue;
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) 
	{
		perror("setsockopt");
		exit(1);
	}

	if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
	{
		close(sockfd);
		perror("server: bind");
		continue;
	}
	break;
}

if (p == NULL)  
{
	fprintf(stderr, "server: failed to bind\n"); //  print..........................
	return 2;
}

freeaddrinfo(servinfo); // all done with this structure

if (listen(sockfd, BACKLOG) == -1) 
{
	perror("listen");
	exit(1);
}

sa.sa_handler = sigchld_handler; // reap all dead processes
sigemptyset(&sa.sa_mask);
sa.sa_flags = SA_RESTART;
if (sigaction(SIGCHLD, &sa, NULL) == -1) 
{
	perror("sigaction");
	exit(1);
}

//	printf("server: waiting for connections...\n");//print ...................................
//----------------------------------------------------------------------------------------------------------	
while(1) 
{  // main accept() loop
sin_size = sizeof their_addr;
new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
	if (new_fd == -1) 
	{
		perror("accept");
		continue;
	}

inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);
//printf("server: got connection from %s\n", s); //print..........................
//-----------------!!!---------------------------------------------------------------------------------  
if(!fork())
{   // this is the child process
	close(sockfd); //child doesn't need the listener
	numbytes= recv(new_fd,data,MAXDATASIZE-1,0); // received the string from client and store in the data[]
	if(numbytes==-1)
	{
	perror("recv: server receives failed!!!!");
	exit(1);
	}
	cmdbuf=strtok(data,",");
//---------list-0------------------------
	if((flag0=strncmp(cmdbuf,"list",4))==0)
	{
	close(0);
	close(1);
	close(2);

	dup2(new_fd,0);
	dup2(new_fd,1);
	dup2(new_fd,2);

	execlp("/usr/bin/ls","ls",NULL);
	error("Could not exec ls");
	}

//---------check-1------------------------
	if((flag1=strncmp(cmdbuf,"check",5))==0)
	{
		fnametoken = strtok(NULL,",");
		
		if(fopen(fnametoken,"r")==NULL)
		{ 
			send(new_fd,"File is not found!",20,0);
		}
		
		else
		{
			send(new_fd, "File exists!",12,0);
		}
		exit(0);                
	} 

//----------display-2--------------------
	else if ((flag1=strncmp(cmdbuf,"display",7))==0)
	{
		fnametoken = strtok(NULL,",");
		
		if(fopen(fnametoken,"r")==NULL)
		{
			send(new_fd,"File is not found!\n",20,0);
		}
		
		else
		{
			if(!fork())
			{
				close(0);
				close(1);
				close(2);
				dup2(new_fd,0);
				dup2(new_fd,1);
				dup2(new_fd,2);
				close(new_fd);

				execlp("/usr/bin/cat","cat",fnametoken,NULL);
				error("execlp error:\n");
			}
		}
		close(new_fd); // insert close(new_fd) the check does not display wrong again!!
		exit(0);
	} 
//----------download---help---quit-------------
}//fork

close(new_fd);  // parent doesn't need this
} //close while()
return 0;
}// close main