#include "startupdialog.h"
#include "ui_startupdialog.h"
#include <QNetworkInterface>
#include <QDesktopServices>


StartupDialog::StartupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartupDialog),
    map_standby(false),
    hot_standby(false),
    cool_standby(false)
{
    //マップ初期化
    map.resize(GameSystem::MAP_HEIGHT);
    for(auto& v : map)v = QVector<GameSystem::MAP_OBJECT >(GameSystem::MAP_WIDTH,GameSystem::MAP_OBJECT ::BLOCK);

    //UI初期化
    ui->setupUi(this);

    //クライアント初期化
    this->cool_client = new TCPClient(2009);
    this->hot_client  = new TCPClient(2010);

    //ローカルIPの探索
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
            this->ui->LocalIPLabel->setText(address.toString());
    }

    //シグナル・スロットの接続
    connect(this->hot_client ,SIGNAL(Connected())   ,this,SLOT(HotConnected()));
    connect(this->hot_client ,SIGNAL(Ready(bool))   ,this,SLOT(SetHotStandby(bool)));
    connect(this->hot_client ,SIGNAL(Disconnected()),this,SLOT(HotDisConnected()));
    connect(this->cool_client,SIGNAL(Connected())   ,this,SLOT(CoolConnected()));
    connect(this->cool_client,SIGNAL(Ready(bool))   ,this,SLOT(SetCoolStandby(bool)));
    connect(this->cool_client,SIGNAL(Disconnected()),this,SLOT(CoolDisConnected()));
}

StartupDialog::~StartupDialog()
{
    delete ui;
}

bool StartupDialog::MapRead(const QString& dir){
    char Map_chars[5];
    Map_chars[static_cast<int>(GameSystem::MAP_OBJECT::NOTHING)] = ' ';
    Map_chars[static_cast<int>(GameSystem::MAP_OBJECT::COOL)]    = 'C';
    Map_chars[static_cast<int>(GameSystem::MAP_OBJECT::HOT)]     = 'H';
    Map_chars[static_cast<int>(GameSystem::MAP_OBJECT::ITEM)]    = 'I';
    Map_chars[static_cast<int>(GameSystem::MAP_OBJECT::BLOCK)]   = '#';

    //旧式のマップフォーマットが不明のためリテラルは暫定とする。

    QFile file(dir);
    int  item_count = 0;
    bool exist_cool = false;
    bool exist_hot  = false;


    if (file.open(QIODevice::ReadOnly)){
        for(int i=0;i<GameSystem::MAP_HEIGHT;i++){
            char buf[1024]={};
            int str_length;
            str_length = file.readLine(buf,1024);

            //文字列が満たない
            if(str_length < GameSystem::MAP_WIDTH){
                QMessageBox msg;
                msg.setText(QString("エラー:") +
                            QString::number(i) + "行目が" +
                            QString::number(GameSystem::MAP_WIDTH) + "文字未満かもしれません。\n" +
                            "そこは空白で埋めて読み込みます。");
                msg.exec();
            }

            //マップ読み出し
            for(int j = 0;j < GameSystem::MAP_WIDTH;j++){
                if(buf[j] == Map_chars[static_cast<int>(GameSystem::MAP_OBJECT::NOTHING)])map[i][j] = GameSystem::MAP_OBJECT::NOTHING;
                if(buf[j] == Map_chars[static_cast<int>(GameSystem::MAP_OBJECT::BLOCK  )])map[i][j] = GameSystem::MAP_OBJECT::BLOCK;
                if(buf[j] == Map_chars[static_cast<int>(GameSystem::MAP_OBJECT::ITEM   )]){
                    map[i][j] = GameSystem::MAP_OBJECT::ITEM;
                    item_count++;
                }
                if(buf[j] == Map_chars[static_cast<int>(GameSystem::MAP_OBJECT::COOL   )]){
                    map[i][j] = GameSystem::MAP_OBJECT::COOL;
                    exist_cool = true;
                }
                if(buf[j] == Map_chars[static_cast<int>(GameSystem::MAP_OBJECT::HOT    )]){
                    map[i][j] = GameSystem::MAP_OBJECT::HOT;
                    exist_hot = true;
                }
            }
        }
        //初期位置が存在しないとか
        if(exist_cool == false || exist_hot == false){
            QMessageBox msg;
            QString user;
            if(exist_cool == false && exist_hot == false)user = "CoolとHot";
            else if(exist_cool == false)user = "Cool";
            else if(exist_hot  == false)user = "Hot";
            msg.setText(QString("エラー:") + user + "の初期位置が存在しません");
            msg.exec();
            return false;
        }
        //アイテム数が少なすぎる
        if(item_count < 4){
            QMessageBox msg;
            msg.setText("警告:なんかアイテム少なくね？まあこのままでも実行できるけどさ");
            msg.exec();
        }
        return true;
    }else{
        return false;
    }

}

void StartupDialog::PushedMapSelect(){

    QString folder = QDir::homePath();
    QString cap    = tr("マップを開く");
    QString filter = tr("マップファイル (*.map)");

    QString filePath = QFileDialog::getOpenFileName( this, cap, folder, filter );
    this->ui->MapDirEdit->setText(filePath);
    SetMapStandby(MapRead(filePath));
}

void StartupDialog::SetMapStandby (bool state){
    map_standby = state;
    this->ui->ServerStartButton->setEnabled(hot_standby && cool_standby && map_standby);
}

