#define GLAD_GL_IMPLEMENTATION
#include "glad.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"

/* Macros */
#define UNUSED(x) (void) (x)

/* Function prototypes */
static void errorcallback(int err, const char *desc);
static void init(void);
static void term(int status, const char *fmt, ...);
static void keycallback(GLFWwindow *window, int key, int scancode, int action,
	int mods);
static void resizecallback(GLFWwindow* window, int width, int height);
static void createwindow(void);

/* Variables */
static GLFWwindow *window;
static const unsigned int openglmajor = 3;
static const unsigned int openglminor = 3;

/* Function implementations */

void
errorcallback(int err, const char *desc)
{
    UNUSED(err);

    fprintf(stderr, "GLFW: %s\n", desc);
}

void
init(void)
{
    glfwSetErrorCallback(errorcallback);

    if (!glfwInit())
	exit(EXIT_FAILURE);
}

void
term(int status, const char *fmt, ...)
{
    va_list ap;

    if (window)
	glfwDestroyWindow(window);

    glfwTerminate();

    if (fmt) {
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
    }

    exit(status);
}

void
keycallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    UNUSED(scancode);
    UNUSED(mods);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void
resizecallback(GLFWwindow* window, int width, int height)
{
    UNUSED(window);

    glViewport(0, 0, width, height);
}

void
createwindow(void)
{
    int version;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, openglmajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, openglminor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    if (!(window = glfwCreateWindow(width, height, title, NULL, NULL)))
	term(EXIT_FAILURE, NULL);

    glfwMakeContextCurrent(window);

    if (!(version = gladLoadGL(glfwGetProcAddress)))
	term(EXIT_FAILURE, "Failed to load OpenGL.\n");
#ifndef NDEBUG
    fprintf(stderr, "Loaded OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version),
	    GLAD_VERSION_MINOR(version));
#endif
    glfwSetKeyCallback(window, keycallback);
    glfwSetFramebufferSizeCallback(window, resizecallback);
    glfwSwapInterval(1);
}

void
drawframe(void)
{
    double time = glfwGetTime();
    UNUSED(time);

    glfwSwapBuffers(window);
}

int
main(void)
{
    init();
    createwindow();

    while (!glfwWindowShouldClose(window)) {
	drawframe();
	glfwPollEvents();
    }

    term(EXIT_SUCCESS, NULL);
}

