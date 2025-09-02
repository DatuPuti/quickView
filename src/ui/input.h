#ifndef INPUT_H
#define INPUT_H

// Forward declaration
class QuickView;

/**
 * @brief Input handling functionality
 */
namespace Input {
    /**
     * @brief Handle input from the user
     * @param app Pointer to the QuickView application instance
     */
    void handleInput(QuickView* app);
    
    /**
     * @brief Process a key press
     * @param app Pointer to the QuickView application instance
     * @param key Key code that was pressed
     */
    void processKey(QuickView* app, int key);
    
    /**
     * @brief Process file view mode keys (scrolling)
     * @param app Pointer to the QuickView application instance
     * @param key Key code that was pressed
     * @return true if key was handled, false otherwise
     */
    bool processFileViewKey(QuickView* app, int key);
}

#endif // INPUT_H
