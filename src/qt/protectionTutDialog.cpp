#include "protectionTutDialog.h"
#include "ui_protectionTutDialog.h"
#include "clientmodel.h"

#include "version.h"

protectionTutDialog::protectionTutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::protectionTutDialog)
{
    ui->setupUi(this);
}

void protectionTutDialog::setModel(ClientModel *model)
{
    if(model)
    {
        ui->versionLabel->setText(model->formatFullVersion());
    }
}

protectionTutDialog::~protectionTutDialog()
{
    delete ui;
}

void protectionTutDialog::on_buttonBox_accepted()
{
    close();
}