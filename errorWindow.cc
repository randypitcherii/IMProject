#include "errorWindow.h"

static void okButton_callback(GtkWidget * widget, GtkWidget * window) {
	gtk_widget_destroy(window);
	gtk_main_quit();
}

void startErrorWindow(const char * message) {
	GtkWidget * errorWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (errorWindow), "Error");
    g_signal_connect (errorWindow, "destroy", G_CALLBACK (gtk_main_quit), NULL);
	gtk_container_set_border_width (GTK_CONTAINER (errorWindow), 5);
    gtk_widget_set_size_request (GTK_WIDGET (errorWindow), 200, 200);

	//initializes gtk activity
	gtk_init (NULL, NULL);

	GtkWidget * label = gtk_label_new (message);
	GtkWidget * okButton = gtk_button_new_with_label ("OK");

	//a table is created here for organizing the widgets.
	GtkWidget * table = gtk_table_new(2, 1, false);
	gtk_container_add(GTK_CONTAINER(errorWindow), table);
	gtk_table_set_row_spacings(GTK_TABLE (table), 5);
    gtk_table_set_col_spacings(GTK_TABLE (table), 5);
    gtk_widget_show (table);

	//add button and label to table.
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0,1, 0,1);
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(table), okButton, 0,1, 1,2);
	gtk_widget_show(okButton);
	
	//wire up the ok button
	g_signal_connect(okButton, "clicked", G_CALLBACK(okButton_callback), errorWindow);

	//everthing is working and now the window is ready to be viewed.
	gtk_widget_show_all(errorWindow);

	gtk_main();
}
