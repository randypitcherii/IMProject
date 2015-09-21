#include <gtk/gtk.h>
#include <stdio.h>

#include "SetupView.h"
#include "NewUserView.h"
#include "ChatView.h"
#include "errorWindow.h"
#include "Comm.h"


#define MAX_MESSAGE_LENGTH 500

int main(int   argc, char *argv[] ) {
	//put this in each view to start gtk stuff:     gtk_init (&argc, &argv);
	//put this at the end of each view to end gtk stuff:		gtk_main();

	//Setup parameters
	char ** user = (char **) malloc(sizeof(char *));
	char ** password = (char **) malloc(sizeof(char *));
	char ** host = (char **) malloc(sizeof(char *));
	char ** sport = (char **) malloc(sizeof(char *));
	char ** response = (char **) malloc(sizeof(char *));
	int buttonPressed;

	//This is the beginning of the process loop for the entire program structure.
	//if someone logs out and the program should restart, a simple "continue" command
	//will reset the whole process. If the program is terminated, a "break" command will exit
	// from any point in the loop.
	while(true) {
		//reset initial parameter values.
		user[0] = password[0] = host[0] = sport[0] = response[0] = NULL;
		buttonPressed = -1;
	
		//Show Setup View.
		buttonPressed = startSetupView(host, sport, user, password);//0 means new user, 1 means login, -1 is a user exit

		if(buttonPressed == -1) {
			//startSetupView returned -1, so it was exited. Exit the whole program now.
			break;
		} if (buttonPressed == 0) {
			//0 indicates newUser was indicated in startSetupView. buttonPressed is now set
			//to the newUser view and will submit a new user and return 1 or be closed by the
			//user and return -1;
			buttonPressed = startNewUserView(host, sport, user, password, response);
		} if (buttonPressed == -1) {
			//newUser must have been exited, so the whole program is exited now.
			break;
		} 

		//to have made it here means either newUser or Setup was commanded to login. both lead to the ChatView below after
		//some validity checks on the user input here.
		
		//Input validation
		if(NULL == user[0] || NULL == password[0] || NULL == host[0] || NULL == sport[0]) {
			startErrorWindow("An input is empty.");
			continue;
		}

		Comm * validityChecker = new Comm(host[0], sport[0], user[0], password[0]);
		char * demand = (char *) malloc(sizeof(char)*14);
		strcpy(demand, "GET-ALL-USERS");
		char * space = (char *) malloc(sizeof(char)*2);
		strcpy(space, " ");

		char * validityResponse = (*validityChecker).sendCommand(demand, space, space);

		if (NULL == validityResponse) {
			startErrorWindow("Could not connect to Server.");
			continue;
		}else if (!strcmp(validityResponse, "ERROR (Wrong password)\r\n")) {
			startErrorWindow("Wrong Username / Password.");
			continue;
		}
		
		//Go to ChatView
		buttonPressed = startChatView(host, sport, user, password); //-1 for user exit, 0 for logout, 1 for new room
		
		if (buttonPressed == -1) {
			break;
		}
	}
	

	return 0;
}
