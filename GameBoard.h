#ifndef GRAPHICFIELD_H
#define GRAPHICFIELD_H

#include <QWidget>
#include <QVector>
#include <QPainter>
#include <QResizeEvent>
#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include "GameSystem.h"

namespace Ui {
class GameBoard;
}

class GameBoard : public QWidget
{
    Q_OBJECT
private:
    int map_width;  //全体高さ
    int map_height; //全体幅

    //Animations
    bool animation    = false; //アニメーション有効
    bool animationing = false; //アニメーション中

    GameSystem::Texture texture; //使用テクスチャタイプ
    QPixmap team_resource[TEAM_COUNT];   //チーム画像
    QPixmap field_resource[4];  //フィールド画像
    QPixmap overray_resource[4];//オーバーレイ画像

public:
    GameSystem::Map field; //フィールド状態
    Field<GameSystem::MAP_OVERLAY> overlay;//オーバーレイ状態

    QSize image_part; //単体画像サイズ
    QPoint team_pos[TEAM_COUNT]; //チーム位置

    GameSystem::Method past_method;//前回のログ

protected:
    void paintEvent (QPaintEvent *event);   //ペイントイベント
    void resizeEvent(QResizeEvent *event); //リサイズイベント

public:
    //テクスチャパス取得
    static QString GetTexturePath(GameSystem::Texture tex);

public:
    //フィールドへアクセスする
    GameSystem::MAP_OBJECT FieldAccess(GameSystem::Method method, const QPoint& pos);
    //周辺情報を取得する
    GameSystem::AroundData FieldAccessAround(GameSystem::TEAM team);
    GameSystem::AroundData FieldAccessAround(GameSystem::Method method, const QPoint& center);
    //メソッドで周辺情報を取得する
    GameSystem::AroundData FieldAccessMethod(GameSystem::Method method);

    //マップを設定する
    void setMap(const GameSystem::Map &map);

    //オーバーレイを全て削除する
    void RefreshOverlay();

    //アニメーション開始
    void PlayAnimation(GameSystem::Method method);

    explicit GameBoard(QWidget *parent = 0);
    ~GameBoard();

private:
    Ui::GameBoard *ui;

signals:
    //アニメーション終了
    void FinishAnimations();

public slots:
    //テクスチャ読み込み
    void ReloadTexture(GameSystem::Texture tex);
};

#endif // GRAPHICFIELD_H
