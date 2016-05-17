#ifndef STARTUPDIALOG_H
#define STARTUPDIALOG_H

#include <QDialog>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include "TcpClient.h"
#include "ComClient.h"
#include "ManualClient.h"
#include "SettingDialog.h"
#include "ClientSettingForm.h"

namespace Ui {
class StartupDialog;
}

class StartupDialog : public QDialog
{
    Q_OBJECT

private:
    Ui::StartupDialog *ui;
    bool team_standby[TEAM_COUNT];
    bool map_standby;

public:
    QString music_text;
    GameSystem::Map map;

    ClientSettingForm* team_client[TEAM_COUNT];

public:
    bool MapRead(const QString& dir);

    explicit StartupDialog(QWidget *parent = 0);
    ~StartupDialog();

public slots:

    void ChangeMusicCombo(QString text);
    void CheckStandby();
    void ShowMapEditDialog();
    void PushedMapSelect();
    void ShowDesignDialog();

    void ClientStandby(ClientSettingForm* client,bool complate);
    void SetMapStandby (bool state);

    void ChangedTexture(QString text);

    //設定
    void Setting();

};

#endif // STARTUPDIALOG_H
