// Copyright (c) 2024 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/externalwalletdialog.h>
#include <qt/walletmodel.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QListWidget>
#include <QGroupBox>
#include <QMessageBox>
#include <QDateTime>

ExternalWalletDialog::ExternalWalletDialog(QWidget *parent) :
    QDialog(parent),
    ui(nullptr),
    walletModel(nullptr),
    isConnected(false)
{
    setupUi();
    setWindowTitle(tr("External Wallet Integration"));
    resize(600, 500);
}

ExternalWalletDialog::~ExternalWalletDialog()
{
    // Cleanup if needed
}

void ExternalWalletDialog::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Provider selection group
    QGroupBox *providerGroup = new QGroupBox(tr("Connect External Wallet"), this);
    QVBoxLayout *providerLayout = new QVBoxLayout(providerGroup);
    
    QHBoxLayout *providerSelectLayout = new QHBoxLayout();
    QLabel *providerLabel = new QLabel(tr("Provider:"), this);
    QComboBox *providerCombo = new QComboBox(this);
    providerCombo->setObjectName("providerCombo");
    providerCombo->addItem(tr("MetaMask"), "metamask");
    providerCombo->addItem(tr("Coinbase Wallet"), "coinbase");
    providerSelectLayout->addWidget(providerLabel);
    providerSelectLayout->addWidget(providerCombo);
    providerLayout->addLayout(providerSelectLayout);
    
    QHBoxLayout *addressLayout = new QHBoxLayout();
    QLabel *addressLabel = new QLabel(tr("Wallet Address:"), this);
    QLineEdit *addressEdit = new QLineEdit(this);
    addressEdit->setObjectName("addressEdit");
    addressEdit->setPlaceholderText(tr("0x..."));
    addressLayout->addWidget(addressLabel);
    addressLayout->addWidget(addressEdit);
    providerLayout->addLayout(addressLayout);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *connectBtn = new QPushButton(tr("Connect"), this);
    connectBtn->setObjectName("connectBtn");
    QPushButton *disconnectBtn = new QPushButton(tr("Disconnect"), this);
    disconnectBtn->setObjectName("disconnectBtn");
    disconnectBtn->setEnabled(false);
    buttonLayout->addWidget(connectBtn);
    buttonLayout->addWidget(disconnectBtn);
    buttonLayout->addStretch();
    providerLayout->addLayout(buttonLayout);
    
    mainLayout->addWidget(providerGroup);
    
    // Connection status
    QLabel *statusLabel = new QLabel(tr("Status: Not Connected"), this);
    statusLabel->setObjectName("statusLabel");
    statusLabel->setStyleSheet("QLabel { padding: 5px; }");
    mainLayout->addWidget(statusLabel);
    
    // Active connections list
    QGroupBox *connectionsGroup = new QGroupBox(tr("Active Connections"), this);
    QVBoxLayout *connectionsLayout = new QVBoxLayout(connectionsGroup);
    
    QListWidget *connectionsList = new QListWidget(this);
    connectionsList->setObjectName("connectionsList");
    connectionsLayout->addWidget(connectionsList);
    
    QPushButton *refreshBtn = new QPushButton(tr("Refresh List"), this);
    refreshBtn->setObjectName("refreshBtn");
    connectionsLayout->addWidget(refreshBtn);
    
    mainLayout->addWidget(connectionsGroup);
    
    // Close button
    QHBoxLayout *closeLayout = new QHBoxLayout();
    closeLayout->addStretch();
    QPushButton *closeBtn = new QPushButton(tr("Close"), this);
    closeBtn->setObjectName("closeBtn");
    closeLayout->addWidget(closeBtn);
    mainLayout->addLayout(closeLayout);
    
    // Connect signals
    connect(providerCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &ExternalWalletDialog::onProviderChanged);
    connect(connectBtn, &QPushButton::clicked, this, &ExternalWalletDialog::onConnectClicked);
    connect(disconnectBtn, &QPushButton::clicked, this, &ExternalWalletDialog::onDisconnectClicked);
    connect(refreshBtn, &QPushButton::clicked, this, &ExternalWalletDialog::onRefreshClicked);
    connect(closeBtn, &QPushButton::clicked, this, &ExternalWalletDialog::reject);
}

