#include <gtk/gtk.h>
#include <gtk-layer-shell/gtk-layer-shell.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

// Animation state
static double animation_time = 0.0;
static guint timeout_id = 0;

// Fire particle structure
typedef struct {
    double x, y;
    double vx, vy;
    double life;
    double size;
    double intensity;
} FireParticle;

#define MAX_PARTICLES 200
static FireParticle particles[MAX_PARTICLES];
static int particle_count = 0;

// Initialize particles along the border
static void init_particles() {
    particle_count = MAX_PARTICLES;
    srand(time(NULL));
    
    for (int i = 0; i < MAX_PARTICLES; i++) {
        FireParticle *p = &particles[i];
        
        // Randomly place particles along the border
        int side = rand() % 4;
        switch (side) {
            case 0: // Top
                p->x = rand() % 1920;
                p->y = 0;
                break;
            case 1: // Right
                p->x = 1920;
                p->y = rand() % 1080;
                break;
            case 2: // Bottom
                p->x = rand() % 1920;
                p->y = 1080;
                break;
            case 3: // Left
                p->x = 0;
                p->y = rand() % 1080;
                break;
        }
        
        p->vx = (rand() % 100 - 50) / 100.0;
        p->vy = (rand() % 100 - 50) / 100.0;
        p->life = 1.0;
        p->size = 5 + (rand() % 15);
        p->intensity = 0.5 + (rand() % 50) / 100.0;
    }
}

// Update particle positions and properties
static void update_particles(double dt) {
    for (int i = 0; i < particle_count; i++) {
        FireParticle *p = &particles[i];
        
        // Update position
        p->x += p->vx * dt * 100;
        p->y += p->vy * dt * 100;
        
        // Update life
        p->life -= dt * 0.5;
        
        // Add some randomness to movement
        p->vx += (rand() % 100 - 50) / 10000.0;
        p->vy += (rand() % 100 - 50) / 10000.0;
        
        // Respawn particle if it dies
        if (p->life <= 0) {
            int side = rand() % 4;
            switch (side) {
                case 0: // Top
                    p->x = rand() % 1920;
                    p->y = 0;
                    break;
                case 1: // Right
                    p->x = 1920;
                    p->y = rand() % 1080;
                    break;
                case 2: // Bottom
                    p->x = rand() % 1920;
                    p->y = 1080;
                    break;
                case 3: // Left
                    p->x = 0;
                    p->y = rand() % 1080;
                    break;
            }
            p->life = 1.0;
            p->vx = (rand() % 100 - 50) / 100.0;
            p->vy = (rand() % 100 - 50) / 100.0;
            p->size = 5 + (rand() % 15);
            p->intensity = 0.5 + (rand() % 50) / 100.0;
        }
    }
}

// Draw green glow effect
static void draw_fire_glow(cairo_t *cr, double x, double y, double size, double intensity, double life) {
    // Create radial gradient for glow effect
    cairo_pattern_t *gradient = cairo_pattern_create_radial(x, y, 0, x, y, size);
    
    // Green colors based on life and intensity
    double red = 0.0;
    double green = 1.0 * intensity * life;
    double blue = 0.0;
    double alpha = 0.8 * intensity * life;
    
    // Inner core (bright green)
    cairo_pattern_add_color_stop_rgba(gradient, 0.0, red, green * 1.2, blue, alpha);
    
    // Middle (main green color)
    cairo_pattern_add_color_stop_rgba(gradient, 0.4, red, green, blue, alpha * 0.7);
    
    // Outer glow (fade out)
    cairo_pattern_add_color_stop_rgba(gradient, 1.0, red, green * 0.5, blue, 0.0);
    
    cairo_set_source(cr, gradient);
    cairo_arc(cr, x, y, size, 0, 2 * M_PI);
    cairo_fill(cr);
    
    cairo_pattern_destroy(gradient);
}

