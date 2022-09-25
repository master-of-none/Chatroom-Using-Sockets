/* 		SERVER PROGRAM
  Author:Shrikrishna Bhat
*/
//Call user-defined headers
#include "server3.h"
//Main Function
int main()
{
	//Initialise a variable to keep control of logins and send information to respective file descriptors
	fd_set master;
	//Initialise a temporary file descriptor for select()
	fd_set read_fds;
	//Initialise a socket variable,file descriptor and a local variable
	int fdmax, i;
	//Initalise a socket variable
	int sockfd= 0;
	struct sockaddr_in my_addr, client_addr;
	
	//Clear master set
	FD_ZERO(&master);
	//Clear temp set
	FD_ZERO(&read_fds);

	//Open TCP conn using function
	connect_request(&sockfd, &my_addr);

	//Set the value of maste
	FD_SET(sockfd, &master);
	
	//Give the value of sockfd to fdmax
	fdmax = sockfd;

	//make it run infinitely
	for(;;){
		//Make a copy of master since select changes the value of master 
		read_fds = master;
		//Select system call is used such that it can monitor multiple file descriptors
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			perror("select");
			exit(1);
		}

		//File Descriptors are monitored
		for (i = 0; i <= fdmax; i++){
			//Checks if the file descriptor is set 
			if (FD_ISSET(i, &read_fds)){
				if (i == sockfd){
					//If it refers to socket then call this fn
					connection_accept(&master, &fdmax, sockfd, &client_addr);
		
	}
				
				else
					//For new Socket call this fn
					send_recv(i, &master, sockfd, fdmax);
			}
		}
	}
	return 0;
}

/* Function to Check the user
   parameter:uname
   return:int
*/

int check_user(char uname[MAX])
{
	//Initalise a file pointer
	FILE *fp;
	//Length of a file
	size_t len=0;
	ssize_t read;

	char line[MAX];
	//Open file
	if((fp=fopen("users.txt","r"))<0){
		printf("Error opening file\n");
		exit(1);
	}

	while ((fread(line,sizeof(line),1,fp))){
		if(strncmp(line,uname,(read-1))==0){
             		fclose(fp);
			return 1;
         	}

	//If exists
	printf("Inside the Chat-Room\n");
     	}
   	fclose(fp);
	return 0;
}

/* Function to validate the user
   parameter:sockfd
   return:int
*/

int validate_user(int sockfd)
{
	//Initalise a recieve buffer and uname to store username
	char recv_buf[MAX],uname[MAX];
	
	//Receieve The username from client
	if ((recv(sockfd,recv_buf,MAX,0))<= 0) {
		printf("Receive Error\n");
	}
       /*printf("%s\n",recv_buf);*/
	
	
	//Call the check_user to check if he is existing or not
	if(check_user(recv_buf)){
		//If the user exists. Then let him to chat
		printf("Welcome To the Chat-Room %s",recv_buf);
		//Add to the local list of users to monitor.
		strcpy(cusers[counter].name,recv_buf);
	
		//Send the response '1' to the client that client can proceed with chatting process
		if (send(sockfd,"1", sizeof("1"), 0) == -1) {
		printf("Socket Error");
		}
	}
	else{
		//User is not registered.Print it
		printf("%s You are not registered!",recv_buf);	
		fflush(stdout);
	
		//Stop The user temporarily and send the response to client such that user can register
		strcpy(uname,recv_buf);

		if (send(sockfd,"0", sizeof("0"), 0) == -1) {
			printf("Send Error");
		}
		//Terminate a recieve buffer with '\0'
		strncpy(recv_buf,"\0",MAX);

		//Receieve the response from client to register
		if ((recv(sockfd,recv_buf,MAX, 0)) <= 0) {
			printf("Receive Error\n");
		}

		//Register The user
		else{
			//Add the user to the file and make him as a registerd user
			FILE *fp;
			fp=fopen("users.txt","a");
			fwrite(uname,1,sizeof(uname),fp);
			/*fwrite("\n",1,sizeof("\n"),fp);*/
			fclose(fp);
		
			printf("User Registered Successfully....\n");	
			//Keep track of the user in the counter
			strcpy(cusers[counter].name,uname);
		}
	}

	strncpy(recv_buf,"\0",MAX);
	fflush(stdout);
	
	/*printf("uname=%s\n",uname);*/

	//If the user is online give his status as 1	
	cusers[counter].status=1;
	//Assign his id with the socket id
	cusers[counter].id=sockfd;
	 
	//He is online.So print his name status
	printf("\nName %d:%s\nStatus %d:%d\nId %d:%d\n",counter,cusers[counter].name,counter,cusers[counter].status,counter,cusers[counter].id);
	
	//Increment The counter for multiple users
	counter++;
}

/* Function to open a TCP Socket
   Parameters:int *sockfd and struct sockaddr_in *server_addr
   return:void
*/
	
