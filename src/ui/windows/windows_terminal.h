#ifndef WINDOWS_TERMINAL_H
#define WINDOWS_TERMINAL_H

#include "../../platform/terminal_interface.h"

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#include <map>
#include <vector>

/**
 * @brief Windows Console API implementation of the terminal interface
 * This class uses Windows Console API to provide terminal functionality
 * on Windows systems
 */
class WindowsTerminal : public ITerminal {
public:
    WindowsTerminal();
    ~WindowsTerminal() override;
    
    // Core terminal operations
    bool initialize() override;
    void shutdown() override;
    void getScreenSize(int& width, int& height) override;
    int getKey() override;
    
    // Window management
    WindowHandle createWindow(int height, int width, int start_y, int start_x) override;
    void destroyWindow(WindowHandle window) override;
    void getWindowSize(WindowHandle window, int& width, int& height) override;
    
    // Drawing operations
    void clearWindow(WindowHandle window) override;
    void drawBorder(WindowHandle window) override;
    void drawText(WindowHandle window, int y, int x, const std::string& text) override;
    void drawHorizontalLine(WindowHandle window, int y, int x, int length) override;
    
    // Text attributes
    void setTextAttribute(WindowHandle window, ColorPair color, bool bold = false, bool reverse = false) override;
    void clearTextAttribute(WindowHandle window, ColorPair color, bool bold = false, bool reverse = false) override;
    
    // Screen operations
    void refreshWindow(WindowHandle window) override;
    void refreshScreen() override;
    void clearScreen() override;
    
    // Cursor operations
    void hideCursor() override;
    void showCursor() override;
    
    // Color support
    bool hasColors() override;
    void initializeColors() override;
    
    // Utility functions
    void centerText(WindowHandle window, int y, const std::string& text) override;
    
    // Platform-specific optimizations
    void enableOptimizations() override;
    void forceCompleteRedraw() override;

private:
    struct WindowInfo {
        int x, y, width, height;
        WORD default_attributes;
        std::vector<std::vector<CHAR_INFO>> buffer;
    };
    
    bool initialized_;
    HANDLE console_output_;
    HANDLE console_input_;
    CONSOLE_SCREEN_BUFFER_INFO original_info_;
    std::map<WindowHandle, WindowInfo> windows_;
    int next_window_id_;
    
    // Helper functions
    WindowInfo* getWindowInfo(WindowHandle handle);
    int mapKeyCode(int windows_key);
    WORD mapColorPair(ColorPair color);
    void updateWindowBuffer(WindowHandle window);
    void drawCharacter(WindowHandle window, int y, int x, char ch, WORD attributes);
};

#else
// Stub implementation for non-Windows platforms
class WindowsTerminal : public ITerminal {
public:
    bool initialize() override { return false; }
    void shutdown() override {}
    void getScreenSize(int& width, int& height) override { width = height = 0; }
    int getKey() override { return -1; }
    WindowHandle createWindow(int, int, int, int) override { return nullptr; }
    void destroyWindow(WindowHandle) override {}
    void getWindowSize(WindowHandle, int&, int&) override {}
    void clearWindow(WindowHandle) override {}
    void drawBorder(WindowHandle) override {}
    void drawText(WindowHandle, int, int, const std::string&) override {}
    void drawHorizontalLine(WindowHandle, int, int, int) override {}
    void setTextAttribute(WindowHandle, ColorPair, bool, bool) override {}
    void clearTextAttribute(WindowHandle, ColorPair, bool, bool) override {}
    void refreshWindow(WindowHandle) override {}
    void refreshScreen() override {}
    void clearScreen() override {}
    void hideCursor() override {}
    void showCursor() override {}
    bool hasColors() override { return false; }
    void initializeColors() override {}
    void centerText(WindowHandle, int, const std::string&) override {}
    void enableOptimizations() override {}
    void forceCompleteRedraw() override {}
};
#endif

#endif // WINDOWS_TERMINAL_H
