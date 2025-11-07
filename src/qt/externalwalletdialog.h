// Copyright (c) 2024 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_EXTERNALWALLETDIALOG_H
#define BITCOIN_QT_EXTERNALWALLETDIALOG_H

#include <QDialog>
#include <QString>
#include <QAbstractButton>

namespace Ui {
class ExternalWalletDialog;
}

class WalletModel;

/** Dialog for connecting external wallets (MetaMask, Coinbase) */
class ExternalWalletDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExternalWalletDialog(QWidget *parent = nullptr);
    ~ExternalWalletDialog();

    void setModel(WalletModel *model);

public Q_SLOTS:
    void accept() override;
    void reject() override;

private Q_SLOTS:
    void onProviderChanged(int index);
    void onConnectClicked();
    void onDisconnectClicked();
    void onRefreshClicked();
    void updateConnectionList();

Q_SIGNALS:
    void walletConnected(const QString& provider, const QString& address, const QString& sessionId);
    void walletDisconnected(const QString& sessionId);

private:
    Ui::ExternalWalletDialog *ui;
    WalletModel *walletModel;
    
    QString currentSessionId;
    bool isConnected;

    void setupUi();
    bool connectWallet(const QString& provider, const QString& address);
    bool disconnectWallet(const QString& sessionId);
    void showConnectionStatus(bool connected, const QString& message);
};

#endif // BITCOIN_QT_EXTERNALWALLETDIALOG_H
