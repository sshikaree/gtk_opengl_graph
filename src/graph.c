#include <stdio.h>
#include <math.h>

#include <glib.h>
#include <epoxy/gl.h>

#include "graph.h"
#include "ui.h"


static GLuint vbo = 0;
static GLuint vao = 0;
static GLuint program = 0;

static GLint attribute_coord2d = 0;
static GLint uniform_offset_x = 0;
static GLint uniform_scale_x = 0;

static float offset_x = 0.0;
//static float offcet_y = 1.0;
static float scale_x = 1.0;


/* Utils */
static GLint get_attrib(GLuint program, const char *name) {
	GLint attribute = glGetAttribLocation(program, name);
	if(attribute == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", name);
	}
	return attribute;
}

static GLint get_uniform(GLuint program, const char *name) {
	GLint uniform = glGetUniformLocation(program, name);
	if(uniform == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", name);
	}
	return uniform;
}


/* Initialize the GL buffers */
static void init_buffers() {
	/* VBO */
	// Create the vertex buffer object
	glGenBuffers (1, &vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);

	// Create our own temporary buffer
	Point graph[2000];

	// Fill it in just like an array
	for (int i = 0; i < 2000; ++i) {
		float x = (i - 1000.0f) / 300.0f;
		graph[i].x = x;
//		graph[i].y = (float)sin((double)x * 10.0) / (1.0f + x * x);
		// y will be calculated inside of vertex shader
		graph[i].y = 0.7f * (float)sin((double)x * 300.0);
	}

	// Tell OpenGL to copy our array to the buffer object
	glBufferData(GL_ARRAY_BUFFER, 2000 * sizeof(Point), graph, GL_STATIC_DRAW);

	/* VAO */
	/* This is mandatory for GL 3.x core profile */
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
}

/* Create and compile a shader */
static GLuint create_shader(int type, const char *src) {
	GLuint shader;
	int status;

	shader = glCreateShader (type);
	glShaderSource (shader, 1, &src, NULL);
	glCompileShader (shader);

	glGetShaderiv (shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)	{
		int log_len;
		char *buffer;

		glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &log_len);

		buffer = g_malloc (log_len + 1);
		glGetShaderInfoLog (shader, log_len, NULL, buffer);

		g_warning ("Compile failure in %s shader:\n%s",
				   type == GL_VERTEX_SHADER ? "vertex" : "fragment",
				   buffer);

		g_free (buffer);

		glDeleteShader (shader);

		return 0;
	}

	return shader;
}

void on_realize(GtkGLArea* gl_area) {
	gtk_gl_area_make_current(gl_area);

	// Print version info:
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	const GLubyte* glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);
	g_print("Renderer: %s\n", renderer);
	g_print("OpenGL version supported %s\n", version);
	g_print("GLSL version supported %s\n", glsl_version);

	if (gtk_gl_area_get_error(gl_area) != NULL)  {
		return;
	}

	// Enable depth buffer:
	gtk_gl_area_set_has_depth_buffer(gl_area, TRUE);
	// Enable alpha:
//	gtk_gl_area_set_has_alpha(gl_area, TRUE);

	/* Init buffers */
	init_buffers();

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

	/* Enable blending */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	/* Create shaders */
	GLuint vshader = create_shader(
				GL_VERTEX_SHADER,
				"#version 130\n"

				"in vec2 coord2d;"
				"out vec4 f_color;"
				"uniform float offset_x;"
				"uniform float scale_x;"

				"void main(void) {"
				"	gl_Position = vec4((coord2d.x + offset_x) * scale_x, 0.7 * sin(coord2d.x * 300.0), 0, 1);"
				"	f_color = vec4(coord2d.xy / 2.0 + 0.5, 1, 1);"
				"}"
	);
	GLuint fshader = create_shader(
				GL_FRAGMENT_SHADER,
				"#version 130\n"

				"in vec4 f_color;"

				"void main(void) {"
				"	gl_FragColor = f_color;"
//				"	gl_FragColor = vec4(0.5, 0.0, 0.5, 1.0);"
				"}"
	);
	program = glCreateProgram();
	glAttachShader(program, fshader);
	glAttachShader(program, vshader);
	glLinkProgram(program);

	GLint link_ok = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		glDeleteProgram(program);
		g_error("glLinkProgram error\n");
		return;
	}


	attribute_coord2d	= get_attrib(program, "coord2d");
	uniform_offset_x	= get_uniform(program, "offset_x");
	uniform_scale_x		= get_uniform(program, "scale_x");

	if (
			attribute_coord2d == -1
			|| uniform_offset_x == -1
			|| uniform_scale_x == -1
	) {
		// TODO:
		//	Should handle errors here instead of util functions ??
		return;
	}

	// Get frame clock:
	GdkGLContext*	glcontext = gtk_gl_area_get_context(gl_area);
	GdkWindow*		glwindow = gdk_gl_context_get_window(glcontext);
	GdkFrameClock*	frame_clock = gdk_window_get_frame_clock(glwindow);

	// Connect update signal:
	g_signal_connect_swapped(
				frame_clock,
				"update",
				G_CALLBACK(gtk_gl_area_queue_render),
				gl_area
				);

	// Start updating:
	gdk_frame_clock_begin_updating(frame_clock);

	g_print("OK\n");
}

gboolean on_render(GtkGLArea* gl_area, GdkGLContext* gl_context) {
//	int window_width	= gtk_widget_get_allocated_width(GTK_WIDGET(ui->gl_area));
//	int window_height	= gtk_widget_get_allocated_height(GTK_WIDGET(ui->gl_area));

	glUseProgram(program);
	glUniform1f(uniform_offset_x, offset_x);
	glUniform1f(uniform_scale_x, scale_x);

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*
	 * ==============
	 * Draw the graph
	 * ==============
	*/
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glEnableVertexAttribArray(attribute_coord2d);
	glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	/* Draw with lines */
	glDrawArrays(GL_LINE_STRIP, 0, 2000);


	// And we are done.
	// Clear up
//	glDisableVertexAttribArray(attribute_coord2d);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Don't propagate signal
	return TRUE;
}

void on_resize(GtkGLArea* gl_area, int width, int height) {

}
