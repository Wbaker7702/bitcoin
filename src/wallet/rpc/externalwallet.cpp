// Copyright (c) 2024 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <wallet/rpc/util.h>
#include <wallet/externalwallet.h>
#include <wallet/wallet.h>
#include <rpc/server.h>
#include <rpc/util.h>
#include <util/translation.h>

#include <univalue.h>

namespace wallet {

RPCHelpMan connectexternalwallet()
{
    return RPCHelpMan{"connectexternalwallet",
        "\nConnect an external wallet provider (MetaMask or Coinbase).\n",
        {
            {"provider", RPCArg::Type::STR, RPCArg::Optional::NO, "Wallet provider: 'metamask' or 'coinbase'"},
            {"address", RPCArg::Type::STR, RPCArg::Optional::NO, "Wallet address"},
            {"session_id", RPCArg::Type::STR, RPCArg::Optional::OMITTED, "Optional session identifier"},
        },
        RPCResult{
            RPCResult::Type::OBJ, "", "",
            {
                {RPCResult::Type::BOOL, "success", "Whether the connection was successful"},
                {RPCResult::Type::STR, "provider", "Wallet provider name"},
                {RPCResult::Type::STR, "address", "Connected wallet address"},
                {RPCResult::Type::STR, "session_id", "Session identifier"},
            }
        },
        RPCExamples{
            HelpExampleCli("connectexternalwallet", "\"metamask\" \"0x1234567890abcdef1234567890abcdef12345678\"")
            + HelpExampleRpc("connectexternalwallet", "\"coinbase\", \"0x1234567890abcdef1234567890abcdef12345678\", \"session123\"")
        },
        [&](const RPCHelpMan& self, const JSONRPCRequest& request) -> UniValue
        {
            std::string provider_str = request.params[0].get_str();
            std::string address = request.params[1].get_str();
            std::string session_id = request.params.size() > 2 ? request.params[2].get_str() : 
                                     (provider_str + "_" + address.substr(0, 8));

            ExternalWalletProvider provider = ExternalWalletManager::ParseProvider(provider_str);
            if (provider == ExternalWalletProvider::UNKNOWN) {
                throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid provider. Use 'metamask' or 'coinbase'");
            }

            // Get or create external wallet manager (singleton pattern would be used in production)
            static ExternalWalletManager manager;
            
            bool success = manager.ConnectWallet(provider, address, session_id);

            UniValue result(UniValue::VOBJ);
            result.pushKV("success", success);
            result.pushKV("provider", ExternalWalletManager::GetProviderName(provider));
            result.pushKV("address", address);
            result.pushKV("session_id", session_id);
            
            return result;
        },
    };
}

RPCHelpMan disconnectexternalwallet()
{
    return RPCHelpMan{"disconnectexternalwallet",
        "\nDisconnect an external wallet.\n",
        {
            {"session_id", RPCArg::Type::STR, RPCArg::Optional::NO, "Session identifier"},
        },
        RPCResult{
            RPCResult::Type::OBJ, "", "",
            {
                {RPCResult::Type::BOOL, "success", "Whether the disconnection was successful"},
                {RPCResult::Type::STR, "session_id", "Session identifier"},
            }
        },
        RPCExamples{
            HelpExampleCli("disconnectexternalwallet", "\"session123\"")
            + HelpExampleRpc("disconnectexternalwallet", "\"session123\"")
        },
        [&](const RPCHelpMan& self, const JSONRPCRequest& request) -> UniValue
        {
            std::string session_id = request.params[0].get_str();

            static ExternalWalletManager manager;
            bool success = manager.DisconnectWallet(session_id);

            UniValue result(UniValue::VOBJ);
            result.pushKV("success", success);
            result.pushKV("session_id", session_id);
            
            return result;
        },
    };
}

RPCHelpMan listexternalwallets()
{
    return RPCHelpMan{"listexternalwallets",
        "\nList all connected external wallets.\n",
        {},
        RPCResult{
            RPCResult::Type::ARR, "", "",
            {
                {RPCResult::Type::OBJ, "", "",
                {
                    {RPCResult::Type::STR, "provider", "Wallet provider name"},
                    {RPCResult::Type::STR, "address", "Wallet address"},
                    {RPCResult::Type::STR, "session_id", "Session identifier"},
                    {RPCResult::Type::BOOL, "connected", "Connection status"},
                    {RPCResult::Type::NUM, "connected_time", "Unix timestamp of connection"},
                }},
            }
        },
        RPCExamples{
            HelpExampleCli("listexternalwallets", "")
            + HelpExampleRpc("listexternalwallets", "")
        },
        [&](const RPCHelpMan& self, const JSONRPCRequest& request) -> UniValue
        {
            static ExternalWalletManager manager;
            auto connections = manager.ListConnections();

            UniValue result(UniValue::VARR);
            for (const auto& conn : connections) {
                UniValue obj(UniValue::VOBJ);
                obj.pushKV("provider", ExternalWalletManager::GetProviderName(conn.provider));
                obj.pushKV("address", conn.address);
                obj.pushKV("session_id", conn.session_id);
                obj.pushKV("connected", conn.is_connected);
                obj.pushKV("connected_time", conn.connected_time);
                result.push_back(obj);
            }
            
            return result;
        },
    };
}

RPCHelpMan getexternalwalletinfo()
{
    return RPCHelpMan{"getexternalwalletinfo",
        "\nGet information about a connected external wallet.\n",
        {
            {"session_id", RPCArg::Type::STR, RPCArg::Optional::NO, "Session identifier"},
        },
        RPCResult{
            RPCResult::Type::OBJ, "", "",
            {
                {RPCResult::Type::STR, "provider", "Wallet provider name"},
                {RPCResult::Type::STR, "address", "Wallet address"},
                {RPCResult::Type::STR, "session_id", "Session identifier"},
                {RPCResult::Type::BOOL, "connected", "Connection status"},
                {RPCResult::Type::NUM, "connected_time", "Unix timestamp of connection"},
            }
        },
        RPCExamples{
            HelpExampleCli("getexternalwalletinfo", "\"session123\"")
            + HelpExampleRpc("getexternalwalletinfo", "\"session123\"")
        },
        [&](const RPCHelpMan& self, const JSONRPCRequest& request) -> UniValue
        {
            std::string session_id = request.params[0].get_str();

            static ExternalWalletManager manager;
            auto conn = manager.GetConnection(session_id);

            if (!conn) {
                throw JSONRPCError(RPC_INVALID_PARAMETER, "External wallet session not found");
            }

            UniValue result(UniValue::VOBJ);
            result.pushKV("provider", ExternalWalletManager::GetProviderName(conn->provider));
            result.pushKV("address", conn->address);
            result.pushKV("session_id", conn->session_id);
            result.pushKV("connected", conn->is_connected);
            result.pushKV("connected_time", conn->connected_time);
            
            return result;
        },
    };
}

} // namespace wallet
