#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <dwmapi.h>
#include <string>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "dwmapi.lib")

using namespace Gdiplus;

const int TITLEBAR_HEIGHT = 40;
const int BUTTON_SIZE = 12;
const int BUTTON_SPACING = 8;
const int BUTTON_MARGIN_LEFT = 12;
const int BUTTON_MARGIN_TOP = (TITLEBAR_HEIGHT - BUTTON_SIZE) / 2;

const Color CLOSE_COLOR(255, 255, 95, 86);
const Color MINIMIZE_COLOR(255, 255, 189, 46);
const Color MAXIMIZE_COLOR(255, 39, 201, 63);

bool g_closeHover = false;
bool g_minimizeHover = false;
bool g_maximizeHover = false;

ULONG_PTR g_gdiplusToken;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void DrawCircularButton(Graphics& g, int x, int y, Color color, bool hover, const wchar_t* symbol)
{
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetPixelOffsetMode(PixelOffsetModeHighQuality);
    
    if (hover) {
        SolidBrush shadow(Color(30, 0, 0, 0));
        g.FillEllipse(&shadow, x - 1, y + 1, BUTTON_SIZE + 2, BUTTON_SIZE + 2);
    }
    
    SolidBrush brush(color);
    g.FillEllipse(&brush, x, y, BUTTON_SIZE, BUTTON_SIZE);
    
    if (hover) {
        FontFamily ff(L"Segoe UI");
        Font font(&ff, 8, FontStyleBold, UnitPixel);
        StringFormat sf;
        sf.SetAlignment(StringAlignmentCenter);
        sf.SetLineAlignment(StringAlignmentCenter);
        SolidBrush text(Color(255, 255, 255, 255));
        RectF rect((REAL)x, (REAL)y, (REAL)BUTTON_SIZE, (REAL)BUTTON_SIZE);
        g.DrawString(symbol, -1, &font, rect, &sf, &text);
    }
}

void DrawTitleBar(Graphics& g, int width)
{
    LinearGradientBrush gradient(
        Point(0, 0), Point(0, TITLEBAR_HEIGHT),
        Color(255, 246, 246, 246),
        Color(255, 224, 224, 224)
    );
    g.FillRectangle(&gradient, 0, 0, width, TITLEBAR_HEIGHT);
    
    Pen border(Color(255, 180, 180, 180), 1.0f);
    g.DrawLine(&border, 0, TITLEBAR_HEIGHT - 1, width, TITLEBAR_HEIGHT - 1);
    
    int cx = BUTTON_MARGIN_LEFT;
    int mx = cx + BUTTON_SIZE + BUTTON_SPACING;
    int xx = mx + BUTTON_SIZE + BUTTON_SPACING;
    
    DrawCircularButton(g, cx, BUTTON_MARGIN_TOP, CLOSE_COLOR, g_closeHover, L"×");
    DrawCircularButton(g, mx, BUTTON_MARGIN_TOP, MINIMIZE_COLOR, g_minimizeHover, L"—");
    DrawCircularButton(g, xx, BUTTON_MARGIN_TOP, MAXIMIZE_COLOR, g_maximizeHover, L"+");
    
    FontFamily ff(L"Segoe UI");
    Font font(&ff, 13, FontStyleRegular, UnitPixel);
    StringFormat sf;
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);
    SolidBrush text(Color(255, 60, 60, 60));
    RectF rect(0, 0, (REAL)width, (REAL)TITLEBAR_HEIGHT);
    g.DrawString(L"My App", -1, &font, rect, &sf, &text);
}

