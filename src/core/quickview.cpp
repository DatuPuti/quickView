#include "quickview.h"
#include "../ui/display.h"
#include "../ui/input.h"
#include "../filesystem/file_operations.h"
#include "../filesystem/image_handler.h"
#include "../utils/utils.h"
#include "../platform/terminal_interface.h"
#include <iostream>
#include <fstream>
#include <algorithm>

QuickView::QuickView(bool debug_mode)
    : terminal_(createTerminal())
    , status_window_(nullptr)
    , content_window_(nullptr)
    , info_window_(nullptr)
    , file_browser_window_(nullptr)
    , running(false)
    , debug_enabled(debug_mode)
    , needs_redraw(true)
    , screen_height(0)
    , screen_width(0)
    , status_message("Ready")
    , current_display_mode(DisplayMode::NORMAL)
    , current_directory(std::filesystem::current_path())
    , selected_file_index(0)
    , file_scroll_offset(0)
    , file_view_scroll_offset(0)
{
}

QuickView::~QuickView() {
    shutdown();
}

bool QuickView::initialize() {
    // Initialize terminal
    if (!terminal_->initialize()) {
        std::cerr << "Error: Failed to initialize terminal" << std::endl;
        return false;
    }

    // Enable terminal optimizations
    terminal_->enableOptimizations();

    // Debug output to stderr (won't interfere with terminal)
    Utils::debugPrint(debug_enabled, "Terminal initialized successfully\n");

    // Get screen dimensions
    terminal_->getScreenSize(screen_width, screen_height);
    Utils::debugPrint(debug_enabled, "Screen dimensions: %dx%d\n", screen_width, screen_height);

    // Setup windows
    setupWindows();

    // Load initial directory
    loadDirectory(current_directory);

    running = true;

    // Start with help screen displayed
    current_display_mode = DisplayMode::HELP;
    setStatusMessage("Help screen - Press any key to start browsing files");

    Utils::debugPrint(debug_enabled, "Initialization complete, entering main loop\n");
    return true;
}

void QuickView::run() {
    if (!running) {
        std::cerr << "Error: Application not initialized" << std::endl;
        return;
    }

    // Main application loop
    Utils::debugPrint(debug_enabled, "Entering main loop, running = %s\n", running ? "true" : "false");
    
    // Initial draw
    drawInterface();
    updateDisplay();
    
    while (running) {
        Utils::debugPrint(debug_enabled, "Loop iteration starting...\n");
        Input::handleInput(this);
        
        // Only redraw if something changed
        if (needs_redraw) {
            drawInterface();
            updateDisplay();
            needs_redraw = false;
        }
        Utils::debugPrint(debug_enabled, "Loop iteration complete\n");
    }
    Utils::debugPrint(debug_enabled, "Exited main loop\n");
}

void QuickView::setupWindows() {
    // Clear screen
    terminal_->clearScreen();
    terminal_->refreshScreen();

    Utils::debugPrint(debug_enabled, "Setting up windows...\n");

    // Calculate window dimensions
    int file_browser_width = screen_width / 4;  // 25% of screen width
    int right_side_width = screen_width - file_browser_width;  // 75% of screen width
    int available_height = screen_height - 1;  // Exclude status bar
    int info_window_height = (available_height * 10) / 100;  // 10% of available height
    int content_window_height = available_height - info_window_height;  // 90% of available height

    // Create status window (bottom line)
    status_window_ = terminal_->createWindow(1, screen_width, screen_height - 1, 0);

    // Create file browser window (left side, 25% width, full height minus status)
    file_browser_window_ = terminal_->createWindow(available_height, file_browser_width, 0, 0);

    // Create content window (right side top, 75% width, 90% height)
    content_window_ = terminal_->createWindow(content_window_height, right_side_width, 0, file_browser_width);

    // Create info window (right side bottom, 75% width, 10% height)
    info_window_ = terminal_->createWindow(info_window_height, right_side_width, content_window_height, file_browser_width);

    Utils::debugPrint(debug_enabled, "Windows created successfully\n");
}

