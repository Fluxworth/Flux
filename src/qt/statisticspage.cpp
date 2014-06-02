#include "statisticspage.h"
#include "ui_statisticspage.h"
#include "main.h"
#include "wallet.h"
#include "base58.h"
#include "clientmodel.h"
#include "fluxrpc.h"
#include "fluxfunction.h"
#include <sstream>
#include <string>

using namespace json_spirit;

StatisticsPage::StatisticsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatisticsPage)
{
    ui->setupUi(this);
    
    setFixedSize(400, 420);
    
    connect(ui->startButton, SIGNAL(pressed()), this, SLOT(updateStatistics()));
    connect(ui->update, SIGNAL(pressed()), this, SLOT(updateInfo()));
    connect(ui->calc, SIGNAL(pressed()), this, SLOT(calculate()));
    connect(ui->node, SIGNAL(pressed()), this, SLOT(updateNet()));
    connect(ui->add, SIGNAL(pressed()), this, SLOT(nodeAdd()));
}

int heightPrevious = -1;
int connectionPrevious = -1;
double volumePrevious = -1;
double rewardPrevious = -1;
double netHashratePrevious = -1;
double hashratePrevious = -1;
double difficultyPrevious = -1;

void StatisticsPage::updateStatistics()
{    
    double pDifficulty = getDifficulty();
    int pHashrate = getNetworkHashesPS();
    double pHashrate2 = 0.000;
    int nHeight = pindexBest->nHeight;
    int lHashrate = this->model->getHashrate();
    double lHashrate2 = 0.000;
    double nSubsidy = getReward();
    double nextDifficulty = GetEstimatedNextDifficulty();
    double volume = getTotalVolume();
    int peers = this->model->getNumConnections();
    lHashrate2 = ((double)lHashrate / 1000);
    pHashrate2 = ((double)pHashrate / 1000);  
    std::string hash = getBlockHash(nHeight);
    int blockTime = getBlockTimestamp(nHeight);
    int blocksInHour = blocksInPastHours(1);
    double usecondsPerBlock = (double) 60 * 60 / blocksInHour;
    int blocksInDay = blocksInPastHours(24);
    double usecondsPerBlock2 = (double) 60 * 60 * 24 / blocksInDay;
    int blockTimeM = getBlockTime(nHeight);
    QString height = QString::number(nHeight);
    QString subsidy = QString::number(nSubsidy, 'f', 6);
    QString difficulty = QString::number(pDifficulty, 'f', 6);
    QString QnextDifficulty = QString::number(nextDifficulty, 'f', 6);
    QString hashrate = QString::number(pHashrate2, 'f', 3);
    QString Qlhashrate = QString::number(lHashrate2, 'f', 3);
    QString secondsPerBlock = QString::number(usecondsPerBlock, 'f', 3);
    QString secondsPerBlock2 = QString::number(usecondsPerBlock2, 'f', 3);
    QString QPeers = QString::number(peers);
    QString qVolume = QLocale(QLocale::English).toString(volume);
    QString QHash = QString::fromUtf8(hash.c_str());
    QString QMTime = QString::number(blockTimeM);
    QDateTime QRTime = QDateTime::fromTime_t(blockTime);
    QString QTime = QRTime.toString("yyyy:MM:dd hh:mm:ss");
    QString QHour = (QString::number(blocksInHour) + " (Average " + secondsPerBlock + " sec /block)");
    QString QDay = (QString::number(blocksInDay) + " (Average " + secondsPerBlock2 + " sec /block)");
    if(nHeight > heightPrevious)
    {
        ui->heightBox->setText("<b><font color=\"green\">" + height + "</font></b>");
    } else {
    ui->heightBox->setText(height);
    }
    
    if(nSubsidy < rewardPrevious)
    {
        ui->rewardBox->setText("<b><font color=\"red\">" + subsidy + "</font></b>");
    } else {
    ui->rewardBox->setText(subsidy);
    }
    
    if(pDifficulty > difficultyPrevious)
    {
        ui->diffBox->setText("<b><font color=\"green\">" + difficulty + "</font></b>");        
    } else if(pDifficulty < difficultyPrevious) {
        ui->diffBox->setText("<b><font color=\"red\">" + difficulty + "</font></b>");
    } else {
        ui->diffBox->setText(difficulty);        
    }
    
    if(nextDifficulty > pDifficulty)
    {
        ui->nextBox->setText("<b><font color=\"green\">" + QnextDifficulty + "</font></b>");        
    } else if(nextDifficulty < pDifficulty) {
        ui->nextBox->setText("<b><font color=\"red\">" + QnextDifficulty + "</font></b>"); 
    } else {
        ui->nextBox->setText(QnextDifficulty);        
    }
    
    if(pHashrate2 > netHashratePrevious)
    {
        ui->hashrateBox->setText("<b><font color=\"green\">" + hashrate + " KH/s</font></b>");             
    } else if(pHashrate2 < netHashratePrevious) {
        ui->hashrateBox->setText("<b><font color=\"red\">" + hashrate + " KH/s</font></b>");        
    } else {
        ui->hashrateBox->setText(hashrate + " KH/s");        
    }
    
    if(lHashrate > hashratePrevious)
    {
        ui->localBox->setText("<b><font color=\"green\">" + Qlhashrate + " KH/s</font></b>");             
    } else if(lHashrate < hashratePrevious) {
        ui->localBox->setText("<b><font color=\"red\">" + Qlhashrate + " KH/s</font></b>");        
    } else {
        ui->localBox->setText(Qlhashrate + " KH/s");      
    }
    
    if(peers > connectionPrevious)
    {
        ui->connectionBox->setText("<b><font color=\"green\">" + QPeers + "</font></b>");             
    } else if(peers < connectionPrevious) {
        ui->connectionBox->setText("<b><font color=\"red\">" + QPeers + "</font></b>");        
    } else {
        ui->connectionBox->setText(QPeers);  
    }

    if(volume > volumePrevious)
    {
        ui->volumeBox->setText("<b><font color=\"green\">" + qVolume + " FLX" + "</font></b>");             
    } else if(volume < volumePrevious) {
        ui->volumeBox->setText("<b><font color=\"red\">" + qVolume + " FLX" + "</font></b>");        
    } else {
        ui->volumeBox->setText(qVolume + " FLX");  
    }
    ui->hashBox->setText(QHash);
    ui->timeBox->setText(QTime);
    ui->timemBox->setText(QMTime);
    ui->hourBox->setText(QHour);
    ui->dayBox->setText(QDay);
    updatePrevious(nHeight, nSubsidy, pDifficulty, pHashrate2, lHashrate, peers, volume);
}

