#ifndef COMM
#define COMM

#define MAX_RESPONSE (10 * 1024)

#include <time.h>
//#include <curses.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>


// This is a comm object that facilitates socket communication between the gui and the server
class Comm {
 public:
	char * _user;
	char * _password;
	char * _host;
	char * _sport;
	int _port;
  
 public:
	int open_client_socket(char * host, int port);
	char * sendCommand(char * demand, char * room, char * message);
	Comm(char * host, char * sport, char * user, char * password);
	
};

#endif

