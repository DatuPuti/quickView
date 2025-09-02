#include "terminal_interface.h"

// Platform-specific includes
#ifdef _WIN32
    #include "../ui/windows/windows_terminal.h"
#else
    #include "../ui/ncurses/ncurses_terminal.h"
#endif

std::unique_ptr<ITerminal> createTerminal() {
#ifdef _WIN32
    return std::make_unique<WindowsTerminal>();
#else
    return std::make_unique<NCursesTerminal>();
#endif
}
