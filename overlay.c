#include <gtk/gtk.h>
#include <gtk-layer-shell/gtk-layer-shell.h>

static void on_draw(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data){
    cairo_set_source_rgba(cr, 0.0, 1.0, 0.0, 0.1);
    cairo_paint(cr);

    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_rectangle(cr, 30, 30, 1860, 1020);
    cairo_fill(cr);
}

static void on_realize(GtkWidget *widget, gpointer user_data){
    GtkWindow *window = GTK_WINDOW(widget);

    // Make entire window click-through (no input region)
    GdkSurface *surface = gtk_native_get_surface(GTK_NATIVE(window));
    if(surface) {
        cairo_region_t *region = cairo_region_create();
        gdk_surface_set_input_region(surface, region);
        cairo_region_destroy(region);
    }
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *drawing_area;
    
    // Set window properties
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Transparent Overlay");
    
    // Make window always on top and skip taskbar
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    
    // Initialize layer-shell for this window
    gtk_layer_init_for_window(GTK_WINDOW(window));

    // Place the window on the overlay layer (topmost)
    gtk_layer_set_layer(GTK_WINDOW(window), GTK_LAYER_SHELL_LAYER_OVERLAY);

    // Anchor to all edges to make it truly fullscreen
    gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_LEFT, TRUE);
    gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_RIGHT, TRUE);
    gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_TOP, TRUE);
    gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_BOTTOM, TRUE);
    
    // Set margins to 0 to ensure no gaps
    gtk_layer_set_margin(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_LEFT, 0);
    gtk_layer_set_margin(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_RIGHT, 0);
    gtk_layer_set_margin(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_TOP, 0);
    gtk_layer_set_margin(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_BOTTOM, 0);

    // Set exclusive zone to -1 to go above all other layers
    gtk_layer_set_exclusive_zone(GTK_WINDOW(window), -1);

    // Make it fullscreen and let Hyprland control the size
    gtk_layer_set_monitor(GTK_WINDOW(window), 0); // Optional: target a specific monitor

    // Let other windows receive input (click-through)
	gtk_layer_set_keyboard_mode(GTK_WINDOW(window), GTK_LAYER_SHELL_KEYBOARD_MODE_NONE);

    // Set transparent background
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "style.css");
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    gtk_widget_add_css_class(window, "transparent-window");

    gtk_widget_set_can_focus(window, FALSE);
    gtk_widget_set_can_target(window, FALSE);

    drawing_area = gtk_drawing_area_new();
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area), on_draw, NULL, NULL);

    gtk_widget_set_name(GTK_WIDGET(window), "overlay");
    gtk_window_set_child(GTK_WINDOW(window), drawing_area);

    g_signal_connect(window, "realize", G_CALLBACK(on_realize), NULL);

    gtk_widget_set_visible(window, TRUE);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.example.overlay", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
