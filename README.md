# macOS Window Library for Windows

A lightweight C/C++ library that creates frameless windows with macOS-style traffic light buttons (🔴🟡🟢) on Windows.

## Features

- ✅ macOS-style traffic light buttons (Red = Close, Yellow = Minimize, Green = Maximize)
- ✅ Smooth window dragging (no flickering)
- ✅ Anti-aliased circular buttons
- ✅ Double-buffered rendering
- ✅ Support for both C and C++
- ✅ Simple API
- ✅ Custom draw callbacks
- ✅ CMake build system

## Requirements

- Windows 7 or later
- MinGW-w64 or MSVC
- CMake 3.15+ (optional, for building)

## Quick Start

### Building with CMake

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

---

### **Building with** `g++` **directly**
```cmd
g++ examples/example_cpp.cpp src/macos_window.cpp -Iinclude -o app.exe -lgdi32 -luser32 -lgdiplus -ldwmapi -mwindows
```

---

### **Usage**
- `C++` Example
```cpp
#include "macos_window.h"

void OnDraw(HDC hdc, int width, int height, void* userData) {
    // Draw your content here
}

int main() {
    MacOSWindow_Init();
    
    MacOSWindowConfig config = {};
    config.title = L"My App";
    config.width = 800;
    config.height = 500;
    config.resizable = TRUE;
    config.centered = TRUE;
    
    MacOSWindow* window = MacOSWindow_Create(&config);
    MacOSWindow_SetDrawCallback(window, OnDraw);
    MacOSWindow_Show(window);
    
    int result = MacOSWindow_Run(window);
    
    MacOSWindow_Destroy(window);
    MacOSWindow_Cleanup();
    
    return result;
}
```

- `C` Example

```c
#include "macos_window.h"

void OnDraw(HDC hdc, int width, int height, void* userData) {
    // Draw your content here
}

int main(void) {
    MacOSWindow_Init();
    
    MacOSWindowConfig config;
    memset(&config, 0, sizeof(config));
    config.title = L"My App";
    config.width = 800;
    config.height = 500;
    config.resizable = TRUE;
    config.centered = TRUE;
    
    MacOSWindow* window = MacOSWindow_Create(&config);
    MacOSWindow_SetDrawCallback(window, OnDraw);
    MacOSWindow_Show(window);
    
    int result = MacOSWindow_Run(window);
    
    MacOSWindow_Destroy(window);
    MacOSWindow_Cleanup();
    
    return result;
}
```

---
### **Installation**

### Method 1: CMake

```bash
git clone https://github.com/USERNAME/macos-window.git
cd macos-window
mkdir build && cd build
cmake ..
cmake --build .
```

---

# For C++ projects
```
g++ your_app.cpp src/macos_window.cpp -Iinclude -o app.exe -lgdi32 -luser32 -lgdiplus -ldwmapi -mwindows
```

# For C projects
```
gcc your_app.c src/macos_window.cpp -Iinclude -o app.exe -lgdi32 -luser32 -lgdiplus -ldwmapi -mwindows -lstdc++
```

---


### Badges

markdown
# macOS Window Library for Windows

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-Windows-blue.svg)
![Language](https://img.shields.io/badge/language-C%2FC%2B%2B-orange.svg)
![Version](https://img.shields.io/badge/version-1.0.0-green.svg)

---

### **API Reference**
- *Types*


- `MacOSWindow` - Window handle structure.
- `MacOSWindowConfig` - Configuration structure.
- `MacOSWindowDrawCallback` - Draw callback type.
- `MacOSWindowCloseCallback` - Close callback type.
- `MacOSWindowResizeCallback` - Resize callback type.

---

### **Functions**

| Function                                  | Description            |
|-------------------------------------------|------------------------|
| MacOSWindow_Init()                        | Initialize the library
| MacOSWindow_Cleanup()                     | Cleanup the library
| MacOSWindow_Create(config)                | Create a new window
| MacOSWindow_Destroy(window)               | Destroy a window
| MacOSWindow_Show(window)                  | Show the window
| MacOSWindow_Hide(window)                  | Hide the window
| MacOSWindow_SetDrawCallback(window, cb)   | Set draw callback
| MacOSWindow_SetCloseCallback(window, cb)  | Set close callback
| MacOSWindow_SetResizeCallback(window, cb) | Set resize callback
| MacOSWindow_SetUserData(window, data)     | Set user data
| MacOSWindow_GetUserData(window)           | Get user data
| MacOSWindow_GetHWND(window)               | Get Windows handle
| MacOSWindow_Redraw(window)                | Force redraw
| MacOSWindow_Run(window)                   | Run message loop

---

### **License**
- MIT License - See LICENSE file

---

### **Author**
- Mohammed Al-Baqer

### Contributing
Contributions are welcome! Please feel free to submit a Pull Request.


### **MIT License**

```
Copyright (c) 2026 Mohammed Al-Baqer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## File: `.gitignore`

```gitignore
# Build directories
build/
out/
dist/

# IDE files
.vscode/
.idea/
*.suo
*.user

# Compiled files
*.exe
*.dll
*.o
*.obj
*.a
*.lib

# CMake
CMakeCache.txt
CMakeFiles/
cmake_install.cmake
Makefile
```

---

## FAQ

**Q: Does this work on Windows 10?**
A: Yes, it works on Windows 7, 10, and 11.

**Q: Can I use this in commercial projects?**
A: Yes, it's MIT licensed.

**Q: How do I change the button colors?**
A: Edit the color constants in `src/macos_window.cpp`.