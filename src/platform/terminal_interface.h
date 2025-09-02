#ifndef TERMINAL_INTERFACE_H
#define TERMINAL_INTERFACE_H

#include <string>
#include <memory>

/**
 * @brief Abstract interface for terminal operations
 * This interface abstracts platform-specific terminal functionality
 * allowing the same code to work on NCurses (Linux/macOS) and Windows Console API
 */
class ITerminal {
public:
    // Window handle type (platform-specific)
    using WindowHandle = void*;
    
    // Color pair constants
    enum ColorPair {
        DEFAULT = 0,
        STATUS_BAR = 1,     // White on blue
        SELECTED = 2,       // Black on yellow  
        ERROR = 3,          // White on red
        DIRECTORY = 4       // Green on black
    };
    
    // Key codes (standardized across platforms)
    enum KeyCode {
        KEY_UNKNOWN = -1,
        KEY_UP_ARROW = 1000,
        KEY_DOWN_ARROW,
        KEY_LEFT_ARROW,
        KEY_RIGHT_ARROW,
        KEY_PAGE_UP,
        KEY_PAGE_DOWN,
        KEY_HOME_KEY,
        KEY_END_KEY,
        KEY_ENTER_KEY,
        KEY_ESCAPE_KEY,
        KEY_RESIZE_EVENT
    };
    
    virtual ~ITerminal() = default;
    
    // Core terminal operations
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual void getScreenSize(int& width, int& height) = 0;
    virtual int getKey() = 0;
    
    // Window management
    virtual WindowHandle createWindow(int height, int width, int start_y, int start_x) = 0;
    virtual void destroyWindow(WindowHandle window) = 0;
    virtual void getWindowSize(WindowHandle window, int& width, int& height) = 0;
    
    // Drawing operations
    virtual void clearWindow(WindowHandle window) = 0;
    virtual void drawBorder(WindowHandle window) = 0;
    virtual void drawText(WindowHandle window, int y, int x, const std::string& text) = 0;
    virtual void drawHorizontalLine(WindowHandle window, int y, int x, int length) = 0;
    
    // Text attributes
    virtual void setTextAttribute(WindowHandle window, ColorPair color, bool bold = false, bool reverse = false) = 0;
    virtual void clearTextAttribute(WindowHandle window, ColorPair color, bool bold = false, bool reverse = false) = 0;
    
    // Screen operations
    virtual void refreshWindow(WindowHandle window) = 0;
    virtual void refreshScreen() = 0;
    virtual void clearScreen() = 0;
    
    // Cursor operations
    virtual void hideCursor() = 0;
    virtual void showCursor() = 0;
    
    // Color support
    virtual bool hasColors() = 0;
    virtual void initializeColors() = 0;
    
    // Utility functions
    virtual void centerText(WindowHandle window, int y, const std::string& text) = 0;
    
    // Platform-specific optimizations
    virtual void enableOptimizations() = 0;
    virtual void forceCompleteRedraw() = 0;
};

/**
 * @brief Factory function to create platform-appropriate terminal instance
 * @return Unique pointer to terminal implementation
 */
std::unique_ptr<ITerminal> createTerminal();

#endif // TERMINAL_INTERFACE_H
