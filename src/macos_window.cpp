/**
 * @file macos_window.cpp
 * @brief macOS Style Window Library Implementation
 * @author Mohammed Al-Baqer
 * @copyright Copyright © 2026 Mohammed Al-Baqer. All rights reserved.
 */

#include "macos_window.h"
#include <windowsx.h>
#include <gdiplus.h>
#include <dwmapi.h>
#include <map>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "dwmapi.lib")

using namespace Gdiplus;

/* Constants */
#define MACOS_TITLEBAR_HEIGHT 40
#define MACOS_BUTTON_SIZE 12
#define MACOS_BUTTON_SPACING 8
#define MACOS_BUTTON_MARGIN_LEFT 12
#define MACOS_BUTTON_MARGIN_TOP ((MACOS_TITLEBAR_HEIGHT - MACOS_BUTTON_SIZE) / 2)

/* Button colors */
static const Color MACOS_CLOSE_COLOR(255, 255, 95, 86);
static const Color MACOS_MINIMIZE_COLOR(255, 255, 189, 46);
static const Color MACOS_MAXIMIZE_COLOR(255, 39, 201, 63);

/* Global state */
static ULONG_PTR g_gdiplusToken = 0;
static std::map<HWND, MacOSWindow*> g_windowMap;
static const wchar_t* g_windowClassName = L"MacOSStyleWindowClass";

/* Forward declarations */
static LRESULT CALLBACK MacOSWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/* Initialize library */
BOOL MacOSWindow_Init(void)
{
    GdiplusStartupInput input;
    return GdiplusStartup(&g_gdiplusToken, &input, NULL) == Ok;
}

/* Cleanup library */
void MacOSWindow_Cleanup(void)
{
    if (g_gdiplusToken) {
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}

/* Draw circular button */
static void DrawCircularButton(Graphics& graphics, int x, int y, 
                                Color color, BOOL hover, const wchar_t* symbol)
{
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);
    
    if (hover) {
        SolidBrush shadowBrush(Color(30, 0, 0, 0));
        graphics.FillEllipse(&shadowBrush, x - 1, y + 1, 
                            MACOS_BUTTON_SIZE + 2, MACOS_BUTTON_SIZE + 2);
    }
    
    SolidBrush brush(color);
    graphics.FillEllipse(&brush, x, y, MACOS_BUTTON_SIZE, MACOS_BUTTON_SIZE);
    
    if (hover) {
        FontFamily fontFamily(L"Segoe UI");
        Font font(&fontFamily, 8, FontStyleBold, UnitPixel);
        
        StringFormat stringFormat;
        stringFormat.SetAlignment(StringAlignmentCenter);
        stringFormat.SetLineAlignment(StringAlignmentCenter);
        
        SolidBrush textBrush(Color(255, 255, 255, 255));
        RectF textRect((REAL)x, (REAL)y, (REAL)MACOS_BUTTON_SIZE, (REAL)MACOS_BUTTON_SIZE);
        graphics.DrawString(symbol, -1, &font, textRect, &stringFormat, &textBrush);
    }
}

/* Draw title bar */
static void DrawTitleBar(Graphics& graphics, MacOSWindow* window, int width)
{
    LinearGradientBrush gradientBrush(
        Point(0, 0), Point(0, MACOS_TITLEBAR_HEIGHT),
        Color(255, 246, 246, 246),
        Color(255, 224, 224, 224)
    );
    graphics.FillRectangle(&gradientBrush, 0, 0, width, MACOS_TITLEBAR_HEIGHT);
    
    Pen borderPen(Color(255, 180, 180, 180), 1.0f);
    graphics.DrawLine(&borderPen, 0, MACOS_TITLEBAR_HEIGHT - 1, 
                      width, MACOS_TITLEBAR_HEIGHT - 1);
    
    int closeX = MACOS_BUTTON_MARGIN_LEFT;
    int minimizeX = closeX + MACOS_BUTTON_SIZE + MACOS_BUTTON_SPACING;
    int maximizeX = minimizeX + MACOS_BUTTON_SIZE + MACOS_BUTTON_SPACING;
    
    DrawCircularButton(graphics, closeX, MACOS_BUTTON_MARGIN_TOP, 
                      MACOS_CLOSE_COLOR, window->closeHover, L"×");
    DrawCircularButton(graphics, minimizeX, MACOS_BUTTON_MARGIN_TOP, 
                      MACOS_MINIMIZE_COLOR, window->minimizeHover, L"—");
    DrawCircularButton(graphics, maximizeX, MACOS_BUTTON_MARGIN_TOP, 
                      MACOS_MAXIMIZE_COLOR, window->maximizeHover, L"+");
    
    if (window->config.title) {
        FontFamily fontFamily(L"Segoe UI");
        Font font(&fontFamily, 13, FontStyleRegular, UnitPixel);
        
        StringFormat stringFormat;
        stringFormat.SetAlignment(StringAlignmentCenter);
        stringFormat.SetLineAlignment(StringAlignmentCenter);
        
        SolidBrush textBrush(Color(255, 60, 60, 60));
        RectF titleRect(0, 0, (REAL)width, (REAL)MACOS_TITLEBAR_HEIGHT);
        graphics.DrawString(window->config.title, -1, &font, 
                           titleRect, &stringFormat, &textBrush);
    }
}

