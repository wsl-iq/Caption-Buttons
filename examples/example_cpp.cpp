/**
 * @file example_cpp.cpp
 * @brief C++ Example using macOS Style Window
 * @author Mohammed Al-Baqer
 */

#include "macos_window.h"
#include <windows.h>

/* Draw callback */
void OnDraw(HDC hdc, int width, int height, void* userData)
{
    /* Draw your content here */
    RECT rect = {0, 0, width, height};
    HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(hdc, &rect, brush);
    DeleteObject(brush);
    
    /* Draw text */
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(100, 100, 100));
    
    HFONT font = CreateFontW(
        24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    );
    
    HGDIOBJ oldFont = SelectObject(hdc, font);
    
    RECT textRect = {0, height / 2 - 20, width, height / 2 + 20};
    DrawTextW(hdc, L"Hello from macOS Window!", -1, &textRect, 
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
    SelectObject(hdc, oldFont);
    DeleteObject(font);
}

/* Close callback */
void OnClose(void* userData)
{
    /* Cleanup before close */
}

int main()
{
    /* Initialize library */
    if (!MacOSWindow_Init()) {
        return -1;
    }
    
    /* Configure window */
    MacOSWindowConfig config = {};
    config.title = L"My macOS Style App";
    config.width = 800;
    config.height = 500;
    config.resizable = TRUE;
    config.centered = TRUE;
    config.backgroundColor = RGB(255, 255, 255);
    
    /* Create window */
    MacOSWindow* window = MacOSWindow_Create(&config);
    
    if (!window) {
        MacOSWindow_Cleanup();
        return -1;
    }
    
    /* Set callbacks */
    MacOSWindow_SetDrawCallback(window, OnDraw);
    MacOSWindow_SetCloseCallback(window, OnClose);
    
    /* Show window */
    MacOSWindow_Show(window);
    
    /* Run message loop */
    int result = MacOSWindow_Run(window);
    
    /* Cleanup */
    MacOSWindow_Destroy(window);
    MacOSWindow_Cleanup();
    
    return result;
}