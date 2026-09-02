/**
 * @file example_c.c
 * @brief C Example using macOS Style Window
 * @author Mohammed Al-Baqer
 */

#include "macos_window.h"

void OnDraw(HDC hdc, int width, int height, void* userData) { /* Draw callback */
    /* Draw your content here */
    RECT rect = {0, 0, width, height};
    HBRUSH brush = CreateSolidBrush(RGB(240, 240, 240));
    FillRect(hdc, &rect, brush);
    DeleteObject(brush);
    
    /* Draw text */
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(50, 50, 50));
    
    HFONT font = CreateFontW(
        20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    );
    
    HGDIOBJ oldFont = SelectObject(hdc, font);
    RECT textRect = {0, height / 2 - 20, width, height / 2 + 20};
    DrawTextW(hdc, L"Hello from C!", -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, oldFont);
    DeleteObject(font);
}

int main(void)
{
    /* Initialize library */
    if (!MacOSWindow_Init()) {
        return -1;
    }
    
    /* Configure window */
    MacOSWindowConfig config;
    memset(&config, 0, sizeof(config));
    config.title = L"C macOS Style App";
    config.width = 600;
    config.height = 400;
    config.resizable = TRUE;
    config.centered = TRUE;
    
    /* Create window */
    MacOSWindow* window = MacOSWindow_Create(&config);
    
    if (!window) {
        MacOSWindow_Cleanup();
        return -1;
    }
    
    MacOSWindow_SetDrawCallback(window, OnDraw); /* Set draw callback */
    MacOSWindow_Show(window);                   /* Show window */
    int result = MacOSWindow_Run(window);      /* Run message loop */
    MacOSWindow_Destroy(window);              /* Cleanup */
    MacOSWindow_Cleanup();
    return result;
}