#include "transactionTutDialog.h"
#include "ui_transactionTutDialog.h"
#include "clientmodel.h"

#include "version.h"

transactionTutDialog::transactionTutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::transactionTutDialog)
{
    ui->setupUi(this);
}

void transactionTutDialog::setModel(ClientModel *model)
{
    if(model)
    {
        ui->versionLabel->setText(model->formatFullVersion());
    }
}

transactionTutDialog::~transactionTutDialog()
{
    delete ui;
}

void transactionTutDialog::on_buttonBox_accepted()
{
    close();
}