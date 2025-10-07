#!/bin/bash
# remove_gitkeep.sh - Removes .gitkeep files from directories that now have other content
# Usage: ./scripts/remove_gitkeep.sh

set -e

# Function to display usage
usage() {
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  -h, --help     Show this help message"
    echo "  -d, --dry-run  Show what would be done without making changes"
    echo "  -v, --verbose  Verbose output"
    echo "  -f, --force    Remove all .gitkeep files (even from empty directories)"
    echo ""
    echo "This script removes .gitkeep files from directories that now contain other files."
    echo "By default, it keeps .gitkeep files in truly empty directories."
}

# Parse command line arguments
DRY_RUN=false
VERBOSE=false
FORCE=false

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            usage
            exit 0
            ;;
        -d|--dry-run)
            DRY_RUN=true
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -f|--force)
            FORCE=true
            shift
            ;;
        *)
            echo "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
done

# Counter for removed files
COUNT=0

echo "Searching for .gitkeep files..."

# Create a temporary file to track count
COUNT_FILE=$(mktemp)
echo "0" > "$COUNT_FILE"

# Find all .gitkeep files and process them
find . -name ".gitkeep" -type f | while read file; do
    dir=$(dirname "$file")
    
    # Count files in the directory (excluding .gitkeep)
    file_count=$(find "$dir" -maxdepth 1 -type f ! -name ".gitkeep" | wc -l)
    
    if [[ $file_count -gt 0 || "$FORCE" == true ]]; then
        # Directory has other files or force removal is enabled
        if [[ "$DRY_RUN" == true ]]; then
            if [[ "$FORCE" == true ]]; then
                echo "Would remove .gitkeep from: $dir (force removal)"
            else
                echo "Would remove .gitkeep from: $dir (directory has $file_count other files)"
            fi
        else
            rm "$file"
            if [[ "$FORCE" == true ]]; then
                echo "Removed .gitkeep from: $dir (force removal)"
            else
                echo "Removed .gitkeep from: $dir (directory has $file_count other files)"
            fi
        fi
        # Increment counter
        COUNT=$(cat "$COUNT_FILE")
        ((COUNT++))
        echo "$COUNT" > "$COUNT_FILE"
    else
        # Directory is still empty, keep .gitkeep
        if [[ "$VERBOSE" == true ]]; then
            echo "Keeping .gitkeep in empty directory: $dir"
        fi
    fi
done

# Read final count
COUNT=$(cat "$COUNT_FILE")
rm -f "$COUNT_FILE"

if [[ $COUNT -eq 0 ]]; then
    echo "No .gitkeep files found that need removal."
else
    if [[ "$DRY_RUN" == true ]]; then
        echo "Would remove $COUNT .gitkeep files."
    else
        echo "Removed $COUNT .gitkeep files."
        echo ""
        echo "To commit these changes:"
        echo "  git add ."
        echo "  git commit -m \"Remove unnecessary .gitkeep files\""
    fi
fi