#!/bin/bash
# add_gitkeep.sh - Adds .gitkeep files to empty directories
# Usage: ./scripts/add_gitkeep.sh

set -e

# Function to display usage
usage() {
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  -h, --help     Show this help message"
    echo "  -d, --dry-run  Show what would be done without making changes"
    echo "  -v, --verbose  Verbose output"
    echo ""
    echo "This script adds .gitkeep files to empty directories that are not ignored."
    echo "Excluded directories: .git, build, node_modules, .vscode, .idea"
}

# Parse command line arguments
DRY_RUN=false
VERBOSE=false

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
        *)
            echo "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
done

# Directories to exclude from processing
EXCLUDE_PATTERN='\.git|build|node_modules|\.vscode|\.idea|\.tmp|dist|out'

# Counter for added files
COUNT=0

echo "Searching for empty directories..."

# Create a temporary file to track count
COUNT_FILE=$(mktemp)
echo "0" > "$COUNT_FILE"

# Find empty directories and process them
find . -type d -empty -not -path "./.git/*" | while read dir; do
    # Skip excluded directories
    if [[ "$dir" =~ $EXCLUDE_PATTERN ]]; then
        if [[ "$VERBOSE" == true ]]; then
            echo "Skipping excluded directory: $dir"
        fi
        continue
    fi
    
    # Check if .gitkeep already exists
    if [[ -f "$dir/.gitkeep" ]]; then
        if [[ "$VERBOSE" == true ]]; then
            echo "Already has .gitkeep: $dir"
        fi
        continue
    fi
    
    if [[ "$DRY_RUN" == true ]]; then
        echo "Would add .gitkeep to: $dir"
    else
        touch "$dir/.gitkeep"
        echo "Added .gitkeep to: $dir"
    fi
    
    # Increment counter
    COUNT=$(cat "$COUNT_FILE")
    ((COUNT++))
    echo "$COUNT" > "$COUNT_FILE"
done

# Read final count
COUNT=$(cat "$COUNT_FILE")
rm -f "$COUNT_FILE"

if [[ $COUNT -eq 0 ]]; then
    echo "No empty directories found that need .gitkeep files."
else
    if [[ "$DRY_RUN" == true ]]; then
        echo "Would add .gitkeep to $COUNT directories."
    else
        echo "Added .gitkeep to $COUNT directories."
        echo ""
        echo "To commit these changes:"
        echo "  git add ."
        echo "  git commit -m \"Add .gitkeep files to preserve empty directory structure\""
    fi
fi