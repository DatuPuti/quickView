#include "display.h"
#include "../utils/utils.h"
#include "../filesystem/file_operations.h"
#include "../core/quickview.h"
#include <algorithm>

namespace Display {
    void drawFileBrowser(ITerminal* terminal,
                        ITerminal::WindowHandle window,
                        const std::vector<std::filesystem::directory_entry>& entries,
                        int selected_index,
                        int scroll_offset,
                        const std::filesystem::path& current_directory) {
        terminal->clearWindow(window);

        // Draw border
        terminal->drawBorder(window);

        // Draw title
        terminal->drawText(window, 0, 2, " Directory ");

        // Get window dimensions
        int max_y, max_x;
        terminal->getWindowSize(window, max_x, max_y);
        int display_height = max_y - 4;  // Account for borders, title, and header

        // Display current directory path (truncated if too long)
        std::string dir_path = current_directory.filename().string();
        if (dir_path.empty()) dir_path = current_directory.string();
        if (dir_path.length() > max_x - 4) {
            dir_path = "..." + dir_path.substr(dir_path.length() - (max_x - 7));
        }
        terminal->drawText(window, 1, 1, dir_path);

        // Draw horizontal line under directory name
        terminal->drawHorizontalLine(window, 2, 1, max_x - 2);
        
        // Display files starting from scroll offset
        int start_y = 3;
        for (int i = 0; i < display_height - 1 && (i + scroll_offset) < entries.size(); i++) {
            int entry_index = i + scroll_offset;
            const auto& entry = entries[entry_index];
            
            // Highlight selected file
            if (entry_index == selected_index) {
                if (terminal->hasColors()) {
                    terminal->setTextAttribute(window, ITerminal::SELECTED);
                } else {
                    terminal->setTextAttribute(window, ITerminal::DEFAULT, false, true); // reverse
                }
            }

            // Get filename and truncate if necessary
            std::string filename = entry.path().filename().string();
            if (filename.length() > max_x - 4) {
                filename = filename.substr(0, max_x - 7) + "...";
            }

            // Add directory indicator
            std::error_code ec;
            if (entry.is_directory(ec) && !ec) {
                filename = "[" + filename + "]";
            }

            terminal->drawText(window, start_y + i, 1, filename);

            // Turn off highlighting
            if (entry_index == selected_index) {
                if (terminal->hasColors()) {
                    terminal->clearTextAttribute(window, ITerminal::SELECTED);
                } else {
                    terminal->clearTextAttribute(window, ITerminal::DEFAULT, false, true); // reverse
                }
            }
        }
    }
    
    void drawStatusBar(ITerminal* terminal,
                      ITerminal::WindowHandle window,
                      const std::vector<std::filesystem::directory_entry>& entries,
                      int selected_index,
                      const std::filesystem::path& current_directory,
                      const std::string& status_message,
                      int screen_width) {
        terminal->clearWindow(window);
        
        // Build status information
        std::string status_info;
        
        if (!entries.empty() && selected_index < entries.size()) {
            const auto& selected_entry = entries[selected_index];
            std::string filename = selected_entry.path().filename().string();
            
            std::error_code ec;
            if (selected_entry.is_directory(ec) && !ec) {
                status_info = "[DIR] " + filename;
            } else if (selected_entry.is_regular_file(ec) && !ec) {
                // Try to get file size
                try {
                    auto file_size = std::filesystem::file_size(selected_entry.path());
                    if (file_size < 1024) {
                        status_info = filename + " (" + std::to_string(file_size) + " bytes)";
                    } else if (file_size < 1024 * 1024) {
                        status_info = filename + " (" + std::to_string(file_size / 1024) + " KB)";
                    } else {
                        status_info = filename + " (" + std::to_string(file_size / (1024 * 1024)) + " MB)";
                    }
                } catch (const std::exception& e) {
                    status_info = filename + " (size unknown)";
                }
            } else {
                status_info = filename + " (special file)";
            }
            
            // Add current directory info
            std::string dir_info = " | " + current_directory.filename().string();
            if (dir_info == " | ") {
                dir_info = " | " + current_directory.string();
            }
            
            // Truncate if too long to fit with version info
            std::string version_info = "quickView v1.0";
            int available_space = screen_width - version_info.length() - 3; // 3 for spacing
            
            std::string full_status = status_info + dir_info;
            if (full_status.length() > available_space) {
                if (status_info.length() > available_space - 3) {
                    status_info = status_info.substr(0, available_space - 3) + "...";
                    full_status = status_info;
                } else {
                    full_status = status_info + "...";
                }
            } else {
                full_status = status_info + dir_info;
            }
            
            terminal->drawText(window, 0, 1, full_status);
        } else {
            terminal->drawText(window, 0, 1, status_message);
        }

        // Display version info on the right
        std::string version_info = "quickView v1.0";
        terminal->drawText(window, 0, screen_width - version_info.length() - 1, version_info);
    }

