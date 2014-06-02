#include "miningpage.h"
#include "ui_miningpage.h"

MiningPage::MiningPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MiningPage)
{ 
    ui->setupUi(this);
    minerSet = false;
    setFixedSize(400, 420);
    minerActive = false;

    typeChanged(0);

    minerProcess = new QProcess(this);
    minerProcess->setProcessChannelMode(QProcess::MergedChannels);

    readTimer = new QTimer(this);

    acceptedShares = 0;
    rejectedShares = 0;

    roundAcceptedShares = 0;
    roundRejectedShares = 0;

    initThreads = 0;

    connect(readTimer, SIGNAL(timeout()), this, SLOT(readProcessOutput()));

    connect(ui->startButton, SIGNAL(pressed()), this, SLOT(startPressed()));
    connect(ui->typeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(typeChanged(int)));
    connect(ui->debugCheckBox, SIGNAL(toggled(bool)), this, SLOT(debugToggled(bool)));
    connect(minerProcess, SIGNAL(started()), this, SLOT(minerStarted()));
    connect(minerProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(minerError(QProcess::ProcessError)));
    connect(minerProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(minerFinished()));
    connect(minerProcess, SIGNAL(readyRead()), this, SLOT(readProcessOutput()));

    // error: no known function for call ClientModel (MiningPage* const)
    // candidates are ClientModel::ClientModel(OptionsModel*, QObject*)
    setModel(new ClientModel(0, this));
}

MiningPage::~MiningPage()
{
    minerProcess->kill();

    delete ui;
}

void MiningPage::setModel(ClientModel *modelIn)
{
    if(minerSet)
        return;
    
    minerSet = true;
    
    this->model = modelIn;
    
    int nugget;
    

    loadSettings();

    
    bool pool = model->getMiningType() == ClientModel::PoolMining;
    
    ui->threadsBox->setValue(model->getMiningThreads());
    
    ui->typeBox->setCurrentIndex(pool ? 1 : 0);
    
//    if (model->getMiningStarted())
//        startPressed();
}

void MiningPage::startPressed()
{
    initThreads = ui->threadsBox->value();
    
    if (minerActive == false)
    {
        saveSettings();
        startPoolMining();
    }
    else
    {
        stopPoolMining();
    }
}

void MiningPage::startPoolMining()
{
    QStringList args;
    QString url = ui->serverLine->text();
    QString kernel = ui->kernel->text();
    QString textureCache = getTextureCache();
    QString offloadSHA = getOffloadSHA();
    QString memoryBlock = getMemoryBlock();
    QString intensity = getIntensity();
    QString concurrency = ui->concurrency->text();
    QString workload = ui->workload->text();
    if(ui->typeBox->currentIndex() != 0)
    {
    if (!url.contains(getURLPrefix()))
        url.prepend(getURLPrefix());
    }
    QString urlLine = QString("%1:%2").arg(url, ui->portLine->text());
    QString userpassLine = QString("%1:%2").arg(ui->usernameLine->text(), ui->passwordLine->text());
    args << "--algo" << "keccak";
    args << "--scantime" << ui->scantimeBox->text().toAscii();
    args << "--url" << urlLine.toAscii();
    args << "--userpass" << userpassLine.toAscii();
    if(ui->useDigger->currentIndex() == 0)
    {
    args << "--threads" << ui->threadsBox->text().toAscii();
    }
    args << "--retries" << "-1"; // Retry forever.
    args << "-P"; // This is needed for this to work correctly on Windows. Extra protocol dump helps flush the buffer quicker.
    if(ui->useDigger->currentIndex() == 2)
    {
    args << "--intensity" << intensity.toAscii();
    args << "--thread-concurrency" << concurrency.toAscii();
    args << "--worksize" << workload.toAscii();
    args << "--gpu-threads" << ui->threadsBox->text().toAscii();
    } else if(ui->useDigger->currentIndex() == 1) {
    args << "--launch-config" << kernel.toAscii();
    if(ui->autotune->currentIndex() == 0)
    {
        args << "--no-autotune";
    }
    args << "--texture-cache" << textureCache.toAscii();
    args << "--hash-parallel" << offloadSHA.toAscii();
    args << "--single-memory" << memoryBlock.toAscii();
    }
    
    threadSpeed.clear();

    acceptedShares = 0;
    rejectedShares = 0;

    roundAcceptedShares = 0;
    roundRejectedShares = 0;

    // If minerd is in current path, then use that. Otherwise, assume minerd is in the path somewhere.
    QString program = QDir::current().filePath(getProgramName());
    if (!QFile::exists(program))
        program = getProgramName();

    if (ui->debugCheckBox->isChecked())
        ui->list->addItem(args.join(" ").prepend(" ").prepend(program));

    ui->mineSpeedLabel->setText("Speed: N/A");
    ui->shareCount->setText("Accepted: 0 - Rejected: 0");
    minerProcess->start(program,args);
    minerProcess->waitForStarted(-1);

    readTimer->start(500); 
}