void QuickView::drawInterface() {
    Utils::debugPrint(debug_enabled, "Drawing interface...\n");

    // Draw file browser
    Display::drawFileBrowser(getTerminal(), getFileBrowserWindow(), getDirectoryEntries(), getSelectedFileIndex(),
                            getFileScrollOffset(), getCurrentDirectory());

    // Draw content based on display mode
    switch (current_display_mode) {
        case DisplayMode::HELP:
            Display::drawHelpContent(getTerminal(), getContentWindow());
            break;
        case DisplayMode::ABOUT:
            Display::drawAboutContent(getTerminal(), getContentWindow());
            break;
        case DisplayMode::FILE_VIEW:
            Display::drawFileViewContent(getTerminal(), getContentWindow(), getFileContentLines(),
                                       getFileViewScrollOffset(),
                                       getDirectoryEntries().empty() ? "" :
                                       getDirectoryEntries()[getSelectedFileIndex()].path().filename().string());
            break;
        case DisplayMode::NORMAL:
        default:
            Display::drawNormalContent(getTerminal(), getContentWindow(), getDirectoryEntries(), getSelectedFileIndex());
            break;
    }

    // Draw info window
    Display::drawInfoWindow(getTerminal(), getInfoWindow(), getDirectoryEntries(), getSelectedFileIndex());

    // Draw status bar
    Display::drawStatusBar(getTerminal(), getStatusWindow(), getDirectoryEntries(), getSelectedFileIndex(),
                          getCurrentDirectory(), getStatusMessage(), getScreenWidth());

    Utils::debugPrint(debug_enabled, "Interface drawn\n");
}

void QuickView::updateDisplay() {
    terminal_->refreshWindow(getFileBrowserWindow());
    terminal_->refreshWindow(getContentWindow());
    terminal_->refreshWindow(getInfoWindow());
    terminal_->refreshWindow(getStatusWindow());
}

void QuickView::setStatusMessage(const std::string& message) {
    status_message = message;
    needs_redraw = true;
}

void QuickView::loadDirectory(const std::filesystem::path& path) {
    // Show immediate feedback
    setStatusMessage("Loading directory...");
    needs_redraw = true;
    drawInterface();
    updateDisplay();
    
    std::string result = FileOperations::loadDirectory(path, directory_entries, debug_enabled);
    
    current_directory = path;
    selected_file_index = 0;
    file_scroll_offset = 0;
    needs_redraw = true;
    
    // Update status with result
    setStatusMessage(result);
}

void QuickView::shutdown() {
    // Clean up windows
    if (status_window_) {
        terminal_->destroyWindow(status_window_);
        status_window_ = nullptr;
    }

    if (content_window_) {
        terminal_->destroyWindow(content_window_);
        content_window_ = nullptr;
    }

    if (info_window_) {
        terminal_->destroyWindow(info_window_);
        info_window_ = nullptr;
    }

    if (file_browser_window_) {
        terminal_->destroyWindow(file_browser_window_);
        file_browser_window_ = nullptr;
    }

    // Shutdown terminal
    if (terminal_) {
        terminal_->shutdown();
    }

    running = false;
}

// Navigation methods
void QuickView::navigateUp() {
    if (directory_entries.empty()) return;

    if (selected_file_index > 0) {
        selected_file_index--;

        // Adjust scroll offset if needed
        if (selected_file_index < file_scroll_offset) {
            file_scroll_offset = selected_file_index;
        }
        needs_redraw = true;
    }
}

void QuickView::navigateDown() {
    if (directory_entries.empty()) return;

    if (selected_file_index < directory_entries.size() - 1) {
        selected_file_index++;

        // Adjust scroll offset if needed
        int max_y, max_x;
        terminal_->getWindowSize(file_browser_window_, max_x, max_y);
        int display_height = max_y - 4;  // Account for borders and header

        if (selected_file_index >= file_scroll_offset + display_height) {
            file_scroll_offset = selected_file_index - display_height + 1;
        }
        needs_redraw = true;
    }
}

void QuickView::navigatePageUp() {
    if (directory_entries.empty()) return;

    // Get window dimensions to calculate page size
    int max_y, max_x;
    terminal_->getWindowSize(file_browser_window_, max_x, max_y);
    int page_size = max_y - 4;  // Account for borders and header

    // Move selection up by page size
    selected_file_index -= page_size;
    if (selected_file_index < 0) {
        selected_file_index = 0;
    }

    // Adjust scroll offset
    file_scroll_offset = selected_file_index;
    if (file_scroll_offset < 0) {
        file_scroll_offset = 0;
    }

    needs_redraw = true;
    setStatusMessage("Page up in file list");
}

