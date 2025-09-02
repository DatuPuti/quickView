#include "image_handler.h"
#include "../utils/utils.h"
#include <algorithm>
#include <cctype>
#include <cstdlib>

namespace ImageHandler {
    bool isImageFile(const std::filesystem::path& file_path) {
        std::string extension = file_path.extension().string();
        
        // Convert to lowercase for comparison
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
        // Common image file extensions
        return extension == ".jpg" || extension == ".jpeg" || 
               extension == ".png" || extension == ".gif" || 
               extension == ".bmp" || extension == ".tiff" || 
               extension == ".tif" || extension == ".webp" || 
               extension == ".svg" || extension == ".ico" || 
               extension == ".ppm" || extension == ".pgm" || 
               extension == ".pbm" || extension == ".xpm" ||
               extension == ".pcx" || extension == ".tga";
    }

    void launchImageViewer(const std::filesystem::path& image_path, bool debug_enabled) {
        std::string command;
        std::string file_path = image_path.string();
        
        // Escape spaces and special characters in the file path
        std::string escaped_path;
        for (char c : file_path) {
            if (c == ' ' || c == '\'' || c == '"' || c == '\\' || c == '&' || c == ';' || c == '|') {
                escaped_path += '\\';
            }
            escaped_path += c;
        }
        
        // Determine the appropriate command based on the operating system
        #ifdef __APPLE__
            // macOS - use 'open' command, suppress all output
            command = "open " + escaped_path + " >/dev/null 2>&1 &";
        #elif __linux__
            // Linux - try xdg-open first, then common image viewers, suppress all output
            command = "xdg-open " + escaped_path + " >/dev/null 2>&1 || "
                     "eog " + escaped_path + " >/dev/null 2>&1 || "
                     "feh " + escaped_path + " >/dev/null 2>&1 || "
                     "display " + escaped_path + " >/dev/null 2>&1 || "
                     "gwenview " + escaped_path + " >/dev/null 2>&1 || "
                     "ristretto " + escaped_path + " >/dev/null 2>&1 &";
        #elif _WIN32
            // Windows - use 'start' command, suppress output
            command = "start \"\" \"" + file_path + "\" >nul 2>&1";
        #else
            // Generic Unix - try xdg-open, suppress all output
            command = "xdg-open " + escaped_path + " >/dev/null 2>&1 &";
        #endif
        
        // Execute the command
        int result = system(command.c_str());
        
        // Debug output
        Utils::debugPrint(debug_enabled, "Launching image viewer with command: %s\n", command.c_str());
        Utils::debugPrint(debug_enabled, "Command result: %d\n", result);
        
        if (result != 0) {
            Utils::debugPrint(debug_enabled, "Warning: Image viewer command may have failed\n");
        }
    }
}
