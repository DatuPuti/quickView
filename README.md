# quickView

A fast, lightweight **cross-platform** file browser built with modern C++17. Works natively on Windows, Linux, and macOS through a clean platform abstraction layer.

## 🚀 Features

- **Cross-Platform**: Native support for Windows, Linux, and macOS
- **Fast Navigation**: Lightning-fast file browsing with optimized performance
- **Image Integration**: Seamlessly launches external image viewers
- **Text File Viewing**: Built-in text file viewer with scrolling
- **Smart Interface**: Responsive layout with file information panels
- **Modern Architecture**: Clean C++17 codebase with platform abstraction

## 📋 Platform Support

| Platform | Status | Implementation |
|----------|--------|----------------|
| **Linux** | ✅ Complete | NCurses |
| **macOS** | ✅ Complete | NCurses |
| **Windows** | ✅ Complete | Windows Console API |

## 🛠️ Quick Start

### Linux/macOS

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt update && sudo apt install build-essential cmake libncurses5-dev

# Build and run
mkdir build && cd build
cmake .. && make
./quickView
```

### Windows

```cmd
# Run the build script
build_windows.bat

# Or manually:
mkdir build_windows && cd build_windows
cmake -G "Visual Studio 16 2019" -A x64 ..
cmake --build . --config Release
cd Release && quickView.exe
```

## 📋 Prerequisites

### All Platforms
- **C++17 compatible compiler**
- **CMake 3.10+**

### Linux/macOS Specific
- **NCurses development libraries**

**Ubuntu/Debian:**

```bash
sudo apt install build-essential cmake libncurses5-dev
```

**CentOS/RHEL/Fedora:**

```bash
sudo yum install gcc-c++ cmake ncurses-devel
```

**macOS:**

```bash
xcode-select --install
brew install cmake
```

### Windows Specific
- **Visual Studio 2019+** or **Visual Studio Build Tools**
- **Windows 10+** (for best console support)

## 🏗️ Building

See [CROSS_PLATFORM.md](CROSS_PLATFORM.md) for detailed build instructions and troubleshooting.

## 🎮 Usage

### Navigation
- **Arrow Keys**: Navigate file list
- **Page Up/Down**: Page through files
- **Home/End**: Jump to top/bottom
- **Enter**: Enter directories
- **v**: View files (launches image viewers for images)

### Interface
- **Left Panel**: File browser with current directory
- **Top Right**: Directory contents preview
- **Bottom Right**: File/directory information
- **Status Bar**: Current selection and application info

### Commands
- **h**: Show help screen
- **a**: Show about information
- **q/ESC**: Quit application

## 🏗️ Architecture

quickView uses a clean platform abstraction layer:

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

## 📁 Project Structure

```
quickView/
├── src/
│   ├── main.cpp                    # Application entry point
│   ├── core/                       # Core application logic
│   │   ├── quickview.h            # Main application class
│   │   └── quickview.cpp          # Core implementation
│   ├── platform/                   # Platform abstraction
│   │   ├── terminal_interface.h   # Abstract terminal interface
│   │   └── terminal_factory.cpp   # Platform detection
│   ├── ui/                         # User interface
│   │   ├── display.h/.cpp         # Display functions
│   │   ├── input.h/.cpp           # Input handling
│   │   ├── ncurses/               # NCurses implementation
│   │   └── windows/               # Windows Console implementation
│   ├── filesystem/                 # File operations
│   │   ├── file_operations.h/.cpp # Directory loading
│   │   └── image_handler.h/.cpp   # Image file handling
│   └── utils/                      # Utility functions
│       ├── utils.h                # Helper functions
│       └── utils.cpp              # Utility implementations
├── build_windows.bat              # Windows build script
├── build_windows.ps1              # PowerShell build script
├── CROSS_PLATFORM.md             # Detailed platform guide
└── CMakeLists.txt                 # Cross-platform build config
```

## 🤝 Contributing

1. **Fork the repository**
2. **Create a feature branch**: `git checkout -b feature/amazing-feature`
3. **Test on multiple platforms** when possible
4. **Follow the architecture**: Use `ITerminal` interface for UI operations
5. **Submit a pull request**

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **NCurses** - Terminal UI library for Linux/macOS
- **Windows Console API** - Native Windows terminal support
- **Modern C++** - C++17 features and best practices
