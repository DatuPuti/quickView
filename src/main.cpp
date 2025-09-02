#include "core/quickview.h"
#include <iostream>
#include <exception>
#include <cstring>

/**
 * @brief Main entry point for the quickView application
 * 
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return int Exit code (0 for success, non-zero for error)
 */
int main(int argc, char* argv[]) {
    try {
        // Check for debug flag
        bool debug_mode = false;
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "--debug") == 0 || strcmp(argv[i], "-d") == 0) {
                debug_mode = true;
                break;
            }
        }

        // Create application instance
        QuickView app(debug_mode);
        
        // Initialize the application
        if (!app.initialize()) {
            std::cerr << "Failed to initialize quickView application" << std::endl;
            return 1;
        }
        
        // Run the main application loop
        app.run();

        // Application will clean up automatically via destructor
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}
