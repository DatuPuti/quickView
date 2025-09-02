#ifndef QUICKVIEW_H
#define QUICKVIEW_H

#include "../platform/terminal_interface.h"
#include <string>
#include <vector>
#include <memory>
#include <filesystem>

/**
 * @brief Main application class for quickView file browser
 */
class QuickView {
public:
    // Display mode enumeration
    enum class DisplayMode {
        NORMAL,
        HELP,
        ABOUT,
        FILE_VIEW
    };

    /**
     * @brief Constructor - initializes the application
     * @param debug_mode Enable debug output to stderr
     */
    QuickView(bool debug_mode = false);

    /**
     * @brief Destructor - cleans up resources
     */
    ~QuickView();

    /**
     * @brief Initialize the application
     * @return true if successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Run the main application loop
     */
    void run();

    /**
     * @brief Shutdown the application
     */
    void shutdown();

    // Public accessors for input handling
    DisplayMode getCurrentDisplayMode() const { return current_display_mode; }
    void setDisplayMode(DisplayMode mode) { current_display_mode = mode; }
    void setRunning(bool running) { this->running = running; }
    void setNeedsRedraw(bool needs_redraw) { this->needs_redraw = needs_redraw; }
    bool isDebugEnabled() const { return debug_enabled; }
    void setStatusMessage(const std::string& message);

    // Navigation methods
    void navigateUp();
    void navigateDown();
    void navigatePageUp();
    void navigatePageDown();
    void navigateHome();
    void navigateEnd();
    void enterDirectory();
    void viewFile();

    // File view scrolling methods
    void scrollFileViewUp();
    void scrollFileViewDown();
    void scrollFileViewPageUp();
    void scrollFileViewPageDown();
    void scrollFileViewHome();
    void scrollFileViewEnd();

    // Window management
    void resizeHandler();

private:
    // Terminal interface
    std::unique_ptr<ITerminal> terminal_;

    // Window handles
    ITerminal::WindowHandle status_window_;
    ITerminal::WindowHandle content_window_;
    ITerminal::WindowHandle info_window_;
    ITerminal::WindowHandle file_browser_window_;

    // Application state
    bool running;
    bool debug_enabled;
    bool needs_redraw;
    int screen_height;
    int screen_width;
    std::string status_message;

    // Display mode
    DisplayMode current_display_mode;

    // File browser state
    std::filesystem::path current_directory;
    std::vector<std::filesystem::directory_entry> directory_entries;
    int selected_file_index;
    int file_scroll_offset;

    // File viewing state
    std::vector<std::string> file_content_lines;
    int file_view_scroll_offset;

    // Private methods
    void setupWindows();
    void drawInterface();
    void updateDisplay();
    void loadDirectory(const std::filesystem::path& path);

public:
    // Public accessors for the refactored modules
    const std::vector<std::filesystem::directory_entry>& getDirectoryEntries() const { return directory_entries; }
    int getSelectedFileIndex() const { return selected_file_index; }
    int getFileScrollOffset() const { return file_scroll_offset; }
    const std::filesystem::path& getCurrentDirectory() const { return current_directory; }
    const std::string& getStatusMessage() const { return status_message; }
    int getScreenWidth() const { return screen_width; }
    const std::vector<std::string>& getFileContentLines() const { return file_content_lines; }
    int getFileViewScrollOffset() const { return file_view_scroll_offset; }

    // Window accessors
    ITerminal::WindowHandle getFileBrowserWindow() const { return file_browser_window_; }
    ITerminal::WindowHandle getContentWindow() const { return content_window_; }
    ITerminal::WindowHandle getInfoWindow() const { return info_window_; }
    ITerminal::WindowHandle getStatusWindow() const { return status_window_; }
    ITerminal* getTerminal() const { return terminal_.get(); }

private:
};

#endif // QUICKVIEW_H
