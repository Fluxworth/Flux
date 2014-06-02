/*
 * Qt4 bitcoin GUI.
 *
 * W.J. van der Laan 2011-2012
 * The Bitcoin Developers 2011-2013
 */
#ifndef WALLETVIEW_H
#define WALLETVIEW_H

#include <QStackedWidget>

class FluxGUI;
class ClientModel;
class WalletModel;
class TransactionView;
class OverviewPage;
class AddressBookPage;
class MiningPage;
class StatisticsPage;
class BlockExplorer;
class SendFluxDialog;
class SignVerifyMessageDialog;
class RPCConsole;

QT_BEGIN_NAMESPACE
class QLabel;
class QModelIndex;
QT_END_NAMESPACE

/*
  WalletView class. This class represents the view to a single wallet.
  It was added to support multiple wallet functionality. Each wallet gets its own WalletView instance.
  It communicates with both the client and the wallet models to give the user an up-to-date view of the
  current core state.
*/
class WalletView : public QStackedWidget
{
    Q_OBJECT

public:
    explicit WalletView(QWidget *parent, FluxGUI *_gui);
    ~WalletView();

    void setFluxGUI(FluxGUI *gui);
    /** Set the client model.
        The client model represents the part of the core that communicates with the P2P network, and is wallet-agnostic.
    */
    void setClientModel(ClientModel *clientModel);
    /** Set the wallet model.
        The wallet model represents a Fluxs wallet, and offers access to the list of transactions, address book and sending
        functionality.
    */
    void setWalletModel(WalletModel *walletModel);

    bool handleURI(const QString &uri);

    void showOutOfSyncWarning(bool fShow);

private:
    FluxGUI *gui;
    ClientModel *clientModel;
    WalletModel *walletModel;

    OverviewPage *overviewPage;
    QWidget *transactionsPage;
    AddressBookPage *addressBookPage;
    AddressBookPage *receiveFluxPage;
    SendFluxDialog *sendFluxPage;
    MiningPage *miningPage;
    StatisticsPage *statisticsPage;
    BlockExplorer *blockExplorer;
    SignVerifyMessageDialog *signVerifyMessageDialog;

    TransactionView *transactionView;

public slots:
    /** Switch to overview (home) page */
    void gotoOverviewPage();
    /** Switch to history (transactions) page */
    void gotoHistoryPage();
    /** Switch to address book page */
    void gotoAddressBookPage();
    /** Switch to receive Flux page */
    void gotoReceiveFluxPage();
    /** Switch to send Flux page */
    void gotoSendFluxPage(QString addr = "");
    /** Switch to mining page */
    void gotoMiningPage();
    /** Switch to Statistics Page*/
    void gotoStatistics();
    /** Wut */
    void gotoBlockExplorer();

    /** Show Sign/Verify Message dialog and switch to sign message tab */
    void gotoSignMessageTab(QString addr = "");
    /** Show Sign/Verify Message dialog and switch to verify message tab */
    void gotoVerifyMessageTab(QString addr = "");

    /** Show incoming transaction notification for new transactions.

        The new items are those between start and end inclusive, under the given parent item.
    */
    void incomingTransaction(const QModelIndex& parent, int start, int /*end*/);
    /** Encrypt the wallet */
    void encryptWallet(bool status);
    /** Backup the wallet */
    void backupWallet();
    /** Change encrypted wallet passphrase */
    void changePassphrase();
    /** Ask for passphrase to unlock wallet temporarily */
    void unlockWallet();

    void setEncryptionStatus();

signals:
    /** Signal that we want to show the main window */
    void showNormalIfMinimized();
};

#endif // WALLETVIEW_H