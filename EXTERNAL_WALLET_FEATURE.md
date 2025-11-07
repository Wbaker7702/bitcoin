# External Wallet Integration Feature

## Overview
This feature adds support for integrating external wallet providers (MetaMask and Coinbase) into Bitcoin Core through RPC commands and a GUI interface.

## Components Added

### 1. Backend Support
**Files Created:**
- `src/wallet/externalwallet.h` - External wallet manager interface
- `src/wallet/externalwallet.cpp` - External wallet connection management
- `src/wallet/rpc/externalwallet.cpp` - RPC commands for external wallet operations

**Key Classes:**
- `ExternalWalletManager` - Manages connections to external wallets
- `ExternalWalletConnection` - Represents a connection to an external wallet
- `ExternalWalletProvider` - Enum for wallet providers (MetaMask, Coinbase)

### 2. RPC Commands
Four new RPC commands have been added to the wallet category:

#### `connectexternalwallet`
Connect an external wallet provider.
```bash
bitcoin-cli connectexternalwallet "metamask" "0x1234567890abcdef1234567890abcdef12345678"
bitcoin-cli connectexternalwallet "coinbase" "0x1234567890abcdef1234567890abcdef12345678" "session123"
```

**Parameters:**
- `provider` (string, required): Wallet provider ('metamask' or 'coinbase')
- `address` (string, required): Wallet address
- `session_id` (string, optional): Session identifier

**Returns:**
```json
{
  "success": true,
  "provider": "MetaMask",
  "address": "0x1234567890abcdef1234567890abcdef12345678",
  "session_id": "metamask_0x123456"
}
```

#### `disconnectexternalwallet`
Disconnect an external wallet.
```bash
bitcoin-cli disconnectexternalwallet "session123"
```

**Parameters:**
- `session_id` (string, required): Session identifier

**Returns:**
```json
{
  "success": true,
  "session_id": "session123"
}
```

#### `listexternalwallets`
List all connected external wallets.
```bash
bitcoin-cli listexternalwallets
```

**Returns:**
```json
[
  {
    "provider": "MetaMask",
    "address": "0x1234567890abcdef1234567890abcdef12345678",
    "session_id": "metamask_0x123456",
    "connected": true,
    "connected_time": 1699356000
  }
]
```

#### `getexternalwalletinfo`
Get information about a specific external wallet connection.
```bash
bitcoin-cli getexternalwalletinfo "session123"
```

**Parameters:**
- `session_id` (string, required): Session identifier

**Returns:**
```json
{
  "provider": "Coinbase Wallet",
  "address": "0x1234567890abcdef1234567890abcdef12345678",
  "session_id": "session123",
  "connected": true,
  "connected_time": 1699356000
}
```

### 3. GUI Integration
**Files Created:**
- `src/qt/externalwalletdialog.h` - External wallet dialog interface
- `src/qt/externalwalletdialog.cpp` - External wallet dialog implementation

**Files Modified:**
- `src/qt/bitcoingui.h` - Added external wallet action
- `src/qt/bitcoingui.cpp` - Integrated external wallet dialog into File menu

**GUI Features:**
- Provider selection (MetaMask or Coinbase Wallet)
- Wallet address input
- Connect/Disconnect buttons
- Active connections list
- Connection status display
- Accessible from File → External Wallet… menu

### 4. RPC Registration
**Files Modified:**
- `src/wallet/rpc/wallet.cpp` - Added external wallet RPC commands to registration

## Usage Examples

### Via RPC
```bash
# Connect MetaMask wallet
bitcoin-cli connectexternalwallet "metamask" "0xABCDEF1234567890"

# List connected wallets
bitcoin-cli listexternalwallets

# Get info about a specific connection
bitcoin-cli getexternalwalletinfo "metamask_0xABCDEF12"

# Disconnect wallet
bitcoin-cli disconnectexternalwallet "metamask_0xABCDEF12"
```

### Via GUI
1. Open Bitcoin Core GUI
2. Navigate to File → External Wallet…
3. Select provider (MetaMask or Coinbase Wallet)
4. Enter wallet address
5. Click "Connect"
6. View active connections in the list
7. Click "Disconnect" to remove connection

## Technical Details

### ExternalWalletManager
The manager maintains a map of session IDs to connection objects and provides:
- Connection management (connect/disconnect)
- Connection retrieval and listing
- Signature verification (placeholder for future implementation)
- Provider name parsing and formatting

### Session Management
Sessions are identified by unique session IDs, which can be:
- Auto-generated: `{provider}_{address_prefix}_{timestamp}`
- User-provided: Custom session identifier

### Security Considerations
- This is a basic integration framework
- Signature verification is stubbed for future implementation
- No private keys are stored in Bitcoin Core
- External wallet addresses are tracked for reference only

## Future Enhancements
1. Implement actual signature verification for each provider
2. Add transaction signing through external wallets
3. Support for additional wallet providers
4. Enhanced security with encrypted session storage
5. WebSocket/API integration for real-time wallet communication
6. Multi-signature support with external wallets

## Build Integration
The new files need to be added to the build system:
- Add to `src/Makefile.am` for wallet sources
- Add to Qt build files for GUI components
- Include in CMake configuration if applicable

## Testing
Manual testing can be performed using:
- RPC commands via `bitcoin-cli`
- GUI dialog from File menu
- Integration with actual MetaMask/Coinbase browser extensions (requires additional implementation)
