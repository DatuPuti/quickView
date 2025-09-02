# Examples Directory

This directory contains practical examples demonstrating how to handle empty directories in Git.

## Empty Directories Example

The `empty_dirs/` subdirectory demonstrates different approaches to maintaining empty directory structure in Git:

- `cache/` - Contains `.gitkeep` for a directory intended for cache files
- `temp/` - Contains `.gitkeep` for temporary files directory  
- `plugins/` - Contains `.gitkeep` for future plugin extensions
- `future_expansion/` - Example of a directory for planned features

## Usage

To see how the scripts work:

```bash
# Add .gitkeep files to empty directories
./scripts/add_gitkeep.sh

# Remove .gitkeep files from directories that now have content
./scripts/remove_gitkeep.sh

# Use dry-run to see what would happen
./scripts/add_gitkeep.sh --dry-run
./scripts/remove_gitkeep.sh --dry-run
```

## Real-world Scenarios

1. **Project Setup**: Use these techniques when setting up project structure
2. **Team Collaboration**: Ensure all developers have the same directory structure
3. **Build Systems**: Preserve directories needed by build tools
4. **Deployment**: Maintain required directory structure in production

See the main documentation at `docs/GIT_EMPTY_DIRECTORIES.md` for comprehensive information.