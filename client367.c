/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 256 // max number of bytes we can get at once 
#define MAX_NUM 256
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
int sockfd, numbytes;  
char buf[MAXDATASIZE];
char tempbuf[MAXDATASIZE];
struct addrinfo hints, *servinfo, *p;
int rv,conn;
char s[INET6_ADDRSTRLEN];
char cmdbuf[MAX_NUM]={0};        
char sedbuf[MAX_NUM]={0};
char revbuf[MAX_NUM]={0};
char fname[MAX_NUM]={0};
char dload[MAX_NUM]={0};
char * buftoken;
FILE *fp;        
int i;
int flag;
if (argc != 2) 
{
	fprintf(stderr,"usage: client hostname\n");
	exit(1);
}

while(1)
{
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (conn=connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
//              printf("Test for connect()=%d\n",conn);
			perror("client: connect");
			continue;
		}
                else
		{
	        // printf("client: For test: connect() is successful,conn=%d\n",conn);
		printf("\n");
		}
		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
//	printf("client: connecting to %s\n", s);
        freeaddrinfo(servinfo); // all done with this structure

//------send data to server----------
	printf("Client: Input Command please:\n");
	fgets(cmdbuf,MAX_NUM,stdin); // command stores in sefbuf[]

//----------list-0----------------------
	if(strncmp(cmdbuf,"list",4)==0)
	{
		if(send(sockfd, cmdbuf,4,0)==-1)
		{
			printf("client send error!!!\n");
		}
                
		if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) 
		{ // received   store in buf
			perror("recv");
			exit(1);
		}
		buf[numbytes] = '\0';// must included this line
		printf("Client lists the files:\n");
		printf("%s\n",buf);
		memset(buf,0,256);
	}

//---------check-1----------------------
	else if(strncmp(cmdbuf,"check",5)==0)
	{
		printf("Input file name:\n");//print...............................
                 // fgets(fname,MAX_NUM,stdin);
		scanf("%s",fname);
		strcpy(sedbuf,"check,");
		strcat(sedbuf,fname);
         	if(send(sockfd, sedbuf, strlen(sedbuf),0)==-1)
		{
			printf("Client:client send error!!\n");
		}
		
		if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) 
		{
			perror("recv");
			exit(1);
		}

		buf[numbytes] = '\0';
		printf("Client checks file:'%s'\n",buf);
		memset(buf,0,256);
             }

//------------display-2-------------------
	else if(strncmp(cmdbuf,"display",7)==0)
	{
		printf("Input the file name:\n");
		// fgets(fname,MAX_NUM,stdin);
		scanf("%s",fname);
		strcpy(sedbuf,"display,");
		strcat(sedbuf,fname);

		if(send(sockfd,sedbuf,strlen(sedbuf),0)==-1)
		{
			printf("Client send error!!\n");
		}
		numbytes=recv(sockfd,buf,MAXDATASIZE-1,0);
		if(numbytes==-1)
		{
			perror("recv");
			exit(1);
		}
		else
		{
			printf("Client displays file:\n");
			while(numbytes==255)
			{
				buf[numbytes]='\0';
				printf("%s",buf);
				numbytes = recv(sockfd,buf,MAXDATASIZE-1,0);
			}
 
			memcpy(tempbuf,buf,numbytes);
			printf("%s",tempbuf);
			memset(tempbuf,0,256);
			memset(buf,0,256);
		}
	}
 
//---------------download-3----------------
	else if(strncmp(cmdbuf,"download",8)==0)
	{
		printf("Input the file name:\n");
		scanf("%s",fname);
		strcpy(sedbuf,"display,");
		strcat(sedbuf,fname);
                    
		if(send(sockfd,sedbuf,strlen(sedbuf),0)==-1)
		{ printf("client send error!!!\n");}

		numbytes=recv(sockfd,buf,MAXDATASIZE-1,0);
		
		if(numbytes==-1)
		{
			perror("recv");
			exit(1);
		}
		buf[numbytes] = '\0'; 
		flag=strncmp("File is not found!",buf,18);
		
		
		if(flag==0)
		{
			printf("%s",buf);    
		}
		
		else if((fp=fopen(fname,"r"))!=NULL) 
         	{
			printf("File '%s' exists. Overwrite it? (y/n)\n", fname);
			char check[MAX_NUM]={0};
			scanf("%s",check);
//			printf("ct33:%s\n",check);
			char checkflagchar= check[0];
//			printf("ct34:checkflagchar='%d'\n",checkflagchar);
			
			if(checkflagchar == 'y') 
			{
				fp = fopen(fname,"w");
				fprintf(fp,"%s",buf);
				while(numbytes=recv(sockfd,buf,255,0)) 
				{
					buf[numbytes] = '\0';
					fprintf(fp,"%s",buf);
				}
				fclose(fp);
				printf("1:Download of file '%s' complete\n",fname);
			}	
	      		else if(checkflagchar == 'n'){printf("Aborted!\n");}	
	      		else{printf("You must input y or x.  You are not supposed to input %c\n",checkflagchar);}
		}

		else 
		{
			fp = fopen(fname,"w");
			fprintf(fp,"%s",buf);
			while(numbytes=recv(sockfd,buf,MAXDATASIZE-1,0))
			{
				buf[numbytes] = '\0';
				fprintf(fp,"%s",buf);
			}
			fclose(fp);
			printf("2:Download of file '%s' complete\n",fname);
		} 		  
	}  // end if


//-------------quit-4--------------
	else if (strncmp(cmdbuf,"quit",4)==0)
	{
		printf("Quit!\n");      
		exit(0);
	}


//------------help--------------------
	else if(strncmp(cmdbuf,"help",4)==0)
	{
		printf("How to use command\n");
		printf("Input 'list'    : list the content of server\n");
		printf("Input 'check'   : check if the server has the file named <file name>\n");
		printf("Input 'display' : display contents of file\n");
		printf("Input 'download':download file <file name>\n");
		printf("Input 'help'    : Get help!\n");
		printf("Input 'quit'    : Quit!!\n");
         } 

	 else
	 {
	 	printf("Please input help to get support.\n");
	 
	 }
//-------------end!!!--------------
//break; // for test!
} // close while
close(sockfd);
return 0;
} // close main