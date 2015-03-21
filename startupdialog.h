#ifndef STARTUPDIALOG_H
#define STARTUPDIALOG_H

#include <QDialog>
#include "TcpClient.h"
#include "ComClient.h"

namespace Ui {
class StartupDialog;
}

class StartupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartupDialog(QWidget *parent = 0);
    ~StartupDialog();

private:
    Ui::StartupDialog *ui;
    bool hot_standby;
    bool cool_standby;
    BaseClient* hot_client;
    BaseClient* cool_client;

public:


public slots:

    void HotConnectionToggled(bool state);
    void HotComboBoxChenged(QString text);

    void CoolConnectionToggled(bool state);
    void CoolComboBoxChenged(QString text);

    void SetHotStandby (bool state);
    void SetCoolStandby(bool state);

};

#endif // STARTUPDIALOG_H
