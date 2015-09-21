#include "ChatView.h"

//text boxes and inputs declared here
GtkWidget * chatRoomsList;
GtkWidget * chatRoomsListScroller;
GtkWidget * messagesList;
GtkWidget * messagesListScroller;
GtkWidget * messageEntry;
GtkWidget * usersInRoomLabel;
GtkWidget * usersList;
GtkWidget * usersListScroller;
GtkWidget * chatViewWindow;
GtkWidget * newRoomWindow;
GtkWidget * newRoomEntry;

char * currentRoom;


Comm * comm;
	
//Creates the freely available comm object from the parameters passed to startChatView
void createCommObject(char * host, char * sport, char * user, char * password) {
	comm = new Comm(host, sport, user, password);
}

//Populates a GtkListStore object with the most updated information
//for Rooms.
void populateStoreRooms(GtkListStore * store) {
	char * noArg = (char *) malloc(sizeof(char)*2);
	noArg[0] = ' ';
	noArg[1] = '\0';
	char * listRooms = (char *) malloc(sizeof(char)*11);
	strcpy(listRooms, "LIST-ROOMS");
	gtk_list_store_clear(store);
	GtkTreeIter iter;
	gtk_list_store_append(store, &iter); //acquires iterator.
	
	char * response = (*comm).sendCommand(listRooms, noArg, noArg);
	char * nextRoom = (char *) malloc(sizeof(char)*30); //30 is used as max displayed room name.

	if (response == NULL) {
		return;
	}

	int i = 0;
	int j = 0;
	while (response[i] != '\0') {
		j = 0;
		while(response[i]!= '\r' && j < 29) {
			nextRoom[j] = response[i];
			j++;
			i++;		
		}
		nextRoom[j] = 0; //null terminator added to response string
		i = i + 2;//skip the '\r' and '\n' characters
		
		//means the end of the list of rooms has been reached because nothing was added to nextRoom
		if (j == 0) { 
			continue;
		}
		
		//adds nextRoom to the store	
		gtk_list_store_set(store, &iter, 0, nextRoom, -1);
			
		//iterates iter
		gtk_list_store_append(store, &iter);

	}
	gtk_list_store_remove(store, &iter); //removes the empty row created by the last iteration.
	return;		
}

// creates a full list for use with selecting rooms.
// a type of 0 makes a Rooms list, a type of 1 makes a messages list, and a type of 2 makes a users list
static GtkWidget * createList() {
	GtkListStore * store = gtk_list_store_new(1, G_TYPE_STRING);
	populateStoreRooms(store);
	
	//create the view components;
	GtkWidget * tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	GtkCellRenderer * renderer = gtk_cell_renderer_text_new();
	GtkTreeViewColumn * column = gtk_tree_view_column_new_with_attributes (" ", renderer, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), GTK_TREE_VIEW_COLUMN(column));
	
	GtkTreeSelection * firstSelection;
	firstSelection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
	gtk_tree_selection_set_mode(firstSelection, GTK_SELECTION_SINGLE);	
	GtkTreeIter iter;

	if (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter)) {
		//this means the model was empty
		return tree;
	}

	gtk_tree_selection_select_iter(firstSelection, &iter);
	char * holder;
	gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, 0, &holder, -1);
	currentRoom = (char *) malloc(sizeof(char)*(strlen(holder) + 1));
	strcpy(currentRoom, holder);

	char * noArg = (char *) malloc(sizeof(char)*2);
	noArg[0] = ' ';
	noArg[1] = '\0';
	char * sendMessage = (char *) malloc(sizeof(char)*13);
	strcpy(sendMessage, "SEND-MESSAGE");
	char * enterRoom = (char *) malloc(sizeof(char)*11);
	strcpy(enterRoom, "ENTER-ROOM");

	if (NULL == currentRoom || currentRoom[0] == '\0') {
		return tree;
	}

	(*comm).sendCommand(enterRoom, currentRoom, noArg);
  
	char * msg = (char *) malloc(sizeof(char) *(strlen((*comm)._user) + 22));
	strcpy(msg, (*comm)._user);
	strcat(msg, " has entered the room");
	(*comm).sendCommand(sendMessage, currentRoom, msg);

	return tree;
}

