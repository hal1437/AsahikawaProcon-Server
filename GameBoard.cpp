#include "GameBoard.h"
#include "ui_GameBoard.h"
#include <QThread>
#include <QtAlgorithms>

#define MIN(x,y) ((x < y)? x : y)
#define MAX(x,y) ((x > y)? x : y)

void GameBoard::PickItem(GameSystem::Method method){

    //移動先がアイテムの場合は壁を置く
    QPoint pos = this->team_pos[static_cast<int>(method.team)];
    if(this->FieldAccess(method,pos) == GameSystem::MAP_OBJECT::ITEM){
        this->field.field[ pos                        .y()][ pos                        .x()] = GameSystem::MAP_OBJECT::NOTHING;
        this->field.field[(pos-method.GetRoteVector()).y()][(pos-method.GetRoteVector()).x()] = GameSystem::MAP_OBJECT::BLOCK;


        this->team_score[static_cast<int>(method.team)]++;
        this->leave_items--;
    }

}


QString GameBoard::GetTexturePath(GameSystem::Texture tex){
    if(tex == GameSystem::Texture::Light)return ":/Light/Texture/Light";
    if(tex == GameSystem::Texture::Heavy)return ":/Heavy/Texture/Heavy";
    if(tex == GameSystem::Texture::Jewel)return ":/Jewel/Texture/Jewel";
}

void GameBoard::resizeEvent(QResizeEvent *event){

    //常に同じアスペクト比になるようにする

    //int p = MIN(event->size().width(),event->size().height());
    //resize(p,p);

    //event->ignore();
    /*
    resize(event->size().height(),event->size().height());
    this->setMinimumSize(event->size().height(),event->size().height());
    */
    image_part.setWidth (static_cast<float>(event->size().width()) / field.size.x());
    image_part.setHeight(static_cast<float>(event->size().height()) / field.size.y());

    ReloadTexture(texture);
}
void GameBoard::paintEvent(QPaintEvent *event){

    QPainter painter(this);
    painter.setRenderHints( painter.renderHints() | QPainter::Antialiasing );
    painter.fillRect(QRect(0,0,width(),height()),Qt::white);


    if(!animation){

        //空白の描画
        for(int i = 0;i < field.size.y();i++){
            for(int j = 0;j < field.size.x();j++){
                if(overlay[i][j] != GameSystem::MAP_OVERLAY::ERASE){
                    if(field.field[i][j] == GameSystem::MAP_OBJECT::NOTHING){
                        painter.drawPixmap(j * image_part.width(),
                                           i * image_part.height(),
                                           field_resource[static_cast<int>(GameSystem::MAP_OBJECT::NOTHING)]);

                    }
                }
            }
        }

        //プレイヤーの描画
        for(int i=0;i<TEAM_COUNT;i++){
            if(0 <= team_pos[i].x() && team_pos[i].x() < field.size.x() &&
               0 <= team_pos[i].y() && team_pos[i].y() < field.size.y()){
                painter.drawPixmap(team_pos[i].x() * image_part.width(),team_pos[i].y() * image_part.height(),team_resource[i]);

            }
        }

        for(int i = 0;i < field.size.y();i++){
            for(int j = 0;j < field.size.x();j++){
                //物体の描画
                if(overlay[i][j] != GameSystem::MAP_OVERLAY::ERASE){
                    //物体の描画
                    if(field.field[i][j] != GameSystem::MAP_OBJECT::NOTHING){
                        painter.drawPixmap(j * image_part.width(),
                                            i * image_part.height(),
                                            field_resource[static_cast<int>(field.field[i][j])]);
                    }

                    //暗闇の描画
                    if(field.discover[i][j] == GameSystem::Discoverer::Unknown){

                        painter.drawPixmap(j * image_part.width() ,
                                           i * image_part.height(),
                                           overray_resource[static_cast<int>(GameSystem::MAP_OVERLAY::BLIND)]);
                    }

                    //オーバーレイの描画
                    if(overlay[i][j] != GameSystem::MAP_OVERLAY::NOTHING){
                        painter.drawPixmap(j * image_part.width() ,
                                           i * image_part.height(),
                                           overray_resource[static_cast<int>(overlay[i][j])]);
                    }
                }
            }
        }
    }

}
void GameBoard::RefreshOverlay(){
    //すべてNOTHINGにする
    for(int i = 0;i < field.size.y();i++){
        for(int j = 0;j < field.size.x();j++){
            overlay[i][j] = GameSystem::MAP_OVERLAY::NOTHING;
        }
    }
}

