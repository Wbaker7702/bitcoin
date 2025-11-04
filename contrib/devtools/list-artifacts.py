#!/usr/bin/env python3
"""
List Bitcoin Core Build Artifacts
===================================

A script to list and describe build artifacts that would be generated
when building Bitcoin Core.

Usage:
    python3 list-artifacts.py [--check-built] [--json]
"""

import os
import json
import argparse
from pathlib import Path

# Expected build artifacts
ARTIFACTS = {
    'core': {
        'bitcoind': {
            'path': 'src/bitcoind',
            'description': 'Bitcoin Core daemon (server)',
            'required': True,
            'size_mb': '~50-100',
        },
        'bitcoin-cli': {
            'path': 'src/bitcoin-cli',
            'description': 'Bitcoin Core RPC client',
            'required': True,
            'size_mb': '~5-10',
        },
        'bitcoin-tx': {
            'path': 'src/bitcoin-tx',
            'description': 'Bitcoin transaction utility',
            'required': True,
            'size_mb': '~5-10',
        },
        'bitcoin-wallet': {
            'path': 'src/bitcoin-wallet',
            'description': 'Bitcoin wallet utility',
            'required': True,
            'size_mb': '~5-10',
        },
        'bitcoin-util': {
            'path': 'src/bitcoin-util',
            'description': 'Bitcoin utility functions',
            'required': True,
            'size_mb': '~5-10',
        },
    },
    'gui': {
        'bitcoin-qt': {
            'path': 'src/qt/bitcoin-qt',
            'description': 'Bitcoin Core GUI (Qt)',
            'required': False,
            'size_mb': '~20-50',
            'requires': 'Qt 5.x',
        },
    },
    'tests': {
        'test_bitcoin': {
            'path': 'src/test/test_bitcoin',
            'description': 'Unit test suite',
            'required': False,
            'size_mb': '~10-20',
        },
        'test_bitcoin-qt': {
            'path': 'src/qt/test/test_bitcoin-qt',
            'description': 'Qt GUI test suite',
            'required': False,
            'size_mb': '~5-10',
            'requires': 'Qt 5.x',
        },
    },
    'benchmarks': {
        'bench_bitcoin': {
            'path': 'src/bench/bench_bitcoin',
            'description': 'Performance benchmark suite',
            'required': False,
            'size_mb': '~5-10',
        },
    },
}

def format_size(size_bytes):
    """Format bytes to human readable size."""
    for unit in ['B', 'KB', 'MB', 'GB']:
        if size_bytes < 1024.0:
            return f"{size_bytes:.1f} {unit}"
        size_bytes /= 1024.0
    return f"{size_bytes:.1f} TB"

def check_artifact(root_dir, artifact_path):
    """Check if artifact exists and get its size."""
    full_path = Path(root_dir) / artifact_path
    if full_path.exists() and full_path.is_file():
        try:
            size = full_path.stat().st_size
            return {'exists': True, 'size': size, 'size_formatted': format_size(size)}
        except OSError:
            return {'exists': False}
    return {'exists': False}

def list_artifacts(root_dir='.', check_built=False, json_output=False):
    """List all expected build artifacts."""
    root = Path(root_dir)
    results = {}
    
    for category, artifacts in ARTIFACTS.items():
        results[category] = {}
        for name, info in artifacts.items():
            artifact_info = {
                'name': name,
                'path': info['path'],
                'description': info['description'],
                'required': info['required'],
                'estimated_size': info.get('size_mb', 'unknown'),
            }
            
            if 'requires' in info:
                artifact_info['requires'] = info['requires']
            
            if check_built:
                check_result = check_artifact(root, info['path'])
                artifact_info['built'] = check_result['exists']
                if check_result['exists']:
                    artifact_info['actual_size'] = check_result['size_formatted']
            
            results[category][name] = artifact_info
    
    if json_output:
        print(json.dumps(results, indent=2))
    else:
        print_artifacts(results, check_built)
    
    return results

def print_artifacts(results, check_built):
    """Print artifacts in human-readable format."""
    print("Bitcoin Core Build Artifacts")
    print("=" * 60)
    print()
    
    for category, artifacts in results.items():
        print(f"{category.upper()}")
        print("-" * 60)
        
        for name, info in artifacts.items():
            status = ""
            if check_built:
                if info.get('built'):
                    status = f" ✓ BUILT ({info.get('actual_size', 'unknown')})"
                else:
                    status = " ✗ NOT BUILT"
            
            required = " [REQUIRED]" if info['required'] else ""
            print(f"  {name:20} {status}{required}")
            print(f"    Path: {info['path']}")
            print(f"    Description: {info['description']}")
            if 'requires' in info:
                print(f"    Requires: {info['requires']}")
            print(f"    Estimated size: {info['estimated_size']} MB")
            print()
    
    if check_built:
        total_built = sum(1 for cat in results.values() 
                         for art in cat.values() if art.get('built'))
        total_expected = sum(len(cat) for cat in results.values())
        print(f"Summary: {total_built}/{total_expected} artifacts built")
        print()

def main():
    parser = argparse.ArgumentParser(
        description='List Bitcoin Core build artifacts'
    )
    parser.add_argument(
        '--root',
        default='.',
        help='Root directory of Bitcoin Core repository'
    )
    parser.add_argument(
        '--check-built',
        action='store_true',
        help='Check if artifacts are actually built'
    )
    parser.add_argument(
        '--json',
        action='store_true',
        help='Output in JSON format'
    )
    
    args = parser.parse_args()
    
    list_artifacts(args.root, args.check_built, args.json)

if __name__ == '__main__':
    main()
