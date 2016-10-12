#include "TcpClient.h"
#include <QSettings>

QString TCPClient::VisibilityString(QString str){
    QString answer;
    for(int i = 0;i < str.size();i++){
        if     (str[i] == '\n')answer.push_back("\\n");
        else if(str[i] == '\r')answer.push_back("\\r");
        else   answer.push_back(str[i]);
    }
    return answer;
}

QString TCPClient::WaitResponce(){
    int ignore = 0;
    while(ignore != this->IGNORE_INVALD){
        //レスポンス待ち
        qDebug() << "WaitStart:";
        if(this->client->waitForReadyRead(this->TIMEOUT)){
            //レスポンスあり

            QString response = "";

            //自動結合
            bool f = false;
            do{
                response += client->readLine();
            }while(*(response.end()-1) != '\n' && this->client->waitForReadyRead(this->TIMEOUT));

            qDebug() << "" + VisibilityString(response);

            //不正文字列：空
            if(response == "" || response == "\n" || response == "\r" || response == "\r\n"){
                ignore++;

                continue;
            }
            //不正文字列：改行なし
            if(response.size() > 0 && *(response.end()-1) != '\n'){
                disconnected_flag = true;
                qDebug() << QString("[Port") + QString::number(this->client->localPort()) +"]:Noting \\n";

                return QString();
            }

            qDebug()<<"test:"+response;
            return response;
        }else{
            //レスポンスなし
            disconnected_flag = true;
            //qDebug() << QString("[Port") + QString::number(this->client->localPort()) +"]:Noting responce";
            return QString();
        }
    }
    disconnected_flag=true;
    //qDebug() << QString("[Port") + QString::number(this->client->localPort()) +"]:Too many invald responce";
    return QString();
}

bool TCPClient::WaitGetReady(){
    if(disconnected_flag)return false;
    //ターン開始文字列
    if(client == nullptr)return false;
    client->write(QString("@\r\n").toUtf8());

    //レスポンス待ち
    QString response = WaitResponce();
    qDebug() << "res:"+response;
    qDebug() << (response == "gr\r\n");
    return (response == "gr\r\n");
}
GameSystem::Method TCPClient::WaitReturnMethod(GameSystem::AroundData data){
    if(disconnected_flag)return GameSystem::Method();
    //周辺情報文字列
    if(client == nullptr)return GameSystem::Method{GameSystem::TEAM::UNKNOWN,
                GameSystem::Method::ACTION::UNKNOWN,
                GameSystem::Method::ROTE::UNKNOWN};
    client->write(QString(data.toString() + "\r\n").toUtf8());
    qDebug() << "WRM";
    qDebug() << data.toString();
    //レスポンス待ち
    QString response = WaitResponce();
    if(response != QString())return GameSystem::Method::fromString(response);
    else return GameSystem::Method{GameSystem::TEAM::UNKNOWN,
                                   GameSystem::Method::ACTION::UNKNOWN,
                                   GameSystem::Method::ROTE::UNKNOWN};
}
bool TCPClient::WaitEndSharp(GameSystem::AroundData data){
    if(disconnected_flag)return false;

    if(client == nullptr)return false;
    //周辺情報文字列
    client->write(QString(data.toString() + "\r\n").toUtf8());
    qDebug() << "WES";
    qDebug() << data.toString();
    //レスポンス待ち
    return(WaitResponce() == "#\r\n");
}


bool TCPClient::OpenSocket(int Port){
    this->server->listen(QHostAddress::Any,Port);
    return true;
}
bool TCPClient::CloseSocket(){
    if(this->client != nullptr)this->client->disconnectFromHost();
    this->client = nullptr;
    this->server->close();
    this->server = new QTcpServer(this);
    emit DisConnect();
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
    disconnected_flag=true;
}

QString TCPClient::GetTeamName(){
    if(this->Name == ""){
        this->Name = client->readAll();
        disconnect(this->client,SIGNAL(readyRead()),this,SLOT(GetTeamName()));
        emit WriteTeamName();
        emit Ready();
        return this->Name;
    }
    return this->Name;
}

TCPClient::TCPClient(QObject *parent) :
    BaseClient(parent)
{
    QSettings* mSettings;
    mSettings = new QSettings( "setting.ini", QSettings::IniFormat ); // iniファイルで設定を保存
    mSettings->setIniCodec( "UTF-8" ); // iniファイルの文字コード
    QVariant v = mSettings->value( "Timeout" );
    if (v.type() != QVariant::Invalid){
        TIMEOUT = v.toInt();
    }

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

