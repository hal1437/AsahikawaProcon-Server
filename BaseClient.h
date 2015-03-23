#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include "GameSystem.h"

class BaseClient : public QObject
{
    Q_OBJECT
    //クライアント純粋仮想クラス
public:
    QString Name;//クラアント名
    QString IP;  //IP
    bool disconnected_flag; //切断フラグ

public:
    //GetReadyを要求するメソッド
    virtual bool WaitGetReady()=0;
    //GetReadyに対する周辺情報のレスポンス
    virtual GameSystem::Method WaitReturnMethod(GameSystem::AroundData data)=0;
    //Methodに対する周辺情報のレスポンスを送信し、#が来るまで待つ
    virtual bool WaitEndSharp(GameSystem::AroundData data)=0;

    explicit BaseClient(QObject *parent = 0);
    virtual ~BaseClient();

signals:
    void Connected();                   //接続
    void ReturnTeamName(QString name);  //チーム名
    void Ready(bool state);             //準備完了シグナル
    void Disconnected();                //切断
public slots:

};

#endif // CLIENT_H
