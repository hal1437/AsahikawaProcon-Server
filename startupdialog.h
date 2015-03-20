#ifndef STARTUPDIALOG_H
#define STARTUPDIALOG_H

#include <QDialog>

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
};

#endif // STARTUPDIALOG_H