void StartupDialog::SetHotStandby (bool state){
    this->hot_standby = state;

    if(state){
        this->ui->HotNameLabel ->setText(this->hot_client->Name == "" ? "Hot" : this->hot_client->Name);
        this->ui->HotIPLabel   ->setText(this->hot_client->IP);
        this->ui->HotStateLabel->setText("準備完了");
        this->ui->HotConnectButton->setText("　切断　");
    }else{
        this->ui->HotNameLabel ->setText("不明");
        this->ui->HotIPLabel   ->setText("不明");
        this->ui->HotStateLabel->setText("未接続");

    }
    this->ui->ServerStartButton->setEnabled(hot_standby && cool_standby && map_standby);
}
void StartupDialog::SetCoolStandby(bool state){
    this->cool_standby = state;

    if(state){
        this->ui->CoolNameLabel ->setText(this->cool_client->Name == "" ? "Cool" : this->cool_client->Name);
        this->ui->CoolIPLabel   ->setText(this->cool_client->IP);
        this->ui->CoolStateLabel->setText("準備完了");
        this->ui->CoolConnectButton->setText("　切断　");
    }else{
        this->ui->CoolNameLabel ->setText("不明");
        this->ui->CoolIPLabel   ->setText("不明");
        this->ui->CoolStateLabel->setText("未接続");

    }
    this->ui->ServerStartButton->setEnabled(hot_standby && cool_standby && map_standby);
}

void StartupDialog::HotConnected  (){
    this->ui->HotNameLabel ->setText("不明");
    this->ui->HotIPLabel   ->setText(this->hot_client->IP);
    this->ui->HotStateLabel->setText("接続中");
    this->ui->HotConnectButton->setText("　切断　");
}
void StartupDialog::CoolConnected (){
    this->ui->CoolNameLabel ->setText("不明");
    this->ui->CoolIPLabel   ->setText(this->cool_client->IP);
    this->ui->CoolStateLabel->setText("接続中");
    this->ui->CoolConnectButton->setText("　切断　");
}
void StartupDialog::HotDisConnected (){
    this->ui->HotConnectButton->toggle();
}
void StartupDialog::CoolDisConnected (){
    this->ui->CoolConnectButton->toggle();
}

void StartupDialog::HotConnectionToggled(bool state){
    if(state){
        dynamic_cast<TCPClient*>(this->hot_client)->OpenSocket();
        this->ui->HotConnectButton->setText("待機終了");
        this->ui->HotStateLabel->setText("接続可能");
        this->ui->HotPortSpinBox->setEnabled(false);
    }else{
        dynamic_cast<TCPClient*>(this->hot_client)->CloseSocket();
        this->ui->HotConnectButton->setText("接続開始");
        this->ui->HotStateLabel->setText("非接続");
        this->ui->HotNameLabel->setText("不明");
        this->ui->HotIPLabel->setText("不明");
        this->ui->HotPortSpinBox->setEnabled(true);
    }
}
void StartupDialog::CoolConnectionToggled(bool state){
    if(state){
        dynamic_cast<TCPClient*>(this->cool_client)->OpenSocket();
        this->ui->CoolConnectButton->setText("待機終了");
        this->ui->CoolStateLabel->setText("接続可能");
        this->ui->CoolPortSpinBox->setEnabled(false);
    }else{
        dynamic_cast<TCPClient*>(this->cool_client)->CloseSocket();
        this->ui->CoolConnectButton->setText("接続開始");
        this->ui->CoolStateLabel->setText("非接続");
        this->ui->CoolNameLabel->setText("不明");
        this->ui->CoolIPLabel->setText("不明");
        this->ui->CoolPortSpinBox->setEnabled(true);
    }
}

void StartupDialog::HotComboBoxChenged(QString text){
    if(text=="TCPユーザー"){
        this->hot_client = new TCPClient(this->ui->HotPortSpinBox->value(),this);
        this->ui->HotStateLabel->setText("非接続");
        this->ui->HotNameLabel->setText("不明");
        this->ui->HotPortSpinBox->setEnabled(true);
        this->ui->HotConnectButton->setEnabled(true);
        SetHotStandby(false);
    }else if(text=="自動くん"){
        this->hot_client = new ComClient(this);
        this->ui->HotStateLabel->setText("自動");
        this->ui->HotNameLabel->setText("自動くん");
        this->ui->HotPortSpinBox->setEnabled(false);
        this->ui->HotConnectButton->setEnabled(false);
        SetHotStandby(true);
    }
    connect(this->hot_client,SIGNAL(Connected())   ,this,SLOT(HotConnected()));
    connect(this->hot_client,SIGNAL(Ready(bool))   ,this,SLOT(SetHotStandby(bool)));
    connect(this->hot_client,SIGNAL(Disconnected()),this,SLOT(HotDisConnected()));
}
void StartupDialog::CoolComboBoxChenged(QString text){
    if(text=="TCPユーザー"){
        this->cool_client = new TCPClient(this->ui->CoolPortSpinBox->value(),this);
        dynamic_cast<TCPClient*>(this->cool_client)->OpenSocket();
        this->ui->CoolStateLabel->setText("非接続");
        this->ui->CoolNameLabel->setText("不明");
        this->ui->CoolPortSpinBox->setEnabled(true);
        this->ui->CoolConnectButton->setEnabled(true);
        SetCoolStandby(false);
    }else if(text=="自動くん"){
        this->cool_client = new ComClient();
        this->ui->CoolStateLabel->setText("自動");
        this->ui->CoolNameLabel->setText("自動くん");
        this->ui->CoolPortSpinBox->setEnabled(false);
        this->ui->CoolConnectButton->setEnabled(false);
        SetCoolStandby(true);
    }
    connect(this->cool_client,SIGNAL(Connected())   ,this,SLOT(CoolConnected()));
    connect(this->cool_client,SIGNAL(Ready(bool))   ,this,SLOT(SetCoolStandby(bool)));
    connect(this->cool_client,SIGNAL(Disconnected()),this,SLOT(CoolDisConnected()));
}

