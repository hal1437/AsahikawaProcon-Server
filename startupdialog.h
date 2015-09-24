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

namespace Ui {
class StartupDialog;
}

class StartupDialog : public QDialog
{
    Q_OBJECT

private:
    Ui::StartupDialog *ui;
    bool map_standby;
    bool hot_standby;
    bool cool_standby;

public:
    GameSystem::Map map;

    BaseClient* hot_client;
    BaseClient* cool_client;

public:
    bool MapRead(const QString& dir);

    explicit StartupDialog(QWidget *parent = 0);
    ~StartupDialog();

public slots:

    void ShowMapEditDialog();

    void PushedMapSelect();

    void SetMapStandby (bool state);
    void SetHotStandby ();
    void SetCoolStandby();

    void HotConnected  ();
    void CoolConnected ();

    void HotDisConnected  ();
    void CoolDisConnected ();

    void HotConnectionToggled (bool state);
    void CoolConnectionToggled(bool state);

    void HotComboBoxChenged (QString text);
    void CoolComboBoxChenged(QString text);

    void ChangedTexture(QString text);

    //設定
    void Setting();

};

#endif // STARTUPDIALOG_H
