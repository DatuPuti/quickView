#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include <filesystem>
#include <vector>
#include <string>

/**
 * @brief File and directory operations
 */
namespace FileOperations {
    /**
     * @brief Load directory contents into a vector
     * @param path Directory path to load
     * @param entries Vector to store directory entries
     * @param debug_enabled Whether debug output is enabled
     * @return Status message describing the result
     */
    std::string loadDirectory(const std::filesystem::path& path, 
                             std::vector<std::filesystem::directory_entry>& entries,
                             bool debug_enabled);
    
    /**
     * @brief Load directory contents for preview (limited entries)
     * @param dir_path Directory path to preview
     * @param preview_entries Vector to store preview entries
     * @param max_entries Maximum number of entries to load
     * @return true if successful, false on error
     */
    bool loadDirectoryPreview(const std::filesystem::path& dir_path,
                             std::vector<std::filesystem::directory_entry>& preview_entries,
                             int max_entries = 200);
}

#endif // FILE_OPERATIONS_H
