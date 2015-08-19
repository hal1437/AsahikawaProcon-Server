#ifndef GAMESYSTEM_H
#define GAMESYSTEM_H

#include <QObject>
#include <QPoint>
#include <QVector>
#include <QtGlobal>
#include <QTime>

//二次元配列エイリアス
template <class T>
using Field = QVector<QVector<T>>;

class GameSystem
{
public:
    const static int MAP_WIDTH  = 21;
    const static int MAP_HEIGHT = 17;

    //チーム
    enum class TEAM{
        COOL,
        HOT ,
    };
    //勝った人
    enum class WINNER{
        COOL,
        HOT ,
        DRAW,
        CONTINUE,
    };
    //接続状態
    enum class CONNECTING_STATUS{
        FINISHED,
        CONTINUE,
    };
    //マップ上に存在する物体
    enum class MAP_OBJECT{
        NOTHING = 0,
        TARGET  = 1,
        BLOCK   = 2,
        ITEM    = 3,
    };
    //テクスチャ
    enum class Texture{
        Light = 0, //あっさり
        Heavy = 1, //こってり
    };
    //マップ上に描画する非物体
    enum class MAP_OVERLAY{
        NOTHING,
        LOOK,
        SEACH,
        GETREADY,
    };
    //ゲーム盤
    struct Map{
        Field<GameSystem::MAP_OBJECT> field;
        int turn;
        QString name;
        QPoint cool_first_point;
        QPoint hot_first_point;
        GameSystem::Texture texture;

        void CreateRandomMap();
        bool Export(QString Filename);
    };


    //クライアントの行動
    struct Method{
        //クライアントの行動
        enum class ACTION{
            WALK,
            LOOK,
            SEACH,
            PUT,
            GETREADY,
            UNKNOWN,
        };
        //方向
        enum class ROTE{
            UP,
            DOWN,
            RIGHT,
            LEFT,
            UNKNOWN
        };
        ACTION action;
        ROTE   rote;

        QPoint  GetRoteVector(); //方向ベクトル取得
        static Method fromString(const QString& str);
    };
    //周辺情報
    struct AroundData{
        CONNECTING_STATUS connect;
        MAP_OBJECT data[9];

        QString toString();      //文字列変換
    };
};

#endif // GAMESYSTEM_H