void MiningPage::stopPoolMining()
{
    ui->mineSpeedLabel->setText("");
    minerProcess->kill();
    readTimer->stop();  
}

void MiningPage::saveSettings()
{    
    
    model->setMiningDebug(ui->debugCheckBox->isChecked());
    
    model->setMiningScanTime(ui->scantimeBox->value());
    
    model->setMiningServer(ui->serverLine->text());
    
    model->setMiningPort(ui->portLine->text());
    
    model->setMiningUsername(ui->usernameLine->text());
    
    model->setMiningPassword(ui->passwordLine->text());
}

void MiningPage::loadSettings()
{
    ui->debugCheckBox->setChecked(model->getMiningDebug());
    
    ui->scantimeBox->setValue(model->getMiningScanTime());
    
    ui->serverLine->setText(model->getMiningServer());
    
    ui->portLine->setText(model->getMiningPort());
    
    ui->usernameLine->setText(model->getMiningUsername());
    
    ui->passwordLine->setText(model->getMiningPassword());
}

void MiningPage::readProcessOutput()
{
    QByteArray output;

    

    minerProcess->reset();

    output = minerProcess->readAll();

    QString outputString(output);

    if (!outputString.isEmpty())
    {
        QStringList list = outputString.split("\n", QString::SkipEmptyParts);
        int i;
        for (i=0; i<list.size(); i++)
        {
            QString line = list.at(i);

            // Ignore protocol dump
            if (!line.startsWith("[") || line.contains("JSON protocol") || line.contains("HTTP hdr"))
                continue;

            if (ui->debugCheckBox->isChecked())
            {
                ui->list->addItem(line.trimmed());
                ui->list->scrollToBottom();
            }

            if (line.contains("(yay!!!)"))
                reportToList("Share accepted", SHARE_SUCCESS, getTime(line));
            else if (line.contains("(booooo)"))
                reportToList("Share rejected", SHARE_FAIL, getTime(line));
            else if (line.contains("LONGPOLL detected new block"))
                reportToList("LONGPOLL detected a new block", LONGPOLL, getTime(line));
            else if (line.contains("Supported options:"))
                reportToList("Miner didn't start properly. Try checking your settings.", ERROR, NULL);
            else if (line.contains("The requested URL returned error: 403"))
                reportToList("Couldn't connect. Please check your username and password.", ERROR, NULL);
            else if (line.contains("HTTP request failed"))
                reportToList("Couldn't connect. Please check pool server and port.", ERROR, NULL);
            else if (line.contains("JSON-RPC call failed"))
                reportToList("Couldn't communicate with server. Retrying in 30 seconds.", ERROR, NULL);
            else if (line.contains("thread ") && line.contains("khash/s"))
            {
                QString threadIDstr = line.at(line.indexOf("thread ")+7);
                int threadID = threadIDstr.toInt();

                int threadSpeedindx = line.indexOf(",");
                QString threadSpeedstr = line.mid(threadSpeedindx);
                threadSpeedstr.chop(8);
                threadSpeedstr.remove(", ");
                threadSpeedstr.remove(" ");
                threadSpeedstr.remove('\n');
                double speed=0;
                speed = threadSpeedstr.toDouble();

                threadSpeed[threadID] = speed;

                updateSpeed();
            }
        }
    }
}

void MiningPage::minerError(QProcess::ProcessError error)
{
    

    if (error == QProcess::FailedToStart)
    {
        reportToList("Miner failed to start. Make sure you have the minerd executable and libraries in the same directory as Flux-Qt.", ERROR, NULL);
    }
}