// Draw animated border glow
static void draw_border_glow(cairo_t *cr, int width, int height) {
    // Animated glow intensity
    //double glow_intensity = 0.5 + 0.3 * sin(animation_time * 2.0);
    double glow_intensity = 0.8;
    // Draw glowing border lines
    cairo_set_line_width(cr, 8.0);
    
    // Top border
    cairo_pattern_t *top_gradient = cairo_pattern_create_linear(0, 0, width, 0);
    cairo_pattern_add_color_stop_rgba(top_gradient, 0.0, 0.0, 0.2, 0.0, 0.0);
    cairo_pattern_add_color_stop_rgba(top_gradient, 0.5, 0.0, 1.0, 0.0, glow_intensity);
    cairo_pattern_add_color_stop_rgba(top_gradient, 1.0, 0.0, 0.2, 0.0, 0.0);
    cairo_set_source(cr, top_gradient);
    cairo_move_to(cr, 0, 4);
    cairo_line_to(cr, width, 4);
    cairo_stroke(cr);
    cairo_pattern_destroy(top_gradient);
    
    // Right border
    cairo_pattern_t *right_gradient = cairo_pattern_create_linear(width, 0, width, height);
    cairo_pattern_add_color_stop_rgba(right_gradient, 0.0, 0.0, 0.2, 0.0, 0.0);
    cairo_pattern_add_color_stop_rgba(right_gradient, 0.5, 0.0, 1.0, 0.0, glow_intensity);
    cairo_pattern_add_color_stop_rgba(right_gradient, 1.0, 0.0, 0.2, 0.0, 0.0);
    cairo_set_source(cr, right_gradient);
    cairo_move_to(cr, width - 4, 0);
    cairo_line_to(cr, width - 4, height);
    cairo_stroke(cr);
    cairo_pattern_destroy(right_gradient);
    
    // Bottom border
    cairo_pattern_t *bottom_gradient = cairo_pattern_create_linear(0, height, width, height);
    cairo_pattern_add_color_stop_rgba(bottom_gradient, 0.0, 0.0, 0.2, 0.0, 0.0);
    cairo_pattern_add_color_stop_rgba(bottom_gradient, 0.5, 0.0, 1.0, 0.0, glow_intensity);
    cairo_pattern_add_color_stop_rgba(bottom_gradient, 1.0, 0.0, 0.2, 0.0, 0.0);
    cairo_set_source(cr, bottom_gradient);
    cairo_move_to(cr, 0, height - 4);
    cairo_line_to(cr, width, height - 4);
    cairo_stroke(cr);
    cairo_pattern_destroy(bottom_gradient);
    
    // Left border
    cairo_pattern_t *left_gradient = cairo_pattern_create_linear(0, 0, 0, height);
    cairo_pattern_add_color_stop_rgba(left_gradient, 0.0, 0.0, 0.2, 0.0, 0.0);
    cairo_pattern_add_color_stop_rgba(left_gradient, 0.5, 0.0, 1.0, 0.0, glow_intensity);
    cairo_pattern_add_color_stop_rgba(left_gradient, 1.0, 0.0, 0.2, 0.0, 0.0);
    cairo_set_source(cr, left_gradient);
    cairo_move_to(cr, 4, 0);
    cairo_line_to(cr, 4, height);
    cairo_stroke(cr);
    cairo_pattern_destroy(left_gradient);
}

static void on_draw(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {
    // Clear background
    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.0);
    cairo_paint(cr);
    
    // Set blend mode for additive blending
    cairo_set_operator(cr, CAIRO_OPERATOR_SCREEN);
    
    // Draw border glow
    draw_border_glow(cr, width, height);
    
    // Draw fire particles
    for (int i = 0; i < particle_count; i++) {
        FireParticle *p = &particles[i];
        if (p->life > 0) {
            draw_fire_glow(cr, p->x, p->y, p->size, p->intensity, p->life);
        }
    }
    
    // Clear the inner area (make it transparent/click-through)
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_rectangle(cr, 30, 30, width - 60, height - 60);
    cairo_fill(cr);
}

static gboolean on_timeout(gpointer user_data) {
    GtkWidget *drawing_area = GTK_WIDGET(user_data);
    
    // Update animation
    animation_time += 0.016; // ~60 FPS
    update_particles(0.016);
    
    // Trigger redraw
    gtk_widget_queue_draw(drawing_area);
    
    return G_SOURCE_CONTINUE;
}

static void on_realize(GtkWidget *widget, gpointer user_data) {
    GtkWindow *window = GTK_WINDOW(widget);
    
    // Make entire window click-through (no input region)
    GdkSurface *surface = gtk_native_get_surface(GTK_NATIVE(window));
    if (surface) {
        cairo_region_t *region = cairo_region_create();
        gdk_surface_set_input_region(surface, region);
        cairo_region_destroy(region);
    }
    
    // Initialize particles
    init_particles();
    
    // Start animation timer
    GtkWidget *drawing_area = gtk_window_get_child(window);
    timeout_id = g_timeout_add(16, on_timeout, drawing_area); // ~60 FPS
}

static void on_window_destroy(GtkWidget *widget, gpointer user_data) {
    if (timeout_id > 0) {
        g_source_remove(timeout_id);
        timeout_id = 0;
    }
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *drawing_area;
    
    // Set window properties
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Green Glow Border Overlay");
    
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
    gtk_layer_set_monitor(GTK_WINDOW(window), 0);
    
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
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);
    
    gtk_widget_set_visible(window, TRUE);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.example.green-glow-overlay", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}