//populates the Messages text view;
void populateMessages(GtkTextBuffer * buffer) {
	if (currentRoom == NULL) {
		return;
	}
	char * getMessages = (char *) malloc(sizeof(char)*13);
	strcpy(getMessages, "GET-MESSAGES");	
	char * zero = (char *) malloc(sizeof(char)*2);
	zero[0] = 48;
	zero[1] = '\0';
	char * response = (*comm).sendCommand(getMessages, zero, currentRoom);
	if (response == NULL) {
		return;
	}
	if (!strcmp(response, "NO-NEW-MESSAGES\r\n")) {
		return;
	}

	char * toBuffer = (char *) malloc(sizeof(char) * strlen(response));
	int i = 0;
	int j = 0;

	while (response[i] != '\0') {
		if (response[i] == '\r') {
			break;
		}
		while(response[i] != ' ') { //discards msgnum from server.
			i++;
		}
		i++; //moves past the first space before user.

		while(response[i] != ' ') { //gets the username first.
			toBuffer[j] = response[i];
			j++;
			i++;
		}
		i++;//moves past the second space

		toBuffer[j] = ':';
		j++;
		toBuffer[j] = '\n';
		j++;

		while(response[i] != '\r') {
			toBuffer[j] = response[i];
			i++;
			j++;
		}
		i = i + 2; //moves past the \r\n at the end of this message.
		toBuffer[j] = '\n';//these separate messages by 2 newlines for easy viewing.
		j++;
		toBuffer[j] = '\n';
		j++;
	}
	toBuffer[j] = '\0';//null terminates the string;
	
	gtk_text_buffer_set_text(buffer, toBuffer, -1);

	GtkTextMark * mark = gtk_text_buffer_get_mark(buffer, "insert");
	gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(messagesList), mark, 0, true, 0, 1);
}

//populates the users text view;
void populateUsers(GtkTextBuffer * buffer) {
	if (currentRoom == NULL) {
		return;
	}
	char * noArg = (char *) malloc(sizeof(char)*2);
	noArg[0] = ' ';
	noArg[1] = '\0';
	char * getUsersInRoom = (char *) malloc(sizeof(char)*18);
	strcpy(getUsersInRoom, "GET-USERS-IN-ROOM");
	char * response = (*comm).sendCommand(getUsersInRoom, currentRoom, noArg);
	if (response == NULL) {
		return;
	}	

	char * toBuffer = (char *) malloc(sizeof(char) * strlen(response));
	int i = 0;
	int j = 0;

	while (response[i] != '\0') {
		while(response[i] != '\r') {
			toBuffer[j] = response[i];
			i++;
			j++;
		}
		i = i + 2; //moves past the \r\n at the end of this message.
		toBuffer[j] = '\n';//adds new line for next user to begin.
		j++;
	}
	toBuffer[j] = '\0';//null terminates the string;
	
	gtk_text_buffer_set_text(buffer, toBuffer, -1);
}

//Creates a text view widget.
//marker - indicator of which txt view to make.
//0 - message to send txt view
//1 - messages txt view
//2 - users txt view
static GtkWidget * createTxtView(int marker) {
    GtkWidget * view;
    GtkTextBuffer * buffer;

    view = gtk_text_view_new ();
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
	
    if (marker == 0) {
		gtk_text_view_set_editable(GTK_TEXT_VIEW (view), true);		
	} else if (marker == 1) {
		populateMessages(buffer);
		gtk_text_view_set_editable(GTK_TEXT_VIEW (view), false);		
	} else if (marker == 2) {
		populateUsers(buffer);
		gtk_text_view_set_editable(GTK_TEXT_VIEW (view), false);
	} else {
		return NULL;
	}

	return view;
}

//sends message to server, clears message box.
static void sendButton_callback(GtkWidget * widget, gpointer * data) {
	if (currentRoom == NULL) {
		return;
	}

	GtkTextIter start;
	GtkTextIter end;
	gtk_text_buffer_get_start_iter(gtk_text_view_get_buffer(GTK_TEXT_VIEW(messageEntry)), &start);
	gtk_text_buffer_get_end_iter(gtk_text_view_get_buffer(GTK_TEXT_VIEW(messageEntry)), &end);
	char * msg = gtk_text_buffer_get_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(messageEntry)), &start,&end, false);

	char * sendMessage = (char *) malloc(sizeof(char)*13);
	strcpy(sendMessage, "SEND-MESSAGE");

	char * response = (*comm).sendCommand(sendMessage, currentRoom, msg);

	gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(messageEntry)), "\0", -1);

	populateMessages(gtk_text_view_get_buffer (GTK_TEXT_VIEW (messagesList)));
}