GameSystem::MAP_OBJECT GameBoard::FieldAccess(GameSystem::Method method, const QPoint& pos){
    //場外
    if(pos.x() <  0              || pos.y() <  0)             return GameSystem::MAP_OBJECT::BLOCK;
    if(pos.x() >= field.size.x() || pos.y() >= field.size.y())return GameSystem::MAP_OBJECT::BLOCK;

    //未開拓の場合は開拓済みフラグをつける
    field.discover[pos.y()][pos.x()] = GameSystem::Discoverer::Cool;

    //オーバーレイ描画
    if(method.action == GameSystem::Method::ACTION::LOOK    )overlay[pos.y()][pos.x()] = GameSystem::MAP_OVERLAY::LOOK;
    if(method.action == GameSystem::Method::ACTION::SEACH   )overlay[pos.y()][pos.x()] = GameSystem::MAP_OVERLAY::SEACH;
    if(method.action == GameSystem::Method::ACTION::GETREADY)overlay[pos.y()][pos.x()] = GameSystem::MAP_OVERLAY::GETREADY;

    //ターゲット位置
    for(int i=0;i<TEAM_COUNT;i++){
        if(static_cast<GameSystem::TEAM>(i) == method.team)continue;//自分はスキップ
        if(team_pos[i] == pos)return GameSystem::MAP_OBJECT::TARGET;
    }

    //通常マップオブジェクト
    return this->field.field[pos.y()][pos.x()];
}

GameSystem::AroundData GameBoard::FinishConnecting(GameSystem::TEAM team){
    GameSystem::AroundData around = FieldAccessAround(GameSystem::Method{team,GameSystem::Method::ACTION::UNKNOWN,GameSystem::Method::ROTE::UNKNOWN},
                                                     team_pos[static_cast<int>(team)]);
    around.finish();
    return around;
}

GameSystem::AroundData GameBoard::FieldAccessAround(GameSystem::TEAM team){
    return FieldAccessAround(GameSystem::Method{team,GameSystem::Method::ACTION::UNKNOWN,GameSystem::Method::ROTE::UNKNOWN},
                             team_pos[static_cast<int>(team)]);
}

GameSystem::AroundData GameBoard::FieldAccessAround(GameSystem::Method method, const QPoint& center){
    GameSystem::AroundData around;
    //接続状態:継続
    around.connect = GameSystem::CONNECTING_STATUS::CONTINUE;
    //周辺情報取得
    for(int i=0;i<9;i++){
        around.data[i] = FieldAccess(method,QPoint(center.x() + (i % 3) - 1,center.y() + (i / 3) - 1));
    }
    return around;
}
GameSystem::AroundData GameBoard::FieldAccessMethod(GameSystem::Method method){
    //周辺情報取得
    switch(method.action){
        case GameSystem::Method::ACTION::PUT:
            if((team_pos[static_cast<int>(method.team)] + method.GetRoteVector()).y() >= 0 &&
               (team_pos[static_cast<int>(method.team)] + method.GetRoteVector()).x() >= 0 &&
               (team_pos[static_cast<int>(method.team)] + method.GetRoteVector()).y() < field.size.y() &&
               (team_pos[static_cast<int>(method.team)] + method.GetRoteVector()).x() < field.size.x()){
                QPoint get_pos = team_pos[static_cast<int>(method.team)] + method.GetRoteVector();
                //put先にアイテムがある場合、残りアイテム数の数の表示を1減らさなければならない
                if(this->field.field[get_pos.y()][get_pos.x()] == GameSystem::MAP_OBJECT::ITEM){
                    this->leave_items --;
                }
                this->field.field[get_pos.y()][get_pos.x()] = GameSystem::MAP_OBJECT::BLOCK;
            }
            return FieldAccessAround(method,team_pos[static_cast<int>(method.team)]);
        case GameSystem::Method::ACTION::LOOK:
            return FieldAccessAround(method,team_pos[static_cast<int>(method.team)] + method.GetRoteVector() * 2);
        case GameSystem::Method::ACTION::WALK:
            team_pos[static_cast<int>(method.team)] += method.GetRoteVector();
            this->PickItem(method);
            return FieldAccessAround(method,team_pos[static_cast<int>(method.team)]);
        case GameSystem::Method::ACTION::SEACH:
            GameSystem::AroundData around;
            //接続状態
            around.connect = GameSystem::CONNECTING_STATUS::CONTINUE;
            //情報取得
            for(int i=1;i<10;i++){
                around.data[i-1] = FieldAccess(method,team_pos[static_cast<int>(method.team)] + method.GetRoteVector() * i);
            }
            return around;
        default:
            return GameSystem::AroundData();
    }
}