/* Check if point is in button */
static BOOL IsInButton(POINT pt, int buttonX)
{
    return pt.x >= buttonX - 4 && pt.x <= buttonX + MACOS_BUTTON_SIZE + 4 &&
           pt.y >= MACOS_BUTTON_MARGIN_TOP - 4 && 
           pt.y <= MACOS_BUTTON_MARGIN_TOP + MACOS_BUTTON_SIZE + 4;
}

/* Handle button clicks */
static void HandleButtonClick(MacOSWindow* window, POINT pt)
{
    int closeX = MACOS_BUTTON_MARGIN_LEFT;
    int minimizeX = closeX + MACOS_BUTTON_SIZE + MACOS_BUTTON_SPACING;
    int maximizeX = minimizeX + MACOS_BUTTON_SIZE + MACOS_BUTTON_SPACING;
    
    if (IsInButton(pt, closeX)) {
        if (window->closeCallback) {
            window->closeCallback(window->userData);
        }
        DestroyWindow(window->hwnd);
        return;
    }
    
    if (IsInButton(pt, minimizeX)) {
        ShowWindow(window->hwnd, SW_MINIMIZE);
        return;
    }
    
    if (IsInButton(pt, maximizeX)) {
        if (IsZoomed(window->hwnd)) {
            ShowWindow(window->hwnd, SW_RESTORE);
        } else {
            ShowWindow(window->hwnd, SW_MAXIMIZE);
        }
        return;
    }
}

