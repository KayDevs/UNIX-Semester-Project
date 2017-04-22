#include "server_functions.h"

void error(const char *msg)
{
	perror(msg);
	exit(1);
}
//function prototype: Jorge Macias
void tcp_comm(int);

//AUTH: Keller Hood
//ARGS: ip is ip address in xxx.xxx.xxx.xxx format, 
//	port is port number, 
//RETURN: returns a struct containing two initialized sockets, one TCP and one UDP 

// Note: init_serv now returns NULL on error -- Enoch
// Note: modifying init_serv to only accept one arg (port #) -- Enoch
// Note: actually, screw it, now it accepts two args: an array of ports and the amnt of ports -- Enoch
struct serv *init_serv(int* ports, int portc) {
	// This is not done, we need to change things to make it so it handles the arguments correctly
	// This function will not work right now because all this code is based on the old function header with 1 argument
	// Note: the size of the ports array is always 3. But, the array might not always contain 3 ints (depending on how many arguments were supplied by the user).
	struct serv *new_serv = malloc(sizeof(struct serv));

	//create + bind + listen tcp socket
	int tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(tcp_sock < 0) {
		printf("Error creating socket.\n");
		return NULL;
	}
	struct sockaddr_in server_addr;  
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(tcp_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		printf("Error binding socket for TCP.\n");
		return NULL;
	}
	listen(tcp_sock, 5);

	new_serv->tcp_fd = tcp_sock;

	// UDP server init, AUTH: Enoch Ng
	int udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (udp_sock < 0) {
		printf("Error creating socket.\n");
		return NULL;
	}

	if (bind(udp_sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
		printf("Error binding socket for UDP.\n");
		return NULL;
	}

	new_serv->udp_fd = udp_sock;

	return new_serv;
}

//AUTH: Keller Hood
//ARGS: a server struct containing tcp and udp fds
//frees server's memory and closes the sockets
void close_serv(struct serv *server) {
	close(server->tcp_fd);
	close(server->udp_fd);
	free(server);
}

//AUTH: Jorge Macias
//ARGS: server struct containing tcp_fd and udp_fd
//TCP socket process
int tcp_proc(struct serv *server)
{
	int sockfd, newsockfd, portno, pid;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	
	clilen = sizeof(cli_addr);
	//implement while loop: Jorge Macias
	while (1)
	{
		newsockfd = accept(server->tcp_fd, 
				(struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) 
			error("ERROR on accept");
		//zombie process signal handler: Jorge Macias
		signal(SIGCHLD,SIG_IGN);
		//implement fork call: Jorge Macias
		pid = fork();
		if (pid < 0) 
			error("ERROR on fork");
		if (pid == 0)
		{
			close (server->tcp_fd);
			tcp_comm(newsockfd); //call tcp_comm function to prepare for additional instance: Jorge Macias
			exit(0);
		}
		else close(newsockfd);
	} //end of while loop: Jorge Macias
	close(server->tcp_fd);
	return 0; 
}

// Handle UDP
// Returns 1 on failure
// AUTH: Enoch Ng
// Child processes are not needed here because UDP is not connection-based, so the server is able to respond to multiple clients without needing multiple processes
int udp_proc(struct serv *server) {
	char buf[1024];
	socklen_t clilen;
	struct sockaddr_in cli_addr;
	clilen = sizeof(cli_addr);

	while (1) {
		int n = recvfrom(server->udp_fd, buf, 1024, 0, (struct sockaddr*) &cli_addr, &clilen); 
		// n contains the number of bytes in the message
		if (n < 0) {
			error("Error reciving UDP message\n");
		}
		write(1, "Received a datagram: ", 21);
		write(1, buf, n);
		n = sendto(server->udp_fd, buf, n, 0, (struct sockaddr*) &cli_addr, clilen);
		if (n < 0) {
			error("Error in sending reply to UDP message\n");
		}
	}

	return 0; // This should never happen ...
}

//AUTH: Jorge Macias
//Function to handle additional instances of client communication
void tcp_comm(int sock)
{
	int a;
	char buff[256];

	bzero(buff, 256);
	a = read(sock, buff, 256);
	if (a < 0) error ("ERROR reading from socket");
	printf("Here is a message from somewhere: %s\n", buff);
	//a = write(sock, "Got some message from someplace", 18); // Keeping this commented in case I break something -- Enoch
	a = write(sock, buff, 256); // Echo server TCP implementation -- Enoch Ng
	if (a < 0) error("ERROR writing from socket");
}
