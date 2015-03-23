#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QKeyEvent>
#include "startupdialog.h"

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

    const int FRAME_RATE = 300;

    TURN process;
    int hot_score;
    int cool_score;

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
};

#endif // MAINWINDOW_H
