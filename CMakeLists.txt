cmake_minimum_required (VERSION 3.2)

project (graph_opengl LANGUAGES C)

find_package(PkgConfig)
find_package(OpenGL REQUIRED)

pkg_check_modules(GTK REQUIRED IMPORTED_TARGET gtk+-3.0)
pkg_check_modules(EPOXY REQUIRED IMPORTED_TARGET epoxy)

set(
	SRC
	src/main.c
	src/ui.h	src/ui.c
	src/graph.h src/graph.c
)

add_executable(graph_opengl ${SRC})

target_link_libraries(graph_opengl m)
target_link_libraries(graph_opengl ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
target_link_libraries(graph_opengl PkgConfig::GTK)
target_link_libraries(graph_opengl PkgConfig::EPOXY)

target_compile_options(graph_opengl PUBLIC -Wall -DGL_GLEXT_PROTOTYPES) 
set_property(TARGET graph_opengl PROPERTY CMAKE_C_STANDARD 11)

