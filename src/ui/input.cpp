#include "input.h"
#include "../core/quickview.h"
#include "../utils/utils.h"

namespace Input {
    void handleInput(QuickView* app) {
        int key = app->getTerminal()->getKey();
        Utils::debugPrint(app->isDebugEnabled(), "Key pressed: %d ('%c')\n", key, (key >= 32 && key <= 126) ? key : '?');
        processKey(app, key);
    }

    void processKey(QuickView* app, int key) {
        // Handle file view mode with scrolling
        if (app->getCurrentDisplayMode() == QuickView::DisplayMode::FILE_VIEW) {
            if (processFileViewKey(app, key)) {
                return;
            } else {
                // Any other key returns to normal mode
                app->setDisplayMode(QuickView::DisplayMode::NORMAL);
                app->setNeedsRedraw(true);
                app->setStatusMessage("Use arrows to navigate, Enter to select, 'v' to view files, 'h' for help, 'q' to quit");
                return;
            }
        }
        
        // Handle other special modes
        if (app->getCurrentDisplayMode() == QuickView::DisplayMode::HELP || 
            app->getCurrentDisplayMode() == QuickView::DisplayMode::ABOUT) {
            // Any key returns to normal mode
            app->setDisplayMode(QuickView::DisplayMode::NORMAL);
            app->setNeedsRedraw(true);
            app->setStatusMessage("Use arrows to navigate, Enter to select, 'v' to view files, 'h' for help, 'q' to quit");
            return;
        }
        
        // Normal mode key handling
        switch (key) {
            case 'q':
            case 'Q':
            case ITerminal::KEY_ESCAPE_KEY:
                app->setRunning(false);
                app->setStatusMessage("Goodbye!");
                break;

            case 'h':
            case 'H':
                app->setDisplayMode(QuickView::DisplayMode::HELP);
                app->setNeedsRedraw(true);
                app->setStatusMessage("Help screen - Press any key to return");
                break;

            case 'a':
            case 'A':
                app->setDisplayMode(QuickView::DisplayMode::ABOUT);
                app->setNeedsRedraw(true);
                app->setStatusMessage("About screen - Press any key to return");
                break;

            case 'v':
            case 'V':
                app->viewFile();
                break;

            case ITerminal::KEY_UP_ARROW:
                app->navigateUp();
                break;

            case ITerminal::KEY_DOWN_ARROW:
                app->navigateDown();
                break;

            case ITerminal::KEY_PAGE_UP:
                app->navigatePageUp();
                break;

            case ITerminal::KEY_PAGE_DOWN:
                app->navigatePageDown();
                break;

            case ITerminal::KEY_HOME_KEY:
                app->navigateHome();
                break;

            case ITerminal::KEY_END_KEY:
                app->navigateEnd();
                break;

            case ITerminal::KEY_ENTER_KEY:
            case '\n':
            case '\r':
                app->enterDirectory();
                break;

            case ITerminal::KEY_RESIZE_EVENT:
                app->resizeHandler();
                break;
                
            default:
                app->setStatusMessage("Use arrows to navigate, Enter to select, 'v' to view files, 'h' for help, 'q' to quit");
                break;
        }
    }

    bool processFileViewKey(QuickView* app, int key) {
        switch (key) {
            case ITerminal::KEY_UP_ARROW:
                app->scrollFileViewUp();
                return true;
            case ITerminal::KEY_DOWN_ARROW:
                app->scrollFileViewDown();
                return true;
            case ITerminal::KEY_PAGE_UP:
                app->scrollFileViewPageUp();
                return true;
            case ITerminal::KEY_PAGE_DOWN:
                app->scrollFileViewPageDown();
                return true;
            case ITerminal::KEY_HOME_KEY:
                app->scrollFileViewHome();
                return true;
            case ITerminal::KEY_END_KEY:
                app->scrollFileViewEnd();
                return true;
            default:
                return false; // Key not handled
        }
    }
}