void MiningPage::minerFinished()
{
    if (getType() == 1)
        reportToList("Solo mining stopped.", ERROR, NULL);
    else
        reportToList("Miner exited.", ERROR, NULL);
    ui->list->addItem("");
    minerActive = false;
    resetMiningButton();
    model->setMining(getMiningType(), false, initThreads, 0);

    
}

void MiningPage::minerStarted()
{
    if (!minerActive)
    {
        if (getType() == 1)
        {
            reportToList("Solo mining started.", ERROR, NULL);
        } else {
            reportToList("Miner started. You might not see any output for a few minutes.", STARTED, NULL);
        }
    }
    minerActive = true;
    resetMiningButton();
    model->setMining(getMiningType(), true, initThreads, 0);
}

void MiningPage::updateSpeed()
{
    

    double totalSpeed=0;
    int totalThreads=0;

    QMapIterator<int, double> iter(threadSpeed);
    while(iter.hasNext())
    {
        iter.next();
        totalSpeed += iter.value();
        totalThreads++;
    }

    

    // If all threads haven't reported the hash speed yet, make an assumption
    if (totalThreads != initThreads)
    {
        totalSpeed = (totalSpeed/totalThreads)*initThreads;
    }

    QString speedString = QString("%1").arg(totalSpeed);
    QString threadsString = QString("%1").arg(initThreads);

    QString acceptedString = QString("%1").arg(acceptedShares);
    QString rejectedString = QString("%1").arg(rejectedShares);

    QString roundAcceptedString = QString("%1").arg(roundAcceptedShares);
    QString roundRejectedString = QString("%1").arg(roundRejectedShares);

    

    if (totalThreads == initThreads)
        ui->mineSpeedLabel->setText(QString("Speed: %1 khash/sec - %2 thread(s)").arg(speedString, threadsString));
    else
        ui->mineSpeedLabel->setText(QString("Speed: ~%1 khash/sec - %2 thread(s)").arg(speedString, threadsString));

    ui->shareCount->setText(QString("Accepted: %1 (%3) - Rejected: %2 (%4)").arg(acceptedString, rejectedString, roundAcceptedString, roundRejectedString));



    model->setMining(getMiningType(), true, initThreads, totalSpeed*1000);

    
}

void MiningPage::reportToList(QString msg, int type, QString time)
{


    QString message;
    if (time == NULL)
        message = QString("[%1] - %2").arg(QTime::currentTime().toString(), msg);
    else
        message = QString("[%1] - %2").arg(time, msg);



    switch(type)
    {
        case SHARE_SUCCESS:
            acceptedShares++;
            roundAcceptedShares++;
            updateSpeed();
            break;

        case SHARE_FAIL:
            rejectedShares++;
            roundRejectedShares++;
            updateSpeed();
            break;

        case LONGPOLL:
            roundAcceptedShares = 0;
            roundRejectedShares = 0;
            break;

        default:
            break;
    }

    ui->list->addItem(message);
    ui->list->scrollToBottom();

    
}

// Function for fetching the time
QString MiningPage::getTime(QString time)
{
    

    if (time.contains("["))
    {
        time.resize(21);
        time.remove("[");
        time.remove("]");
        time.remove(0,11);

        
        return time;
    }
    else
        return NULL;
}

void MiningPage::enableMiningControls(bool enable)
{
    ui->typeBox->setEnabled(enable);
    ui->threadsBox->setEnabled(enable);
    ui->scantimeBox->setEnabled(enable);
    ui->serverLine->setEnabled(enable);
    ui->portLine->setEnabled(enable);
    ui->usernameLine->setEnabled(enable);
    ui->passwordLine->setEnabled(enable);
}

void MiningPage::enablePoolMiningControls(bool enable)
{
    ui->scantimeBox->setEnabled(enable);
    ui->serverLine->setEnabled(enable);
    ui->portLine->setEnabled(enable);
    ui->usernameLine->setEnabled(enable);
    ui->passwordLine->setEnabled(enable);
}

ClientModel::MiningType MiningPage::getMiningType()
{
    

    if (ui->typeBox->currentIndex() == 0)  // Solo Mining
    {
        

        return ClientModel::SoloMining;
    }
    else if (ui->typeBox->currentIndex() == 1)  // Pool Mining
    {
        

        return ClientModel::PoolMining;
    }

    

    return ClientModel::SoloMining;
}

