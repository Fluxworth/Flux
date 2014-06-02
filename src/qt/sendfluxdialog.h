#ifndef SENDFLUXDIALOG_H
#define SENDFLUXDIALOG_H

#include <QDialog>

namespace Ui {
    class SendFluxDialog;
}
class WalletModel;
class SendFluxEntry;
class SendFluxRecipient;

QT_BEGIN_NAMESPACE
class QUrl;
QT_END_NAMESPACE

/** Dialog for sending Fluxs */
class SendFluxDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SendFluxDialog(QWidget *parent = 0);
    ~SendFluxDialog();

    void setModel(WalletModel *model);

    /** Set up the tab chain manually, as Qt messes up the tab chain by default in some cases (issue https://bugreports.qt-project.org/browse/QTBUG-10907).
     */
    QWidget *setupTabChain(QWidget *prev);

    void setAddress(const QString &address);
    void pasteEntry(const SendFluxRecipient &rv);
    bool handleURI(const QString &uri);

public slots:
    void clear();
    void reject();
    void accept();
    SendFluxEntry *addEntry();
    void updateRemoveEnabled();
    void setBalance(qint64 balance, qint64 unconfirmedBalance, qint64 immatureBalance);

private:
    Ui::SendFluxDialog *ui;
    WalletModel *model;
    bool fNewRecipientAllowed;

private slots:
    void on_sendButton_clicked();
    void removeEntry(SendFluxEntry* entry);
    void updateDisplayUnit();
};

#endif // SENDFLUXDIALOG_H
