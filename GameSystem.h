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

//チーム情報マクロ
#define TEAM_COUNT 2
#define TEAMS COOL,HOT

class GameSystem
{
public:
    //通常マップサイズ
    //const static int MAP_WIDTH  = 21;
    //const static int MAP_HEIGHT = 17;

    //決戦マップサイズ
    //const static int MAP_WIDTH  = 15;
    //const static int MAP_HEIGHT = 17;

    //通常マップサイズ
    const static int DEFAULT_MAP_WIDTH  = 15;
    const static int DEFAULT_MAP_HEIGHT = 17;

    //チーム
    enum class TEAM{
        TEAMS,
        UNKNOWN
    };

    //チーム情報
    struct TEAM_PROPERTY {
        const static int TEAMS_COUNT = TEAM_COUNT; //チーム数

        //チーム名取得
        static QString getTeamName(GameSystem::TEAM team);
    };

    //勝った人
    enum class WINNER{
        TEAMS,
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

    //探索済みかの判定
    enum class Discoverer{
        Unknown = 0,
        Hot     = 1,
        Cool    = 2,
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
        BLIND,
        ERASE,
    };
    //ゲーム盤
    struct Map{
        Field<GameSystem::MAP_OBJECT> field;
        Field<GameSystem::Discoverer> discover;
        int turn;                       //ターン
        QString name;                   //ステージ名
        QPoint size;                    //マップサイズ
        QPoint team_first_point[TEAM_COUNT];//チーム初期位置
        GameSystem::Texture texture;    //テクスチャ

        Map();

        void SetSize(QPoint size);
        QPoint MirrorPoint(const QPoint& pos);

        void CreateRandomMap();
        bool Import(QString Filename);
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
        TEAM   team;
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
