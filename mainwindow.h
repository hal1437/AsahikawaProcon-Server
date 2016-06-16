#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QKeyEvent>
#include "startupdialog.h"
#include <fstream>
#include <QDateTime>
#include <QFile>
#include <QSound>
#include <QDataStream>
#include <QMediaPlayer>
#include <QThread>
#include "StableLog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:

    int player;      //次ターン行動プレイヤー
    int team_score[TEAM_COUNT]; //スコア

    int FRAME_RATE = 150;   //ゲームフレームレート
    QTimer* clock;          //ゲームクロック
    QTimer* startup_anime;  //開始アニメーション
    QTimer* teamshow_anime; //チーム表示アニメーション
    QTimer* blind_anime;    //まっくらアニメーション
    StartupDialog* startup; //スタートアップダイアログ
    QSound* music;          //音楽

    int leave_items=0;
    bool silent;

    bool dark;              //暗転処理

    QFile* file;    //ログファイル
    StableLog log;//ログストリーム
    int anime_map_time  = 6000;//マップ構築アニメーション時間
    int anime_team_time = 2000;//チーム配置アニメーション時間
    int anime_blind_time = 1000;//まっくらアニメーション時間

protected:
    void keyPressEvent(QKeyEvent* event);
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    void SaveFile();

    //ゲーム進行
    void StepGame();
    //アイテム取得の判定
    void PickItem(GameSystem::Method method);
    //決着判定
    GameSystem::WINNER Judge();
    //決着
    void Finish(GameSystem::WINNER win);

    void StartAnimation();
    void ShowTeamAnimation();
    void BlindAnimation();
};

#endif // MAINWINDOW_H
