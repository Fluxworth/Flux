#ifndef STATISTICSPAGE_H
#define STATISTICSPAGE_H

#include "clientmodel.h"
#include "main.h"
#include "wallet.h"
#include "base58.h"

#include <QWidget>

#include <QDir>
#include <QFile>
#include <QProcess>
#include <QTime>
#include <QTimer>
#include <QStringList>
#include <QMap>
#include <QSettings>
#include <QSlider>

namespace Ui {
class StatisticsPage;
}
class ClientModel;

class StatisticsPage : public QWidget
{
    Q_OBJECT

public:
    explicit StatisticsPage(QWidget *parent = 0);
    ~StatisticsPage();
    
    void setModel(ClientModel *model);
    
    int heightPrevious;
    int connectionPrevious;
    double volumePrevious;
    double rewardPrevious;
    double netHashratePrevious;
    double hashratePrevious;
    double difficultyPrevious;
    
public slots:
    int hashMultiplier();
    
    void updateInfo();
    void calculate();
    void updateStatistics();
    void updatePrevious(int, double, double, double, double, int, double);
    void updateNet();
    void nodeAdd();
private slots:

private:
    Ui::StatisticsPage *ui;
    ClientModel *model;
    
};

#endif // STATISTICSPAGE_H
