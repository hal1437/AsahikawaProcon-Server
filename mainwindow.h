#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QKeyEvent>
#include "startupdialog.h"
#include <fstream>
#include <QDateTime>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    enum TURN{
        COOL_GETREADY,
        COOL_ACTION,
        HOT_GETREADY,
        HOT_ACTION,
    };

    const int FRAME_RATE = 150;

    TURN process;
    int hot_score;
    int cool_score;
    std::ofstream log;

    QTimer* clock;
    StartupDialog* startup;

protected:
    void keyPressEvent(QKeyEvent* event);
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    //ゲーム進行
    void StepGame();
    //アイテム取得の判定
    void PickItem(GameSystem::TEAM team,GameSystem::Method method,QPoint& pos);
    //決着判定
    GameSystem::WINNER Judge();
    //決着
    void Finish(GameSystem::WINNER win);
};

#endif // MAINWINDOW_H
