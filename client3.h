//All Header Files
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<errno.h>

//Max Buffer Length	
#define MAX 1024

//Initalise a flag to monitor the status of logins
static int flag=0;

//Initialise a user array to get the user name
char users[MAX];

//Function Declarations
void connect_request(int *sockfd, struct sockaddr_in *server_addr);
void send_user(int sockfd);
void send_recv(int i, int sockfd);