/* Window procedure */
static LRESULT CALLBACK MacOSWindowProc(HWND hwnd, UINT uMsg, 
                                         WPARAM wParam, LPARAM lParam)
{
    MacOSWindow* window = NULL;
    
    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
        window = (MacOSWindow*)cs->lpCreateParams;
        window->hwnd = hwnd;
        g_windowMap[hwnd] = window;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
    } else {
        window = (MacOSWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }
    
    if (!window) {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    
    switch (uMsg) {
        case WM_CREATE: {
            MARGINS margins = {1, 1, 1, 1};
            DwmExtendFrameIntoClientArea(hwnd, &margins);
            SetTimer(hwnd, 1, 16, NULL);
            return 0;
        }
        
        case WM_TIMER: {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hwnd, &pt);
            
            int closeX = MACOS_BUTTON_MARGIN_LEFT;
            int minimizeX = closeX + MACOS_BUTTON_SIZE + MACOS_BUTTON_SPACING;
            int maximizeX = minimizeX + MACOS_BUTTON_SIZE + MACOS_BUTTON_SPACING;
            
            BOOL newCloseHover = IsInButton(pt, closeX);
            BOOL newMinimizeHover = IsInButton(pt, minimizeX);
            BOOL newMaximizeHover = IsInButton(pt, maximizeX);
            
            if (newCloseHover != window->closeHover ||
                newMinimizeHover != window->minimizeHover ||
                newMaximizeHover != window->maximizeHover) {
                window->closeHover = newCloseHover;
                window->minimizeHover = newMinimizeHover;
                window->maximizeHover = newMaximizeHover;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            
            int width = clientRect.right - clientRect.left;
            int height = clientRect.bottom - clientRect.top;
            
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, width, height);
            HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);
            
            Graphics graphics(memDC);
            graphics.SetSmoothingMode(SmoothingModeAntiAlias);
            
            SolidBrush clearBrush(Color(255, 255, 255, 255));
            graphics.FillRectangle(&clearBrush, 0, 0, width, height);
            
            DrawTitleBar(graphics, window, width);
            
            if (window->drawCallback) {
                window->drawCallback(memDC, width, height - MACOS_TITLEBAR_HEIGHT, 
                                    window->userData);
            }
            
            BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);
            
            SelectObject(memDC, oldBitmap);
            DeleteObject(memBitmap);
            DeleteDC(memDC);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_NCHITTEST: {
            POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            ScreenToClient(hwnd, &pt);
            
            int closeX = MACOS_BUTTON_MARGIN_LEFT;
            int minimizeX = closeX + MACOS_BUTTON_SIZE + MACOS_BUTTON_SPACING;
            int maximizeX = minimizeX + MACOS_BUTTON_SIZE + MACOS_BUTTON_SPACING;
            
            if (IsInButton(pt, closeX) || IsInButton(pt, minimizeX) || 
                IsInButton(pt, maximizeX)) {
                return HTCLIENT;
            }
            
            if (pt.y >= 0 && pt.y <= MACOS_TITLEBAR_HEIGHT) {
                return HTCAPTION;
            }
            
            return HTCLIENT;
        }
        
        case WM_LBUTTONDOWN: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            
            int closeX = MACOS_BUTTON_MARGIN_LEFT;
            int minimizeX = closeX + MACOS_BUTTON_SIZE + MACOS_BUTTON_SPACING;
            int maximizeX = minimizeX + MACOS_BUTTON_SIZE + MACOS_BUTTON_SPACING;
            
            if (IsInButton(pt, closeX) || IsInButton(pt, minimizeX) || 
                IsInButton(pt, maximizeX)) {
                HandleButtonClick(window, pt);
                return 0;
            }
            
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        
        case WM_SIZE: {
            if (window->resizeCallback) {
                RECT rect;
                GetClientRect(hwnd, &rect);
                window->resizeCallback(rect.right - rect.left, 
                                      rect.bottom - rect.top - MACOS_TITLEBAR_HEIGHT,
                                      window->userData);
            }
            return 0;
        }
        
        case WM_ERASEBKGND:
            return 1;
        
        case WM_DESTROY: {
            KillTimer(hwnd, 1);
            g_windowMap.erase(hwnd);
            PostQuitMessage(0);
            return 0;
        }
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

/* Create window */
MacOSWindow* MacOSWindow_Create(const MacOSWindowConfig* config)
{
    static BOOL classRegistered = FALSE;
    
    if (!classRegistered) {
        WNDCLASSW wc = {};
        wc.lpfnWndProc = MacOSWindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = g_windowClassName;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = NULL;
        RegisterClassW(&wc);
        classRegistered = TRUE;
    }
    
    MacOSWindow* window = new MacOSWindow();
    ZeroMemory(window, sizeof(MacOSWindow));
    window->config = *config;
    
    DWORD style = WS_POPUP | WS_MINIMIZEBOX;
    if (config->resizable) {
        style |= WS_MAXIMIZEBOX | WS_THICKFRAME;
    }
    
    int x = CW_USEDEFAULT, y = CW_USEDEFAULT;
    if (config->centered) {
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        x = (screenWidth - config->width) / 2;
        y = (screenHeight - config->height) / 2;
    }
    
    HWND hwnd = CreateWindowExW(
        WS_EX_APPWINDOW,
        g_windowClassName,
        config->title ? config->title : L"",
        style,
        x, y,
        config->width, config->height,
        NULL, NULL, GetModuleHandle(NULL), window
    );
    
    if (!hwnd) {
        delete window;
        return NULL;
    }
    
    return window;
}

/* Destroy window */
void MacOSWindow_Destroy(MacOSWindow* window)
{
    if (window) {
        if (window->hwnd) {
            DestroyWindow(window->hwnd);
        }
        delete window;
    }
}

/* Show window */
void MacOSWindow_Show(MacOSWindow* window)
{
    if (window && window->hwnd) {
        ShowWindow(window->hwnd, SW_SHOW);
        UpdateWindow(window->hwnd);
    }
}

/* Hide window */
void MacOSWindow_Hide(MacOSWindow* window)
{
    if (window && window->hwnd) {
        ShowWindow(window->hwnd, SW_HIDE);
    }
}

/* Set draw callback */
void MacOSWindow_SetDrawCallback(MacOSWindow* window, MacOSWindowDrawCallback callback)
{
    if (window) {
        window->drawCallback = callback;
    }
}

/* Set close callback */
void MacOSWindow_SetCloseCallback(MacOSWindow* window, MacOSWindowCloseCallback callback)
{
    if (window) {
        window->closeCallback = callback;
    }
}

/* Set resize callback */
void MacOSWindow_SetResizeCallback(MacOSWindow* window, MacOSWindowResizeCallback callback)
{
    if (window) {
        window->resizeCallback = callback;
    }
}

/* Set user data */
void MacOSWindow_SetUserData(MacOSWindow* window, void* userData)
{
    if (window) {
        window->userData = userData;
    }
}

/* Get user data */
void* MacOSWindow_GetUserData(MacOSWindow* window)
{
    return window ? window->userData : NULL;
}

/* Get window handle */
HWND MacOSWindow_GetHWND(MacOSWindow* window)
{
    return window ? window->hwnd : NULL;
}

/* Force redraw */
void MacOSWindow_Redraw(MacOSWindow* window)
{
    if (window && window->hwnd) {
        InvalidateRect(window->hwnd, NULL, TRUE);
    }
}

/* Run message loop */
int MacOSWindow_Run(MacOSWindow* window)
{
    if (!window) return -1;
    
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (int)msg.wParam;
}