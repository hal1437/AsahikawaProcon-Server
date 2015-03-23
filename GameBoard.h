#ifndef GRAPHICFIELD_H
#define GRAPHICFIELD_H

#include <QWidget>
#include <QVector>
#include <QPainter>
#include <QResizeEvent>
#include <QDebug>
#include "GameSystem.h"

namespace Ui {
class GameBoard;
}

class GameBoard : public QWidget
{
    Q_OBJECT
private:
    int map_width;
    int map_height;

    float image_part_width;
    float image_part_height;

    QImage team_resource[2];
    QImage field_resource[5];

protected:
    void resizeEvent(QResizeEvent *event); //リサイズイベント
    void paintEvent(QPaintEvent *event);         //ペイントイベント
public:

    QPoint cool_pos;
    QPoint hot_pos;
    GameSystem::Map field;
    Field<GameSystem::MAP_OVERLAY> overlay;

    //フィールドへアクセスする
    GameSystem::MAP_OBJECT FieldAccess(GameSystem::TEAM team, const QPoint& pos);
    //周辺情報を取得する
    GameSystem::AroundData FieldAccessAround(GameSystem::TEAM team,const QPoint& center);
    //メソッドで周辺情報を取得する
    GameSystem::AroundData FieldAccessMethod(GameSystem::TEAM team,GameSystem::Method method,QPoint& pos);

    //マップを設定する
    void setMap(const GameSystem::Map &map);

    //オーバーレイを全て削除する
    void RefreshOverlay();

    explicit GameBoard(QWidget *parent = 0);
    ~GameBoard();

private:
    Ui::GameBoard *ui;

public slots:
};

#endif // GRAPHICFIELD_H
