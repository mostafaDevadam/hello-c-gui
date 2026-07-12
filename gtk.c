#include <gtk/gtk.h>

// Callback function executed when the button is clicked
static void on_button_clicked(GtkWidget *widget, gpointer data) {
    g_print("Hello, World!\n");
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *button;

    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create the main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "C GUI Application");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);

    // Connect the window close event to exit the program safely
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a button with a label
    button = gtk_button_new_with_label("Click Me");

    // Connect the button click event to our callback function
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);

    // Add the button to the main window container
    gtk_container_add(GTK_CONTAINER(window), button);

    // Show all widgets inside the window
    gtk_widget_show_all(window);

    // Enter the main interactive event loop
    gtk_main();

    return 0;
}
