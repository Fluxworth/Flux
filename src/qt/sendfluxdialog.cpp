#include "sendfluxdialog.h"
#include "ui_sendfluxdialog.h"

#include "walletmodel.h"
#include "fluxunits.h"
#include "addressbookpage.h"
#include "optionsmodel.h"
#include "sendfluxentry.h"
#include "guiutil.h"
#include "askpassphrasedialog.h"
#include "base58.h"

#include <QMessageBox>
#include <QTextDocument>
#include <QScrollBar>

SendFluxDialog::SendFluxDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SendFluxDialog),
    model(0)
{
    ui->setupUi(this);

#ifdef Q_OS_MAC // Icons on push buttons are very uncommon on Mac
    ui->addButton->setIcon(QIcon());
    ui->clearButton->setIcon(QIcon());
    ui->sendButton->setIcon(QIcon());
#endif

    addEntry();

    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addEntry()));
    connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clear()));

    fNewRecipientAllowed = true;
}

void SendFluxDialog::setModel(WalletModel *model)
{
    this->model = model;

    for(int i = 0; i < ui->entries->count(); ++i)
    {
        SendFluxEntry *entry = qobject_cast<SendFluxEntry*>(ui->entries->itemAt(i)->widget());
        if(entry)
        {
            entry->setModel(model);
        }
    }
    if(model && model->getOptionsModel())
    {
        setBalance(model->getBalance(), model->getUnconfirmedBalance(), model->getImmatureBalance());
        connect(model, SIGNAL(balanceChanged(qint64, qint64, qint64)), this, SLOT(setBalance(qint64, qint64, qint64)));
        connect(model->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));
    }
}

SendFluxDialog::~SendFluxDialog()
{
    delete ui;
}

