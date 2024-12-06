#define GLAD_GL_IMPLEMENTATION
#include "glad.h"
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
static char *createshadercode(const char *filename, size_t *size);
static void deleteshadercode(char **code);
static int loadshaders(void);
static void drawframe(void);

/* Variables */
static const char readonlybinary[] = "rb";
static GLFWwindow *window;

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

char *
createshadercode(const char *filename, size_t *size)
{
    FILE *fp;
    char *code;

    if ((fp = fopen(filename, readonlybinary)) == NULL)
        term(EXIT_FAILURE, "Could not open file %s.\n", filename);

    if (fseek(fp, 0L, SEEK_END) != 0)
        term(EXIT_FAILURE, "Error on seeking file %s.\n", filename);
    *size = ftell(fp);
    rewind(fp);
    code = (char *) malloc(*size * sizeof(char));
    if (fread(code, sizeof(char), *size, fp) < *size)
        term(EXIT_FAILURE, "Error reading file %s.\n", filename);

    if (fclose(fp) == EOF)
        term(EXIT_FAILURE, "Error on closing file %s.\n", filename);

    return code;
}

void
deleteshadercode(char **code)
{
    free(*code);
}

int
loadshaders(void)
{
    size_t vertexcodesize, fragmentcodesize;
    char *vertexcode   = createshadercode(vertexspirv, &vertexcodesize);
    char *fragmentcode = createshadercode(fragmentspirv, &fragmentcodesize);
    GLuint vertexshader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
    GLint iscompiled, maxlength;
    GLchar *log;

    /* TODO: write createshadermodule */
    glShaderBinary(1, &vertexshader, GL_SHADER_BINARY_FORMAT_SPIR_V,
	    (const void *) vertexcode, vertexcodesize);
    deleteshadercode(&vertexcode);
    glSpecializeShader(vertexshader, (const GLchar*) shaderentry, 0, NULL,
	    NULL);
    glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &iscompiled);
    if (!iscompiled) {
	glGetShaderiv(vertexshader, GL_INFO_LOG_LENGTH, &maxlength);
	if (maxlength) {
	    log = (GLchar *) malloc(maxlength * sizeof(GLchar));
	    glGetShaderInfoLog(vertexshader, maxlength, &maxlength, &log[0]);
	    fprintf(stderr, "%s\n", (char *) log);
	    free(log);
	    return 0;
	}
    }

    glShaderBinary(1, &fragmentshader, GL_SHADER_BINARY_FORMAT_SPIR_V,
	    (const void *) fragmentcode, fragmentcodesize);
    deleteshadercode(&fragmentcode);
    glSpecializeShader(fragmentshader, (const GLchar*) shaderentry, 0, NULL,
	    NULL);
    glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &iscompiled);
    if (!iscompiled) {
	glGetShaderiv(fragmentshader, GL_INFO_LOG_LENGTH, &maxlength);
	if (maxlength) {
	    log = (GLchar *) malloc(maxlength * sizeof(GLchar));
	    glGetShaderInfoLog(fragmentshader, maxlength, &maxlength, &log[0]);
	    fprintf(stderr, "%s\n", (char *) log);
	    free(log);
	    return 0;
	}
    }

    return 1;
}

void
drawframe(void)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
}

int
main(void)
{
    init();
    createwindow();
    if (!loadshaders())
	term(EXIT_FAILURE, "Failed to load shaders.\n");

    while (!glfwWindowShouldClose(window)) {
	drawframe();
	glfwPollEvents();
    }

    term(EXIT_SUCCESS, NULL);
}

