# External Wallet Integration - Quick Reference

## New RPC Commands

### connectexternalwallet
**Purpose:** Connect MetaMask or Coinbase wallet to Bitcoin Core

**Syntax:**
```bash
connectexternalwallet "provider" "address" ["session_id"]
```

**Arguments:**
1. `provider` (string, required) - "metamask" or "coinbase"
2. `address` (string, required) - Wallet address (e.g., "0x...")
3. `session_id` (string, optional) - Custom session identifier

**Example:**
```bash
bitcoin-cli connectexternalwallet "metamask" "0x742d35Cc6634C0532925a3b844Bc9e7595f0bEb"
```

**Result:**
```json
{
  "success": true,
  "provider": "MetaMask",
  "address": "0x742d35Cc6634C0532925a3b844Bc9e7595f0bEb",
  "session_id": "metamask_0x742d35_1699356000"
}
```

---

### disconnectexternalwallet
**Purpose:** Disconnect an external wallet

**Syntax:**
```bash
disconnectexternalwallet "session_id"
```

**Arguments:**
1. `session_id` (string, required) - Session identifier from connection

**Example:**
```bash
bitcoin-cli disconnectexternalwallet "metamask_0x742d35_1699356000"
```

**Result:**
```json
{
  "success": true,
  "session_id": "metamask_0x742d35_1699356000"
}
```

---

### listexternalwallets
**Purpose:** List all connected external wallets

**Syntax:**
```bash
listexternalwallets
```

**Example:**
```bash
bitcoin-cli listexternalwallets
```

**Result:**
```json
[
  {
    "provider": "MetaMask",
    "address": "0x742d35Cc6634C0532925a3b844Bc9e7595f0bEb",
    "session_id": "metamask_0x742d35_1699356000",
    "connected": true,
    "connected_time": 1699356000
  },
  {
    "provider": "Coinbase Wallet",
    "address": "0x123456789abcdef123456789abcdef123456789a",
    "session_id": "coinbase_custom_session",
    "connected": true,
    "connected_time": 1699356100
  }
]
```

---

### getexternalwalletinfo
**Purpose:** Get detailed information about a specific external wallet connection

**Syntax:**
```bash
getexternalwalletinfo "session_id"
```

**Arguments:**
1. `session_id` (string, required) - Session identifier

**Example:**
```bash
bitcoin-cli getexternalwalletinfo "metamask_0x742d35_1699356000"
```

**Result:**
```json
{
  "provider": "MetaMask",
  "address": "0x742d35Cc6634C0532925a3b844Bc9e7595f0bEb",
  "session_id": "metamask_0x742d35_1699356000",
  "connected": true,
  "connected_time": 1699356000
}
```

---

## GUI Access

**Menu Path:** File → External Wallet…

**Features:**
- Select wallet provider (MetaMask or Coinbase Wallet)
- Enter wallet address
- Connect/Disconnect buttons
- View active connections
- Real-time connection status

**Steps to Connect:**
1. Open Bitcoin Core
2. Click **File** → **External Wallet…**
3. Select provider from dropdown
4. Enter wallet address (0x...)
5. Click **Connect**
6. Connection appears in active list

**Steps to Disconnect:**
1. Select connection from list
2. Click **Disconnect**
3. Confirmation shown in status

---

## Integration Points

### C++ API
```cpp
#include <wallet/externalwallet.h>

// Create manager
wallet::ExternalWalletManager manager;

// Connect wallet
bool success = manager.ConnectWallet(
    wallet::ExternalWalletProvider::METAMASK,
    "0x742d35Cc6634C0532925a3b844Bc9e7595f0bEb",
    "my_session"
);

// List connections
auto connections = manager.ListConnections();

// Get specific connection
auto conn = manager.GetConnection("my_session");

// Disconnect
manager.DisconnectWallet("my_session");
```

### Provider Support
- **MetaMask:** Use provider string "metamask"
- **Coinbase Wallet:** Use provider string "coinbase"

---

## Error Handling

**Invalid Provider:**
```json
{
  "error": "Invalid provider. Use 'metamask' or 'coinbase'"
}
```

**Session Not Found:**
```json
{
  "error": "External wallet session not found"
}
```

**Missing Parameters:**
```json
{
  "error": "Required parameter missing"
}
```

---

## Session ID Format

**Auto-generated:**
```
{provider}_{address_prefix}_{timestamp}
```

Example: `metamask_0x742d35_1699356000`

**Custom:**
Any string provided by user in optional parameter

---

## Files Reference

**Backend:**
- `src/wallet/externalwallet.h` - Manager interface
- `src/wallet/externalwallet.cpp` - Implementation
- `src/wallet/rpc/externalwallet.cpp` - RPC commands

**GUI:**
- `src/qt/externalwalletdialog.h` - Dialog interface
- `src/qt/externalwalletdialog.cpp` - Dialog implementation

**Modified:**
- `src/qt/bitcoingui.h` - Added menu action
- `src/qt/bitcoingui.cpp` - Menu integration
- `src/wallet/rpc/wallet.cpp` - RPC registration

---

## Limitations (Current Version)

- Signature verification is framework-only (not yet implemented)
- No actual wallet communication (placeholder)
- Session storage is in-memory (not persistent)
- No transaction signing integration (yet)

---

## Future Enhancements

- [ ] Real wallet communication via WebSocket/HTTP
- [ ] Signature verification implementation
- [ ] Transaction signing through external wallets
- [ ] Persistent session storage
- [ ] Additional wallet provider support
- [ ] Enhanced security features
- [ ] Multi-signature support

---

*For detailed information, see EXTERNAL_WALLET_FEATURE.md*
