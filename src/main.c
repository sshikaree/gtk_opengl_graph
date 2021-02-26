#include <gtk/gtk.h>

#include "ui.h"
#include "graph.h"

int main (int argc, char **argv) {
	gtk_init(&argc, &argv);

	ui_init();

	gtk_main();

}
