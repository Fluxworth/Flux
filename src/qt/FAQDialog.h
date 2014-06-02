#ifndef FAQDIALOG_H
#define FAQDIALOG_H

#include <QDialog>

namespace Ui {
    class FAQDialog;
}
class ClientModel;

/** "FAQ" dialog box */
class FAQDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FAQDialog(QWidget *parent = 0);
    ~FAQDialog();

    void setModel(ClientModel *model);
private:
    Ui::FAQDialog *ui;

private slots:
    void on_buttonBox_accepted();
};

#endif // FAQDIALOG_H
