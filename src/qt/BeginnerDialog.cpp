#include "BeginnerDialog.h"
#include "ui_BeginnerDialog.h"
#include "clientmodel.h"

#include "version.h"

BeginnerDialog::BeginnerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BeginnerDialog)
{
    ui->setupUi(this);
}

void BeginnerDialog::setModel(ClientModel *model)
{
    if(model)
    {
        ui->versionLabel->setText(model->formatFullVersion());
    }
}

BeginnerDialog::~BeginnerDialog()
{
    delete ui;
}

void BeginnerDialog::on_buttonBox_accepted()
{
    close();
}