    void drawInfoWindow(ITerminal* terminal,
                       ITerminal::WindowHandle window,
                       const std::vector<std::filesystem::directory_entry>& entries,
                       int selected_index) {
        terminal->clearWindow(window);

        // Draw border
        terminal->drawBorder(window);

        // Draw title
        terminal->drawText(window, 0, 2, " Info ");

        // Show file/directory information
        if (!entries.empty() && selected_index < entries.size()) {
            const auto& selected_entry = entries[selected_index];

            std::error_code ec;
            if (selected_entry.is_directory(ec) && !ec) {
                drawDirectoryInfo(terminal, window, selected_entry);
            } else {
                drawFileInfo(terminal, window, selected_entry);
            }
        } else {
            // Get window dimensions
            int max_y, max_x;
            terminal->getWindowSize(window, max_x, max_y);

            int center_y = max_y / 2;
            terminal->centerText(window, center_y, "No file selected");
        }
    }

    void drawDirectoryInfo(ITerminal* terminal, ITerminal::WindowHandle window, const std::filesystem::directory_entry& dir_entry) {
        // Get window dimensions
        int max_y, max_x;
        terminal->getWindowSize(window, max_x, max_y);

        // Show directory name (truncated if necessary)
        std::string dirname = dir_entry.path().filename().string();
        if (dirname.empty()) dirname = dir_entry.path().string();
        if (dirname.length() > max_x - 4) {
            dirname = dirname.substr(0, max_x - 7) + "...";
        }
        terminal->drawText(window, 1, 2, "[" + dirname + "]");

        // Count directory contents
        try {
            int total_dirs = 0, total_files = 0;

            std::error_code ec;
            for (const auto& entry : std::filesystem::directory_iterator(dir_entry.path(), ec)) {
                if (ec) break;

                std::error_code entry_ec;
                if (entry.is_directory(entry_ec) && !entry_ec) {
                    total_dirs++;
                } else {
                    total_files++;
                }
            }

            terminal->drawText(window, 2, 2, "Directory");
            terminal->drawText(window, 3, 2, "Total: " + std::to_string(total_dirs) + " dirs, " + std::to_string(total_files) + " files");

            // Show truncated path if there's space
            if (max_y > 4) {
                std::string path_display = dir_entry.path().string();
                if (path_display.length() > max_x - 4) {
                    // Show end of path with "..."
                    path_display = "..." + path_display.substr(path_display.length() - (max_x - 7));
                }
                terminal->drawText(window, max_y - 2, 2, path_display);
            }

        } catch (const std::exception& e) {
            terminal->drawText(window, 2, 2, "Directory");
            terminal->drawText(window, 3, 2, "Error reading contents");
        }
    }

    void drawFileInfo(ITerminal* terminal, ITerminal::WindowHandle window, const std::filesystem::directory_entry& file_entry) {
        // Get window dimensions
        int max_y, max_x;
        terminal->getWindowSize(window, max_x, max_y);

        // Show filename (truncated if necessary)
        std::string filename = file_entry.path().filename().string();
        if (filename.length() > max_x - 4) {
            filename = filename.substr(0, max_x - 7) + "...";
        }
        terminal->drawText(window, 1, 2, filename);

        // Show file type and size on one line
        std::string type_info;
        std::error_code ec;
        if (file_entry.is_regular_file(ec) && !ec) {
            try {
                auto file_size = std::filesystem::file_size(file_entry.path());
                if (file_size < 1024) {
                    type_info = "File (" + std::to_string(file_size) + " bytes)";
                } else if (file_size < 1024 * 1024) {
                    type_info = "File (" + std::to_string(file_size / 1024) + " KB)";
                } else if (file_size < 1024 * 1024 * 1024) {
                    type_info = "File (" + std::to_string(file_size / (1024 * 1024)) + " MB)";
                } else {
                    type_info = "File (" + std::to_string(file_size / (1024 * 1024 * 1024)) + " GB)";
                }
            } catch (const std::exception& e) {
                type_info = "File (size unknown)";
            }

            // Show file extension if available and space permits
            std::string extension = file_entry.path().extension().string();
            if (!extension.empty() && max_y > 3) {
                terminal->drawText(window, 3, 2, "Ext: " + extension);
            }

        } else if (file_entry.is_directory(ec) && !ec) {
            type_info = "Directory";
        } else if (file_entry.is_symlink(ec) && !ec) {
            type_info = "Symbolic Link";
        } else {
            type_info = "Special File";
        }

        // Truncate type info if too long
        if (type_info.length() > max_x - 4) {
            type_info = type_info.substr(0, max_x - 7) + "...";
        }
        terminal->drawText(window, 2, 2, type_info);

        // Show truncated path if there's space
        if (max_y > 4) {
            std::string path_display = file_entry.path().string();
            if (path_display.length() > max_x - 4) {
                // Show end of path with "..."
                path_display = "..." + path_display.substr(path_display.length() - (max_x - 7));
            }
            terminal->drawText(window, max_y - 2, 2, path_display);
        }
    }







