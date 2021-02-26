#ifndef UI_H
#define UI_H

#include <gtk/gtk.h>

typedef struct UI {
	GtkWindow*	main_window;
	GtkGLArea*	gl_area;
} UI;

extern UI* ui;

void ui_init(void);


#endif // UI_H