void StatisticsPage::updatePrevious(int nHeight, double nSubsidy, double pDifficulty, double pHashrate2, double lHashrate, int peers, double volume)
{
    heightPrevious = nHeight;
    rewardPrevious = nSubsidy;
    difficultyPrevious = pDifficulty;
    netHashratePrevious = pHashrate2;
    hashratePrevious = lHashrate;
    connectionPrevious = peers;
    volumePrevious = volume;
}

void StatisticsPage::updateInfo()
{
    int nethashrate = getNetworkHashesPS();
    double difficulty = getDifficulty();
    double reward = getReward();
    double profitPerKH = (1000 / (double)nethashrate) * reward * 60 * 24;
    double profitPerMH = (1000000 / (double)nethashrate) * reward * 60 * 24;
    double nethashrate2 = 0.000;
    nethashrate2 = ((double)nethashrate / 1000);
    QString QHashrate = QString::number(nethashrate2, 'f', 3);
    QString QReward = QString::number(reward, 'f', 6);
    QString QDifficulty = QString::number(difficulty, 'f', 6);
    QString QKH = QString::number(profitPerKH, 'f', 6);
    QString QMH = QString::number(profitPerMH, 'f', 6);
    ui->hashrate->setText(QHashrate + " KH/s");
    ui->difficulty->setText(QDifficulty);
    ui->reward->setText(QReward + " FLX");
    ui->KH->setText(QKH + " FLX");
    ui->MH->setText(QMH + " FLX");
}

void StatisticsPage::calculate()
{
    int nethashrate = getNetworkHashesPS();
    int target = ui->target->value();
    int multiplier = hashMultiplier();
    double reward = getReward();
    double final = ((target * multiplier) / (double)nethashrate) * reward * 60 * 24;
    QString QFinal = QString::number(final, 'f', 6);
    ui->end->setText("<b>Expected Profit: " + QFinal + " FLX /day</b>");
    updateInfo();
}

int StatisticsPage::hashMultiplier()
{
    int s = ui->combo->currentIndex();
    int i = 1;
    if(s == 0)
    {
        return 1 ;
    } else {
        while(s > 0)
        {
            s--;
            i *= 1000;
        }
        return i;
    }
}

void StatisticsPage::updateNet()
{
    std::string nodes = getNodeInfo();
    QString QNodes = QString::fromUtf8(nodes.c_str());
    ui->nodes->setText(QNodes);
}

void StatisticsPage::setModel(ClientModel *model)
{
    updateStatistics();
    this->model = model;
    ui->versionLabel->setText(model->formatFullVersion());
    ui->versionLabel->setText(model->formatFullVersion());
    updateInfo();
    updateNet();
}

void StatisticsPage::nodeAdd()
{
    std::string node = ui->addnode->text().toStdString();
    bool s = addnode(node);
    QString m;
    if(s)
    {
        m = "<b><font color=green>Successfully connected to node</font></b>";
    } else {
        m = "<b><font color=red>Node connection failed</font></b>";
    }
    ui->noderesult->setText(m);
    updateNet();
    updateInfo();
    updateStatistics();
}

StatisticsPage::~StatisticsPage()
{
    delete ui;
}