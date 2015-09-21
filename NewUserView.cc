#include "SetupView.h"
#include "Comm.h"

#include <stdio.h>
#include <gtk/gtk.h>

//text boxes and inputs declared here
GtkWidget * newUserViewhostText;
GtkWidget * newUserViewhostInputBox;
GtkWidget * newUserViewportText;
GtkWidget * newUserViewportInputBox;
GtkWidget * newUserViewuserText;
GtkWidget * newUserViewuserInputBox;
GtkWidget * newUserViewpasswordText;
GtkWidget * newUserViewpasswordInputBox;
GtkWidget * newUserViewWindow;
char * newUserViewtempHost;
char * newUserViewtempSport;
char * newUserViewtempUser;
char * newUserViewtempPassword;
char * newUserViewtempResponse;
	

//gets entry data in the event that login is clicked and sets the value pointed to by loginValid to true to indicate login was pressed;
static void newUserLoginButton_callback( GtkWidget * widget, gboolean * loginValid) {

	newUserViewtempHost = (char *) malloc(sizeof(char) * (strlen(gtk_entry_get_text (GTK_ENTRY (newUserViewhostInputBox))) + 1));
	strcpy(newUserViewtempHost, gtk_entry_get_text (GTK_ENTRY (newUserViewhostInputBox)));

	newUserViewtempSport = (char *) malloc(sizeof(char) * (strlen(gtk_entry_get_text (GTK_ENTRY (newUserViewportInputBox))) + 1));
	strcpy(newUserViewtempSport, gtk_entry_get_text (GTK_ENTRY (newUserViewportInputBox)));

	newUserViewtempUser = (char *) malloc(sizeof(char) * (strlen(gtk_entry_get_text (GTK_ENTRY (newUserViewuserInputBox))) + 1));
	strcpy(newUserViewtempUser, gtk_entry_get_text (GTK_ENTRY (newUserViewuserInputBox)));

	newUserViewtempPassword = (char *) malloc(sizeof(char) * (strlen(gtk_entry_get_text (GTK_ENTRY (newUserViewpasswordInputBox))) + 1));
	strcpy(newUserViewtempPassword, gtk_entry_get_text (GTK_ENTRY (newUserViewpasswordInputBox)));

	loginValid[0] = true;	
	gtk_widget_destroy(newUserViewWindow);

	char * noArg = (char *) malloc(sizeof(char)*2);
	noArg[0] = ' ';
	noArg[1] = '\0';
	char * addUser = (char *) malloc(sizeof(char)*9);
	strcpy(addUser, "ADD-USER");

	Comm * comm = new Comm(newUserViewtempHost, newUserViewtempSport, newUserViewtempUser, newUserViewtempPassword);
	newUserViewtempResponse = 	(*comm).sendCommand(addUser, noArg, noArg);
	
	gtk_main_quit();
}

//returns 0 if NewUser is pressed and 1 if LogIn is pressed.
int startNewUserView(char ** host, char ** sport, char ** user, char ** password, char ** response) {
	gboolean * loginValid = g_new(gboolean, 1);
	loginValid[0] =  false; //used to track if newUserLoginButton was pushed

	//initializes gtk activity
	gtk_init (NULL, NULL);

	//The main window is instantiated and defined.
    newUserViewWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (newUserViewWindow), "Chat Client Add New User");
    g_signal_connect (newUserViewWindow, "destroy", G_CALLBACK (gtk_main_quit), NULL);
	gtk_container_set_border_width (GTK_CONTAINER (newUserViewWindow), 35);
    gtk_widget_set_size_request (GTK_WIDGET (newUserViewWindow), 350, 300);

	
	//text boxes and inputs declared here
    newUserViewhostText = gtk_label_new ("Host: ");
	newUserViewhostInputBox = gtk_entry_new ();
	newUserViewportText = gtk_label_new ("Port: ");
	newUserViewportInputBox = gtk_entry_new ();
	newUserViewuserText = gtk_label_new ("Username: ");
	newUserViewuserInputBox = gtk_entry_new ();
	newUserViewpasswordText = gtk_label_new ("Password: ");
	newUserViewpasswordInputBox = gtk_entry_new ();
	gtk_entry_set_visibility(GTK_ENTRY(newUserViewpasswordInputBox), (gboolean) false);
	

	//buttons declared here.
	GtkWidget * newUserLoginButton = gtk_button_new_with_label ("Add New User");

	//a table is created here for organizing the widgets.
	GtkWidget * table = gtk_table_new(6, 4, false);
	gtk_container_add(GTK_CONTAINER(newUserViewWindow), table);
	gtk_table_set_row_spacings(GTK_TABLE (table), 5);
    gtk_table_set_col_spacings(GTK_TABLE (table), 5);
    gtk_widget_show (table);

	//Widgets are now placed in the table
	gtk_table_attach_defaults(GTK_TABLE(table), newUserViewhostText, 0,1, 0,1);
	gtk_widget_show(newUserViewhostText);
	gtk_table_attach_defaults(GTK_TABLE(table), newUserViewhostInputBox, 2,3, 0,1);
	gtk_widget_show(newUserViewhostInputBox);
	gtk_table_attach_defaults(GTK_TABLE(table), newUserViewportText, 0,1, 1,2);
	gtk_widget_show(newUserViewportText);
	gtk_table_attach_defaults(GTK_TABLE(table), newUserViewportInputBox, 2,3, 1,2);
	gtk_widget_show(newUserViewportInputBox);
	gtk_table_attach_defaults(GTK_TABLE(table), newUserViewuserText, 0,1, 2,3);
	gtk_widget_show(newUserViewuserText);
	gtk_table_attach_defaults(GTK_TABLE(table), newUserViewuserInputBox, 2,3, 2,3);
	gtk_widget_show(newUserViewuserInputBox);
	gtk_table_attach_defaults(GTK_TABLE(table), newUserViewpasswordText, 0,1, 3,4);
	gtk_widget_show(newUserViewpasswordText);
	gtk_table_attach_defaults(GTK_TABLE(table), newUserViewpasswordInputBox, 2,3, 3,4);
	gtk_widget_show(newUserViewpasswordInputBox);
	gtk_table_attach_defaults(GTK_TABLE(table), newUserLoginButton, 0,3, 4,5);
	gtk_widget_show(newUserLoginButton);
	
	//Wire up the bottons here
	g_signal_connect(newUserLoginButton, "clicked", G_CALLBACK(newUserLoginButton_callback), loginValid);

    //Everything works and is now ready to be shown
    gtk_widget_show (newUserViewWindow);

	gtk_main();

	if (loginValid[0]) {
		//login was pushed, the entry data is passed here
		host[0] = newUserViewtempHost;
		sport[0] = newUserViewtempSport;
		user[0] = newUserViewtempUser;
		password[0] = newUserViewtempPassword;
		return 1;
	} else {
		//the user clicked exit instead of one of the buttons.
		return -1; 
	}
}