//logs out
static void logoutButton_callback(GtkWidget * widget, gboolean * logoutPressed) {
	logoutPressed[0] = true;

	char * sendMessage = (char *) malloc(sizeof(char)*13);
	strcpy(sendMessage, "SEND-MESSAGE");
	char * noArg = (char *) malloc(sizeof(char)*2);
	strcpy(noArg, " ");
	char * msg = (char *) malloc(sizeof(char) *(strlen((*comm)._user) + 19));
	strcpy(msg, (*comm)._user);
	strcat(msg, " has left the room");
	char * leaveRoom = (char *) malloc(sizeof(char)*11);
	strcpy(leaveRoom, "LEAVE-ROOM");

	if (NULL != currentRoom) {
		(*comm).sendCommand(sendMessage, currentRoom, msg);
		(*comm).sendCommand(leaveRoom, currentRoom, noArg);
  }

	gtk_widget_destroy(chatViewWindow);
	gtk_widget_destroy(newRoomWindow);
	gtk_main_quit();
}

static void onExit_callback(GtkWidget * widget, gpointer * data) {
	char * sendMessage = (char *) malloc(sizeof(char)*13);
	strcpy(sendMessage, "SEND-MESSAGE");
	char * noArg = (char *) malloc(sizeof(char)*2);
	strcpy(noArg, " ");
	char * msg = (char *) malloc(sizeof(char) *(strlen((*comm)._user) + 19));
	strcpy(msg, (*comm)._user);
	strcat(msg, " has left the room");
	char * leaveRoom = (char *) malloc(sizeof(char)*11);
	strcpy(leaveRoom, "LEAVE-ROOM");

	if (NULL != currentRoom) {
		(*comm).sendCommand(sendMessage, currentRoom, msg);
		(*comm).sendCommand(leaveRoom, currentRoom, noArg);
  }

	gtk_widget_destroy(chatViewWindow);
	gtk_widget_destroy(newRoomWindow);
	gtk_main_quit();
}

static void newRoomButton_callback(GtkWidget * widget, gpointer * data) {
	gtk_widget_show(newRoomWindow);
}

static void destroyNewRoomWindow_callback(GtkWidget * widget, gpointer * data) {
	gtk_widget_hide(newRoomWindow);
	gtk_entry_set_text(GTK_ENTRY(newRoomEntry), "");
}

// starts new room.
static void createRoomButton_callback(GtkWidget * widget, gboolean * newRoomPressed) {
	char * newRoomName = (char *) gtk_entry_get_text(GTK_ENTRY(newRoomEntry));
	if(newRoomName == NULL || newRoomName[0] == '\0') {
		destroyNewRoomWindow_callback(newRoomWindow, NULL);	
		return;
	}

	char * noArg = (char *) malloc(sizeof(char)*2);
	noArg[0] = ' ';
	noArg[1] = '\0';
	char * createRoom = (char *) malloc(sizeof(char)*12);
	strcpy(createRoom, "CREATE-ROOM");
	
	(*comm).sendCommand(createRoom, newRoomName, noArg);
	populateStoreRooms(GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(chatRoomsList))));

	destroyNewRoomWindow_callback(newRoomWindow, NULL);	
}