    // Stub implementations for remaining functions - to be completed
    void drawNormalContent(ITerminal* terminal, ITerminal::WindowHandle window,
                          const std::vector<std::filesystem::directory_entry>& entries,
                          int selected_index) {
        terminal->clearWindow(window);
        terminal->drawBorder(window);

        // Get window dimensions
        int max_y, max_x;
        terminal->getWindowSize(window, max_x, max_y);

        // Show directory contents if a directory is selected
        if (!entries.empty() && selected_index < entries.size()) {
            const auto& selected_entry = entries[selected_index];

            std::error_code ec;
            if (selected_entry.is_directory(ec) && !ec) {
                drawDirectoryContentsInWindow(terminal, selected_entry.path(), window);
            } else {
                // Show file preview or placeholder for files
                int center_y = max_y / 2;
                terminal->centerText(window, center_y - 1, "File Preview");
                terminal->centerText(window, center_y + 1, "File details shown in info panel below");
            }
        } else {
            // Show welcome/placeholder content
            int center_y = max_y / 2;
            terminal->centerText(window, center_y - 2, "Content Area");
            terminal->centerText(window, center_y, "Directory contents shown here");
            terminal->centerText(window, center_y + 2, "Use arrow keys to navigate files");
            terminal->centerText(window, center_y + 3, "Press Enter to open directories");
        }
    }

    void drawHelpContent(ITerminal* terminal, ITerminal::WindowHandle window) {
        terminal->clearWindow(window);
        terminal->drawBorder(window);

        terminal->drawText(window, 2, 2, "Welcome to quickView!");
        terminal->drawText(window, 3, 2, "=====================");
        terminal->drawText(window, 5, 2, "File Browser Navigation:");
        terminal->drawText(window, 6, 4, "UP/DOWN  - Navigate file list");
        terminal->drawText(window, 7, 4, "PgUp/PgDn- Page through file list");
        terminal->drawText(window, 8, 4, "HOME/END - Go to top/bottom of list");
        terminal->drawText(window, 9, 4, "ENTER    - Enter directory/select file");
        terminal->drawText(window, 11, 2, "File Viewing (press 'v' on a file):");
        terminal->drawText(window, 12, 4, "UP/DOWN  - Scroll line by line");
        terminal->drawText(window, 13, 4, "PgUp/PgDn- Scroll page by page");
        terminal->drawText(window, 14, 4, "HOME/END - Go to top/bottom");
        terminal->drawText(window, 16, 2, "Interface Layout:");
        terminal->drawText(window, 17, 4, "Left Panel    - File browser");
        terminal->drawText(window, 18, 4, "Top Right     - Directory/file contents");
        terminal->drawText(window, 19, 4, "Bottom Right  - File/directory information");
        terminal->drawText(window, 20, 4, "Status Bar    - Current selection details");
        terminal->drawText(window, 22, 2, "General Commands:");
        terminal->drawText(window, 23, 4, "v, V     - View files (opens images in viewer)");
        terminal->drawText(window, 24, 4, "h, H     - Show this help");
        terminal->drawText(window, 25, 4, "a, A     - Show about information");
        terminal->drawText(window, 26, 4, "q, Q     - Quit application");
        terminal->drawText(window, 27, 4, "ESC      - Quit application");

        terminal->drawText(window, 29, 2, "Press any key to start browsing files...");
    }

    void drawAboutContent(ITerminal* terminal, ITerminal::WindowHandle window) {
        terminal->clearWindow(window);
        terminal->drawBorder(window);

        terminal->centerText(window, 5, "quickView v1.0");
        terminal->centerText(window, 7, "A C++ NCurses GUI Application");
        terminal->centerText(window, 9, "by Thomas Borland");
        terminal->centerText(window, 11, "Built with modern C++17");
        terminal->centerText(window, 13, "Press any key to return...");
    }

