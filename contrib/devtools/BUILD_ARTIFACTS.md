# Bitcoin Core Build Artifacts

This document describes the build artifacts that can be generated from the Bitcoin Core source code.

## Build Artifacts Overview

When building Bitcoin Core, the following artifacts are generated:

### Core Binaries

1. **bitcoind** - Bitcoin Core daemon (server)
   - Location: `src/bitcoind`
   - Purpose: Main Bitcoin node software
   - Runtime: Can run as daemon or foreground process

2. **bitcoin-cli** - Bitcoin Core RPC client
   - Location: `src/bitcoin-cli`
   - Purpose: Command-line interface for interacting with bitcoind
   - Usage: `bitcoin-cli [options] <command> [params]`

3. **bitcoin-tx** - Bitcoin transaction utility
   - Location: `src/bitcoin-tx`
   - Purpose: Create and manipulate Bitcoin transactions
   - Usage: `bitcoin-tx [options] <command> [params]`

4. **bitcoin-wallet** - Bitcoin wallet utility
   - Location: `src/bitcoin-wallet`
   - Purpose: Wallet management operations
   - Usage: `bitcoin-wallet [options] <command> [params]`

5. **bitcoin-util** - Bitcoin utility
   - Location: `src/bitcoin-util`
   - Purpose: Various utility functions
   - Usage: `bitcoin-util [options] <command> [params]`

### GUI Application

6. **bitcoin-qt** - Bitcoin Core GUI (Qt)
   - Location: `src/qt/bitcoin-qt`
   - Purpose: Graphical user interface for Bitcoin Core
   - Requirements: Qt 5.x development libraries
   - Can be disabled with: `--without-gui`

### Test Binaries

7. **test_bitcoin** - Unit test suite
   - Location: `src/test/test_bitcoin`
   - Purpose: Run unit tests for Bitcoin Core
   - Usage: `./src/test/test_bitcoin [test_name]`

8. **test_bitcoin-qt** - Qt GUI test suite
   - Location: `src/qt/test/test_bitcoin-qt`
   - Purpose: Run GUI unit tests
   - Requires: Qt 5.x

### Benchmark Tools

9. **bench_bitcoin** - Benchmark suite
   - Location: `src/bench/bench_bitcoin`
   - Purpose: Performance benchmarking
   - Usage: `./src/bench/bench_bitcoin [benchmark_name]`

### Libraries

The build process also generates several static libraries:

- `libbitcoin_common.a` - Common utilities
- `libbitcoin_consensus.a` - Consensus code
- `libbitcoin_crypto.a` - Cryptographic functions
- `libbitcoin_node.a` - Node functionality
- `libbitcoin_util.a` - Utility functions
- `libbitcoin_wallet.a` - Wallet functionality (if enabled)
- `libbitcoin_cli.a` - CLI functionality
- `libbitcoin_zmq.a` - ZMQ interface (if enabled)
- `libbitcoin_qt.a` - Qt GUI library (if enabled)

## Building Artifacts

### Prerequisites

Essential build tools:
```bash
sudo apt-get install build-essential libtool autotools-dev automake pkg-config python3
```

### Basic Build Steps

1. **Generate configure script** (if not present):
   ```bash
   ./autogen.sh
   ```

2. **Configure the build**:
   ```bash
   ./configure
   ```
   
   Common configure options:
   - `--disable-wallet` - Build without wallet support
   - `--without-gui` - Build without Qt GUI
   - `--enable-debug` - Enable debug symbols
   - `--prefix=/usr/local` - Set installation prefix

3. **Build the project**:
   ```bash
   make -j$(nproc)
   ```
   
   Use `-jN` to specify number of parallel jobs.

4. **Install** (optional):
   ```bash
   make install
   ```

### Using the Build Script

A helper script is provided to automate the build process:

```bash
# Check dependencies
./contrib/devtools/build-artifacts.sh --check-deps-only

# Configure only
./contrib/devtools/build-artifacts.sh --configure-only

# Full build
./contrib/devtools/build-artifacts.sh

# Build without wallet
./contrib/devtools/build-artifacts.sh --disable-wallet

# Build without GUI
./contrib/devtools/build-artifacts.sh --disable-gui

# Debug build
./contrib/devtools/build-artifacts.sh --enable-debug
```

