#ifndef SENDFLUXENTRY_H
#define SENDFLUXSENTRY_H

#include <QFrame>

namespace Ui {
    class SendFluxEntry;
}
class WalletModel;
class SendFluxRecipient;

/** A single entry in the dialog for sending Flux. */
class SendFluxEntry : public QFrame
{
    Q_OBJECT

public:
    explicit SendFluxEntry(QWidget *parent = 0);
    ~SendFluxEntry();

    void setModel(WalletModel *model);
    bool validate();
    SendFluxRecipient getValue();

    /** Return whether the entry is still empty and unedited */
    bool isClear();

    void setValue(const SendFluxRecipient &value);
    void setAddress(const QString &address);

    /** Set up the tab chain manually, as Qt messes up the tab chain by default in some cases (issue https://bugreports.qt-project.org/browse/QTBUG-10907).
     */
    QWidget *setupTabChain(QWidget *prev);

    void setFocus();

public slots:
    void setRemoveEnabled(bool enabled);
    void clear();

signals:
    void removeEntry(SendFluxEntry *entry);

private slots:
    void on_deleteButton_clicked();
    void on_payTo_textChanged(const QString &address);
    void on_addressBookButton_clicked();
    void on_pasteButton_clicked();
    void updateDisplayUnit();

private:
    Ui::SendFluxEntry *ui;
    WalletModel *model;
};

#endif // SENDFLUXENTRY_H
