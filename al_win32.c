#include <stdio.h>
#include <windows.h>

#include "al.h"
#include "util.h"

/* Variables */
static const char classname[] = "Main Window";
HWND hwnd;
int minimised;
int quitting = 0;

static LRESULT CALLBACK
windowproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
    case WM_DESTROY:
        /* Request to quit */
	quitting = 1;
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

int
al_createwindow(const char *title, unsigned int width, unsigned int height)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASS wc = {
	/* Allocate unique device context for OpenGL */
        .style         = CS_OWNDC,
        .lpfnWndProc   = windowproc,
        .cbClsExtra    = 0,
        .cbWndExtra    = 0,
        .hInstance     = hInstance,
        .hIcon         = NULL,
        .hCursor       = NULL,
        .hbrBackground = NULL,
        .lpszMenuName  = NULL,
        .lpszClassName = classname
    };

    RegisterClass(&wc);
    hwnd = CreateWindowEx(0, classname, title,
	    (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX),
	    CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL,
            hInstance, NULL);

    if (hwnd == NULL) {
	return 0;
    } else {
	/* We're not using WinMain so we don't get the default nCmdShow */
	ShowWindow(hwnd, SW_SHOWDEFAULT);
	return 1;
    }
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
		terminate("Windows message error.\n");
	    else
		onmessage(&msg);
	running = 0;
    /* If the window has been minimised wait for WM_SIZE or WM_DESTROY */
    } else if (minimised) {
	while (minimised && !quitting &&
		(result = GetMessage(&msg, NULL, 0, 0)) != 0)
	    if (result == -1 )
		terminate("Windows message error.\n");
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
