#include "GameBoard.h"
#include "ui_GameBoard.h"
#include <QThread>

QString GameBoard::GetTexturePath(GameSystem::Texture tex){
    if(tex == GameSystem::Texture::Light)return ":/Light/Texture/Light";
    if(tex == GameSystem::Texture::Heavy)return ":/Heavy/Texture/Heavy";
}

void GameBoard::resizeEvent(QResizeEvent *event){

    //常に同じアスペクト比になるようにするとか？

    image_part.setWidth(static_cast<float>(event->size().width ()) / field.size.x());
    image_part.setHeight(static_cast<float>(event->size().height()) / field.size.y());

    ReloadTexture(texture);
}
void GameBoard::paintEvent(QPaintEvent *event){
/*
    image_part.setWidth (static_cast<float>(size().width ()) / field.size.x());
    image_part.setHeight(static_cast<float>(size().height()) / field.size.y());
    ReloadTexture(texture);
*/

    QPainter painter(this);
    painter.setRenderHints( painter.renderHints() | QPainter::Antialiasing );
    painter.fillRect(QRect(0,0,width(),height()),Qt::white);


    if(!animation){
        //空白の描画
        for(int i = 0;i < field.size.y();i++){
            for(int j = 0;j < field.size.x();j++){
            }
        }

        //プレイヤーの描画
        for(int i=0;i<TEAM_COUNT;i++){
            painter.drawPixmap(team_pos[i].x() * image_part.width(),team_pos[i].y() * image_part.height(),team_resource[i]);
        }

        for(int i = 0;i < field.size.y();i++){
            for(int j = 0;j < field.size.x();j++){
                //物体の描画
                if(field.field[i][j] != GameSystem::MAP_OBJECT::NOTHING){
                    painter.drawPixmap(j * image_part.width(),
                                        i * image_part.height(),
                                        field_resource[static_cast<int>(field.field[i][j])]);
                }else{
                    //空白の描画
                    painter.drawPixmap(j * image_part.width(),
                                        i * image_part.height(),
                                        field_resource[static_cast<int>(GameSystem::MAP_OBJECT::NOTHING)]);

                }
                //オーバーレイの描画
                if(overlay[i][j] != GameSystem::MAP_OVERLAY::NOTHING){
                    painter.drawPixmap(j * image_part.width(),
                                        i * image_part.height(),
                                        overray_resource[static_cast<int>(overlay[i][j])]);
                }
            }
        }

    }else{
        //const std::pair<GameSystem::TEAM,GameSystem::Method>& process = past_log;
        //アニメーション
    }
}
void GameBoard::RefreshOverlay(){
    //すべてNOTINGにする
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
                this->field.field[get_pos.y()][get_pos.x()] = GameSystem::MAP_OBJECT::BLOCK;
                 }
            return FieldAccessAround(method,team_pos[static_cast<int>(method.team)]);
        case GameSystem::Method::ACTION::LOOK:
            return FieldAccessAround(method,team_pos[static_cast<int>(method.team)] + method.GetRoteVector() * 2);
        case GameSystem::Method::ACTION::WALK:
            team_pos[static_cast<int>(method.team)] += method.GetRoteVector();
            return FieldAccessAround(method,team_pos[static_cast<int>(method.team)]);
        case GameSystem::Method::ACTION::SEACH:
            GameSystem::AroundData around;
            //接続状態
            around.connect = GameSystem::CONNECTING_STATUS::CONTINUE;
            //情報取得
            for(int i=1;i<10;i++)around.data[i-1] = FieldAccess(method,team_pos[static_cast<int>(method.team)] + method.GetRoteVector() * i);
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
    this->overray_resource[static_cast<int>(GameSystem::MAP_OVERLAY::SEACH)]    = QPixmap(path + "/Seach.png");

    //変形
    for(QPixmap& img:team_resource )img = img.scaled(image_part.width(),image_part.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    for(QPixmap& img:field_resource){
        if(!img.isNull())img = img.scaled(image_part.width(),image_part.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    }
    for(QPixmap& img:overray_resource){
        if(!img.isNull())img = img.scaled(image_part.width(),image_part.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    }
}
