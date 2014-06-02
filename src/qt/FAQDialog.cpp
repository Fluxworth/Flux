#include "FAQDialog.h"
#include "ui_FAQDialog.h"
#include "clientmodel.h"

#include "version.h"

FAQDialog::FAQDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FAQDialog)
{
    ui->setupUi(this);
}

void FAQDialog::setModel(ClientModel *model)
{
    if(model)
    {
        ui->versionLabel->setText(model->formatFullVersion());
    }
}

FAQDialog::~FAQDialog()
{
    delete ui;
}

void FAQDialog::on_buttonBox_accepted()
{
    close();
}