void QuickView::navigatePageDown() {
    if (directory_entries.empty()) return;

    // Get window dimensions to calculate page size
    int max_y, max_x;
    terminal_->getWindowSize(file_browser_window_, max_x, max_y);
    int page_size = max_y - 4;  // Account for borders and header

    // Move selection down by page size
    selected_file_index += page_size;
    if (selected_file_index >= directory_entries.size()) {
        selected_file_index = directory_entries.size() - 1;
    }

    // Adjust scroll offset
    int display_height = max_y - 4;
    file_scroll_offset = selected_file_index - display_height + 1;
    if (file_scroll_offset < 0) {
        file_scroll_offset = 0;
    }

    // Make sure we don't scroll past the end
    int max_scroll = (int)directory_entries.size() - display_height;
    if (max_scroll < 0) max_scroll = 0;
    if (file_scroll_offset > max_scroll) {
        file_scroll_offset = max_scroll;
    }

    needs_redraw = true;
    setStatusMessage("Page down in file list");
}

void QuickView::navigateHome() {
    if (directory_entries.empty()) return;

    selected_file_index = 0;
    file_scroll_offset = 0;
    needs_redraw = true;
    setStatusMessage("Top of file list");
}

void QuickView::navigateEnd() {
    if (directory_entries.empty()) return;

    selected_file_index = directory_entries.size() - 1;

    // Adjust scroll offset to show the last item
    int max_y, max_x;
    terminal_->getWindowSize(file_browser_window_, max_x, max_y);
    int display_height = max_y - 4;  // Account for borders and header

    file_scroll_offset = (int)directory_entries.size() - display_height;
    if (file_scroll_offset < 0) {
        file_scroll_offset = 0;
    }

    needs_redraw = true;
    setStatusMessage("Bottom of file list");
}

void QuickView::enterDirectory() {
    if (directory_entries.empty() || selected_file_index >= directory_entries.size()) {
        return;
    }

    const auto& selected_entry = directory_entries[selected_file_index];

    // Check if it's a directory first (with error handling)
    std::error_code ec;
    if (selected_entry.is_directory(ec) && !ec) {
        std::filesystem::path new_path = selected_entry.path();

        // Handle ".." parent directory
        if (selected_entry.path().filename() == "..") {
            new_path = current_directory.parent_path();
        }

        // Show immediate feedback before loading
        std::string dir_name = new_path.filename().string();
        if (dir_name.empty()) dir_name = new_path.string();
        setStatusMessage("Entering " + dir_name + "...");
        needs_redraw = true;

        loadDirectory(new_path);
    } else if (ec) {
        setStatusMessage("Cannot access: " + std::string(ec.message()));
    }
    // For files, the status bar will automatically show the file info
}

void QuickView::viewFile() {
    if (directory_entries.empty() || selected_file_index >= directory_entries.size()) {
        setStatusMessage("No file selected");
        return;
    }

    const auto& selected_entry = directory_entries[selected_file_index];

    // Only view regular files
    std::error_code ec;
    if (!selected_entry.is_regular_file(ec) || ec) {
        setStatusMessage("Cannot view: not a regular file");
        return;
    }

    // Check if it's an image file
    if (ImageHandler::isImageFile(selected_entry.path())) {
        // Launch default image viewer and continue browsing
        ImageHandler::launchImageViewer(selected_entry.path(), debug_enabled);
        setStatusMessage("Image opened in default viewer");

        // Force a complete screen refresh to prevent display corruption from external command
        terminal_->forceCompleteRedraw();
        setupWindows();

        // Redraw the entire interface after refresh
        drawInterface();
        updateDisplay();
        return;
    }

    // Clear previous content
    file_content_lines.clear();
    file_view_scroll_offset = 0;

    try {
        std::ifstream file(selected_entry.path());
        if (!file.is_open()) {
            setStatusMessage("Error: Cannot open file");
            return;
        }

        // Check file size to avoid loading huge files
        auto file_size = std::filesystem::file_size(selected_entry.path());
        const size_t MAX_FILE_SIZE = 10 * 1024 * 1024;  // 10MB limit

        if (file_size > MAX_FILE_SIZE) {
            setStatusMessage("Error: File too large to view (>10MB)");
            return;
        }

        // Read file line by line
        std::string line;
        int line_count = 0;
        const int MAX_LINES = 10000;  // Limit number of lines

        while (std::getline(file, line) && line_count < MAX_LINES) {
            // Replace tabs with spaces for better display
            std::string display_line;
            for (char c : line) {
                if (c == '\t') {
                    display_line += "    ";  // 4 spaces for tab
                } else if (c >= 32 || c == '\n') {  // Printable characters
                    display_line += c;
                } else {
                    display_line += '.';  // Replace non-printable with dot
                }
            }
            file_content_lines.push_back(display_line);
            line_count++;
        }

        file.close();

        if (line_count >= MAX_LINES) {
            file_content_lines.push_back("... (file truncated at " + std::to_string(MAX_LINES) + " lines)");
        }

        // Switch to file view mode
        current_display_mode = DisplayMode::FILE_VIEW;
        needs_redraw = true;
        setStatusMessage("File view - Press any key to return");

    } catch (const std::exception& e) {
        setStatusMessage("Error reading file: " + std::string(e.what()));
        Utils::debugPrint(debug_enabled, "Error reading file %s: %s\n", selected_entry.path().string().c_str(), e.what());
    }
}

