#!/usr/bin/env bash
# Copyright (c) 2024 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

# Build Artifacts Script for Bitcoin Core
# This script helps build Bitcoin Core artifacts

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Default values
BUILD_DIR="${BUILD_DIR:-./build}"
PARALLEL_JOBS="${PARALLEL_JOBS:-$(nproc)}"
DISABLE_WALLET="${DISABLE_WALLET:-0}"
DISABLE_GUI="${DISABLE_GUI:-0}"
ENABLE_DEBUG="${ENABLE_DEBUG:-0}"

# Print colored output
info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if command exists
check_command() {
    if ! command -v "$1" &> /dev/null; then
        error "$1 is not installed"
        return 1
    fi
    info "$1 found: $(command -v $1)"
    return 0
}

# Check build dependencies
check_dependencies() {
    info "Checking build dependencies..."
    
    local missing=0
    
    # Essential build tools
    check_command g++ || missing=1
    check_command make || missing=1
    
    # Autotools (may not be needed if configure already exists)
    if [ ! -f "./configure" ]; then
        check_command autoreconf || warn "autoreconf not found - configure script may already exist"
        check_command autoconf || warn "autoconf not found - configure script may already exist"
        check_command automake || warn "automake not found - configure script may already exist"
        check_command libtool || warn "libtool not found - configure script may already exist"
    fi
    
    check_command pkg-config || warn "pkg-config not found - some features may be disabled"
    check_command python3 || missing=1
    
    if [ $missing -eq 1 ]; then
        error "Essential dependencies are missing. Please install them first."
        echo ""
        echo "For Ubuntu/Debian:"
        echo "  sudo apt-get install build-essential libtool autotools-dev automake pkg-config python3"
        echo ""
        echo "For Fedora:"
        echo "  sudo dnf install gcc-c++ libtool make autoconf automake python3"
        return 1
    fi
    
    return 0
}

# Generate configure script
generate_configure() {
    if [ -f "./configure" ]; then
        info "Configure script already exists"
        return 0
    fi
    
    info "Generating configure script..."
    if [ -f "./autogen.sh" ]; then
        ./autogen.sh
        info "Configure script generated successfully"
    else
        error "autogen.sh not found"
        return 1
    fi
}

# Configure the build
configure_build() {
    info "Configuring build..."
    
    local configure_args=()
    
    # Set prefix if BUILD_DIR is specified
    if [ -n "$BUILD_DIR" ]; then
        configure_args+=("--prefix=$(realpath $BUILD_DIR)")
    fi
    
    # Disable wallet if requested
    if [ "$DISABLE_WALLET" = "1" ]; then
        configure_args+=("--disable-wallet")
        info "Wallet support disabled"
    fi
    
    # Disable GUI if requested
    if [ "$DISABLE_GUI" = "1" ]; then
        configure_args+=("--without-gui")
        info "GUI support disabled"
    fi
    
    # Enable debug if requested
    if [ "$ENABLE_DEBUG" = "1" ]; then
        configure_args+=("--enable-debug")
        info "Debug build enabled"
    fi
    
    # Use depends if available
    if [ -d "./depends" ]; then
        info "Using depends directory for dependencies"
        configure_args+=("--prefix=$(pwd)/depends/$(depends/config.guess)")
    fi
    
    info "Running configure with args: ${configure_args[*]}"
    ./configure "${configure_args[@]}" || {
        error "Configure failed"
        warn "You may need to install additional dependencies:"
        warn "  libevent-dev libboost-dev libsqlite3-dev"
        warn "Or use depends: make -C depends"
        return 1
    }
}

# Build the project
build_project() {
    info "Building Bitcoin Core with $PARALLEL_JOBS parallel jobs..."
    
    make -j"$PARALLEL_JOBS" || {
        error "Build failed"
        return 1
    }
    
    info "Build completed successfully!"
}

# List build artifacts
list_artifacts() {
    info "Build artifacts:"
    echo ""
    
    local artifacts=(
        "src/bitcoind"
        "src/bitcoin-cli"
        "src/bitcoin-tx"
        "src/bitcoin-wallet"
        "src/bitcoin-util"
        "src/qt/bitcoin-qt"
        "src/test/test_bitcoin"
    )
    
    for artifact in "${artifacts[@]}"; do
        if [ -f "$artifact" ]; then
            local size=$(du -h "$artifact" | cut -f1)
            echo -e "  ${GREEN}✓${NC} $artifact ($size)"
        else
            echo -e "  ${RED}✗${NC} $artifact (not built)"
        fi
    done
    
    echo ""
    info "Build directory: $BUILD_DIR"
}

# Main function
main() {
    info "Bitcoin Core Build Artifacts Script"
    info "==================================="
    echo ""
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --build-dir)
                BUILD_DIR="$2"
                shift 2
                ;;
            --jobs|-j)
                PARALLEL_JOBS="$2"
                shift 2
                ;;
            --disable-wallet)
                DISABLE_WALLET=1
                shift
                ;;
            --disable-gui)
                DISABLE_GUI=1
                shift
                ;;
            --enable-debug)
                ENABLE_DEBUG=1
                shift
                ;;
            --check-deps-only)
                check_dependencies
                exit $?
                ;;
            --configure-only)
                check_dependencies || exit 1
                generate_configure || exit 1
                configure_build || exit 1
                exit 0
                ;;
            --help|-h)
                echo "Usage: $0 [options]"
                echo ""
                echo "Options:"
                echo "  --build-dir DIR      Build directory (default: ./build)"
                echo "  --jobs N, -j N       Number of parallel jobs (default: nproc)"
                echo "  --disable-wallet    Disable wallet support"
                echo "  --disable-gui       Disable GUI (bitcoin-qt)"
                echo "  --enable-debug      Enable debug build"
                echo "  --check-deps-only   Only check dependencies"
                echo "  --configure-only    Only run configure"
                echo "  --help, -h          Show this help message"
                exit 0
                ;;
            *)
                error "Unknown option: $1"
                echo "Use --help for usage information"
                exit 1
                ;;
        esac
    done
    
    # Run build steps
    check_dependencies || exit 1
    generate_configure || exit 1
    configure_build || exit 1
    build_project || exit 1
    list_artifacts
    
    info "Build completed successfully!"
    info "Binaries are located in: src/"
}

# Run main function
main "$@"