## Build Options

### Minimal Build (No Wallet, No GUI)

```bash
./configure --disable-wallet --without-gui
make -j$(nproc)
```

This generates:
- `bitcoind`
- `bitcoin-cli`
- `bitcoin-tx`
- `bitcoin-util`
- `test_bitcoin`

### Full Build (All Features)

```bash
./configure
make -j$(nproc)
```

This generates all artifacts including:
- All core binaries
- `bitcoin-qt` (GUI)
- Wallet support
- All test binaries

### Debug Build

```bash
./configure --enable-debug
make -j$(nproc)
```

Includes debug symbols for debugging.

## Artifact Locations

After building, artifacts are located in:

- **Binaries**: `src/` directory
- **Libraries**: `src/.libs/` and component directories
- **Object files**: Component-specific `.o` files
- **Installation**: As specified by `--prefix` (default: `/usr/local`)

## Verifying Build Artifacts

After building, verify artifacts exist:

```bash
# Check core binaries
ls -lh src/bitcoind src/bitcoin-cli src/bitcoin-tx src/bitcoin-wallet src/bitcoin-util

# Check GUI (if built)
ls -lh src/qt/bitcoin-qt

# Check test binaries
ls -lh src/test/test_bitcoin

# Get file sizes
du -sh src/bitcoind src/bitcoin-cli src/bitcoin-tx
```

## Testing Build Artifacts

### Run Unit Tests

```bash
./src/test/test_bitcoin
```

### Run Benchmarks

```bash
./src/bench/bench_bitcoin
```

### Test bitcoind

```bash
# Start bitcoind in regtest mode
./src/bitcoind -regtest -daemon

# Test with bitcoin-cli
./src/bitcoin-cli -regtest getblockchaininfo

# Stop bitcoind
./src/bitcoin-cli -regtest stop
```

## Build Dependencies

### Required Dependencies

- **libevent** - Event notification library
- **boost** - C++ libraries
- **SQLite** - Database (for descriptor wallet)

### Optional Dependencies

- **Berkeley DB** - Legacy wallet support
- **Qt 5** - GUI support
- **ZMQ** - ZeroMQ messaging
- **miniupnpc/natpmp** - Port mapping

### Using Depends

Bitcoin Core includes a `depends` system for building dependencies:

```bash
cd depends
make
cd ..
./configure --prefix=$(pwd)/depends/$(depends/config.guess)
make
```

## Build Size Estimates

Typical sizes for built artifacts:

- `bitcoind`: ~50-100 MB (with debug symbols: ~200-300 MB)
- `bitcoin-cli`: ~5-10 MB
- `bitcoin-tx`: ~5-10 MB
- `bitcoin-wallet`: ~5-10 MB
- `bitcoin-qt`: ~20-50 MB (depends on Qt)

## Troubleshooting

### Configure Fails

- Install missing dependencies
- Check error messages for specific requirements
- Use `depends` system if system packages are incompatible

### Build Fails

- Check compiler version (GCC 7+ or Clang 5+)
- Ensure sufficient memory (1.5GB+ recommended)
- Try reducing parallel jobs: `make -j1`
- Check for error messages in output

### Missing Features

- Verify dependencies are installed
- Check configure output for disabled features
- Review `config.log` for details

## CI/CD Integration

The build artifacts can be integrated into CI/CD pipelines:

```bash
# Example CI build
./autogen.sh
./configure --disable-wallet --without-gui
make -j$(nproc)
make check  # Run tests
```

## Additional Resources

- [build-unix.md](../../doc/build-unix.md) - Detailed Unix build instructions
- [build-osx.md](../../doc/build-osx.md) - macOS build instructions
- [build-windows.md](../../doc/build-windows.md) - Windows build instructions
- [developer-notes.md](../../doc/developer-notes.md) - Development guidelines
