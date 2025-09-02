#ifndef IMAGE_HANDLER_H
#define IMAGE_HANDLER_H

#include <filesystem>

/**
 * @brief Image file handling functionality
 */
namespace ImageHandler {
    /**
     * @brief Check if a file is an image based on extension
     * @param file_path Path to the file
     * @return true if file is an image, false otherwise
     */
    bool isImageFile(const std::filesystem::path& file_path);
    
    /**
     * @brief Launch the default system image viewer for a file
     * @param image_path Path to the image file
     * @param debug_enabled Whether debug output is enabled
     */
    void launchImageViewer(const std::filesystem::path& image_path, bool debug_enabled);
}

#endif // IMAGE_HANDLER_H