void ExternalWalletDialog::setModel(WalletModel *model)
{
    this->walletModel = model;
}

void ExternalWalletDialog::onProviderChanged(int index)
{
    // Provider changed, could update UI based on provider
}

void ExternalWalletDialog::onConnectClicked()
{
    QComboBox *providerCombo = findChild<QComboBox*>("providerCombo");
    QLineEdit *addressEdit = findChild<QLineEdit*>("addressEdit");
    
    if (!providerCombo || !addressEdit) return;
    
    QString provider = providerCombo->currentData().toString();
    QString address = addressEdit->text().trimmed();
    
    if (address.isEmpty()) {
        QMessageBox::warning(this, tr("Input Error"), 
            tr("Please enter a wallet address."));
        return;
    }
    
    if (connectWallet(provider, address)) {
        showConnectionStatus(true, tr("Successfully connected to %1").arg(providerCombo->currentText()));
        updateConnectionList();
        
        QPushButton *connectBtn = findChild<QPushButton*>("connectBtn");
        QPushButton *disconnectBtn = findChild<QPushButton*>("disconnectBtn");
        if (connectBtn) connectBtn->setEnabled(false);
        if (disconnectBtn) disconnectBtn->setEnabled(true);
    } else {
        showConnectionStatus(false, tr("Failed to connect to external wallet"));
    }
}

void ExternalWalletDialog::onDisconnectClicked()
{
    if (!currentSessionId.isEmpty()) {
        if (disconnectWallet(currentSessionId)) {
            showConnectionStatus(false, tr("Disconnected from external wallet"));
            updateConnectionList();
            currentSessionId.clear();
            
            QPushButton *connectBtn = findChild<QPushButton*>("connectBtn");
            QPushButton *disconnectBtn = findChild<QPushButton*>("disconnectBtn");
            if (connectBtn) connectBtn->setEnabled(true);
            if (disconnectBtn) disconnectBtn->setEnabled(false);
        }
    }
}

void ExternalWalletDialog::onRefreshClicked()
{
    updateConnectionList();
}

void ExternalWalletDialog::updateConnectionList()
{
    QListWidget *list = findChild<QListWidget*>("connectionsList");
    if (!list) return;
    
    list->clear();
    
    // In a real implementation, this would fetch from the RPC backend
    // For now, show placeholder
    if (!currentSessionId.isEmpty() && isConnected) {
        QComboBox *providerCombo = findChild<QComboBox*>("providerCombo");
        QLineEdit *addressEdit = findChild<QLineEdit*>("addressEdit");
        
        if (providerCombo && addressEdit) {
            QString item = QString("%1: %2 (Session: %3)")
                .arg(providerCombo->currentText())
                .arg(addressEdit->text())
                .arg(currentSessionId);
            list->addItem(item);
        }
    }
}

bool ExternalWalletDialog::connectWallet(const QString& provider, const QString& address)
{
    // Generate session ID
    currentSessionId = QString("%1_%2_%3")
        .arg(provider)
        .arg(address.left(8))
        .arg(QDateTime::currentSecsSinceEpoch());
    
    // In a real implementation, this would call the RPC command
    // For now, simulate success
    isConnected = true;
    
    Q_EMIT walletConnected(provider, address, currentSessionId);
    return true;
}

bool ExternalWalletDialog::disconnectWallet(const QString& sessionId)
{
    // In a real implementation, this would call the RPC command
    isConnected = false;
    
    Q_EMIT walletDisconnected(sessionId);
    return true;
}

void ExternalWalletDialog::showConnectionStatus(bool connected, const QString& message)
{
    QLabel *statusLabel = findChild<QLabel*>("statusLabel");
    if (statusLabel) {
        QString status = connected ? 
            tr("Status: Connected - %1").arg(message) :
            tr("Status: %1").arg(message);
        statusLabel->setText(status);
        statusLabel->setStyleSheet(connected ? 
            "QLabel { padding: 5px; color: green; }" :
            "QLabel { padding: 5px; color: red; }");
    }
}

void ExternalWalletDialog::accept()
{
    QDialog::accept();
}

void ExternalWalletDialog::reject()
{
    QDialog::reject();
}