// updates window with info for the newly selected room.
static void newRoomSelected_callback(GtkWidget * widget, gpointer * data) {
	char * noArg = (char *) malloc(sizeof(char)*2);
	noArg[0] = ' ';
	noArg[1] = '\0';
	char * sendMessage = (char *) malloc(sizeof(char)*13);
	strcpy(sendMessage, "SEND-MESSAGE");
	char * leaveRoom = (char *) malloc(sizeof(char)*11);
	strcpy(leaveRoom, "LEAVE-ROOM");
	char * enterRoom = (char *) malloc(sizeof(char)*11);
	strcpy(enterRoom, "ENTER-ROOM");

	GtkTreeIter  iter;
	GtkTreeSelection * current = gtk_tree_view_get_selection(GTK_TREE_VIEW(chatRoomsList));

	if (!gtk_tree_selection_get_selected(current, NULL, &iter)) {
		return;
	}
	
	char * msg = (char *) malloc(sizeof(char) *(strlen((*comm)._user) + 19));
	strcpy(msg, (*comm)._user);
	strcat(msg, " has left the room");
	if (NULL != currentRoom) {
		(*comm).sendCommand(sendMessage, currentRoom, msg);
		(*comm).sendCommand(leaveRoom, currentRoom, noArg);
	}
	
	char * holder;

	gtk_tree_model_get(gtk_tree_view_get_model(GTK_TREE_VIEW(chatRoomsList)), &iter, 0, &holder, -1);

	currentRoom = (char *) malloc(sizeof(char)*(strlen(holder) + 1));
	strcpy(currentRoom, holder);

	(*comm).sendCommand(enterRoom, currentRoom, noArg);
	
	msg = (char *) malloc(sizeof(char) *(strlen((*comm)._user) + 22));
	strcpy(msg, (*comm)._user);
	strcat(msg, " has entered the room");
	(*comm).sendCommand(sendMessage, currentRoom, msg);

	populateMessages(gtk_text_view_get_buffer (GTK_TEXT_VIEW (messagesList)));
	populateUsers(gtk_text_view_get_buffer (GTK_TEXT_VIEW (usersList)));
}

// starts new room.
static gboolean updater_callback(GtkWidget * widget, gboolean * newRoomPressed) {
	populateMessages(gtk_text_view_get_buffer (GTK_TEXT_VIEW (messagesList)));
	populateUsers(gtk_text_view_get_buffer (GTK_TEXT_VIEW (usersList)));

	return true;
}

