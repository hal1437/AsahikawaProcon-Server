#ifndef MAPEDITERDIALOG_H
#define MAPEDITERDIALOG_H

#include <QDialog>

namespace Ui {
class MapEditerDialog;
}

class MapEditerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MapEditerDialog(QWidget *parent = 0);
    ~MapEditerDialog();

private:
    Ui::MapEditerDialog *ui;
};

#endif // MAPEDITERDIALOG_H
