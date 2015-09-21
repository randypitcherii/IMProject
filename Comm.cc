
#include "Comm.h"

int Comm::open_client_socket(char * host, int port) {
	// Initialize socket address structure
	struct  sockaddr_in socketAddress;
	
	// Clear sockaddr structure
	memset((char *)&socketAddress,0,sizeof(socketAddress));
	
	// Set family to Internet 
	socketAddress.sin_family = AF_INET;
	
	// Set port
	socketAddress.sin_port = htons((u_short)port);
	
	// Get host table entry for this host
	struct  hostent  *ptrh = gethostbyname(host);
	if ( ptrh == NULL ) {
		//perror("gethostbyname");
		return -1;
	}
	
	// Copy the host ip address to socket address structure
	memcpy(&socketAddress.sin_addr, ptrh->h_addr, ptrh->h_length);
	
	// Get TCP transport protocol entry
	struct  protoent *ptrp = getprotobyname("tcp");
	if ( ptrp == NULL ) {
		//perror("getprotobyname");
		return -1;
	}
	
	// Create a tcp socket
	int sock = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
	if (sock < 0) {
		//perror("socket");
		return -1;
	}
	
	// Connect the socket to the specified server
	if (connect(sock, (struct sockaddr *)&socketAddress,
		    sizeof(socketAddress)) < 0) {
		//perror("connect");
		return -1;
	}
	
	return sock;
}

//
char * Comm::sendCommand(char * demand, char * room, char * message) {
	int sock = open_client_socket( _host, _port);

	if (sock<0) {
		return NULL;
	}
	
	//build command
	char * command = (char *) malloc(sizeof(char) * (strlen(demand) + strlen(_user) + strlen(_password) + strlen(room) + strlen(message) + 5)); //the 4 is for the 4 spaces in the largest possible command.
	strcpy(command, demand);
	strcat(command, " ");
	strcat(command, _user);
	strcat(command, " ");
	strcat(command, _password);
	strcat(command, " ");
	strcat(command, room);
	strcat(command, " ");
	strcat(command, message);

	// Send command
	write(sock, command, strlen(command));
	write(sock, "\r\n",2);

	char * responseFromServer = (char *) malloc(MAX_RESPONSE * sizeof(char));

	// Keep reading until connection is closed or MAX_REPONSE
	int n = 0;
	int len = 0;
	while ((n=read(sock, responseFromServer+len, MAX_RESPONSE - len))>0) {
		len += n;
	}
	responseFromServer[len]=0;

	close(sock);

	return responseFromServer;
}

// This initializes the core parameters needed for sending commands to the server.
Comm::Comm(char * host, char * sport, char * user, char * password) {
	_host = host;
	_sport = sport;
	_user = user;
	_password = password;
	sscanf(_sport, "%d", &_port);
}



