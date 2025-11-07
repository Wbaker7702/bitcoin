// Copyright (c) 2024 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <wallet/externalwallet.h>
#include <util/time.h>
#include <algorithm>

namespace wallet {

bool ExternalWalletManager::ConnectWallet(ExternalWalletProvider provider, const std::string& address, const std::string& session_id)
{
    if (session_id.empty() || address.empty()) {
        return false;
    }

    ExternalWalletConnection connection;
    connection.provider = provider;
    connection.address = address;
    connection.session_id = session_id;
    connection.is_connected = true;
    connection.connected_time = GetTime();
    
    m_connections[session_id] = connection;
    return true;
}

bool ExternalWalletManager::DisconnectWallet(const std::string& session_id)
{
    auto it = m_connections.find(session_id);
    if (it != m_connections.end()) {
        it->second.is_connected = false;
        m_connections.erase(it);
        return true;
    }
    return false;
}

std::optional<ExternalWalletConnection> ExternalWalletManager::GetConnection(const std::string& session_id) const
{
    auto it = m_connections.find(session_id);
    if (it != m_connections.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<ExternalWalletConnection> ExternalWalletManager::ListConnections() const
{
    std::vector<ExternalWalletConnection> connections;
    for (const auto& [session_id, conn] : m_connections) {
        connections.push_back(conn);
    }
    return connections;
}

bool ExternalWalletManager::VerifySignature(const std::string& session_id, const std::string& message, const std::string& signature)
{
    auto conn = GetConnection(session_id);
    if (!conn || !conn->is_connected) {
        return false;
    }
    
    // TODO: Implement actual signature verification based on provider
    // For now, return true if connection exists
    return !signature.empty() && !message.empty();
}

std::string ExternalWalletManager::GetProviderName(ExternalWalletProvider provider)
{
    switch (provider) {
        case ExternalWalletProvider::METAMASK:
            return "MetaMask";
        case ExternalWalletProvider::COINBASE:
            return "Coinbase Wallet";
        case ExternalWalletProvider::UNKNOWN:
        default:
            return "Unknown";
    }
}

ExternalWalletProvider ExternalWalletManager::ParseProvider(const std::string& provider_str)
{
    std::string lower = provider_str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "metamask") {
        return ExternalWalletProvider::METAMASK;
    } else if (lower == "coinbase") {
        return ExternalWalletProvider::COINBASE;
    }
    return ExternalWalletProvider::UNKNOWN;
}

} // namespace wallet
