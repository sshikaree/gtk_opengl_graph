#ifndef GRAPH_H
#define GRAPH_H

#include <gtk/gtk.h>

#include <epoxy/gl.h>

typedef struct Point {
	GLfloat x;
	GLfloat y;
} Point;


void on_resize(GtkGLArea* gl_area, int width, int height);
void on_realize(GtkGLArea* gl_area);
gboolean on_render(GtkGLArea* gl_area, GdkGLContext* gl_context);






#endif // GRAPH_H
