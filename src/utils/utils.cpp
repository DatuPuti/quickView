#include "utils.h"
#include <cstdarg>
#include <cstdio>

namespace Utils {
    void debugPrint(bool debug_enabled, const char* format, ...) {
        if (!debug_enabled) return;

        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
    }
}
