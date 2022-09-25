/* 		CLIENT PROGRAM
  Author:Shrikrishna Bhat
*/

//User-defined header
#include "client3.h"

//Main Function	
int main()
{
	//Initialise a socket variable,file descriptor and a local variable
	int sockfd,fdmax,i;
	//Initialise a structure variable for socket
	struct sockaddr_in server_addr;
	//Initialise a variable to keep control of logins and send information to respective file descriptors
	fd_set master;
	//Initialise a temporary file descriptor for select()
	fd_set read_fds;
	
	//Open TCP socket and connect by calling function	
	connect_request(&sockfd, &server_addr); 
	
	//Clear master set
	FD_ZERO(&master);
	//Clear temp set
        FD_ZERO(&read_fds);
	
	//Set the value of master
        FD_SET(0, &master);
        FD_SET(sockfd, &master);

	//Give the value of sockfd to fdmax
	fdmax = sockfd;
	
	//Enter the client name
	printf("\n Enter User Name:");
	scanf("%s",users);	
	getchar();
	
	//call function to send user to server
	send_user(sockfd);
	
	//Make it run infinitey
	for(;;){

		//Make a copy of master since select changes the value of master 
		read_fds = master;
		
		//Select system call is used such that it can monitor multiple file descriptors
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			perror("Select Error: ");
			exit(1);
		}
		
		//File Descriptors are monitored
		for(i=0; i <= fdmax; i++ )
			//Checks if the file descriptor is set 
			if(FD_ISSET(i,&read_fds)){
				/*printf("%s:\n",users);*/
				//Call a function
				send_recv(i,sockfd);
			}
	}
	//Make client to exit
	printf("Client has quited Successfully\n");
	//close socket
	close(sockfd);
	return 0;
}
/* Function to open a TCP Socket
   Parameters:int *sockfd and struct sockaddr_in *server_addr
   return:void
*/
		
void connect_request(int *sockfd, struct sockaddr_in *server_addr)
{
	//Open a socket
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket Error: ");
		exit(1);
	}
	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(6000);
	server_addr->sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(server_addr->sin_zero, '\0', sizeof server_addr->sin_zero);
	
	//Connect to server
	if(connect(*sockfd, (struct sockaddr *)server_addr, sizeof(struct sockaddr)) == -1) {
		perror("Connect Error: ");
		exit(1);
	}
}

/* Function: send_user
   parameters: int sockfd
   return:void
*/

void send_user(int sockfd)
{

	//Initalise a receive buffer to recieve from server
	char recv_buf[MAX];
	//Initalise values for response and choice
	int response,choice;
	/*printf("sending name");*/
	
	//Send user to server
	send(sockfd, users, strlen(users), 0);
	//Receive ack from server
	recv(sockfd, recv_buf, sizeof(recv_buf), 0);
	//Note the response given by server
	response=atoi(recv_buf);

	if(response==0){
		printf("You are Not Registered\n");
		printf("Do you want to enter the Chatroom?\n1:Yes\n2:No\n");
		scanf("%d",&choice);
		if(choice==1){
			//Enter user to chat by sending to server
			send(sockfd, users, strlen(users), 0);
			response=1;
		}
		
		else{
			//If other options exit chat room
			printf("Exitting Chat-Room");	
			exit(0);
		}
	}
	//If entered chat-room
	if(response==1)
		printf("Welcome to the Chat-Room: %s\nEnter the desired message and press <enter> to send.\nEnter <quit> or <exit> to exit from chat room\n",users);
	//flush the screen
	fflush(stdout);
}

/* Function:send and receive
   Parameters: variable i and sockfd-socket_descriptor
   returns:void
*/

void send_recv(int i, int sockfd)
{	
	//Everytime increment the flag for each terminal
	flag++;
	//Initalise buffers to send and recieve
	char send_buf[MAX];
	char recv_buf[MAX];
	//Initalise values to check bytes recieved
	int nbyte_recvd;
	//A temporary user array
	char users1[MAX];

	//For every new terminal to exit if user enters quit
	if (i == 0){
		/*printf("\nU:");*/
		//get user input from chat
		fgets(send_buf,MAX,stdin);
		
		//if entered quit
		if (strcmp(send_buf,"quit\n") == 0
		    || strcmp(send_buf,"exit\n")==0) {
			exit(0);
		}
		//Else other message
		else if (strcmp(send_buf,"$cu\n") == 0) {
                        /*printf("client sending %s\n","$cu");*/
			//Send to socket by checking current message typed                       
			send(sockfd,"$cu\n", strlen("$cu\n"), 0);
                        /*fflush(stdout);
                        //printf("Completed sending");
                        //fflush(stdout);
			*/

                }
		//To display message as "uname: "
		else{
			//Copy present user to temporary user1 array
			strcpy(users1,users);
			//Append with:
			strcat(users1,":");
			//Concatenate with send_buf
			strcat(users1,send_buf);
			//copy users1 to send_buf
			strcpy(send_buf,users1);
			
			//Send to server 
			send(sockfd, send_buf, strlen(send_buf), 0);
			//End with null character
			strcpy(users1,"\0");
		}
	}
	//If not store in a recieve buffer and print on screen
	else {
		nbyte_recvd = recv(sockfd, recv_buf, MAX, 0);
		recv_buf[nbyte_recvd] = '\0';
		printf("%s\n",recv_buf);
		fflush(stdout);
	}
}
		

