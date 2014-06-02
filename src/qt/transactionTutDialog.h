#ifndef TRANSACTIONTUTDIALOG_H
#define TRANSACTIONTUTDIALOG_H

#include <QDialog>

namespace Ui {
    class transactionTutDialog;
}
class ClientModel;

/** "transactionTut" dialog box */
class transactionTutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit transactionTutDialog(QWidget *parent = 0);
    ~transactionTutDialog();

    void setModel(ClientModel *model);
private:
    Ui::transactionTutDialog *ui;

private slots:
    void on_buttonBox_accepted();
};

#endif // transactionTutDIALOG_H