void QuickView::resizeHandler() {
    // Handle terminal resize
    terminal_->getScreenSize(screen_width, screen_height);

    // Delete old windows
    if (status_window_) terminal_->destroyWindow(status_window_);
    if (content_window_) terminal_->destroyWindow(content_window_);
    if (info_window_) terminal_->destroyWindow(info_window_);
    if (file_browser_window_) terminal_->destroyWindow(file_browser_window_);

    // Recreate windows with new dimensions
    setupWindows();
    needs_redraw = true;

    setStatusMessage("Terminal resized");
}

// File view scrolling methods
void QuickView::scrollFileViewUp() {
    if (file_view_scroll_offset > 0) {
        file_view_scroll_offset--;
        needs_redraw = true;
        setStatusMessage("Scrolled up");
    } else {
        setStatusMessage("Already at top of file");
    }
}

void QuickView::scrollFileViewDown() {
    // Get window dimensions to calculate max scroll
    int max_y, max_x;
    terminal_->getWindowSize(content_window_, max_x, max_y);
    int display_height = max_y - 5;  // Account for borders, title, and bottom margin

    int max_scroll = (int)file_content_lines.size() - display_height;
    if (max_scroll < 0) max_scroll = 0;

    if (file_view_scroll_offset < max_scroll) {
        file_view_scroll_offset++;
        needs_redraw = true;
        setStatusMessage("Scrolled down");
    } else {
        setStatusMessage("Already at end of file");
    }
}

void QuickView::scrollFileViewPageUp() {
    // Get window dimensions to calculate page size
    int max_y, max_x;
    terminal_->getWindowSize(content_window_, max_x, max_y);
    int page_size = max_y - 5;  // Account for borders, title, and bottom margin

    file_view_scroll_offset -= page_size;
    if (file_view_scroll_offset < 0) {
        file_view_scroll_offset = 0;
    }
    needs_redraw = true;
    setStatusMessage("Page up");
}

void QuickView::scrollFileViewPageDown() {
    // Get window dimensions to calculate page size and max scroll
    int max_y, max_x;
    terminal_->getWindowSize(content_window_, max_x, max_y);
    int page_size = max_y - 5;  // Account for borders, title, and bottom margin
    int display_height = page_size;

    int max_scroll = (int)file_content_lines.size() - display_height;
    if (max_scroll < 0) max_scroll = 0;

    file_view_scroll_offset += page_size;
    if (file_view_scroll_offset > max_scroll) {
        file_view_scroll_offset = max_scroll;
    }
    needs_redraw = true;
    setStatusMessage("Page down");
}

void QuickView::scrollFileViewHome() {
    file_view_scroll_offset = 0;
    needs_redraw = true;
    setStatusMessage("Top of file");
}

void QuickView::scrollFileViewEnd() {
    // Get window dimensions to calculate max scroll
    int max_y, max_x;
    terminal_->getWindowSize(content_window_, max_x, max_y);
    int display_height = max_y - 5;  // Account for borders, title, and bottom margin

    int max_scroll = (int)file_content_lines.size() - display_height;
    if (max_scroll < 0) max_scroll = 0;

    file_view_scroll_offset = max_scroll;
    needs_redraw = true;
    setStatusMessage("End of file");
}
