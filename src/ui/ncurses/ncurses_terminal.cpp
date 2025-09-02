#include "ncurses_terminal.h"
#include "../../utils/utils.h"
#include <algorithm>

NCursesTerminal::NCursesTerminal() : initialized_(false) {
}

NCursesTerminal::~NCursesTerminal() {
    shutdown();
}

bool NCursesTerminal::initialize() {
    if (initialized_) return true;
    
    // Initialize NCurses
    if (!initscr()) {
        return false;
    }
    
    // Configure NCurses settings
    cbreak();           // Disable line buffering
    noecho();           // Don't echo pressed keys
    keypad(stdscr, TRUE); // Enable special keys
    hideCursor();       // Hide cursor
    nodelay(stdscr, FALSE); // Remove timeout for faster response
    
    // Optimize NCurses for better performance
    intrflush(stdscr, FALSE);  // Don't flush on interrupt
    meta(stdscr, TRUE);        // Enable 8-bit characters
    
    // Initialize colors if available
    if (has_colors()) {
        initializeColors();
    }
    
    initialized_ = true;
    return true;
}

void NCursesTerminal::shutdown() {
    if (!initialized_) return;
    
    // Clean up all windows
    for (auto& pair : windows_) {
        if (pair.second && pair.second != stdscr) {
            delwin(pair.second);
        }
    }
    windows_.clear();
    
    // End NCurses
    endwin();
    initialized_ = false;
}

void NCursesTerminal::getScreenSize(int& width, int& height) {
    getmaxyx(stdscr, height, width);
}

int NCursesTerminal::getKey() {
    int key = getch();
    return mapKeyCode(key);
}

ITerminal::WindowHandle NCursesTerminal::createWindow(int height, int width, int start_y, int start_x) {
    WINDOW* win = newwin(height, width, start_y, start_x);
    if (!win) return nullptr;
    
    WindowHandle handle = static_cast<WindowHandle>(win);
    windows_[handle] = win;
    return handle;
}

void NCursesTerminal::destroyWindow(WindowHandle window) {
    WINDOW* win = getNCursesWindow(window);
    if (win && win != stdscr) {
        delwin(win);
        windows_.erase(window);
    }
}

void NCursesTerminal::getWindowSize(WindowHandle window, int& width, int& height) {
    WINDOW* win = getNCursesWindow(window);
    if (win) {
        getmaxyx(win, height, width);
    } else {
        width = height = 0;
    }
}

void NCursesTerminal::clearWindow(WindowHandle window) {
    WINDOW* win = getNCursesWindow(window);
    if (win) {
        werase(win);
    }
}

void NCursesTerminal::drawBorder(WindowHandle window) {
    WINDOW* win = getNCursesWindow(window);
    if (win) {
        box(win, 0, 0);
    }
}

void NCursesTerminal::drawText(WindowHandle window, int y, int x, const std::string& text) {
    WINDOW* win = getNCursesWindow(window);
    if (win) {
        mvwprintw(win, y, x, "%s", text.c_str());
    }
}

void NCursesTerminal::drawHorizontalLine(WindowHandle window, int y, int x, int length) {
    WINDOW* win = getNCursesWindow(window);
    if (win) {
        mvwhline(win, y, x, ACS_HLINE, length);
    }
}

void NCursesTerminal::setTextAttribute(WindowHandle window, ColorPair color, bool bold, bool reverse) {
    WINDOW* win = getNCursesWindow(window);
    if (win) {
        int attrs = 0;
        if (has_colors()) {
            attrs |= COLOR_PAIR(mapColorPair(color));
        }
        if (bold) attrs |= A_BOLD;
        if (reverse) attrs |= A_REVERSE;
        
        if (attrs) wattron(win, attrs);
    }
}

void NCursesTerminal::clearTextAttribute(WindowHandle window, ColorPair color, bool bold, bool reverse) {
    WINDOW* win = getNCursesWindow(window);
    if (win) {
        int attrs = 0;
        if (has_colors()) {
            attrs |= COLOR_PAIR(mapColorPair(color));
        }
        if (bold) attrs |= A_BOLD;
        if (reverse) attrs |= A_REVERSE;
        
        if (attrs) wattroff(win, attrs);
    }
}

void NCursesTerminal::refreshWindow(WindowHandle window) {
    WINDOW* win = getNCursesWindow(window);
    if (win) {
        wrefresh(win);
    }
}

void NCursesTerminal::refreshScreen() {
    refresh();
}

void NCursesTerminal::clearScreen() {
    clear();
}

void NCursesTerminal::hideCursor() {
    curs_set(0);
}

void NCursesTerminal::showCursor() {
    curs_set(1);
}

bool NCursesTerminal::hasColors() {
    return ::has_colors();
}

void NCursesTerminal::initializeColors() {
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);   // Status bar
    init_pair(2, COLOR_BLACK, COLOR_YELLOW); // Selected item
    init_pair(3, COLOR_WHITE, COLOR_RED);    // Error messages
    init_pair(4, COLOR_GREEN, COLOR_BLACK);  // Directories
}

void NCursesTerminal::centerText(WindowHandle window, int y, const std::string& text) {
    WINDOW* win = getNCursesWindow(window);
    if (win) {
        int win_width = getmaxx(win);
        int x = (win_width - text.length()) / 2;
        mvwprintw(win, y, x, "%s", text.c_str());
    }
}

void NCursesTerminal::enableOptimizations() {
    // NCurses-specific optimizations already applied in initialize()
}

void NCursesTerminal::forceCompleteRedraw() {
    clearok(stdscr, TRUE);
    clear();
    refresh();
}

// Private helper functions
WINDOW* NCursesTerminal::getNCursesWindow(WindowHandle handle) {
    if (!handle) return stdscr;
    
    auto it = windows_.find(handle);
    return (it != windows_.end()) ? it->second : nullptr;
}

int NCursesTerminal::mapKeyCode(int ncurses_key) {
    switch (ncurses_key) {
        case KEY_UP: return KEY_UP_ARROW;
        case KEY_DOWN: return KEY_DOWN_ARROW;
        case KEY_LEFT: return KEY_LEFT_ARROW;
        case KEY_RIGHT: return KEY_RIGHT_ARROW;
        case KEY_PPAGE: return KEY_PAGE_UP;
        case KEY_NPAGE: return KEY_PAGE_DOWN;
        case KEY_HOME: return KEY_HOME_KEY;
        case KEY_END: return KEY_END_KEY;
        case KEY_ENTER:
        case '\n':
        case '\r': return KEY_ENTER_KEY;
        case 27: return KEY_ESCAPE_KEY;
        case KEY_RESIZE: return KEY_RESIZE_EVENT;
        default: return ncurses_key;
    }
}

int NCursesTerminal::mapColorPair(ColorPair color) {
    switch (color) {
        case STATUS_BAR: return 1;
        case SELECTED: return 2;
        case ERROR: return 3;
        case DIRECTORY: return 4;
        default: return 0;
    }
}