int MiningPage::getType()
{
    if (ui->typeBox->currentIndex() == 0)  // Solo Mining
    {
        return 1;
    }
    else if (ui->typeBox->currentIndex() == 1) // P2Pool Mining
    {
        return 2;
    }
    else if (ui->typeBox->currentIndex() == 2)  // Pool Mining
    {
        return 3;
    }
    return 1;  
}

int MiningPage::getDigger()
{
    if(ui->useDigger->currentIndex() == 0) // CPU Mining
    {
        return 1;
    } 
    else if(ui->useDigger->currentIndex() == 1) //CUDA Mining
    {
        return 2;
    }
    else 
    {
        return 3; //OpenCL Mining
    }
}

const char* MiningPage::getTextureCache()
{
    if(ui->textureCache->currentIndex() == 0)
    {
        return "0";
    }
    else if (ui->textureCache->currentIndex() == 1)
    {
        return "1";
    }
    else
    {
        return "2";
    }
}

const char* MiningPage::getOffloadSHA()
{
    if(ui->offloadSHA->currentIndex() == 0)
    {
        return "0";
    }
    else if (ui->offloadSHA->currentIndex() == 1)
    {
        return "1";
    }
    else
    {
        return "2";
    }
}

const char* MiningPage::getMemoryBlock()
{
    if(ui->memoryBlock->currentIndex() == 0)
    {
        return "0";
    }
    else
    {
        return "1";
    }
}

const char* MiningPage::getIntensity()
{
    if(ui->intensity->value() == 1)
    {
        return "1";
    }
    else if(ui->intensity->value() == 2)
    {
        return "2";
    }
    else if(ui->intensity->value() == 3)
    {
        return "3";
    }
    else if(ui->intensity->value() == 4)
    {
        return "4";
    }
    else if(ui->intensity->value() == 5)
    {
        return "5";
    }
    else if(ui->intensity->value() == 6)
    {
        return "6";
    }
    else if(ui->intensity->value() == 7)
    {
        return "7";
    }
    else if(ui->intensity->value() == 8)
    {
        return "8";
    }
    else if(ui->intensity->value() == 9)
    {
        return "9";
    }
    else if(ui->intensity->value() == 10)
    {
        return "10";
    }
    else if(ui->intensity->value() == 11)
    {
        return "11";
    }
    else if(ui->intensity->value() == 12)
    {
        return "12";
    }
    else if(ui->intensity->value() == 13)
    {
        return "13";
    }
    else if(ui->intensity->value() == 14)
    {
        return "14";
    }    
    else if(ui->intensity->value() == 15)
    {
        return "15";
    }    
    else if(ui->intensity->value() == 16)
    {
        return "16";
    }    
    else if(ui->intensity->value() == 17)
    {
        return "17";
    }   
    else if(ui->intensity->value() == 18)
    {
        return "18";
    }    
    else if(ui->intensity->value() == 19)
    {
        return "19";
    }    
    else if(ui->intensity->value() == 20)
    {
        return "20";
    }    
    else
    {
        return "1";
    }
}

const char* MiningPage::getProgramName()
{
    if(ui->useDigger->currentIndex() == 0) // CPU Mining
    {
        return "minerd";
    } 
    else if(ui->useDigger->currentIndex() == 1) //CUDA Mining
    {
        return "cudaminer";
    }
    else 
    {
        return "cgminer"; //OpenCL Mining
    }
}

const char* MiningPage::getURLPrefix()
{
    if (ui->typeBox->currentIndex() == 0)  // Solo Mining
    {
        return "";
    }
    else if (ui->typeBox->currentIndex() == 1) // P2Pool Mining
    {
        return "http://";
    }
    else if (ui->typeBox->currentIndex() == 2)  // Pool Mining
    {
        return "stratum+tcp://";
    }
}

void MiningPage::typeChanged(int index)
{
    if (index == 0)  // Solo Mining
    {
        enablePoolMiningControls(true);
    }
    else if (index == 1)  // Pool Mining
    {
        enablePoolMiningControls(true);
    }
}

void MiningPage::debugToggled(bool checked)
{
    

    model->setMiningDebug(checked);
}

void MiningPage::resetMiningButton()
{
    

    ui->startButton->setText(minerActive ? "Stop Mining" : "Start Mining");
    enableMiningControls(!minerActive);
}
