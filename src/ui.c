#include "ui.h"
#include "graph.h"

static UI _ui;
UI* ui = &_ui;

void ui_init() {
	ui->main_window	= GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	gtk_window_set_title(GTK_WINDOW(ui->main_window), "OpenGL Area test");
	gtk_window_set_default_size(GTK_WINDOW(ui->main_window), 640, 480);
	g_signal_connect(GTK_WIDGET(ui->main_window), "destroy", gtk_main_quit, NULL);

	ui->gl_area	= GTK_GL_AREA(gtk_gl_area_new());
	gtk_widget_set_size_request(GTK_WIDGET(ui->gl_area), 640, 480);
	gtk_container_add(GTK_CONTAINER(ui->main_window), GTK_WIDGET(ui->gl_area));


	/* We need to initialize and free GL resources, so we use
	 * the realize and unrealize signals on the widget
	 */
	g_signal_connect(GTK_WIDGET(ui->gl_area), "realize", G_CALLBACK(on_realize), NULL);
	g_signal_connect(GTK_WIDGET(ui->gl_area), "resize", G_CALLBACK(on_resize), NULL);

	/* The main "draw" call for GtkGLArea */
	g_signal_connect(GTK_WIDGET(ui->gl_area), "render", G_CALLBACK(on_render), NULL);

	gtk_widget_show_all(GTK_WIDGET(ui->main_window));
}
