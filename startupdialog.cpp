#include "startupdialog.h"
#include "ui_startupdialog.h"
#include "MapEditerDialog.h"
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

    //クライアント初期化
    this->team_client[static_cast<int>(GameSystem::TEAM::COOL)] = ui->CoolGroupBox->client;
    this->team_client[static_cast<int>(GameSystem::TEAM::HOT )] = ui->HotGroupBox ->client;

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


void StartupDialog::ShowMapEditDialog(){
    MapEditerDialog diag(map);
    if(diag.exec()){
        if(diag.filepath == ""){
            this->ui->MapDirEdit->setText("[CUSTOM MAP]");
            map = diag.GetMap();
        }
        else{
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
}
void StartupDialog::PushedMapSelect(){
    QString folder = QDir::currentPath();
    QString cap    = tr("マップを開く");
    QString filter = tr("マップファイル (*.map)");

    QString filePath = QFileDialog::getOpenFileName( this, cap, folder, filter );
    this->ui->MapDirEdit->setText(filePath);
    SetMapStandby(MapRead(filePath));
}

void StartupDialog::ClientStandby(BaseClient* client,bool complate){
    team_standby[qFind(team_client,team_client+TEAM_COUNT,client) - team_client] = complate;
}

void StartupDialog::SetMapStandby (bool state){
    map_standby = state;
    bool all_of = true;
    for(int i=0;i<TEAM_COUNT;i++){
        if(!team_standby[i])all_of = false;
    }
    this->ui->ServerStartButton->setEnabled(all_of && map_standby);
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