//returns 0 if newRoom is pressed, 1 if logout is pressed, -1 if user exits.
int startChatView(char ** host, char ** sport, char ** user, char ** password) {
	//First, a Comm object is created to communicate with server;
	createCommObject(host[0], sport[0], user[0], password[0]);
	//additional flow control data structures 
	currentRoom = NULL;	
	gboolean * logoutPressed = g_new(gboolean, 1);
	gboolean * newRoomPressed = g_new(gboolean, 1);
	logoutPressed[0] =  false; //used to track if logout was pushed.
	newRoomPressed[0] = false; //used to track if newRoom was pushed.

	//initializes gtk activity
	gtk_init (NULL, NULL);

	//The main window is instantiated and defined.
    chatViewWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	char * windowTitle = (char *) malloc(sizeof(char) * (strlen(user[0]) + 1 + 16));
	strcpy(windowTitle, "Chat Client for ");
	strcat(windowTitle, user[0]);
	gtk_window_set_title (GTK_WINDOW (chatViewWindow), windowTitle);
    g_signal_connect (chatViewWindow, "destroy", G_CALLBACK (onExit_callback), NULL);
	gtk_container_set_border_width (GTK_CONTAINER (chatViewWindow), 10);
    gtk_widget_set_size_request (GTK_WIDGET (chatViewWindow), 900, 600);

	//The newRoom window is instantiated and defined
	newRoomWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	const char * newRoomTitle = "Create New Room";
	gtk_window_set_title (GTK_WINDOW (newRoomWindow), newRoomTitle);
    g_signal_connect (newRoomWindow, "destroy", G_CALLBACK (destroyNewRoomWindow_callback), NULL);
	gtk_container_set_border_width (GTK_CONTAINER (newRoomWindow), 5);
    gtk_widget_set_size_request (GTK_WIDGET (newRoomWindow), 100, 150);

	
	//text boxes and inputs created here
	chatRoomsList = createList();
	chatRoomsListScroller = gtk_scrolled_window_new (NULL, NULL);
	messagesList = createTxtView(1);
	messagesListScroller = gtk_scrolled_window_new (NULL, NULL);
	messageEntry = createTxtView(0);
	usersInRoomLabel = gtk_label_new ("Users in Room");
	usersList = createTxtView(2);
	usersListScroller = gtk_scrolled_window_new (NULL, NULL);
	newRoomEntry = gtk_entry_new();
	
	//Scrollers are refined here.
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(chatRoomsListScroller), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(messagesListScroller), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(usersListScroller), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (chatRoomsListScroller), chatRoomsList);
	gtk_container_add (GTK_CONTAINER (messagesListScroller), messagesList);
	gtk_container_add (GTK_CONTAINER (usersListScroller), usersList);

	//buttons declared here.
	GtkWidget * sendButton = gtk_button_new_with_label ("Send");
	GtkWidget * newRoomButton = gtk_button_new_with_label ("New Room");
	GtkWidget * logoutButton = gtk_button_new_with_label ("Logout");
	GtkWidget * createRoomButton = gtk_button_new_with_label ("Create Room");

	//a table is created here for organizing the widgets in the chatView Window.
	GtkWidget * table = gtk_table_new(5, 5, true);
	gtk_container_add(GTK_CONTAINER(chatViewWindow), table);
	gtk_table_set_row_spacings(GTK_TABLE (table), 10);
    gtk_table_set_col_spacings(GTK_TABLE (table), 10);
    gtk_widget_show (table);

	//a table is created here for organizing the widgets in the newRoom Window.
	GtkWidget * newRoomTable = gtk_table_new(2, 1, true);
	gtk_container_add(GTK_CONTAINER(newRoomWindow), newRoomTable);
	gtk_table_set_row_spacings(GTK_TABLE (newRoomTable), 5);
    gtk_table_set_col_spacings(GTK_TABLE (newRoomTable), 1);
   	gtk_widget_show (newRoomTable);

	//Widgets are now placed in the chatView table
	gtk_table_attach_defaults(GTK_TABLE(table), chatRoomsListScroller, 0,1, 0,3);
	gtk_widget_show(chatRoomsListScroller);
	gtk_widget_show(chatRoomsList);
	gtk_table_attach_defaults(GTK_TABLE(table), messagesListScroller, 1,4, 0,3);
	gtk_widget_show(messagesListScroller);
	gtk_widget_show(messagesList);
	gtk_table_attach_defaults(GTK_TABLE(table), messageEntry, 0,3, 3,5);
	gtk_widget_show(messageEntry);
	gtk_table_attach_defaults(GTK_TABLE(table), usersInRoomLabel, 4,5, 0,1);
	gtk_widget_show(usersInRoomLabel);
	gtk_table_attach_defaults(GTK_TABLE(table), usersListScroller, 4,5, 1,3);
	gtk_widget_show(usersListScroller);
	gtk_widget_show(usersList);
	gtk_table_attach_defaults(GTK_TABLE(table), sendButton, 3,4, 3,5);
	gtk_widget_show(sendButton);
	gtk_table_attach_defaults(GTK_TABLE(table), newRoomButton, 4,5, 3,4);
	gtk_widget_show(newRoomButton);
	gtk_table_attach_defaults(GTK_TABLE(table), logoutButton, 4,5, 4,5);
	gtk_widget_show(logoutButton);

	//Widgets are now placed in the newRoom table
	gtk_table_attach_defaults(GTK_TABLE(newRoomTable), newRoomEntry, 0,1, 0,1);
	gtk_widget_show(newRoomEntry);
	gtk_table_attach_defaults(GTK_TABLE(newRoomTable), createRoomButton, 0,1, 1,2);
	gtk_widget_show(createRoomButton);
	
	//Wire up the buttons here
	g_signal_connect(sendButton, "clicked", G_CALLBACK(sendButton_callback), NULL);
	g_signal_connect(logoutButton, "clicked", G_CALLBACK(logoutButton_callback), logoutPressed);
	g_signal_connect(newRoomButton, "clicked", G_CALLBACK(newRoomButton_callback), newRoomPressed);
	g_signal_connect(createRoomButton, "clicked", G_CALLBACK(createRoomButton_callback), NULL);

	//Wire up response to room selection change
	GtkTreeSelection * select;
	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(chatRoomsList));
	g_signal_connect(G_OBJECT(select), "changed", G_CALLBACK(newRoomSelected_callback), NULL);

	//Wire up updating mechanisms here.
	g_timeout_add(2000, (GSourceFunc) updater_callback, NULL);

    //Everything works and is now ready to be shown
    gtk_widget_show (chatViewWindow);

	gtk_main();

	if (logoutPressed[0]) {
		//logout was pushed	
		return 1;
	}
	if (newRoomPressed[0]) {
		//newRoom was pushed
		return 0;
	} else {
		//the user clicked exit instead of one of the buttons.
		return -1; 
	}
}

