# Codebase Explorer Tool

A Python tool to explore and analyze the Bitcoin Core codebase structure, generating useful reports for developers.

## Features

- **Code Statistics**: Count files, lines of code, and basic metrics
- **Include Analysis**: Find the most frequently included headers
- **Class Discovery**: List all classes and their locations
- **RPC Method Analysis**: Extract and catalog RPC methods
- **JSON Export**: Export structured data for further processing

## Usage

### Basic Usage

Generate all reports:
```bash
python3 contrib/devtools/codebase-explorer.py --all
```

Generate specific reports:
```bash
# Statistics only
python3 contrib/devtools/codebase-explorer.py --stats

# Include dependencies
python3 contrib/devtools/codebase-explorer.py --includes

# Class listings
python3 contrib/devtools/codebase-explorer.py --classes

# RPC methods
python3 contrib/devtools/codebase-explorer.py --rpc
```

### Options

- `--root DIR`: Root directory of Bitcoin Core repository (default: current directory)
- `--stats`: Generate code statistics report
- `--includes`: Analyze include dependencies
- `--classes`: List all classes and their locations
- `--rpc`: Analyze RPC methods
- `--all`: Generate all reports
- `--output DIR`: Output directory for reports (default: `./codebase-reports`)

### Output

The tool generates Markdown reports in the output directory:

- `statistics.md`: Code statistics and metrics
- `includes.md`: Most frequently included headers
- `classes.md`: Complete list of classes by file
- `rpc-methods.md`: All RPC methods organized by file
- `codebase-data.json`: Raw data in JSON format

## Examples

### Quick Overview
```bash
python3 contrib/devtools/codebase-explorer.py --stats --output ./reports
```

### Complete Analysis
```bash
python3 contrib/devtools/codebase-explorer.py --all --output ./codebase-analysis
```

## Use Cases

1. **Understanding Codebase Structure**: Get a high-level overview of the codebase organization
2. **Finding Dependencies**: Identify which headers are most commonly used
3. **Locating Classes**: Quickly find where specific classes are defined
4. **RPC Documentation**: Generate a catalog of all RPC methods
5. **Code Metrics**: Track codebase size and complexity over time

## Requirements

- Python 3.6+
- Access to Bitcoin Core source code

## Limitations

- Currently analyzes C++ and Python files only
- Class extraction uses regex and may miss edge cases
- RPC method detection focuses on `static RPCHelpMan` patterns
- File encoding errors are silently ignored

## Future Enhancements

Potential improvements:
- Complexity metrics (cyclomatic complexity, etc.)
- Function extraction and analysis
- Dependency graph visualization
- Integration with other devtools
- More accurate parsing using AST
