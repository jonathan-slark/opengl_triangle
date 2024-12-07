#define GLAD_GL_IMPLEMENTATION
#include "glad.h"
#include <GLFW/glfw3.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"

/* Macros */
#define COUNT(x)  (sizeof(x) / sizeof(x[0]))
#define UNUSED(x) (void) (x)

/* Function prototypes */
static void errorcallback(int err, const char *desc);
static void init(void);
static void term(int status, const char *fmt, ...);
static void keycallback(GLFWwindow *window, int key, int scancode, int action,
	int mods);
static void resizecallback(GLFWwindow* window, int width, int height);
#ifndef NDEBUG
void APIENTRY gldebugoutput(GLenum source, GLenum type, unsigned int id,
	GLenum severity, GLsizei length, const char *message,
	const void *userparam);
#endif /* !NDEBUG */
static void createwindow(void);
static char *createshadercode(const char *filename, size_t *size);
static void deleteshadercode(char **code);
static int loadshaders(void);
static void drawframe(void);

/* Variables */
static const unsigned int ignorelog[] = {
    131185 /* Buffer info */
};
static const float vertices[] = {
     0.5f,  0.5f, 0.0f, /* top right */
     0.5f, -0.5f, 0.0f, /* bottom right */
    -0.5f, -0.5f, 0.0f, /* bottom left */
    -0.5f,  0.5f, 0.0f  /* top left */
};
static const unsigned int indices[] = {
    0, 1, 3, /* first triangle */
    1, 2, 3  /* second triangle */
};
static const unsigned int verticecount = 3;
static const char readonlybinary[] = "rb";
static GLFWwindow *window;
static GLuint program, vbo, vao, ebo;

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

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(program);
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

#ifndef NDEBUG

int
ismember(const unsigned int array[], size_t size, unsigned int value)
{
    size_t i;

    for (i = 0; i < size; i++)
	if (array[i] == value)
	    return 1;

    return 0;
}

void APIENTRY gldebugoutput(GLenum source, GLenum type, unsigned int id,
	GLenum severity, GLsizei length, const char *message,
	const void *userparam)
{
    UNUSED(source);
    UNUSED(type);
    UNUSED(severity);
    UNUSED(length);
    UNUSED(userparam);

    if (ismember(ignorelog, sizeof(ignorelog), id))
	return;

    fprintf(stderr, "%u: %s\n", id, message);
}
#endif /* !NDEBUG */

void
createwindow(void)
{
    int version, flags;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, openglmajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, openglminor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif /* !NDEBUG */
    if (!(window = glfwCreateWindow(width, height, title, NULL, NULL)))
	term(EXIT_FAILURE, NULL);

    glfwMakeContextCurrent(window);

    if (!(version = gladLoadGL(glfwGetProcAddress)))
	term(EXIT_FAILURE, "Failed to load OpenGL.\n");
#ifndef NDEBUG
    fprintf(stderr, "Loaded OpenGL %d.%d.\n", GLAD_VERSION_MAJOR(version),
	    GLAD_VERSION_MINOR(version));
#endif /* !NDEBUG */

    glfwSetKeyCallback(window, keycallback);
    glfwSetFramebufferSizeCallback(window, resizecallback);
    glfwSwapInterval(1);

#ifndef NDEBUG
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(gldebugoutput, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
		GL_TRUE);
    }
#endif /* !NDEBUG */
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

GLuint
createshaderbin(GLenum type, const char *code, size_t size)
{
    GLuint shader;
    GLint iscompiled, maxlength;
    GLchar *log;

    shader = glCreateShader(type);
    /* Requires OpenGL 4.6 */
    glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V,
	    (const void *) code, size);
    glSpecializeShader(shader, (const GLchar*) shaderentry, 0, NULL,
	    NULL);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &iscompiled);
    if (!iscompiled) {
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxlength);
	if (maxlength) {
	    log = (GLchar *) malloc(maxlength * sizeof(GLchar));
	    glGetShaderInfoLog(shader, maxlength, &maxlength, &log[0]);
	    fprintf(stderr, (char *) log);
	    free(log);
	    glDeleteShader(shader);
	}
	return 0;
    }

    return shader;
}

int
loadshaders(void)
{
    size_t vertexcodesize, fragmentcodesize;
    char *vertexcode, *fragmentcode;
    GLuint vertexshader, fragmentshader;
    GLint islinked, maxlength;
    GLchar *log;
    
    vertexcode = createshadercode(vertexspirv, &vertexcodesize);
    fragmentcode = createshadercode(fragmentspirv, &fragmentcodesize);
    vertexshader = createshaderbin(GL_VERTEX_SHADER, vertexcode,
	    vertexcodesize);
    fragmentshader = createshaderbin(GL_FRAGMENT_SHADER, fragmentcode,
	    fragmentcodesize);
    deleteshadercode(&vertexcode);
    deleteshadercode(&fragmentcode);

    if (!vertexshader || !fragmentshader)
	return 0;

    program = glCreateProgram();
    glAttachShader(program, vertexshader);
    glAttachShader(program, fragmentshader);
    glLinkProgram(program);
    glDeleteShader(vertexshader);
    glDeleteShader(fragmentshader);

    glGetProgramiv(program, GL_LINK_STATUS, (int *) &islinked);
    if (!islinked) {
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxlength);
	if (maxlength) {
	    log = (GLchar *) malloc(maxlength * sizeof(GLchar));
	    glGetProgramInfoLog(program, maxlength, &maxlength, &log[0]);
	    fprintf(stderr, (char *) log);
	    free(log);
	}
	glDeleteProgram(program);
	return 0;
    }

    return 1;
}

void
loadvertices(void)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);  
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
	    GL_STATIC_DRAW);

    glVertexAttribPointer(0, verticecount, GL_FLOAT, GL_FALSE,
	    verticecount * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    /* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); */
}

void
drawframe(void)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, COUNT(indices), GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
}

int
main(void)
{
    init();
    createwindow();
    if (!loadshaders())
	term(EXIT_FAILURE, "Failed to load shaders.\n");
    loadvertices();

    while (!glfwWindowShouldClose(window)) {
	drawframe();
	glfwPollEvents();
    }

    term(EXIT_SUCCESS, NULL);
}

