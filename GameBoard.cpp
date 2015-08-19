#include "GameBoard.h"
#include "ui_GameBoard.h"


void GameBoard::resizeEvent(QResizeEvent *event){

    //常に同じアスペクト比になるようにするとか？

    image_part_width  = static_cast<float>(event->size().width ()) / GameSystem::MAP_WIDTH;
    image_part_height = static_cast<float>(event->size().height()) / GameSystem::MAP_HEIGHT;

    ReloadTexture(texture);
}
void GameBoard::paintEvent(QPaintEvent *event)//描画イベント
{
    QPainter painter(this);
    painter.setRenderHints( painter.renderHints() | QPainter::Antialiasing );
    painter.fillRect(QRect(0,0,width(),height()),Qt::white);

    //空白の描画
    for(int i = 0;i < GameSystem::MAP_HEIGHT;i++){
        for(int j = 0;j < GameSystem::MAP_WIDTH;j++){
            painter.drawImage(j * image_part_width,i * image_part_height,field_resource[static_cast<int>(GameSystem::MAP_OBJECT::NOTHING)]);
        }
    }

    //プレイヤーの描画
    painter.drawImage(cool_pos.x() * image_part_width,cool_pos.y() * image_part_height,team_resource[static_cast<int>(GameSystem::TEAM::COOL)]);
    painter.drawImage(hot_pos.x()  * image_part_width,hot_pos.y()  * image_part_height,team_resource[static_cast<int>(GameSystem::TEAM::HOT )]);

    //空白の描画
    for(int i = 0;i < GameSystem::MAP_HEIGHT;i++){
        for(int j = 0;j < GameSystem::MAP_WIDTH;j++){
            if(field.field[i][j] != GameSystem::MAP_OBJECT::NOTHING){
                painter.drawImage(j * image_part_width,i * image_part_height,field_resource[static_cast<int>(field.field[i][j])]);
            }
        }
    }

    //オーバーレイの描画
    for(int i = 0;i < GameSystem::MAP_HEIGHT;i++){
        for(int j = 0;j < GameSystem::MAP_WIDTH;j++){
            if(overlay[i][j] != GameSystem::MAP_OVERLAY::NOTHING){
                painter.drawImage(j * image_part_width,i * image_part_height,overray_resource[static_cast<int>(overlay[i][j])]);
            }
        }
    }
}
void GameBoard::RefreshOverlay(){
    //すべてNOTINGにする
    for(int i = 0;i < GameSystem::MAP_HEIGHT;i++){
        for(int j = 0;j < GameSystem::MAP_WIDTH;j++){
            overlay[i][j] = GameSystem::MAP_OVERLAY::NOTHING;
        }
    }
}

