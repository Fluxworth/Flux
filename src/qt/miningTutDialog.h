#ifndef MININGTUTDIALOG_H
#define MININGTUTDIALOG_H

#include <QDialog>

namespace Ui {
    class miningTutDialog;
}
class ClientModel;

/** "miningTut" dialog box */
class miningTutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit miningTutDialog(QWidget *parent = 0);
    ~miningTutDialog();

    void setModel(ClientModel *model);
private:
    Ui::miningTutDialog *ui;

private slots:
    void on_buttonBox_accepted();
};

#endif // miningTutDIALOG_H
