#!/usr/bin/env python3
"""
============================================================================
Caelis - Static Analysis Script
============================================================================
This script runs static analysis tools on the C++ codebase.
"""

import os
import sys
import subprocess
import argparse
import json
from pathlib import Path

def check_tool(tool_name, install_instructions):
    """Check if a tool is available."""
    try:
        result = subprocess.run([tool_name, '--version'], 
                              capture_output=True, text=True)
        if result.returncode == 0:
            print(f"✓ Found {tool_name}")
            return True
    except FileNotFoundError:
        pass
    
    print(f"✗ {tool_name} not found")
    print(f"  Install: {install_instructions}")
    return False

def run_cppcheck(source_dirs):
    """Run cppcheck static analysis."""
    print("\n" + "="*60)
    print("Running cppcheck...")
    print("="*60)
    
    cmd = [
        'cppcheck',
        '--enable=all',
        '--inconclusive',
        '--std=c++17',
        '--suppress=missingIncludeSystem',
        '--suppress=unusedFunction',  # Often false positives in game engines
        '--quiet',
        '--template={file}:{line}: {severity}: {message} [{id}]'
    ]
    
    # Add source directories
    for source_dir in source_dirs:
        if os.path.exists(source_dir):
            cmd.append(source_dir)
    
    try:
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        if result.stdout:
            print("Issues found:")
            print(result.stdout)
        else:
            print("No issues found by cppcheck!")
        
        if result.stderr:
            print("Warnings/Info:")
            print(result.stderr)
            
        return result.returncode
    except Exception as e:
        print(f"Error running cppcheck: {e}")
        return 1

def run_clang_tidy(build_dir):
    """Run clang-tidy static analysis."""
    print("\n" + "="*60)
    print("Running clang-tidy...")
    print("="*60)
    
    compile_commands = os.path.join(build_dir, 'compile_commands.json')
    
    if not os.path.exists(compile_commands):
        print(f"ERROR: {compile_commands} not found!")
        print("Please build the project with CMAKE_EXPORT_COMPILE_COMMANDS=ON")
        return 1
    
    # Find source files
    source_files = []
    for root, dirs, files in os.walk('.'):
        dirs[:] = [d for d in dirs if d not in ['build', 'external', '.git']]
        for file in files:
            if file.endswith(('.cpp', '.cc', '.cxx')):
                source_files.append(os.path.join(root, file))
    
    if not source_files:
        print("No source files found!")
        return 0
    
    print(f"Analyzing {len(source_files)} source files...")
    
    issues_found = 0
    for source_file in source_files:
        cmd = [
            'clang-tidy',
            source_file,
            f'-p={build_dir}',
            '--quiet'
        ]
        
        try:
            result = subprocess.run(cmd, capture_output=True, text=True)
            
            if result.stdout and 'warning:' in result.stdout:
                print(f"\nIssues in {source_file}:")
                print(result.stdout)
                issues_found += 1
                
        except Exception as e:
            print(f"Error analyzing {source_file}: {e}")
    
    if issues_found == 0:
        print("No issues found by clang-tidy!")
    else:
        print(f"\nclang-tidy found issues in {issues_found} files.")
    
    return 0

def run_include_what_you_use():
    """Run include-what-you-use analysis."""
    print("\n" + "="*60)
    print("Running include-what-you-use...")
    print("="*60)
    
    print("include-what-you-use requires compilation with special flags.")
    print("This is complex to set up automatically. Please refer to:")
    print("https://include-what-you-use.org/")
    
    return 0

def main():
    parser = argparse.ArgumentParser(description='Run static analysis on C++ code')
    parser.add_argument('--tool', choices=['cppcheck', 'clang-tidy', 'iwyu', 'all'],
                       default='all', help='Which tool to run')
    parser.add_argument('--build-dir', default='build',
                       help='Build directory (for clang-tidy)')
    
    args = parser.parse_args()
    
    print("Caelis Static Analysis")
    print("="*60)
    
    # Define source directories
    source_dirs = ['SilicaEngine/src', 'SilicaEngine/include', 'Fractura/src']
    
    # Check available tools
    tools_available = {}
    
    if args.tool in ['cppcheck', 'all']:
        tools_available['cppcheck'] = check_tool('cppcheck', 
            'apt install cppcheck / brew install cppcheck / choco install cppcheck')
    
    if args.tool in ['clang-tidy', 'all']:
        tools_available['clang-tidy'] = check_tool('clang-tidy',
            'apt install clang-tidy / brew install llvm / install LLVM')
    
    if args.tool in ['iwyu', 'all']:
        tools_available['iwyu'] = check_tool('include-what-you-use',
            'Complex setup - see https://include-what-you-use.org/')
    
    # Run available tools
    exit_code = 0
    
    if args.tool in ['cppcheck', 'all'] and tools_available.get('cppcheck'):
        exit_code |= run_cppcheck(source_dirs)
    
    if args.tool in ['clang-tidy', 'all'] and tools_available.get('clang-tidy'):
        exit_code |= run_clang_tidy(args.build_dir)
    
    if args.tool in ['iwyu', 'all']:
        exit_code |= run_include_what_you_use()
    
    print("\n" + "="*60)
    print("Static analysis completed!")
    print("="*60)
    
    return exit_code

if __name__ == '__main__':
    sys.exit(main())