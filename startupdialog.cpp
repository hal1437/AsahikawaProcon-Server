#include "startupdialog.h"
#include "ui_startupdialog.h"

void StartupDialog::SetHotStandby (bool state){
    this->hot_standby = state;
    this->ui->ServerStartButton->setEnabled(hot_standby == true && cool_standby == true);
}

void StartupDialog::SetCoolStandby(bool state){
    this->cool_standby = state;
    this->ui->ServerStartButton->setEnabled(hot_standby == true && cool_standby == true);
}

StartupDialog::StartupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartupDialog),
    hot_standby(false),
    cool_standby(false)
{
    ui->setupUi(this);
    this->hot_client = new TCPClient(0);
    this->cool_client = new TCPClient(0);
}

StartupDialog::~StartupDialog()
{
    delete ui;
}

void StartupDialog::HotConnectionToggled(bool state){
    if(state){
        this->ui->HotConnectButton->setText("接続可能");
        this->ui->HotStateLabel->setText("接続可能");
        this->ui->HotPortSpinBox->setEnabled(false);
        SetCoolStandby(false);
    }else{
        this->ui->HotConnectButton->setText("待機開始");
        this->ui->HotStateLabel->setText("非接続");
        this->ui->HotPortSpinBox->setEnabled(true);
        SetCoolStandby(true);
    }
}

void StartupDialog::HotComboBoxChenged(QString text){
    if(text=="TCPユーザー"){

        this->ui->HotStateLabel->setText("非接続");
        this->ui->HotNameLabel->setText("不明");
        this->ui->HotPortSpinBox->setEnabled(true);
        this->ui->HotConnectButton->setEnabled(true);
        SetHotStandby(false);
    }else if(text=="自動くん"){
        this->ui->HotStateLabel->setText("自動");
        this->ui->HotNameLabel->setText("自動くん");
        this->ui->HotPortSpinBox->setEnabled(false);
        this->ui->HotConnectButton->setEnabled(false);
        SetHotStandby(true);
    }
}

void StartupDialog::CoolConnectionToggled(bool state){
    if(state){
        this->ui->CoolConnectButton->setText("接続可能");
        this->ui->CoolStateLabel->setText("接続可能");
        this->ui->CoolPortSpinBox->setEnabled(false);
    }else{
        this->ui->CoolConnectButton->setText("待機開始");
        this->ui->CoolStateLabel->setText("非接続");
        this->ui->CoolPortSpinBox->setEnabled(true);
    }
}

void StartupDialog::CoolComboBoxChenged(QString text){

    if(text=="TCPユーザー"){
        this->ui->CoolStateLabel->setText("非接続");
        this->ui->CoolNameLabel->setText("不明");
        this->ui->CoolPortSpinBox->setEnabled(true);
        this->ui->CoolConnectButton->setEnabled(true);
        SetCoolStandby(false);
    }else if(text=="自動くん"){
        this->ui->CoolStateLabel->setText("自動");
        this->ui->CoolNameLabel->setText("自動くん");
        this->ui->CoolPortSpinBox->setEnabled(false);
        this->ui->CoolConnectButton->setEnabled(false);
        SetCoolStandby(true);
    }
}

