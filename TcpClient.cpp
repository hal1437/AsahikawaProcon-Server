#include "TcpClient.h"

bool TCPClient::WaitGetReady(){
    //ターン開始文字列
    client->write(QString("@\r\n").toUtf8());

    //レスポンス待ち
    if(this->client->waitForReadyRead(this->TIMEOUT)){
        //レスポンスあり

        QString response;
        response = client->readAll();
        qDebug() << response;
        return (response == "gr\r\n");
    }else{
        //レスポンスなし

        qDebug() << QString("[WaitGetReady Port") + QString::number(this->Port) +"]:Noting responce";
        return false;
    }

}
GameSystem::Method TCPClient::WaitReturnMethod(GameSystem::AroundData data){
    //周辺情報文字列
    client->write(QString(data.toString() + "\r\n").toUtf8());

    //レスポンス待ち
    if(this->client->waitForReadyRead(this->TIMEOUT)){
        //レスポンスあり

        QString response;
        response = client->readAll();
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
    //周辺情報文字列
    client->write(QString(data.toString() + "\r\n").toUtf8());

    //レスポンス待ち
    if(this->client->waitForReadyRead(this->TIMEOUT)){
        //レスポンスあり

        QString response;
        response = client->readAll();
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
    return true;
}
bool TCPClient::CloseSocket(){
    this->server->close();
    emit Ready(false);
    return true;
}
bool TCPClient::isConnecting(){
    return this->server->isListening();
}
void TCPClient::NewConnect(){
    this->client = this->server->nextPendingConnection();
    this->IP     = this->client->localAddress().toString();
    connect(this->client,SIGNAL(readyRead()),this,SLOT(GetTeamName()));
    connect(this->client,SIGNAL(disconnected()),this,SLOT(DisConnect()));
    emit Connected();
}
void TCPClient::DisConnect(){
    emit Disconnected();
    this->client = nullptr;
    this->IP   = "";
    this->Name = "";
}

QString TCPClient::GetTeamName(){
    if(this->Name == ""){
        this->Name = client->readAll();
        disconnect(this->client,SIGNAL(readyRead()),this,SLOT(GetTeamName()));
        emit ReturnTeamName(this->Name);
        emit Ready(true);
        return this->Name;
    }
    return this->Name;
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

