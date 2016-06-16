#include "startupdialog.h"
#include "ui_startupdialog.h"
#include "MapEditerDialog.h"
#include "DesignDialog.h"
#include <QNetworkInterface>
#include <QDesktopServices>
#include <QHostInfo>

StartupDialog::StartupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartupDialog),
    map_standby(false)
{
    //UI初期化
    ui->setupUi(this);
    music_text = ui->GameMusicCombo->currentText();
    ui->CoolGroupBox->SetPortSpin(2009);
    ui->HotGroupBox ->SetPortSpin(2010);

    //クライアント初期化
    this->team_client[static_cast<int>(GameSystem::TEAM::COOL)] = ui->CoolGroupBox;
    this->team_client[static_cast<int>(GameSystem::TEAM::HOT )] = ui->HotGroupBox ;
    for(int i=0;i<TEAM_COUNT;i++){
        team_standby[i] = false;
    }

    connect(ui->CoolGroupBox,SIGNAL(Standby(ClientSettingForm*,bool)),this,SLOT(ClientStandby(ClientSettingForm*,bool)));
    connect(ui->HotGroupBox ,SIGNAL(Standby(ClientSettingForm*,bool)),this,SLOT(ClientStandby(ClientSettingForm*,bool)));

    //ローカルIPの探索
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
            this->ui->LocalIPLabel->setText(address.toString());
    }
    this->ui->HostName->setText(QHostInfo::localHostName());

    map.CreateRandomMap();
    map_standby = true;
}

StartupDialog::~StartupDialog()
{
    delete ui;
}


void StartupDialog::ChangeMusicCombo(QString text){
    music_text = text;
}

void StartupDialog::CheckStandby(){
    bool all_of = true;
    for(int i=0;i<TEAM_COUNT;i++){
        if(!team_standby[i])all_of = false;
    }
    this->ui->ServerStartButton->setEnabled(all_of && map_standby);
}

void StartupDialog::ShowMapEditDialog(){
    MapEditerDialog diag(map);
    if(diag.exec()){
        if(diag.filepath == ""){
            this->ui->MapDirEdit->setText("[CUSTOM MAP]");
            map = diag.GetMap();
        }else{
            //再読み込み
            if(MapRead(diag.filepath))this->ui->MapDirEdit->setText(diag.filepath);
        }
    }
    SetMapStandby(true);
}

bool StartupDialog::MapRead(const QString& dir){
    //ファイルからマップを読み込む
    this->map.Import(dir);
    map.size.setY(map.field.size());
    return true;
}
void StartupDialog::PushedMapSelect(){
    QString folder = QDir::currentPath();
    QString cap    = tr("マップを開く");
    QString filter = tr("マップファイル (*.map)");

    QString filePath = QFileDialog::getOpenFileName( this, cap, folder, filter );
    this->ui->MapDirEdit->setText(filePath);
    SetMapStandby(MapRead(filePath));
}

void StartupDialog::ClientStandby(ClientSettingForm* client,bool complate){
    for(int i=0;i<TEAM_COUNT;i++){
        if(team_client[i] == client){
            team_standby[i] = complate;
            CheckStandby();
            return;
        }
    }
}

void StartupDialog::SetMapStandby (bool state){
    map_standby = state;
    CheckStandby();
}

void StartupDialog::ChangedTexture(QString text){
    if(text == "あっさり")this->map.texture = GameSystem::Texture::Light;
    if(text == "こってり")this->map.texture = GameSystem::Texture::Heavy;
}

void StartupDialog::Setting(){
    SettingDialog* diag;
    diag = new SettingDialog;
    if(diag->exec() == QDialog::Accepted){
        //設定を保存
        diag->Export();
    }
}

void StartupDialog::ShowDesignDialog(){
    DesignDialog* diag;
    diag = new DesignDialog;
    if(diag->exec() == QDialog::Accepted){
        //設定を保存
        diag->Export();
    }
}

