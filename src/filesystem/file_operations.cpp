#include "file_operations.h"
#include "../utils/utils.h"
#include <algorithm>
#include <cctype>

namespace FileOperations {
    std::string loadDirectory(const std::filesystem::path& path, 
                             std::vector<std::filesystem::directory_entry>& entries,
                             bool debug_enabled) {
        try {
            entries.clear();
            entries.reserve(1000); // Reserve space to avoid reallocations
            
            // Add parent directory entry if not at root
            if (path.has_parent_path() && path != path.root_path()) {
                entries.emplace_back(path.parent_path() / "..");
            }
            
            // Load directory contents with error handling for individual entries
            std::error_code ec;
            for (const auto& entry : std::filesystem::directory_iterator(path, ec)) {
                if (ec) {
                    Utils::debugPrint(debug_enabled, "Error reading directory entry: %s\n", ec.message().c_str());
                    continue;
                }
                
                // Skip entries that cause errors
                std::error_code entry_ec;
                if (entry.exists(entry_ec) && !entry_ec) {
                    entries.push_back(entry);
                }
            }
            
            // Fast sort with optimized comparator
            std::sort(entries.begin(), entries.end(),
                      [debug_enabled](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) {
                          // Cache the directory status to avoid repeated filesystem calls
                          std::error_code ec_a, ec_b;
                          bool a_is_dir = a.is_directory(ec_a);
                          bool b_is_dir = b.is_directory(ec_b);
                          
                          // Handle errors gracefully
                          if (ec_a || ec_b) {
                              return a.path().filename().string() < b.path().filename().string();
                          }
                          
                          // Directories come first
                          if (a_is_dir && !b_is_dir) return true;
                          if (!a_is_dir && b_is_dir) return false;
                          
                          // Within same type, sort alphabetically (case-insensitive)
                          std::string name_a = a.path().filename().string();
                          std::string name_b = b.path().filename().string();
                          std::transform(name_a.begin(), name_a.end(), name_a.begin(), ::tolower);
                          std::transform(name_b.begin(), name_b.end(), name_b.begin(), ::tolower);
                          return name_a < name_b;
                      });
            
            Utils::debugPrint(debug_enabled, "Loaded directory: %s (%zu entries)\n", path.string().c_str(), entries.size());
            
            return "Loaded " + std::to_string(entries.size()) + " entries";
            
        } catch (const std::exception& e) {
            Utils::debugPrint(debug_enabled, "Error loading directory %s: %s\n", path.string().c_str(), e.what());
            return "Error loading directory: " + std::string(e.what());
        }
    }
    
    bool loadDirectoryPreview(const std::filesystem::path& dir_path,
                             std::vector<std::filesystem::directory_entry>& preview_entries,
                             int max_entries) {
        try {
            preview_entries.clear();
            preview_entries.reserve(max_entries);
            
            std::error_code ec;
            int count = 0;
            
            for (const auto& entry : std::filesystem::directory_iterator(dir_path, ec)) {
                if (ec) {
                    return false;
                }
                
                preview_entries.push_back(entry);
                
                // Limit entries for performance
                if (++count >= max_entries) {
                    break;
                }
            }
            
            // Sort entries: directories first, then files
            std::sort(preview_entries.begin(), preview_entries.end(),
                      [](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) {
                          std::error_code ec_a, ec_b;
                          bool a_is_dir = a.is_directory(ec_a);
                          bool b_is_dir = b.is_directory(ec_b);
                          
                          if (ec_a || ec_b) {
                              return a.path().filename().string() < b.path().filename().string();
                          }
                          
                          if (a_is_dir && !b_is_dir) return true;
                          if (!a_is_dir && b_is_dir) return false;
                          return a.path().filename().string() < b.path().filename().string();
                      });
            
            return true;
            
        } catch (const std::exception& e) {
            return false;
        }
    }
}
