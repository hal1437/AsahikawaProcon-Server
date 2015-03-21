#include "TcpClient.h"

bool TCPClient::WaitGetReady(){
    QDataStream stream(client);
    //ターン開始文字列
    stream << QString("@\r\n");

    //レスポンス待ち
    if(this->client->waitForReadyRead(this->TIMEOUT)){
        //レスポンスあり

        QString response;
        stream   >> response;
        qDebug() << response;
        return (response == "gr\r\n");
    }else{
        //レスポンスなし

        qDebug() << QString("[WaitGetReady Port") + QString::number(this->Port) +"]:Noting responce";
        return false;
    }

}

GameSystem::Method TCPClient::WaitReturnMethod(GameSystem::AroundData data){
    QDataStream stream(client);
    //周辺情報文字列
    stream << data.toString() << "\r\n";

    //レスポンス待ち
    if(this->client->waitForReadyRead(this->TIMEOUT)){
        //レスポンスあり

        QString response;
        stream   >> response;
        qDebug() << response;
        return GameSystem::Method::fromString(response);
    }else{
        //レスポンスなし

        qDebug() << QString("[WaitReturnMethod Port") + QString::number(this->Port) +"]:Noting responce";
        return GameSystem::Method{GameSystem::Method::ACTION::UNKNOWN,
                                  GameSystem::Method::ROTE::UNKNOWN};
    }


}
bool TCPClient::WaitEndSharp(GameSystem::AroundData data){
    QDataStream stream(client);
    //周辺情報文字列
    stream << data.toString() << "\r\n";

    //レスポンス待ち
    if(this->client->waitForReadyRead(this->TIMEOUT)){
        //レスポンスあり

        QString response;
        stream   >> response;
        qDebug() << response;
        return (response == "#\r\n");
    }else{
        //レスポンスなし

        qDebug() << QString("[WaitEndSharp Port") + QString::number(this->Port) +"]:Noting responce";
        return false;
    }

}


bool TCPClient::OpenSocket(){
    this->server->listen(QHostAddress::Any,this->Port);
}
bool TCPClient::CloseSocket(){
    this->server->close();
}
bool TCPClient::isConnecting(){
    return this->server->isListening();
}
void TCPClient::NewConnect(){
    this->client = this->server->nextPendingConnection();
    emit Connected(this->Port);
    emit Ready();
}

TCPClient::TCPClient(int port,QObject *parent) :
    BaseClient(parent),
    Port(port)
{
    this->server = new QTcpServer(this);
    this->client = nullptr;
    //接続最大数を1に固定
    this->server->setMaxPendingConnections(1);
    //シグナルとスロットを接続
    connect(this->server,SIGNAL(newConnection()),this,SLOT(NewConnect()));
}

TCPClient::~TCPClient()
{
    if(isConnecting()){
        CloseSocket();
    }
}

