/**
 * @file macos_window.h
 * @brief macOS Style Window Library for Windows
 * @author Mohammed Al-Baqer
 * @copyright Copyright © 2026 Mohammed Al-Baqer. All rights reserved.
 * 
 * This library provides a frameless window with macOS-style traffic light
 * buttons (red, yellow, green) for Windows applications.
 * 
 * Supports both C and C++.
 */

#ifndef MACOS_WINDOW_H
#define MACOS_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

/* Window configuration structure */
typedef struct {
    const wchar_t* title;           /* Window title */
    int width;                      /* Window width */
    int height;                     /* Window height */
    COLORREF backgroundColor;       /* Content background color */
    BOOL resizable;                 /* Allow window resizing */
    BOOL centered;                  /* Center window on screen */
} MacOSWindowConfig;

/* Callback function types */
typedef void (*MacOSWindowDrawCallback)(HDC hdc, int width, int height, void* userData);
typedef void (*MacOSWindowCloseCallback)(void* userData);
typedef void (*MacOSWindowResizeCallback)(int width, int height, void* userData);

/* Window handle structure */
typedef struct {
    HWND hwnd;                                          /* Windows handle */
    MacOSWindowConfig config;                           /* Configuration */
    MacOSWindowDrawCallback drawCallback;               /* Draw callback */
    MacOSWindowCloseCallback closeCallback;             /* Close callback */
    MacOSWindowResizeCallback resizeCallback;           /* Resize callback */
    void* userData;                                     /* User data */
    BOOL closeHover;                                    /* Internal: close button hover */
    BOOL minimizeHover;                                 /* Internal: minimize button hover */
    BOOL maximizeHover;                                 /* Internal: maximize button hover */
} MacOSWindow;

/* Initialize the library (call once at startup) */
BOOL MacOSWindow_Init(void);

/* Cleanup the library (call once at shutdown) */
void MacOSWindow_Cleanup(void);

/* Create a new macOS style window */
MacOSWindow* MacOSWindow_Create(const MacOSWindowConfig* config);

/* Destroy a macOS style window */
void MacOSWindow_Destroy(MacOSWindow* window);

/* Show the window */
void MacOSWindow_Show(MacOSWindow* window);

/* Hide the window */
void MacOSWindow_Hide(MacOSWindow* window);

/* Set draw callback */
void MacOSWindow_SetDrawCallback(MacOSWindow* window, MacOSWindowDrawCallback callback);

/* Set close callback */
void MacOSWindow_SetCloseCallback(MacOSWindow* window, MacOSWindowCloseCallback callback);

/* Set resize callback */
void MacOSWindow_SetResizeCallback(MacOSWindow* window, MacOSWindowResizeCallback callback);

/* Set user data */
void MacOSWindow_SetUserData(MacOSWindow* window, void* userData);

/* Get user data */
void* MacOSWindow_GetUserData(MacOSWindow* window);

/* Get window handle */
HWND MacOSWindow_GetHWND(MacOSWindow* window);

/* Force redraw */
void MacOSWindow_Redraw(MacOSWindow* window);

/* Run message loop (blocks until window closes) */
int MacOSWindow_Run(MacOSWindow* window);

#ifdef __cplusplus
}
#endif

#endif /* MACOS_WINDOW_H */