    void drawFileViewContent(ITerminal* terminal, ITerminal::WindowHandle window,
                            const std::vector<std::string>& file_lines,
                            int scroll_offset,
                            const std::string& filename) {
        terminal->clearWindow(window);
        terminal->drawBorder(window);

        // Get window dimensions
        int max_y, max_x;
        terminal->getWindowSize(window, max_x, max_y);

        if (file_lines.empty()) {
            terminal->centerText(window, max_y / 2, "No file content to display");
            terminal->centerText(window, max_y / 2 + 2, "Press any key to return...");
            return;
        }

        // Draw title
        std::string display_filename = filename;
        // Truncate filename if too long
        if (display_filename.length() > max_x - 6) {
            display_filename = display_filename.substr(0, max_x - 9) + "...";
        }
        terminal->drawText(window, 1, 2, "File: " + display_filename);

        // Draw horizontal line
        terminal->drawHorizontalLine(window, 2, 2, max_x - 4);

        // Display file content
        int display_height = max_y - 5;  // Account for borders, title, and bottom margin
        int start_line = 3;

        for (int i = 0; i < display_height && (i + scroll_offset) < file_lines.size(); i++) {
            int line_index = i + scroll_offset;
            const std::string& line = file_lines[line_index];

            // Truncate line if too long for window
            std::string display_line = line;
            if (display_line.length() > max_x - 4) {
                display_line = display_line.substr(0, max_x - 4);
            }

            terminal->drawText(window, start_line + i, 2, display_line);
        }

        // Show scroll indicator and controls
        if (file_lines.size() > display_height) {
            std::string scroll_info = "Lines " + std::to_string(scroll_offset + 1) + "-" +
                                    std::to_string(std::min(scroll_offset + display_height, (int)file_lines.size())) +
                                    " of " + std::to_string(file_lines.size()) +
                                    " | UP/DOWN:scroll PgUp/PgDn:page HOME/END:top/bottom ESC:exit";
            terminal->drawText(window, max_y - 2, 2, scroll_info);
        } else {
            terminal->drawText(window, max_y - 2, 2, "UP/DOWN:scroll PgUp/PgDn:page HOME/END:top/bottom ESC:exit");
        }
    }



    void drawDirectoryContentsInWindow(ITerminal* terminal, const std::filesystem::path& dir_path, ITerminal::WindowHandle window) {
        // Get window dimensions (border already drawn by parent)
        int max_y, max_x;
        terminal->getWindowSize(window, max_x, max_y);

        // Draw title
        terminal->drawText(window, 1, 2, "Contents:");

        // Show full directory path with proper truncation
        std::string full_path = dir_path.string();
        std::string display_path;

        // Apply standard path shortening technique
        int available_width = max_x - 6; // Account for borders and brackets
        if (full_path.length() > available_width) {
            // Show beginning and end of path with "..." in middle
            int prefix_len = available_width / 3;
            int suffix_len = available_width - prefix_len - 3; // 3 for "..."

            if (prefix_len > 0 && suffix_len > 0) {
                display_path = full_path.substr(0, prefix_len) + "..." +
                              full_path.substr(full_path.length() - suffix_len);
            } else {
                // Fallback: just truncate from end
                display_path = full_path.substr(0, available_width - 3) + "...";
            }
        } else {
            display_path = full_path;
        }

        terminal->drawText(window, 2, 2, display_path);

        // Draw horizontal line
        terminal->drawHorizontalLine(window, 3, 2, max_x - 4);

        std::vector<std::filesystem::directory_entry> preview_entries;
        if (FileOperations::loadDirectoryPreview(dir_path, preview_entries)) {
            // Display entries
            int display_height = max_y - 6;  // Account for borders, title, and bottom margin
            int entries_shown = 0;

            for (const auto& entry : preview_entries) {
                if (entries_shown >= display_height) break;

                std::string filename = entry.path().filename().string();

                // Truncate filename if too long
                if (filename.length() > max_x - 6) {
                    filename = filename.substr(0, max_x - 9) + "...";
                }

                // Add directory indicator and color
                std::error_code ec;
                if (entry.is_directory(ec) && !ec) {
                    filename = "[" + filename + "]";
                    // Use color for directories if available
                    if (terminal->hasColors()) {
                        terminal->setTextAttribute(window, ITerminal::DIRECTORY);
                    }
                }

                terminal->drawText(window, 4 + entries_shown, 2, filename);

                // Turn off color
                if (terminal->hasColors()) {
                    terminal->clearTextAttribute(window, ITerminal::DIRECTORY);
                }

                entries_shown++;
            }

            // Show overflow indicator at bottom if needed
            if (preview_entries.size() > display_height) {
                terminal->drawText(window, max_y - 2, 2, "... and " + std::to_string(preview_entries.size() - display_height) + " more items");
            }
        } else {
            terminal->drawText(window, 5, 2, "Error reading directory");
        }
    }
}
