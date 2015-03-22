#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include "BaseClient.h"

class TCPClient : public BaseClient
{
    Q_OBJECT
    //TCPユーザー用の接続サーバー
private:
    const static int TIMEOUT =10000;   //タイムアウト
    const int Port;             //開放ポート
    QTcpServer*  server;        //TCPサーバー
    QTcpSocket*  client;        //TCPクライアント
public:

    virtual bool WaitGetReady();
    virtual GameSystem::Method WaitReturnMethod(GameSystem::AroundData data);
    virtual bool WaitEndSharp(GameSystem::AroundData data);

    //ポート開放
    bool OpenSocket();
    //ポート開放
    bool CloseSocket();
    //接続確認
    bool isConnecting();

    explicit TCPClient(int Port, QObject *parent = 0);
    virtual ~TCPClient();

signals:

private slots:
    void NewConnect (); //新規接続
    void DisConnect (); //切断

public slots:
    QString GetTeamName();//チーム名取得
};

#endif // TCPCLIENT_H
