#ifndef NCURSES_TERMINAL_H
#define NCURSES_TERMINAL_H

#include "../../platform/terminal_interface.h"
#include <ncurses.h>
#include <map>

/**
 * @brief NCurses implementation of the terminal interface
 * This class wraps NCurses functionality to provide a consistent interface
 * for Linux and macOS systems
 */
class NCursesTerminal : public ITerminal {
public:
    NCursesTerminal();
    ~NCursesTerminal() override;
    
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
    bool initialized_;
    std::map<WindowHandle, WINDOW*> windows_;
    
    // Helper functions
    WINDOW* getNCursesWindow(WindowHandle handle);
    int mapKeyCode(int ncurses_key);
    int mapColorPair(ColorPair color);
};

#endif // NCURSES_TERMINAL_H
