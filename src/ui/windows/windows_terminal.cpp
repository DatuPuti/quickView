#include "windows_terminal.h"

#ifdef _WIN32
#include <iostream>
#include <algorithm>

// Windows Console color constants
#ifndef FOREGROUND_WHITE
#define FOREGROUND_WHITE (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#endif
#ifndef FOREGROUND_BLACK
#define FOREGROUND_BLACK 0
#endif
#ifndef BACKGROUND_WHITE
#define BACKGROUND_WHITE (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE)
#endif
#ifndef BACKGROUND_BLACK
#define BACKGROUND_BLACK 0
#endif
#ifndef BACKGROUND_YELLOW
#define BACKGROUND_YELLOW (BACKGROUND_RED | BACKGROUND_GREEN)
#endif

WindowsTerminal::WindowsTerminal() 
    : initialized_(false)
    , console_output_(INVALID_HANDLE_VALUE)
    , console_input_(INVALID_HANDLE_VALUE)
    , next_window_id_(1) {
}

WindowsTerminal::~WindowsTerminal() {
    shutdown();
}

bool WindowsTerminal::initialize() {
    if (initialized_) return true;
    
    // Get console handles
    console_output_ = GetStdHandle(STD_OUTPUT_HANDLE);
    console_input_ = GetStdHandle(STD_INPUT_HANDLE);
    
    if (console_output_ == INVALID_HANDLE_VALUE || console_input_ == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    // Save original console state
    if (!GetConsoleScreenBufferInfo(console_output_, &original_info_)) {
        return false;
    }
    
    // Configure console for better input handling
    DWORD input_mode;
    GetConsoleMode(console_input_, &input_mode);
    input_mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    input_mode |= ENABLE_WINDOW_INPUT;
    SetConsoleMode(console_input_, input_mode);
    
    // Initialize colors and cursor
    initializeColors();
    hideCursor();
    
    initialized_ = true;
    return true;
}

void WindowsTerminal::shutdown() {
    if (!initialized_) return;

    // Clear the screen before shutdown
    clearScreen();

    // Restore original console state
    SetConsoleTextAttribute(console_output_, original_info_.wAttributes);
    showCursor();

    // Clean up windows
    windows_.clear();

    initialized_ = false;
}

void WindowsTerminal::getScreenSize(int& width, int& height) {
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (GetConsoleScreenBufferInfo(console_output_, &info)) {
        width = info.srWindow.Right - info.srWindow.Left + 1;
        height = info.srWindow.Bottom - info.srWindow.Top + 1;
    } else {
        width = 80;
        height = 25;
    }
}

int WindowsTerminal::getKey() {
    INPUT_RECORD input;
    DWORD events_read;
    
    while (true) {
        if (!ReadConsoleInput(console_input_, &input, 1, &events_read)) {
            return KEY_UNKNOWN;
        }
        
        if (input.EventType == KEY_EVENT && input.Event.KeyEvent.bKeyDown) {
            return mapKeyCode(input.Event.KeyEvent.wVirtualKeyCode);
        } else if (input.EventType == WINDOW_BUFFER_SIZE_EVENT) {
            return KEY_RESIZE_EVENT;
        }
    }
}

ITerminal::WindowHandle WindowsTerminal::createWindow(int height, int width, int start_y, int start_x) {
    WindowHandle handle = reinterpret_cast<WindowHandle>(next_window_id_++);
    
    WindowInfo info;
    info.x = start_x;
    info.y = start_y;
    info.width = width;
    info.height = height;
    info.default_attributes = original_info_.wAttributes;
    
    // Initialize buffer
    info.buffer.resize(height);
    for (auto& row : info.buffer) {
        row.resize(width);
        for (auto& cell : row) {
            cell.Char.AsciiChar = ' ';
            cell.Attributes = info.default_attributes;
        }
    }
    
    windows_[handle] = info;
    return handle;
}

void WindowsTerminal::destroyWindow(WindowHandle window) {
    windows_.erase(window);
}

void WindowsTerminal::getWindowSize(WindowHandle window, int& width, int& height) {
    WindowInfo* info = getWindowInfo(window);
    if (info) {
        width = info->width;
        height = info->height;
    } else {
        width = height = 0;
    }
}

void WindowsTerminal::clearWindow(WindowHandle window) {
    WindowInfo* info = getWindowInfo(window);
    if (!info) return;
    
    for (auto& row : info->buffer) {
        for (auto& cell : row) {
            cell.Char.AsciiChar = ' ';
            cell.Attributes = info->default_attributes;
        }
    }
}

void WindowsTerminal::drawBorder(WindowHandle window) {
    WindowInfo* info = getWindowInfo(window);
    if (!info) return;
    
    // Draw border using ASCII characters (Windows Console doesn't have box drawing by default)
    // Top and bottom borders
    for (int x = 0; x < info->width; x++) {
        drawCharacter(window, 0, x, '-', info->default_attributes);
        drawCharacter(window, info->height - 1, x, '-', info->default_attributes);
    }
    
    // Left and right borders
    for (int y = 0; y < info->height; y++) {
        drawCharacter(window, y, 0, '|', info->default_attributes);
        drawCharacter(window, y, info->width - 1, '|', info->default_attributes);
    }
    
    // Corners
    drawCharacter(window, 0, 0, '+', info->default_attributes);
    drawCharacter(window, 0, info->width - 1, '+', info->default_attributes);
    drawCharacter(window, info->height - 1, 0, '+', info->default_attributes);
    drawCharacter(window, info->height - 1, info->width - 1, '+', info->default_attributes);
}

void WindowsTerminal::drawText(WindowHandle window, int y, int x, const std::string& text) {
    WindowInfo* info = getWindowInfo(window);
    if (!info || y < 0 || y >= info->height) return;
    
    for (size_t i = 0; i < text.length() && (x + i) < info->width; i++) {
        if ((x + i) >= 0) {
            drawCharacter(window, y, x + i, text[i], info->default_attributes);
        }
    }
}

void WindowsTerminal::drawHorizontalLine(WindowHandle window, int y, int x, int length) {
    for (int i = 0; i < length; i++) {
        drawCharacter(window, y, x + i, '-', getWindowInfo(window)->default_attributes);
    }
}

// ... Additional Windows-specific implementations would continue here
// For brevity, I'll show the key mapping and helper functions

int WindowsTerminal::mapKeyCode(int windows_key) {
    switch (windows_key) {
        case VK_UP: return KEY_UP_ARROW;
        case VK_DOWN: return KEY_DOWN_ARROW;
        case VK_LEFT: return KEY_LEFT_ARROW;
        case VK_RIGHT: return KEY_RIGHT_ARROW;
        case VK_PRIOR: return KEY_PAGE_UP;
        case VK_NEXT: return KEY_PAGE_DOWN;
        case VK_HOME: return KEY_HOME_KEY;
        case VK_END: return KEY_END_KEY;
        case VK_RETURN: return KEY_ENTER_KEY;
        case VK_ESCAPE: return KEY_ESCAPE_KEY;
        default: return windows_key;
    }
}

WindowsTerminal::WindowInfo* WindowsTerminal::getWindowInfo(WindowHandle handle) {
    auto it = windows_.find(handle);
    return (it != windows_.end()) ? &it->second : nullptr;
}

void WindowsTerminal::drawCharacter(WindowHandle window, int y, int x, char ch, WORD attributes) {
    WindowInfo* info = getWindowInfo(window);
    if (!info || y < 0 || y >= info->height || x < 0 || x >= info->width) return;
    
    info->buffer[y][x].Char.AsciiChar = ch;
    info->buffer[y][x].Attributes = attributes;
}

void WindowsTerminal::setTextAttribute(WindowHandle window, ColorPair color, bool bold, bool reverse) {
    WindowInfo* info = getWindowInfo(window);
    if (!info) return;

    WORD attributes = mapColorPair(color);
    if (bold) attributes |= FOREGROUND_INTENSITY;
    if (reverse) {
        // Swap foreground and background
        WORD fg = attributes & 0x0F;
        WORD bg = (attributes & 0xF0) >> 4;
        attributes = (fg << 4) | bg;
    }

    info->default_attributes = attributes;
}

void WindowsTerminal::clearTextAttribute(WindowHandle window, ColorPair color, bool bold, bool reverse) {
    WindowInfo* info = getWindowInfo(window);
    if (!info) return;

    // Restore original attributes
    info->default_attributes = original_info_.wAttributes;
}

void WindowsTerminal::refreshWindow(WindowHandle window) {
    updateWindowBuffer(window);
}

void WindowsTerminal::refreshScreen() {
    // Refresh all windows
    for (auto& pair : windows_) {
        updateWindowBuffer(pair.first);
    }
}

void WindowsTerminal::clearScreen() {
    COORD coord = {0, 0};
    DWORD written;
    CONSOLE_SCREEN_BUFFER_INFO info;

    if (GetConsoleScreenBufferInfo(console_output_, &info)) {
        DWORD size = info.dwSize.X * info.dwSize.Y;
        FillConsoleOutputCharacter(console_output_, ' ', size, coord, &written);
        FillConsoleOutputAttribute(console_output_, info.wAttributes, size, coord, &written);
        SetConsoleCursorPosition(console_output_, coord);
    }
}

void WindowsTerminal::hideCursor() {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 1;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(console_output_, &info);
}

void WindowsTerminal::showCursor() {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 25;
    info.bVisible = TRUE;
    SetConsoleCursorInfo(console_output_, &info);
}

bool WindowsTerminal::hasColors() {
    return true;
}

void WindowsTerminal::initializeColors() {
    // Windows Console supports colors by default
    // No special initialization needed
}

void WindowsTerminal::centerText(WindowHandle window, int y, const std::string& text) {
    WindowInfo* info = getWindowInfo(window);
    if (info) {
        int x = (info->width - text.length()) / 2;
        drawText(window, y, x, text);
    }
}

void WindowsTerminal::enableOptimizations() {
    // Enable Windows Console optimizations
    DWORD mode;
    if (GetConsoleMode(console_output_, &mode)) {
        mode |= ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT;
        SetConsoleMode(console_output_, mode);
    }
}

void WindowsTerminal::forceCompleteRedraw() {
    clearScreen();
    refreshScreen();
}

void WindowsTerminal::updateWindowBuffer(WindowHandle window) {
    WindowInfo* info = getWindowInfo(window);
    if (!info) return;

    // Write the window buffer to the console
    COORD buffer_size = {(SHORT)info->width, (SHORT)info->height};
    COORD buffer_coord = {0, 0};
    SMALL_RECT write_region = {
        (SHORT)info->x,
        (SHORT)info->y,
        (SHORT)(info->x + info->width - 1),
        (SHORT)(info->y + info->height - 1)
    };

    // Flatten the 2D buffer for WriteConsoleOutput
    std::vector<CHAR_INFO> flat_buffer;
    flat_buffer.reserve(info->width * info->height);

    for (const auto& row : info->buffer) {
        for (const auto& cell : row) {
            flat_buffer.push_back(cell);
        }
    }

    WriteConsoleOutput(console_output_, flat_buffer.data(), buffer_size, buffer_coord, &write_region);
}

WORD WindowsTerminal::mapColorPair(ColorPair color) {
    switch (color) {
        case STATUS_BAR:
            return FOREGROUND_WHITE | BACKGROUND_BLUE;
        case SELECTED:
            return FOREGROUND_BLACK | BACKGROUND_YELLOW;
        case ERROR:
            return FOREGROUND_WHITE | BACKGROUND_RED;
        case DIRECTORY:
            return FOREGROUND_GREEN;
        default:
            return FOREGROUND_WHITE;
    }
}

#endif // _WIN32
