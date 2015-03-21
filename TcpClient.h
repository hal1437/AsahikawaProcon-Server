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
    QDataStream  stream;        //データストリーム
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
    void Connected   (int port); //接続
    void Disconnected(int port); //切断

private slots:
    void NewConnect();//新規接続
};

#endif // TCPCLIENT_H
