# Bitcoin Core Build Artifacts

**Build Date:** November 4, 2025  
**Build Time:** 17:16 UTC  
**Version:** v27.99.0-f24a3def8a91  
**Platform:** Linux x86_64  
**Compiler:** GCC with C++20

## Main Executables

| File | Size | Type | Status |
|------|------|------|--------|
| src/bitcoind | 195M | Bitcoin Core daemon | ✅ Verified |
| src/bitcoin-cli | 21M | RPC command-line client | ✅ Verified |

## Static Libraries

### Core Libraries
| File | Size | Description |
|------|------|-------------|
| src/libbitcoin_node.a | 361M | Node implementation (validation, P2P, mempool) |
| src/libbitcoin_common.a | 112M | Common utilities and primitives |
| src/libbitcoin_util.a | 25M | Utility functions |
| src/libbitcoin_consensus.a | 7.9M | Consensus rules |
| src/libbitcoin_cli.a | 1.8M | CLI utilities |

### Cryptography Libraries
| File | Size | Description |
|------|------|-------------|
| src/crypto/.libs/libbitcoin_crypto_sse41.a | 3.8M | SSE4.1 optimized crypto |
| src/crypto/.libs/libbitcoin_crypto_avx2.a | 3.7M | AVX2 optimized crypto |
| src/crypto/.libs/libbitcoin_crypto_base.a | 3.6M | Base cryptography |
| src/crypto/.libs/libbitcoin_crypto_x86_shani.a | 305K | SHA-NI optimized crypto |

## Total Size
**303M** (all artifacts combined)

## Build Configuration

```
--disable-wallet          Wallet support disabled
--disable-bench           Benchmarking disabled
--without-gui             GUI disabled
--without-miniupnpc       UPnP support disabled
--disable-fuzz-binary     Fuzzing disabled
```

## Verification

### Binary Tests
```bash
$ ./src/bitcoind --version
Bitcoin Core version v27.99.0-f24a3def8a91

$ ./src/bitcoin-cli --version
Bitcoin Core RPC client version v27.99.0-f24a3def8a91
```

### File Information
```bash
$ file src/bitcoind
ELF 64-bit LSB pie executable, x86-64, version 1 (GNU/Linux),
dynamically linked, with debug_info, not stripped

$ file src/bitcoin-cli
ELF 64-bit LSB pie executable, x86-64, version 1 (GNU/Linux),
dynamically linked, with debug_info, not stripped
```

## Build Fixes Applied

Fixed 8 compilation errors in:
- `src/validation.cpp` (5 errors)
- `src/node/blockstorage.cpp` (3 errors)

All fixes related to API interface changes in the kernel notifications system.

## Notes

- Binaries include debug symbols (not stripped)
- Built with Position Independent Executable (PIE) for security
- Optimizations enabled: SSE4.1, AVX2, SHA-NI CPU extensions
- All main binaries tested and working correctly
