#ifndef MANUALCLIENTDIALOG_H
#define MANUALCLIENTDIALOG_H

#include <QDialog>
#include <QKeyEvent>
#include "GameSystem.h"

namespace Ui {
class ManualClientDialog;
}

class ManualClientDialog : public QDialog
{
    Q_OBJECT
public:
    GameSystem::Method next_method; //実行される関数

    explicit ManualClientDialog(QWidget *parent = 0);
    ~ManualClientDialog();


protected:
    void closeEvent(QCloseEvent* ce);
    void keyPressEvent(QKeyEvent* event);

private:
    Ui::ManualClientDialog *ui;

    //現在のメソッドのアクションを取得
    GameSystem::Method::ACTION GetAction();

signals:
    //関数決定シグナル
    void ReadyAction();

    void CloseWindow();

private slots:
    //各スロット
    void UPButtonClicked();
    void DOWNButtonClicked();
    void RIGHTButtonClicked();
    void LEFTButtonClicked();

public slots:
    void AppendLog(const QString& str);

};

#endif // MANUALCLIENT_H