void connect_request(int *sockfd, struct sockaddr_in *my_addr)
{
	//Variable if connect Exists	
	int yes = 1;
	
	//Open Socket	
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}
		
	//Connect Socket
	my_addr->sin_family = AF_INET;
	my_addr->sin_port = htons(6000);
	my_addr->sin_addr.s_addr = INADDR_ANY;
	memset(my_addr->sin_zero, '\0', sizeof my_addr->sin_zero);
		
	if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}
	
	//Bind The Socket	
	if (bind(*sockfd, (struct sockaddr *)my_addr, sizeof(struct sockaddr)) == -1) {
		perror("Unable to bind");
		exit(1);
	}
	//Listen to The Connection
	if (listen(*sockfd, 10)==-1) {
		perror("listen");
		exit(1);
	}
	printf("\nTCP Server Waiting for client on port 6000\n");
	fflush(stdout);
}

/* Function To accept Connection from Client
   Parameters:fd_set *master, int *fdmax, int sockfd, struct sockaddr_in *client_addr
   return:void
*/

void connection_accept(fd_set *master, int *fdmax, int sockfd, struct sockaddr_in *client_addr)
{
	//Length of address	
	socklen_t addrlen;
	//Socket Variable
	int newsockfd;
	
	//Length of socket structure
	addrlen = sizeof(struct sockaddr_in);
	
	//Accept Connection
	if((newsockfd = accept(sockfd, (struct sockaddr *)client_addr, &addrlen)) == -1) {
		perror("Accept Error");
		exit(1);
	}
	else {
		//Set the new socket and assign it a connection
		FD_SET(newsockfd, master);
		if(newsockfd > *fdmax){
			*fdmax = newsockfd;
		}
		printf("New connection Accepted: %s on Port number: %d \n",inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
	}
		//Check For the validity of the user passed for the new connection
		validate_user(newsockfd);					
}

/* Function To send The message to all the users
   Parameters:int j,int i,int sockfd,int bytes,char* recv_buf,fd_set *master,int cmd
   return:void
*/
					
void send_to_all(int j, int i, int sockfd, int nbytes_recvd, char *recv_buf, fd_set *master,int cmd)
{
	//Check if all the file descriptors are set correctly
	if (FD_ISSET(j,master)){
		if(cmd==1){
			//Check for the value if single client is present
			if(i==j){
                        	if (send(j,recv_buf,nbytes_recvd,0) == -1) {
                                	perror("Send Error");
                		}
			}
		}

		//For multiple Clients
		else{
			if (j != sockfd && j!=i) {
                        	if (send(j, recv_buf, nbytes_recvd, 0) == -1) {
                     			perror("Send Error");
                        	}
				}
		}	
	}
}	

/* Function: To send and receive messagaes
   Parameters:int i,fd_set *master,int sockfd,int fdmax
   Return:void
*/
		
void send_recv(int i, fd_set *master, int sockfd, int fdmax)
{
	//Initialise to receive bytes
	int nbytes_recvd, j,k;
	char recv_buf[MAX], buf[MAX];
	//Initalise to check the sender
	int sender;
	//Check if the message is sent or not
	if ((nbytes_recvd = recv(i,recv_buf,MAX,0))<=0) {
		if (nbytes_recvd == 0) {
			printf("Socket %d Hung\n", i);
			
			//Check for the Users who left the Chat-Room
			for(k=0;k<=counter;k++){
				if(i==cusers[k].id && cusers[k].status==1){
					cusers[k].status=0;
					printf("\n User: %s has left Chat-Room\nStatus:%d\n",cusers[k].name,cusers[k].status);
				}
			}
		}
		else {
			perror("Receive Error");
		}
		//Give The value to the current sender
		sender=i;
		//close the connection
		close(i);
		
		//Clear the i flag and master
		FD_CLR(i, master);
	}
	else { 
		/*printf("%s\n", recv_buf);
		 //printf("server received %s.",recv_buf);*/
		
		//Check User and store in db
      		if(strncmp(recv_buf,"$cu",(nbytes_recvd-1))==0){
      	  		printf("Check user began!");
                	/*fflush(stdout);*/

			//Send The list
                	printf("Sending list\n");

                	for(j =4; j<=fdmax; j++){
                		/*printf("Sending it to id:%d",cusers[k].id);*/
	
				//Give the value of counter flag to each user
				printf("Counter value:%d\nId no.%d is sending\nvalue of j=%d\n",counter,sender,j);
			
				for(k=0;k<counter;k++){
                			if(cusers[k].status==1)
						//Send message to all the users
						send_to_all(j, i,sockfd, sizeof(cusers[k].name),cusers[k].name, master,1);
				}
                	}
			//Flush the screen
			fflush(stdout);
	
                	printf("Completed Sending list\n");
       		 }
		//Send message to all the users
		else
			 for(j = 0; j <= fdmax; j++){
                        	send_to_all(j, i, sockfd, nbytes_recvd, recv_buf, master,0);
               	 	}	
	}
}

