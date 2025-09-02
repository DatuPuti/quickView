#ifndef DISPLAY_H
#define DISPLAY_H

#include "../platform/terminal_interface.h"
#include <filesystem>
#include <vector>
#include <string>

// Forward declaration
class QuickView;

/**
 * @brief Display and drawing functionality for the UI
 */
namespace Display {
    /**
     * @brief Draw the file browser window
     * @param terminal Terminal interface
     * @param window File browser window handle
     * @param entries Directory entries to display
     * @param selected_index Currently selected file index
     * @param scroll_offset Scroll offset for the list
     * @param current_directory Current directory path
     */
    void drawFileBrowser(ITerminal* terminal,
                        ITerminal::WindowHandle window,
                        const std::vector<std::filesystem::directory_entry>& entries,
                        int selected_index,
                        int scroll_offset,
                        const std::filesystem::path& current_directory);
    
    /**
     * @brief Draw the status bar
     * @param terminal Terminal interface
     * @param window Status window handle
     * @param entries Directory entries
     * @param selected_index Currently selected file index
     * @param current_directory Current directory path
     * @param status_message Status message to display
     * @param screen_width Screen width for layout
     */
    void drawStatusBar(ITerminal* terminal,
                      ITerminal::WindowHandle window,
                      const std::vector<std::filesystem::directory_entry>& entries,
                      int selected_index,
                      const std::filesystem::path& current_directory,
                      const std::string& status_message,
                      int screen_width);

    /**
     * @brief Draw the info window (bottom right)
     * @param terminal Terminal interface
     * @param window Info window handle
     * @param entries Directory entries
     * @param selected_index Currently selected file index
     */
    void drawInfoWindow(ITerminal* terminal,
                       ITerminal::WindowHandle window,
                       const std::vector<std::filesystem::directory_entry>& entries,
                       int selected_index);

    /**
     * @brief Draw normal content (directory preview or welcome)
     * @param terminal Terminal interface
     * @param window Content window handle
     * @param entries Directory entries
     * @param selected_index Currently selected file index
     */
    void drawNormalContent(ITerminal* terminal,
                          ITerminal::WindowHandle window,
                          const std::vector<std::filesystem::directory_entry>& entries,
                          int selected_index);

    /**
     * @brief Draw help content
     * @param terminal Terminal interface
     * @param window Content window handle
     */
    void drawHelpContent(ITerminal* terminal, ITerminal::WindowHandle window);

    /**
     * @brief Draw about content
     * @param terminal Terminal interface
     * @param window Content window handle
     */
    void drawAboutContent(ITerminal* terminal, ITerminal::WindowHandle window);

    /**
     * @brief Draw file view content
     * @param terminal Terminal interface
     * @param window Content window handle
     * @param file_lines File content lines
     * @param scroll_offset File view scroll offset
     * @param filename Name of the file being viewed
     */
    void drawFileViewContent(ITerminal* terminal,
                            ITerminal::WindowHandle window,
                            const std::vector<std::string>& file_lines,
                            int scroll_offset,
                            const std::string& filename);

    /**
     * @brief Draw directory information in info window
     * @param terminal Terminal interface
     * @param window Info window handle
     * @param dir_entry Directory entry
     */
    void drawDirectoryInfo(ITerminal* terminal, ITerminal::WindowHandle window, const std::filesystem::directory_entry& dir_entry);

    /**
     * @brief Draw file information in info window
     * @param terminal Terminal interface
     * @param window Info window handle
     * @param file_entry File entry
     */
    void drawFileInfo(ITerminal* terminal, ITerminal::WindowHandle window, const std::filesystem::directory_entry& file_entry);

    /**
     * @brief Draw directory contents in a window
     * @param terminal Terminal interface
     * @param dir_path Directory path to display
     * @param window Window handle to draw in
     */
    void drawDirectoryContentsInWindow(ITerminal* terminal, const std::filesystem::path& dir_path, ITerminal::WindowHandle window);
}

#endif // DISPLAY_H
