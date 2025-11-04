#!/usr/bin/env python3
"""
Bitcoin Core Codebase Explorer
==============================

A tool to explore and analyze the Bitcoin Core codebase structure,
generating useful reports for developers.

Usage:
    python3 codebase-explorer.py [options]

Options:
    --stats           Generate code statistics report
    --includes        Analyze include dependencies
    --classes         List all classes and their locations
    --rpc             Analyze RPC methods
    --complexity      Analyze code complexity metrics
    --all             Generate all reports
    --output DIR      Output directory for reports (default: ./codebase-reports)
"""

import os
import re
import sys
import json
import argparse
from collections import defaultdict, Counter
from pathlib import Path
from typing import Dict, List, Set, Tuple

class CodebaseExplorer:
    def __init__(self, root_dir: str):
        self.root_dir = Path(root_dir)
        self.src_dir = self.root_dir / "src"
        self.stats = {
            'files': {'cpp': 0, 'h': 0, 'py': 0, 'total': 0},
            'lines': {'cpp': 0, 'h': 0, 'py': 0, 'total': 0},
            'classes': [],
            'functions': 0,
            'includes': defaultdict(int),
            'rpc_methods': [],
        }
        
    def scan_files(self) -> List[Path]:
        """Scan for source files in the codebase."""
        files = []
        for ext in ['*.cpp', '*.h', '*.py']:
            files.extend(self.src_dir.rglob(ext))
        return files
    
    def count_lines(self, filepath: Path) -> int:
        """Count non-empty lines in a file."""
        try:
            with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                return sum(1 for line in f if line.strip())
        except Exception:
            return 0
    
    def extract_classes(self, content: str, filepath: Path) -> List[Dict]:
        """Extract class definitions from C++ code."""
        classes = []
        # Match class definitions
        pattern = r'class\s+(\w+)(?:\s*:\s*[^{]+)?\s*\{'
        for match in re.finditer(pattern, content):
            class_name = match.group(1)
            # Skip forward declarations
            if not content[match.end():match.end()+100].strip().startswith(';'):
                classes.append({
                    'name': class_name,
                    'file': str(filepath.relative_to(self.root_dir)),
                    'line': content[:match.start()].count('\n') + 1
                })
        return classes
    
    def extract_includes(self, content: str) -> List[str]:
        """Extract #include statements."""
        includes = []
        for line in content.split('\n'):
            line = line.strip()
            if line.startswith('#include'):
                # Extract the included file
                match = re.search(r'[<"]([^>"]+)[>"]', line)
                if match:
                    includes.append(match.group(1))
        return includes
    
    def extract_rpc_methods(self, content: str, filepath: Path) -> List[Dict]:
        """Extract RPC method definitions."""
        methods = []
        # Look for static RPCHelpMan definitions
        pattern = r'static\s+RPCHelpMan\s+(\w+)\s*\(\)'
        for match in re.finditer(pattern, content):
            method_name = match.group(1)
            methods.append({
                'name': method_name,
                'file': str(filepath.relative_to(self.root_dir)),
                'line': content[:match.start()].count('\n') + 1
            })
        return methods
    
    def analyze_file(self, filepath: Path):
        """Analyze a single file."""
        try:
            with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
        except Exception:
            return
        
        ext = filepath.suffix[1:]  # Remove the dot
        if ext in ['cpp', 'h']:
            # Count lines
            lines = self.count_lines(filepath)
            self.stats['lines'][ext] += lines
            self.stats['lines']['total'] += lines
            self.stats['files'][ext] += 1
            self.stats['files']['total'] += 1
            
            # Extract classes
            classes = self.extract_classes(content, filepath)
            self.stats['classes'].extend(classes)
            
            # Extract includes
            includes = self.extract_includes(content)
            for inc in includes:
                self.stats['includes'][inc] += 1
            
            # Extract RPC methods (only in RPC files)
            if 'rpc' in str(filepath):
                rpc_methods = self.extract_rpc_methods(content, filepath)
                self.stats['rpc_methods'].extend(rpc_methods)
                
        elif ext == 'py':
            lines = self.count_lines(filepath)
            self.stats['lines']['py'] += lines
            self.stats['lines']['total'] += lines
            self.stats['files']['py'] += 1
            self.stats['files']['total'] += 1
    
    def generate_stats_report(self) -> str:
        """Generate a statistics report."""
        report = []
        report.append("# Bitcoin Core Codebase Statistics\n")
        report.append("## File Statistics\n")
        report.append(f"- Total files: {self.stats['files']['total']}")
        report.append(f"  - C++ source (.cpp): {self.stats['files']['cpp']}")
        report.append(f"  - C++ headers (.h): {self.stats['files']['h']}")
        report.append(f"  - Python (.py): {self.stats['files']['py']}")
        report.append("")
        report.append("## Line Counts\n")
        report.append(f"- Total lines: {self.stats['lines']['total']:,}")
        report.append(f"  - C++ source: {self.stats['lines']['cpp']:,}")
        report.append(f"  - C++ headers: {self.stats['lines']['h']:,}")
        report.append(f"  - Python: {self.stats['lines']['py']:,}")
        report.append("")
        report.append(f"## Classes Found: {len(self.stats['classes'])}\n")
        return '\n'.join(report)
    
    def generate_includes_report(self, top_n: int = 20) -> str:
        """Generate include dependency report."""
        report = []
        report.append("# Most Frequently Included Headers\n")
        report.append(f"Top {top_n} most included headers:\n")
        
        sorted_includes = sorted(
            self.stats['includes'].items(),
            key=lambda x: x[1],
            reverse=True
        )[:top_n]
        
        for header, count in sorted_includes:
            report.append(f"- `{header}`: {count} times")
        
        return '\n'.join(report)
    
    def generate_classes_report(self) -> str:
        """Generate classes report."""
        report = []
        report.append(f"# Classes in Bitcoin Core ({len(self.stats['classes'])} total)\n")
        
        # Group by file
        by_file = defaultdict(list)
        for cls in self.stats['classes']:
            by_file[cls['file']].append(cls)
        
        report.append("## Classes by File\n")
        for filepath in sorted(by_file.keys()):
            classes = by_file[filepath]
            report.append(f"### {filepath}\n")
            for cls in classes:
                report.append(f"- `{cls['name']}` (line {cls['line']})")
            report.append("")
        
        return '\n'.join(report)
    
    def generate_rpc_report(self) -> str:
        """Generate RPC methods report."""
        report = []
        report.append(f"# RPC Methods ({len(self.stats['rpc_methods'])} found)\n")
        
        # Group by file
        by_file = defaultdict(list)
        for method in self.stats['rpc_methods']:
            by_file[method['file']].append(method)
        
        report.append("## RPC Methods by File\n")
        for filepath in sorted(by_file.keys()):
            methods = by_file[filepath]
            report.append(f"### {filepath}\n")
            for method in methods:
                report.append(f"- `{method['name']}` (line {method['line']})")
            report.append("")
        
        return '\n'.join(report)
    
    def analyze(self):
        """Run the full analysis."""
        print("Scanning codebase...")
        files = self.scan_files()
        print(f"Found {len(files)} files")
        
        for i, filepath in enumerate(files):
            if (i + 1) % 100 == 0:
                print(f"Analyzed {i + 1}/{len(files)} files...")
            self.analyze_file(filepath)
        
        print(f"Analysis complete! Found:")
        print(f"  - {len(self.stats['classes'])} classes")
        print(f"  - {len(self.stats['rpc_methods'])} RPC methods")
        print(f"  - {len(self.stats['includes'])} unique includes")