GameSystem::MAP_OBJECT GameBoard::FieldAccess(GameSystem::TEAM team, const QPoint& pos,GameSystem::Method::ACTION action){
    //場外
    if(pos.x() <  0                     || pos.y() <  0)                     return GameSystem::MAP_OBJECT::BLOCK;
    if(pos.x() >= GameSystem::MAP_WIDTH || pos.y() >= GameSystem::MAP_HEIGHT)return GameSystem::MAP_OBJECT::BLOCK;
    //有効
    if(action == GameSystem::Method::ACTION::LOOK)    this->overlay[pos.y()][pos.x()] = GameSystem::MAP_OVERLAY::LOOK;
    if(action == GameSystem::Method::ACTION::SEACH)   this->overlay[pos.y()][pos.x()] = GameSystem::MAP_OVERLAY::SEACH;
    if(action == GameSystem::Method::ACTION::GETREADY)this->overlay[pos.y()][pos.x()] = GameSystem::MAP_OVERLAY::GETREADY;

    if(team == GameSystem::TEAM::COOL && pos == hot_pos  ||
       team == GameSystem::TEAM::HOT  && pos == cool_pos)return GameSystem::MAP_OBJECT::TARGET;
    else return this->field.field[pos.y()][pos.x()];
}
GameSystem::AroundData GameBoard::FieldAccessAround(GameSystem::TEAM team, const QPoint& center,GameSystem::Method::ACTION action){
    GameSystem::AroundData around;
    //接続状態
    around.connect = GameSystem::CONNECTING_STATUS::CONTINUE;
    //周辺情報取得
    for(int i=0;i<9;i++){
        around.data[i] = FieldAccess(team,QPoint(center.x() + (i % 3) - 1,center.y() + (i / 3) - 1),action);
    }
    return around;
}
GameSystem::AroundData GameBoard::FieldAccessMethod(GameSystem::TEAM team, GameSystem::Method method, QPoint &pos){
    //周辺情報取得
    switch(method.action){
        case GameSystem::Method::ACTION::PUT:
            if((pos + method.GetRoteVector()).y() >= 0 &&(pos + method.GetRoteVector()).x() >= 0&&
               (pos + method.GetRoteVector()).y() < GameSystem::MAP_HEIGHT &&(pos + method.GetRoteVector()).x() < GameSystem::MAP_WIDTH){
                this->field.field[(pos + method.GetRoteVector()).y()][(pos + method.GetRoteVector()).x()] = GameSystem::MAP_OBJECT::BLOCK;
            }
            return FieldAccessAround(team,pos,method.action);
        case GameSystem::Method::ACTION::LOOK:
            return FieldAccessAround(team,pos + method.GetRoteVector() * 2,method.action);
        case GameSystem::Method::ACTION::WALK:
            pos += method.GetRoteVector();
            return FieldAccessAround(team,pos,method.action);
        case GameSystem::Method::ACTION::SEACH:
            GameSystem::AroundData around;
            //接続状態
            around.connect = GameSystem::CONNECTING_STATUS::CONTINUE;
            //情報取得
            for(int i=0;i<9;i++)around.data[i] = FieldAccess(team,pos + method.GetRoteVector() * i,method.action);
            return around;
        default:
            return GameSystem::AroundData();
    }
}

void GameBoard::setMap(const GameSystem::Map& map){
    field = map;
    this->texture = map.texture;
    ReloadTexture(texture);

    cool_pos = field.cool_first_point;
    hot_pos  = field.hot_first_point;
    map_height = field.field.size();
    map_width  = field.field[0].size();
    //オーバーレイ初期化
    overlay.resize(map_height);
    for(auto& v : overlay)v = QVector<GameSystem::MAP_OVERLAY>(map_width,GameSystem::MAP_OVERLAY::NOTHING);

}
QString GameBoard::GetTexturePath(GameSystem::Texture tex){
    if(tex == GameSystem::Texture::Light)return ":/Light/Texture/Light";
    if(tex == GameSystem::Texture::Heavy)return ":/Heavy/Texture/Heavy";
}

GameBoard::GameBoard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameBoard)
{
    image_part_width  = 32.0;
    image_part_height = 32.0;
    //画像読み込み
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
    this->team_resource   [static_cast<int>(GameSystem::TEAM::COOL)]            = QImage(path + "/Cool.png");
    this->team_resource   [static_cast<int>(GameSystem::TEAM::HOT)]             = QImage(path + "/Hot.png");
    this->field_resource  [static_cast<int>(GameSystem::MAP_OBJECT::NOTHING)]   = QImage(path + "/Floor.png");
    this->field_resource  [static_cast<int>(GameSystem::MAP_OBJECT::ITEM)]      = QImage(path + "/Item.png");
    this->field_resource  [static_cast<int>(GameSystem::MAP_OBJECT::BLOCK)]     = QImage(path + "/Block.png");
    this->overray_resource[static_cast<int>(GameSystem::MAP_OVERLAY::NOTHING)]  = QImage();
    this->overray_resource[static_cast<int>(GameSystem::MAP_OVERLAY::LOOK)]     = QImage(path + "/Look.png");
    this->overray_resource[static_cast<int>(GameSystem::MAP_OVERLAY::GETREADY)] = QImage(path + "/Getready.png");
    this->overray_resource[static_cast<int>(GameSystem::MAP_OVERLAY::SEACH)]    = QImage(path + "/Search.png");

    //変形
    for(QImage& img:team_resource   )img = img.scaled(image_part_width,image_part_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    for(QImage& img:field_resource  )img = img.scaled(image_part_width,image_part_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    for(QImage& img:overray_resource){
        if(img != QImage())img = img.scaled(image_part_width,image_part_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    }
}
