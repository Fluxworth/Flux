#ifndef PROTECTIONTUTDIALOG_H
#define PROTECTIONTUTDIALOG_H

#include <QDialog>

namespace Ui {
    class protectionTutDialog;
}
class ClientModel;

/** "protectionTut" dialog box */
class protectionTutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit protectionTutDialog(QWidget *parent = 0);
    ~protectionTutDialog();

    void setModel(ClientModel *model);
private:
    Ui::protectionTutDialog *ui;

private slots:
    void on_buttonBox_accepted();
};

#endif // protectionTutDIALOG_H
