#ifndef UTILS_H
#define UTILS_H

#include <string>

/**
 * @brief Utility functions for quickView application
 */
namespace Utils {
    /**
     * @brief Debug print function (only prints if debug enabled)
     * @param debug_enabled Whether debug is enabled
     * @param format Printf-style format string
     * @param ... Arguments for format string
     */
    void debugPrint(bool debug_enabled, const char* format, ...);
}

#endif // UTILS_H
