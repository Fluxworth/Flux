#ifndef BEGINNERDIALOG_H
#define BEGINNERDIALOG_H

#include <QDialog>

namespace Ui {
    class BeginnerDialog;
}
class ClientModel;

/** "Beginner" dialog box */
class BeginnerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BeginnerDialog(QWidget *parent = 0);
    ~BeginnerDialog();

    void setModel(ClientModel *model);
private:
    Ui::BeginnerDialog *ui;

private slots:
    void on_buttonBox_accepted();
};

#endif // BEGINNERDIALOG_H
