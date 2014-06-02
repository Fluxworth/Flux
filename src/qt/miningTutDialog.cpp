#include "miningTutDialog.h"
#include "ui_miningTutDialog.h"
#include "clientmodel.h"

#include "version.h"

miningTutDialog::miningTutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::miningTutDialog)
{
    ui->setupUi(this);
}

void miningTutDialog::setModel(ClientModel *model)
{
    if(model)
    {
        ui->versionLabel->setText(model->formatFullVersion());
    }
}

miningTutDialog::~miningTutDialog()
{
    delete ui;
}

void miningTutDialog::on_buttonBox_accepted()
{
    close();
}