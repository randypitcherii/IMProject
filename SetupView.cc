#include "SetupView.h"

//text boxes and inputs declared here
GtkWidget * hostText;
GtkWidget * hostInputBox;
GtkWidget * portText;
GtkWidget * portInputBox;
GtkWidget * userText;
GtkWidget * userInputBox;
GtkWidget * passwordText;
GtkWidget * passwordInputBox;
GtkWidget * setupWindow;
char * tempHost;
char * tempSport;
char * tempUser;
char * tempPassword;
	

//gets entry data in the event that login is clicked and sets the value pointed to by loginValid to true to indicate login was pressed;
static void login_callback( GtkWidget * widget, gboolean * loginValid) {

	tempHost = (char *) malloc(sizeof(char) * (strlen(gtk_entry_get_text (GTK_ENTRY (hostInputBox))) + 1));
	strcpy(tempHost, gtk_entry_get_text (GTK_ENTRY (hostInputBox)));

	tempSport = (char *) malloc(sizeof(char) * (strlen(gtk_entry_get_text (GTK_ENTRY (portInputBox))) + 1));
	strcpy(tempSport, gtk_entry_get_text (GTK_ENTRY (portInputBox)));

	tempUser = (char *) malloc(sizeof(char) * (strlen(gtk_entry_get_text (GTK_ENTRY (userInputBox))) + 1));
	strcpy(tempUser, gtk_entry_get_text (GTK_ENTRY (userInputBox)));

	tempPassword = (char *) malloc(sizeof(char) * (strlen(gtk_entry_get_text (GTK_ENTRY (passwordInputBox))) + 1));
	strcpy(tempPassword, gtk_entry_get_text (GTK_ENTRY (passwordInputBox)));

	loginValid[0] = true;	
	gtk_widget_destroy(setupWindow);
	gtk_main_quit();
}

//this sets newUserValid[0] to true to indicate new user was pressed and ends the system setup window.
static void newUser_callback(GtkWidget * widget, gboolean * newUserValid) {
	newUserValid[0] = true;
	gtk_widget_destroy(setupWindow);
	gtk_main_quit();
}

//returns 0 if NewUser is pressed and 1 if LogIn is pressed.
int startSetupView(char ** host, char ** sport, char ** user, char ** password) {
	gboolean * loginValid = g_new(gboolean, 1);
	gboolean * newUserValid = g_new(gboolean, 1);
	loginValid[0] =  false; //used to track if login was pushed
	newUserValid[0] = false; //used to track if newUSer was pushed

	//initializes gtk activity
	gtk_init (NULL, NULL);

	//The main window is instantiated and defined.
    setupWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (setupWindow), "Chat Client Setup");
    g_signal_connect (setupWindow, "destroy", G_CALLBACK (gtk_main_quit), NULL);
	gtk_container_set_border_width (GTK_CONTAINER (setupWindow), 20);
    gtk_widget_set_size_request (GTK_WIDGET (setupWindow), 450, 350);

	
	//text boxes and inputs declared here
    hostText = gtk_label_new ("Host: ");
	hostInputBox = gtk_entry_new ();
	portText = gtk_label_new ("Port: ");
	portInputBox = gtk_entry_new ();
	userText = gtk_label_new ("Username: ");
	userInputBox = gtk_entry_new ();
	passwordText = gtk_label_new ("Password: ");
	passwordInputBox = gtk_entry_new ();
	gtk_entry_set_visibility(GTK_ENTRY(passwordInputBox), (gboolean) false);
	

	//buttons declared here.
	GtkWidget * loginButton = gtk_button_new_with_label ("Login");
	GtkWidget * newUserButton = gtk_button_new_with_label ("New User");

	//a table is created here for organizing the widgets.
	GtkWidget * table = gtk_table_new(6, 4, false);
	gtk_container_add(GTK_CONTAINER(setupWindow), table);
	gtk_table_set_row_spacings(GTK_TABLE (table), 5);
    gtk_table_set_col_spacings(GTK_TABLE (table), 5);
    gtk_widget_show (table);

	//Widgets are now placed in the table
	gtk_table_attach_defaults(GTK_TABLE(table), hostText, 0,1, 0,1);
	gtk_widget_show(hostText);
	gtk_table_attach_defaults(GTK_TABLE(table), hostInputBox, 2,3, 0,1);
	gtk_widget_show(hostInputBox);
	gtk_table_attach_defaults(GTK_TABLE(table), portText, 0,1, 1,2);
	gtk_widget_show(portText);
	gtk_table_attach_defaults(GTK_TABLE(table), portInputBox, 2,3, 1,2);
	gtk_widget_show(portInputBox);
	gtk_table_attach_defaults(GTK_TABLE(table), userText, 0,1, 2,3);
	gtk_widget_show(userText);
	gtk_table_attach_defaults(GTK_TABLE(table), userInputBox, 2,3, 2,3);
	gtk_widget_show(userInputBox);
	gtk_table_attach_defaults(GTK_TABLE(table), passwordText, 0,1, 3,4);
	gtk_widget_show(passwordText);
	gtk_table_attach_defaults(GTK_TABLE(table), passwordInputBox, 2,3, 3,4);
	gtk_widget_show(passwordInputBox);
	gtk_table_attach_defaults(GTK_TABLE(table), loginButton, 0,1, 4,5);
	gtk_widget_show(loginButton);
	gtk_table_attach_defaults(GTK_TABLE(table), newUserButton, 2,3, 4,5);
	gtk_widget_show(newUserButton);
	
	//Wire up the bottons here
	g_signal_connect(loginButton, "clicked", G_CALLBACK(login_callback), loginValid);
	g_signal_connect(newUserButton, "clicked", G_CALLBACK(newUser_callback), newUserValid);

    //Everything works and is now ready to be shown
    gtk_widget_show (setupWindow);

	gtk_main();

	if (loginValid[0]) {
		//login was pushed, the entry data is passed here
		host[0] = tempHost;
		sport[0] = tempSport;
		user[0] = tempUser;
		password[0] = tempPassword;
	
		return 1;
	}
	if (newUserValid[0]) {
		//newUser was pushed
		return 0;
	} else {
		//the user clicked exit instead of one of the buttons.
		return -1; 
	}
}

