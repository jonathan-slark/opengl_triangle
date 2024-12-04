/*
 * Sample code by Nick Rolfe used as a reference:
 * https://gist.github.com/nickrolfe/1127313ed1dbf80254b614a721b3ee9c
 */

#include <stdio.h>
#include <GL/gl.h>
#include <windows.h>

#include "al.h"
#include "util.h"

/* OpenGL extensions */

typedef HGLRC WINAPI wglCreateContextAttribsARB_t(HDC hDC, HGLRC hShareContext,
        const int *attribList);
static wglCreateContextAttribsARB_t *wglCreateContextAttribsARB;
#define WGL_CONTEXT_MAJOR_VERSION_ARB    0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB    0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB     0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001

typedef BOOL WINAPI wglChoosePixelFormatARB_t(HDC hdc,
	const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats,
	int *piFormats, UINT *nNumFormats);
static wglChoosePixelFormatARB_t *wglChoosePixelFormatARB;
#define WGL_DRAW_TO_WINDOW_ARB    0x2001
#define WGL_ACCELERATION_ARB      0x2003
#define WGL_SUPPORT_OPENGL_ARB    0x2010
#define WGL_DOUBLE_BUFFER_ARB     0x2011
#define WGL_PIXEL_TYPE_ARB        0x2013
#define WGL_COLOR_BITS_ARB        0x2014
#define WGL_DEPTH_BITS_ARB        0x2022
#define WGL_STENCIL_BITS_ARB      0x2023
#define WGL_FULL_ACCELERATION_ARB 0x2027
#define WGL_TYPE_RGBA_ARB         0x202B

/* Variables */
static const char classname[] = "Main Window";
static const DWORD windowstyle = WS_OVERLAPPEDWINDOW;
static HWND hwnd;
static int minimised;
static int quitting = 0;
static HGLRC hrc;
static HDC hdc;

/* Function implementations */

void
enableexts(void)
{
    /* Disable gcc warning as this is the way wglGetProcAddress works */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
    wglCreateContextAttribsARB = (wglCreateContextAttribsARB_t *)
	wglGetProcAddress("wglCreateContextAttribsARB");
    wglChoosePixelFormatARB = (wglChoosePixelFormatARB_t *)
	wglGetProcAddress("wglChoosePixelFormatARB");
#pragma GCC diagnostic pop
}

void
createcontext(void)
{
    enableexts();
}

void
deletecontext(void)
{
    wglDeleteContext(hrc);
    ReleaseDC(hwnd, hdc);
}

static LRESULT CALLBACK
windowproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
    case WM_CREATE:
	createcontext();
	return 0;
    case WM_DESTROY:
        /* Request to quit */
	quitting = 1;
	deletecontext();
        PostQuitMessage(0);
        return 0;
    case WM_SIZE:
        switch(wparam) {
        case SIZE_MINIMIZED:
            minimised = 1;
            break;
        default:
            minimised = 0;
            break;
        }
        return 0;
    }

    /* If we don't handle the message, use the default handler */
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

void
al_createwindow(const char *title, unsigned int width, unsigned int height)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    RECT rect = {
	.left = 0,
	.top = 0,
	.right = width,
	.bottom = height
    };
    WNDCLASS wc = {
	/* Allocate unique device context for OpenGL */
        .style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
        .lpfnWndProc   = windowproc,
        .cbClsExtra    = 0,
        .cbWndExtra    = 0,
        .hInstance     = hInstance,
        .hIcon         = NULL,
        .hCursor       = LoadCursor(0, IDC_ARROW),
        .hbrBackground = NULL,
        .lpszMenuName  = NULL,
        .lpszClassName = classname
    };

    if (!RegisterClass(&wc))
	terminate("Unable to register class.");

    /* Adjust window rect so client area is size we requested */
    if (!AdjustWindowRect(&rect, windowstyle, FALSE))
	terminate("Unable to adjust window rect.");

    hwnd = CreateWindowEx(0, classname, title, windowstyle, CW_USEDEFAULT,
	    CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
	    NULL, NULL, hInstance, NULL);
    if (!hwnd)
	terminate("Unable to create window.");

    /* We're not using WinMain so we don't get the default nCmdShow */
    ShowWindow(hwnd, SW_SHOWDEFAULT);

    if (!UpdateWindow(hwnd))
	terminate("Unable to update window.");
}

void
onmessage(MSG *msg)
{
    TranslateMessage(msg);
    DispatchMessage(msg);
}

int
al_pollevents(void)
{
    int result;
    MSG msg;
    int running = 1;

    /* If quitting then wait for WM_QUIT */
    if (quitting) {
	while ((result = GetMessage(&msg, NULL, 0, 0)) != 0)
	    if (result == -1 )
		terminate("Windows message error.");
	    else
		onmessage(&msg);
	running = 0;
    /* If the window has been minimised wait for WM_SIZE or WM_DESTROY */
    } else if (minimised) {
	while (minimised && !quitting &&
		(result = GetMessage(&msg, NULL, 0, 0)) != 0)
	    if (result == -1 )
		terminate("Windows message error.");
	    else
		onmessage(&msg);

	/* WM_QUIT may have already been processed */
	if (msg.message == WM_QUIT)
	    running = 0;
    /* Normally we don't block */
    } else if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
	onmessage(&msg);
    }

    return running;
}

void
al_drawframe(void)
{
    glClearColor(1.0f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (!SwapBuffers(hdc))
	terminate("Unable to swap buffers.");
}
