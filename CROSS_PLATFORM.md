# quickView - Cross-Platform Build Guide

quickView is a modern C++17 file browser that works on Windows, Linux, and macOS through a clean platform abstraction layer.

## Platform Support

| Platform | Status | Implementation | Notes |
|----------|--------|----------------|-------|
| Linux    | ✅ Complete | NCurses | Fully tested and working |
| macOS    | ✅ Complete | NCurses | Same as Linux implementation |
| Windows  | ✅ Complete | Windows Console API | Native Windows implementation |

## Architecture

quickView uses a platform abstraction layer (`ITerminal` interface) that allows the same core application logic to run on different platforms:

```
Core Application (Platform Independent)
    ↓
ITerminal Interface (Abstract)
    ↓
┌─────────────────┬─────────────────┐
│  NCursesTerminal │ WindowsTerminal │
│  (Linux/macOS)   │   (Windows)     │
└─────────────────┴─────────────────┘
```

## Building

### Linux/macOS

**Prerequisites:**
- CMake 3.10+
- GCC 7+ or Clang 5+
- NCurses development libraries

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install build-essential cmake libncurses5-dev
```

**CentOS/RHEL/Fedora:**
```bash
sudo yum install gcc-c++ cmake ncurses-devel
# or for newer versions:
sudo dnf install gcc-c++ cmake ncurses-devel
```

**macOS:**
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install CMake (via Homebrew)
brew install cmake
```

**Build:**
```bash
mkdir build
cd build
cmake ..
make
./quickView
```

### Windows

**Prerequisites:**
- Visual Studio 2019+ or Visual Studio Build Tools
- CMake 3.10+

**Option 1: Using Batch Script**
```cmd
build_windows.bat
```

**Option 2: Using PowerShell**
```powershell
.\build_windows.ps1
```

**Option 3: Manual Build**
```cmd
mkdir build_windows
cd build_windows
cmake -G "Visual Studio 16 2019" -A x64 ..
cmake --build . --config Release
```

**Run:**
```cmd
cd build_windows\Release
quickView.exe
```

## Platform-Specific Features

### Linux/macOS (NCurses)
- Full color support
- Unicode character support
- Terminal resize handling
- Optimized for terminal environments

### Windows (Console API)
- Native Windows Console integration
- Windows color scheme support
- Proper Windows key handling
- Console buffer management

## Development

### Adding New Platforms

To add support for a new platform:

1. Create new terminal implementation:
   ```cpp
   class NewPlatformTerminal : public ITerminal {
       // Implement all ITerminal methods
   };
   ```

2. Update `terminal_factory.cpp`:
   ```cpp
   #ifdef NEW_PLATFORM
       return std::make_unique<NewPlatformTerminal>();
   #endif
   ```

3. Update `CMakeLists.txt` with platform detection and libraries.

### Platform Abstraction Benefits

- **Single Codebase**: Core logic works on all platforms
- **Native Performance**: Each platform uses optimal APIs
- **Maintainable**: Platform-specific code isolated
- **Testable**: Each component can be tested independently
- **Extensible**: Easy to add new platforms or features

## Troubleshooting

### Linux/macOS Issues

**NCurses not found:**
```bash
# Ubuntu/Debian
sudo apt install libncurses5-dev

# CentOS/RHEL
sudo yum install ncurses-devel
```

**Compiler too old:**
- Requires C++17 support (GCC 7+, Clang 5+)
- Update compiler or use newer distribution

### Windows Issues

**Visual Studio not found:**
- Install Visual Studio 2019+ or Visual Studio Build Tools
- Ensure C++ development tools are installed

**CMake not found:**
- Download from https://cmake.org/download/
- Add to PATH environment variable

**Console encoding issues:**
- Run `chcp 65001` to set UTF-8 encoding
- Or use Windows Terminal for better Unicode support

## Performance

quickView is optimized for performance on all platforms:

- **Smart Redraw**: Only updates changed screen areas
- **Efficient Navigation**: Minimal filesystem operations
- **Memory Management**: Optimized data structures
- **Platform Optimizations**: Uses best APIs for each platform

## Contributing

When contributing cross-platform code:

1. Test on multiple platforms when possible
2. Use the `ITerminal` interface for all UI operations
3. Keep platform-specific code in terminal implementations
4. Update build scripts and documentation for new features

## License

quickView is released under the MIT License. See LICENSE file for details.
