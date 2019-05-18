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

#define MAXDATASIZE 500 // max number of bytes we can get at once 
#define MAXHANDLESIZE 10 // max number of bytes allowed in client's handle




// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Function: setupConnect
// This function sets up the connection
// Input: hostname and port number
// Output: the socket file descriptor (int)

int setupConnect(char* hostname, char* portno){
    int sockfd;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN]; 
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(hostname, portno, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        exit(1);
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure
	
	return sockfd;
}


//Function: Chat
//This function runs the chat. Recieving and
//sending messages.
//Input: the socket file desctiptor
//and a handle.
//Output: none

void chat(int socket_fd, char* handle){
	char buf[MAXDATASIZE];
	int numbytes;
	int quit;
	while(1){
	
		//send a message
		memset(buf, 0, MAXDATASIZE); //clear the buffer (fill it with '0')
		printf("%s> ", handle);
		fgets(buf, MAXDATASIZE-1, stdin);
		quit = strncmp(buf, "\\quit", 4);
		if (quit == 0){ 
			//received quit message; exit from function.
			if(send(socket_fd, "Connection has been closed by the Client\n", 41, 0) == -1){
				//There was an error. Exit with error.
				perror("send");
				exit(1);
			}
			//no error. Exit without error after closing the socket.
			close(socket_fd);
			exit(0);
		}
		else{
			//check for error when sending a message (buf), if so exit with error
			if(send(socket_fd, buf, strlen(buf), 0) == -1){
				perror("send");
				exit(1);
			}
		}
		
		//receive
		if ((numbytes = recv(socket_fd, buf, MAXDATASIZE-1, 0)) == -1) {
			perror("recv");
			exit(1);
		}
		// If server quits, quit client.
		if (strncmp(buf, "Connection closed by Server", 27) == 0){
			printf("%s\n", buf);
			close(socket_fd);
			exit(0);
		}
		//if nothing goes wrong, print the message.
		buf[numbytes-1] = '\0';
		printf("Server> %s\n",buf);
	}
	return;
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes; // These are socket file discriptor and number of bytes
	char buf[MAXDATASIZE];
	char handle[MAXHANDLESIZE];
	size_t len;

	//check for proper number of inputs
	if (argc != 3) {
	    fprintf(stderr,"usage: client hostname portnumber\n");
	    exit(1);
	}

	//connect to Server using previously defined function setupConnect
	sockfd = setupConnect(argv[1], argv[2]);

	//get handle and send to Server
	printf("Please enter a handle up to %d characters: ", MAXHANDLESIZE);
	fgets(handle, MAXHANDLESIZE, stdin); //get handle from stdin
	len = strlen(handle) - 1;
	if (handle[len] == '\n') //remove the newline character
        handle[len] = '\0';
        if(send(sockfd, handle, strlen(handle), 0) == -1){
		perror("send");
		exit(1);
	}
	
	//Begin chat
	printf("\nWait for prompt to begin typing message\n");
	printf("Type '\\quit' to quit at any time\n\n");
	chat(sockfd, handle);      

    close(sockfd);

    return 0;
}