bool IsInButton(POINT pt, int bx)
{
    return pt.x >= bx - 4 && pt.x <= bx + BUTTON_SIZE + 4 &&
           pt.y >= BUTTON_MARGIN_TOP - 4 && pt.y <= BUTTON_MARGIN_TOP + BUTTON_SIZE + 4;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE: {
            MARGINS m = {1, 1, 1, 1};
            DwmExtendFrameIntoClientArea(hwnd, &m);
            SetTimer(hwnd, 1, 16, NULL);
            return 0;
        }
        
        case WM_TIMER: {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hwnd, &pt);
            
            int cx = BUTTON_MARGIN_LEFT;
            int mx = cx + BUTTON_SIZE + BUTTON_SPACING;
            int xx = mx + BUTTON_SIZE + BUTTON_SPACING;
            
            bool nc = IsInButton(pt, cx);
            bool nm = IsInButton(pt, mx);
            bool nx = IsInButton(pt, xx);
            
            if (nc != g_closeHover || nm != g_minimizeHover || nx != g_maximizeHover) {
                g_closeHover = nc;
                g_minimizeHover = nm;
                g_maximizeHover = nx;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT rc;
            GetClientRect(hwnd, &rc);
            int w = rc.right - rc.left;
            int h = rc.bottom - rc.top;
            
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP bmp = CreateCompatibleBitmap(hdc, w, h);
            HBITMAP old = (HBITMAP)SelectObject(memDC, bmp);
            
            Graphics g(memDC);
            g.SetSmoothingMode(SmoothingModeAntiAlias);
            
            SolidBrush bg(Color(255, 255, 255, 255));
            g.FillRectangle(&bg, 0, 0, w, h);
            
            DrawTitleBar(g, w);
            
            // Content
            FontFamily ff(L"Segoe UI");
            Font font(&ff, 20, FontStyleRegular, UnitPixel);
            StringFormat sf;
            sf.SetAlignment(StringAlignmentCenter);
            SolidBrush text(Color(255, 100, 100, 100));
            RectF rect(0, (REAL)(TITLEBAR_HEIGHT + 150), (REAL)w, 40);
            g.DrawString(L"Welcome to macOS Window!", -1, &font, rect, &sf, &text);
            
            BitBlt(hdc, 0, 0, w, h, memDC, 0, 0, SRCCOPY);
            
            SelectObject(memDC, old);
            DeleteObject(bmp);
            DeleteDC(memDC);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_NCHITTEST: {
            POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            ScreenToClient(hwnd, &pt);
            
            int cx = BUTTON_MARGIN_LEFT;
            int mx = cx + BUTTON_SIZE + BUTTON_SPACING;
            int xx = mx + BUTTON_SIZE + BUTTON_SPACING;
            
            if (IsInButton(pt, cx) || IsInButton(pt, mx) || IsInButton(pt, xx))
                return HTCLIENT;
            
            if (pt.y >= 0 && pt.y <= TITLEBAR_HEIGHT)
                return HTCAPTION;
            
            return HTCLIENT;
        }
        
        case WM_LBUTTONDOWN: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            
            int cx = BUTTON_MARGIN_LEFT;
            int mx = cx + BUTTON_SIZE + BUTTON_SPACING;
            int xx = mx + BUTTON_SIZE + BUTTON_SPACING;
            
            if (IsInButton(pt, cx)) {
                DestroyWindow(hwnd);
                return 0;
            }
            if (IsInButton(pt, mx)) {
                ShowWindow(hwnd, SW_MINIMIZE);
                return 0;
            }
            if (IsInButton(pt, xx)) {
                if (IsZoomed(hwnd)) ShowWindow(hwnd, SW_RESTORE);
                else ShowWindow(hwnd, SW_MAXIMIZE);
                return 0;
            }
            
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        
        case WM_ERASEBKGND:
            return 1;
        
        case WM_DESTROY:
            KillTimer(hwnd, 1);
            PostQuitMessage(0);
            return 0;
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    GdiplusStartupInput input;
    GdiplusStartup(&g_gdiplusToken, &input, NULL);
    
    const wchar_t CLASS[] = L"TestMacOSWindow";
    
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    RegisterClassW(&wc);
    
    HWND hwnd = CreateWindowExW(
        WS_EX_APPWINDOW, CLASS, L"My App",
        WS_POPUP | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 500,
        NULL, NULL, hInstance, NULL
    );
    
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    GdiplusShutdown(g_gdiplusToken);
    return 0;
}