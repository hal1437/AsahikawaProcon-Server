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

    const int FRAME_RATE = 500;

protected:
    void keyPressEvent(QKeyEvent* event);
public:
    QTimer* clock;
    StartupDialog* startup;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
