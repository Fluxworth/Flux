#ifndef BLOCKEXPLORER_H
#define BLOCKEXPLORER_H

#include "clientmodel.h"

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
class BlockExplorer;
}
class ClientModel;

class BlockExplorer : public QWidget
{
    Q_OBJECT

public:
    explicit BlockExplorer(QWidget *parent = 0);
    ~BlockExplorer();
    
    void setModel(ClientModel *model);
    
public slots:
    
    void nextBlock();
    void previousBlock();
    void blockClicked();
    void txClicked();
    void updateExplorer(bool);

private slots:

private:
    Ui::BlockExplorer *ui;
    ClientModel *model;
    
};

#endif // BLOCKEXPLORER_H