def main():
    parser = argparse.ArgumentParser(
        description='Explore and analyze Bitcoin Core codebase'
    )
    parser.add_argument(
        '--root',
        default='.',
        help='Root directory of Bitcoin Core repository (default: current directory)'
    )
    parser.add_argument(
        '--stats',
        action='store_true',
        help='Generate code statistics report'
    )
    parser.add_argument(
        '--includes',
        action='store_true',
        help='Analyze include dependencies'
    )
    parser.add_argument(
        '--classes',
        action='store_true',
        help='List all classes and their locations'
    )
    parser.add_argument(
        '--rpc',
        action='store_true',
        help='Analyze RPC methods'
    )
    parser.add_argument(
        '--all',
        action='store_true',
        help='Generate all reports'
    )
    parser.add_argument(
        '--output',
        default='./codebase-reports',
        help='Output directory for reports (default: ./codebase-reports)'
    )
    
    args = parser.parse_args()
    
    if not args.stats and not args.includes and not args.classes and not args.rpc and not args.all:
        parser.print_help()
        return
    
    explorer = CodebaseExplorer(args.root)
    explorer.analyze()
    
    output_dir = Path(args.output)
    output_dir.mkdir(exist_ok=True)
    
    if args.all or args.stats:
        report = explorer.generate_stats_report()
        output_file = output_dir / 'statistics.md'
        with open(output_file, 'w') as f:
            f.write(report)
        print(f"Statistics report written to {output_file}")
    
    if args.all or args.includes:
        report = explorer.generate_includes_report()
        output_file = output_dir / 'includes.md'
        with open(output_file, 'w') as f:
            f.write(report)
        print(f"Includes report written to {output_file}")
    
    if args.all or args.classes:
        report = explorer.generate_classes_report()
        output_file = output_dir / 'classes.md'
        with open(output_file, 'w') as f:
            f.write(report)
        print(f"Classes report written to {output_file}")
    
    if args.all or args.rpc:
        report = explorer.generate_rpc_report()
        output_file = output_dir / 'rpc-methods.md'
        with open(output_file, 'w') as f:
            f.write(report)
        print(f"RPC methods report written to {output_file}")
    
    # Save raw data as JSON
    json_file = output_dir / 'codebase-data.json'
    json_data = {
        'stats': dict(explorer.stats['files']),
        'lines': dict(explorer.stats['lines']),
        'class_count': len(explorer.stats['classes']),
        'rpc_method_count': len(explorer.stats['rpc_methods']),
        'unique_includes': len(explorer.stats['includes']),
    }
    with open(json_file, 'w') as f:
        json.dump(json_data, f, indent=2)
    print(f"Raw data saved to {json_file}")


if __name__ == '__main__':
    main()
