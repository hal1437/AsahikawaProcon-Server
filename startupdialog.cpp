#include "startupdialog.h"
#include "ui_startupdialog.h"
#include <QNetworkInterface>

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
    this->ui->ServerStartButton->setEnabled(hot_standby == true && cool_standby == true);
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
    this->ui->ServerStartButton->setEnabled(hot_standby == true && cool_standby == true);
}

StartupDialog::StartupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartupDialog),
    hot_standby(false),
    cool_standby(false)
{
    ui->setupUi(this);
    this->cool_client = new TCPClient(2009);
    this->hot_client  = new TCPClient(2010);
    //ローカルIPの探索
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
            this->ui->LocalIPLabel->setText(address.toString());
    }
    connect(this->hot_client ,SIGNAL(Ready(bool)),this,SLOT(SetHotStandby(bool)));
    connect(this->cool_client,SIGNAL(Ready(bool)),this,SLOT(SetCoolStandby(bool)));
}

StartupDialog::~StartupDialog()
{
    delete ui;
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
    connect(this->hot_client,SIGNAL(Ready(bool)),this,SLOT(SetHotStandby(bool)));
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
    connect(this->cool_client,SIGNAL(Ready(bool)),this,SLOT(SetCoolStandby(bool)));
}