void SendFluxDialog::on_sendButton_clicked()
{
    QList<SendFluxRecipient> recipients;
    bool valid = true;

    if(!model)
        return;

    for(int i = 0; i < ui->entries->count(); ++i)
    {
        SendFluxEntry *entry = qobject_cast<SendFluxEntry*>(ui->entries->itemAt(i)->widget());
        if(entry)
        {
            if(entry->validate())
            {
                recipients.append(entry->getValue());
            }
            else
            {
                valid = false;
            }
        }
    }

    if(!valid || recipients.isEmpty())
    {
        return;
    }

    // Format confirmation message
    QStringList formatted;
    foreach(const SendFluxRecipient &rcp, recipients)
    {
#if QT_VERSION < 0x050000
        formatted.append(tr("<b>%1</b> to %2 (%3)").arg(FluxUnits::formatWithUnit(FluxUnits::FLX, rcp.amount), Qt::escape(rcp.label), rcp.address));
#else
        formatted.append(tr("<b>%1</b> to %2 (%3)").arg(FluxUnits::formatWithUnit(FluxUnits::FLX, rcp.amount), rcp.label.toHtmlEscaped(), rcp.address));
#endif
    }

    fNewRecipientAllowed = false;

    QMessageBox::StandardButton retval = QMessageBox::question(this, tr("Confirm send Flux"),
                          tr("Are you sure you want to send %1?").arg(formatted.join(tr(" and "))),
          QMessageBox::Yes|QMessageBox::Cancel,
          QMessageBox::Cancel);

    if(retval != QMessageBox::Yes)
    {
        fNewRecipientAllowed = true;
        return;
    }

    WalletModel::UnlockContext ctx(model->requestUnlock());
    if(!ctx.isValid())
    {
        // Unlock wallet was cancelled
        fNewRecipientAllowed = true;
        return;
    }

    WalletModel::SendFluxReturn sendstatus = model->sendFlux(recipients);
    switch(sendstatus.status)
    {
    case WalletModel::InvalidAddress:
        QMessageBox::warning(this, tr("Send Flux"),
            tr("The recipient address is not valid, please recheck."),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::InvalidAmount:
        QMessageBox::warning(this, tr("Send Flux"),
            tr("The amount to pay must be larger than 0."),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::AmountExceedsBalance:
        QMessageBox::warning(this, tr("Send Flux"),
            tr("The amount exceeds your balance."),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::AmountWithFeeExceedsBalance:
        QMessageBox::warning(this, tr("Send Flux"),
            tr("The total exceeds your balance when the %1 transaction fee is included.").
            arg(FluxUnits::formatWithUnit(FluxUnits::FLX, sendstatus.fee)),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::DuplicateAddress:
        QMessageBox::warning(this, tr("Send Flux"),
            tr("Duplicate address found, can only send to each address once per send operation."),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::TransactionCreationFailed:
        QMessageBox::warning(this, tr("Send Flux"),
            tr("Error: Transaction creation failed!"),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::TransactionCommitFailed:
        QMessageBox::warning(this, tr("Send Flux"),
            tr("Error: The transaction was rejected. This might happen if some of the Flux in your wallet were already spent, such as if you used a copy of wallet.dat and Flux were spent in the copy but not marked as spent here."),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::Aborted: // User aborted, nothing to do
        break;
    case WalletModel::OK:
        accept();
        break;
    }
    fNewRecipientAllowed = true;
}

void SendFluxDialog::clear()
{
    // Remove entries until only one left
    while(ui->entries->count())
    {
        ui->entries->takeAt(0)->widget()->deleteLater();
    }
    addEntry();

    updateRemoveEnabled();

    ui->sendButton->setDefault(true);
}

void SendFluxDialog::reject()
{
    clear();
}

void SendFluxDialog::accept()
{
    clear();
}

SendFluxEntry *SendFluxDialog::addEntry()
{
    SendFluxEntry *entry = new SendFluxEntry(this);
    entry->setModel(model);
    ui->entries->addWidget(entry);
    connect(entry, SIGNAL(removeEntry(SendFluxEntry*)), this, SLOT(removeEntry(SendFluxEntry*)));

    updateRemoveEnabled();

    // Focus the field, so that entry can start immediately
    entry->clear();
    entry->setFocus();
    ui->scrollAreaWidgetContents->resize(ui->scrollAreaWidgetContents->sizeHint());
    qApp->processEvents();
    QScrollBar* bar = ui->scrollArea->verticalScrollBar();
    if(bar)
        bar->setSliderPosition(bar->maximum());
    return entry;
}

void SendFluxDialog::updateRemoveEnabled()
{
    // Remove buttons are enabled as soon as there is more than one send-entry
    bool enabled = (ui->entries->count() > 1);
    for(int i = 0; i < ui->entries->count(); ++i)
    {
        SendFluxEntry *entry = qobject_cast<SendFluxEntry*>(ui->entries->itemAt(i)->widget());
        if(entry)
        {
            entry->setRemoveEnabled(enabled);
        }
    }
    setupTabChain(0);
}

void SendFluxDialog::removeEntry(SendFluxEntry* entry)
{
    entry->deleteLater();
    updateRemoveEnabled();
}

QWidget *SendFluxDialog::setupTabChain(QWidget *prev)
{
    for(int i = 0; i < ui->entries->count(); ++i)
    {
        SendFluxEntry *entry = qobject_cast<SendFluxEntry*>(ui->entries->itemAt(i)->widget());
        if(entry)
        {
            prev = entry->setupTabChain(prev);
        }
    }
    QWidget::setTabOrder(prev, ui->addButton);
    QWidget::setTabOrder(ui->addButton, ui->sendButton);
    return ui->sendButton;
}

void SendFluxDialog::setAddress(const QString &address)
{
    SendFluxEntry *entry = 0;
    // Replace the first entry if it is still unused
    if(ui->entries->count() == 1)
    {
        SendFluxEntry *first = qobject_cast<SendFluxEntry*>(ui->entries->itemAt(0)->widget());
        if(first->isClear())
        {
            entry = first;
        }
    }
    if(!entry)
    {
        entry = addEntry();
    }

    entry->setAddress(address);
}

void SendFluxDialog::pasteEntry(const SendFluxRecipient &rv)
{
    if(!fNewRecipientAllowed)
        return;

    SendFluxEntry *entry = 0;
    // Replace the first entry if it is still unused
    if(ui->entries->count() == 1)
    {
        SendFluxEntry *first = qobject_cast<SendFluxEntry*>(ui->entries->itemAt(0)->widget());
        if(first->isClear())
        {
            entry = first;
        }
    }
    if(!entry)
    {
        entry = addEntry();
    }

    entry->setValue(rv);
}

bool SendFluxDialog::handleURI(const QString &uri)
{
    SendFluxRecipient rv;
    // URI has to be valid
    if (GUIUtil::parseFluxURI(uri, &rv))
    {
        CFluxAddress address(rv.address.toStdString());
        if (!address.IsValid())
            return false;
        pasteEntry(rv);
        return true;
    }

    return false;
}

void SendFluxDialog::setBalance(qint64 balance, qint64 unconfirmedBalance, qint64 immatureBalance)
{
    Q_UNUSED(unconfirmedBalance);
    Q_UNUSED(immatureBalance);
    if(!model || !model->getOptionsModel())
        return;

    int unit = model->getOptionsModel()->getDisplayUnit();
    ui->labelBalance->setText(FluxUnits::formatWithUnit(unit, balance));
}

void SendFluxDialog::updateDisplayUnit()
{
    if(model && model->getOptionsModel())
    {
        // Update labelBalance with the current balance and the current unit
        ui->labelBalance->setText(FluxUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), model->getBalance()));
    }
}
