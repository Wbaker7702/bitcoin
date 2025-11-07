// Copyright (c) 2024 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_WALLET_EXTERNALWALLET_H
#define BITCOIN_WALLET_EXTERNALWALLET_H

#include <string>
#include <map>
#include <memory>
#include <optional>

namespace wallet {

enum class ExternalWalletProvider {
    METAMASK,
    COINBASE,
    UNKNOWN
};

struct ExternalWalletConnection {
    ExternalWalletProvider provider;
    std::string address;
    std::string session_id;
    bool is_connected;
    int64_t connected_time;
    std::map<std::string, std::string> metadata;
};

class ExternalWalletManager {
public:
    ExternalWalletManager() = default;
    ~ExternalWalletManager() = default;

    // Connect to external wallet
    bool ConnectWallet(ExternalWalletProvider provider, const std::string& address, const std::string& session_id);
    
    // Disconnect external wallet
    bool DisconnectWallet(const std::string& session_id);
    
    // Get connection status
    std::optional<ExternalWalletConnection> GetConnection(const std::string& session_id) const;
    
    // List all active connections
    std::vector<ExternalWalletConnection> ListConnections() const;
    
    // Verify wallet signature
    bool VerifySignature(const std::string& session_id, const std::string& message, const std::string& signature);
    
    // Get provider name
    static std::string GetProviderName(ExternalWalletProvider provider);
    
    // Parse provider from string
    static ExternalWalletProvider ParseProvider(const std::string& provider_str);

private:
    std::map<std::string, ExternalWalletConnection> m_connections;
};

} // namespace wallet

#endif // BITCOIN_WALLET_EXTERNALWALLET_H
