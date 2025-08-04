#!/usr/bin/env python3
"""
============================================================================
Caelis - Code Formatting Script
============================================================================
This script formats C++ source code using clang-format.
"""

import os
import sys
import subprocess
import argparse
from pathlib import Path

def find_source_files(directory):
    """Find all C++ source files in the directory."""
    extensions = ['.cpp', '.c', '.h', '.hpp', '.cc', '.cxx']
    source_files = []
    
    for root, dirs, files in os.walk(directory):
        # Skip build and external directories
        dirs[:] = [d for d in dirs if d not in ['build', 'external', '.git']]
        
        for file in files:
            if any(file.endswith(ext) for ext in extensions):
                source_files.append(os.path.join(root, file))
    
    return source_files

def check_clang_format():
    """Check if clang-format is available."""
    try:
        result = subprocess.run(['clang-format', '--version'], 
                              capture_output=True, text=True)
        if result.returncode == 0:
            print(f"Found clang-format: {result.stdout.strip()}")
            return True
    except FileNotFoundError:
        pass
    
    print("ERROR: clang-format not found!")
    print("Please install clang-format:")
    print("  Windows: Install LLVM from https://llvm.org/builds/")
    print("  Linux: sudo apt install clang-format (or equivalent)")
    print("  macOS: brew install clang-format")
    return False

def format_file(file_path, dry_run=False):
    """Format a single file with clang-format."""
    try:
        if dry_run:
            # Check if formatting would change the file
            result = subprocess.run(['clang-format', '--dry-run', '--Werror', file_path],
                                  capture_output=True, text=True)
            return result.returncode != 0  # Returns True if formatting needed
        else:
            # Format the file in-place
            result = subprocess.run(['clang-format', '-i', file_path],
                                  capture_output=True, text=True)
            return result.returncode == 0  # Returns True if successful
    except Exception as e:
        print(f"Error formatting {file_path}: {e}")
        return False

def main():
    parser = argparse.ArgumentParser(description='Format C++ source code with clang-format')
    parser.add_argument('--check', action='store_true', 
                       help='Check if files need formatting (dry run)')
    parser.add_argument('--directory', '-d', default='.',
                       help='Directory to format (default: current directory)')
    
    args = parser.parse_args()
    
    # Check if clang-format is available
    if not check_clang_format():
        return 1
    
    # Find source files
    print(f"Searching for source files in: {os.path.abspath(args.directory)}")
    source_files = find_source_files(args.directory)
    
    if not source_files:
        print("No source files found.")
        return 0
    
    print(f"Found {len(source_files)} source files.")
    
    if args.check:
        print("Checking formatting...")
        files_needing_format = []
        
        for file_path in source_files:
            if format_file(file_path, dry_run=True):
                files_needing_format.append(file_path)
                print(f"  NEEDS FORMATTING: {file_path}")
            else:
                print(f"  OK: {file_path}")
        
        if files_needing_format:
            print(f"\n{len(files_needing_format)} files need formatting.")
            print("Run without --check to format them.")
            return 1
        else:
            print("\nAll files are properly formatted!")
            return 0
    else:
        print("Formatting files...")
        formatted_count = 0
        
        for file_path in source_files:
            if format_file(file_path, dry_run=False):
                print(f"  FORMATTED: {file_path}")
                formatted_count += 1
            else:
                print(f"  ERROR: {file_path}")
        
        print(f"\nFormatted {formatted_count}/{len(source_files)} files.")
        return 0

if __name__ == '__main__':
    sys.exit(main())