void GameBoard::setMap(const GameSystem::Map& map){
    field = map;
    this->texture = map.texture;
    ReloadTexture(texture);

    for(int i=0;i<TEAM_COUNT;i++){
        team_pos[i] = field.team_first_point[i];
    }
    map_height = field.field.size();
    map_width  = field.field[0].size();
    //オーバーレイ初期化
    overlay.resize(map_height);
    for(auto& v : overlay)v = QVector<GameSystem::MAP_OVERLAY>(map_width,GameSystem::MAP_OVERLAY::NOTHING);
    field.discover.resize(map_height);
    for(auto& v : field.discover)v = QVector<GameSystem::Discoverer>(map_width,GameSystem::Discoverer::Cool);
}


void GameBoard::PlayAnimation(GameSystem::Method method){
    //アニメーション
}



GameBoard::GameBoard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameBoard)
{
    image_part = QSize(32.0,32.0);
    //画像読み込み
    ReloadTexture(GameSystem::Texture::Light);
    ui->setupUi(this);
    for(int i = 0; i < TEAM_COUNT; i++){
        this->team_score[i] = 0;
    }
    leave_items = 0;
}

GameBoard::~GameBoard()
{
    delete ui;
}


void GameBoard::ReloadTexture(GameSystem::Texture tex){
    this->texture = tex;

    QString path = GetTexturePath(tex);

    //画像読み込み
    this->team_resource   [static_cast<int>(GameSystem::TEAM::COOL)]            = QPixmap(path + "/Cool.png");
    this->team_resource   [static_cast<int>(GameSystem::TEAM::HOT)]             = QPixmap(path + "/Hot.png");
    this->field_resource  [static_cast<int>(GameSystem::MAP_OBJECT::NOTHING)]   = QPixmap(path + "/Floor.png");
    this->field_resource  [static_cast<int>(GameSystem::MAP_OBJECT::TARGET)]    = QPixmap();
    this->field_resource  [static_cast<int>(GameSystem::MAP_OBJECT::ITEM)]      = QPixmap(path + "/Item.png");
    this->field_resource  [static_cast<int>(GameSystem::MAP_OBJECT::BLOCK)]     = QPixmap(path + "/Block.png");
    this->overray_resource[static_cast<int>(GameSystem::MAP_OVERLAY::NOTHING)]  = QPixmap();
    this->overray_resource[static_cast<int>(GameSystem::MAP_OVERLAY::LOOK)]     = QPixmap(path + "/Look.png");
    this->overray_resource[static_cast<int>(GameSystem::MAP_OVERLAY::GETREADY)] = QPixmap(path + "/Getready.png");
    this->overray_resource[static_cast<int>(GameSystem::MAP_OVERLAY::SEACH)]    = QPixmap(path + "/Search.png");
    this->overray_resource[static_cast<int>(GameSystem::MAP_OVERLAY::BLIND)]    = QPixmap(path + "/Blind.png");

    //変形
    for(QPixmap& img:team_resource )img = img.scaled(image_part.width(),image_part.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    for(QPixmap& img:field_resource){
        if(!img.isNull())img = img.scaled(image_part.width(),image_part.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    }
    for(QPixmap& img:overray_resource){
        if(!img.isNull())img = img.scaled(image_part.width(),image_part.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    }
}

