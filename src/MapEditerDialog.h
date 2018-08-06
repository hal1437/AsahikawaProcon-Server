#ifndef MAPEDITERDIALOG_H
#define MAPEDITERDIALOG_H

#include <QDialog>
#include <QMouseEvent>
#include <QListWidgetItem>
#include "GameSystem.h"

namespace Ui {
class MapEditerDialog;
}

class MapEditerDialog : public QDialog
{
    Q_OBJECT

public:
    static const int IMAGE_PART_SIZE = 25;
    QString filepath;

    GameSystem::Map GetMap();
    explicit MapEditerDialog(GameSystem::Map map,QWidget *parent = 0);
    ~MapEditerDialog();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    Ui::MapEditerDialog *ui;
    bool clicking;

private slots:
    void Export();                    //ファイル書き出し
    void FillItem(const QPoint& pos); //設置
    void Clear();                     //全消し
    void SpinChanged(int value);
    void ComboChanged(QString value);
    void ReCount();

public slots:
    void SelectItem(QListWidgetItem* next, QListWidgetItem* old);
};

#endif // MAPEDITERDIALOG_H
