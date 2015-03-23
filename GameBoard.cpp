#include "GameBoard.h"
#include "ui_GameBoard.h"


void GameBoard::resizeEvent(QResizeEvent *event){

    //常に同じアスペクト比になるようにするとか？

    image_part_width  = static_cast<float>(event->size().width ()) / GameSystem::MAP_WIDTH;
    image_part_height = static_cast<float>(event->size().height()) / GameSystem::MAP_HEIGHT;

    //画像読み込み
    this->team_resource[static_cast<int>(GameSystem::TEAM::COOL)]    = QImage(":/Pictures/cool1.jpg");
    this->team_resource[static_cast<int>(GameSystem::TEAM::HOT)]     = QImage(":/Pictures/hot1.jpg");
    this->field_resource[static_cast<int>(GameSystem::MAP_OBJECT::NOTHING)] = QImage(":/Pictures/floor1.jpg");
    this->field_resource[static_cast<int>(GameSystem::MAP_OBJECT::ITEM)]    = QImage(":/Pictures/item1.jpg");
    this->field_resource[static_cast<int>(GameSystem::MAP_OBJECT::BLOCK)]   = QImage(":/Pictures/block1.jpg");

    //変形
    for(QImage& img:team_resource )img = img.scaled(image_part_width,image_part_height,Qt::IgnoreAspectRatio);
    for(QImage& img:field_resource)img = img.scaled(image_part_width,image_part_height,Qt::IgnoreAspectRatio);

}
void GameBoard::paintEvent(QPaintEvent *event)//描画イベント
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);//アンチエイリアスセット
    painter.setPen(QPen(Qt::green, 3));

    //画像の描画
    for(int i = 0;i < GameSystem::MAP_HEIGHT;i++){
        for(int j = 0;j < GameSystem::MAP_WIDTH;j++){
            painter.drawImage(j * image_part_width,i * image_part_height,field_resource[static_cast<int>(field.field[i][j])]);
        }
    }
    //プレイヤーの描画
    painter.drawImage(cool_pos.x() * image_part_width,cool_pos.y() * image_part_height,team_resource[static_cast<int>(GameSystem::TEAM::COOL)]);
    painter.drawImage(hot_pos.x()  * image_part_width,hot_pos.y()  * image_part_height,team_resource[static_cast<int>(GameSystem::TEAM::HOT )]);
    //オーバーレイの描画
    for(int i = 0;i < GameSystem::MAP_HEIGHT;i++){
        for(int j = 0;j < GameSystem::MAP_WIDTH;j++){
            if(overlay[i][j] != GameSystem::MAP_OVERLAY::NOTHING){
                QPoint center = QPoint(image_part_width * j + image_part_width / 2.0,image_part_height * i + image_part_height / 2.0);
                painter.drawLine(QLine(QPoint(center.x()+7,center.y()),QPoint(center.x()-7,center.y())));
                painter.drawLine(QLine(QPoint(center.x(),center.y()+7),QPoint(center.x(),center.y()-7)));
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

GameSystem::MAP_OBJECT GameBoard::FieldAccess(GameSystem::TEAM team, const QPoint& pos){
    //場外
    if(pos.x() <  0                     || pos.y() <  0)                     return GameSystem::MAP_OBJECT::BLOCK;
    if(pos.x() >= GameSystem::MAP_WIDTH || pos.y() >= GameSystem::MAP_HEIGHT)return GameSystem::MAP_OBJECT::BLOCK;
    //有効
    this->overlay[pos.y()][pos.x()] = GameSystem::MAP_OVERLAY::GETREADY;
    if(team == GameSystem::TEAM::COOL && pos == hot_pos  ||
       team == GameSystem::TEAM::HOT  && pos == cool_pos)return GameSystem::MAP_OBJECT::TARGET;
    else return this->field.field[pos.y()][pos.x()];
}
GameSystem::AroundData GameBoard::FieldAccessAround(GameSystem::TEAM team, const QPoint& center){
    GameSystem::AroundData around;
    //接続状態
    around.connect = GameSystem::CONNECTING_STATUS::CONTINUE;
    //周辺情報取得
    for(int i=0;i<9;i++){
        around.data[i] = FieldAccess(team,QPoint(center.x() + (i % 3) - 1,center.y() + (i / 3) - 1));
    }
    return around;
}
GameSystem::AroundData GameBoard::FieldAccessMethod(GameSystem::TEAM team, GameSystem::Method method, QPoint &pos){
    //周辺情報取得
    switch(method.action){
        case GameSystem::Method::ACTION::PUT:
            return FieldAccessAround(team,pos);
        case GameSystem::Method::ACTION::LOOK:
            return FieldAccessAround(team,pos + method.GetRoteVector() * 2);
        case GameSystem::Method::ACTION::WALK:
            pos += method.GetRoteVector();
            return FieldAccessAround(team,pos);
        case GameSystem::Method::ACTION::SEACH:
            GameSystem::AroundData around;
            //接続状態
            around.connect = GameSystem::CONNECTING_STATUS::CONTINUE;
            //情報取得
            for(int i=0;i<9;i++)around.data[i] = FieldAccess(team,pos + method.GetRoteVector() * i);
            return around;
        default:
            return GameSystem::AroundData();
    }
}

void GameBoard::setMap(const GameSystem::Map& map){
    field = map;

    cool_pos = field.cool_first_point;
    hot_pos  = field.hot_first_point;
    map_height = field.field.size();
    map_width  = field.field[0].size();
    //オーバーレイ初期化
    overlay.resize(map_height);
    for(auto& v : overlay)v = QVector<GameSystem::MAP_OVERLAY>(map_width,GameSystem::MAP_OVERLAY::NOTHING);

}

GameBoard::GameBoard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameBoard)
{
    //画像読み込み
    this->team_resource[static_cast<int>(GameSystem::TEAM::COOL)]    = QImage(":/Pictures/cool1.jpg");
    this->team_resource[static_cast<int>(GameSystem::TEAM::HOT)]     = QImage(":/Pictures/hot1.jpg");
    this->field_resource[static_cast<int>(GameSystem::MAP_OBJECT::NOTHING)] = QImage(":/Pictures/floor1.jpg");
    this->field_resource[static_cast<int>(GameSystem::MAP_OBJECT::ITEM)]    = QImage(":/Pictures/item1.jpg");
    this->field_resource[static_cast<int>(GameSystem::MAP_OBJECT::BLOCK)]   = QImage(":/Pictures/block1.jpg");

    image_part_width  = 32.0;
    image_part_height = 32.0;

    //変形
    for(QImage& img:team_resource )img = img.scaled(image_part_width,image_part_height,Qt::IgnoreAspectRatio);
    for(QImage& img:field_resource)img = img.scaled(image_part_width,image_part_height,Qt::IgnoreAspectRatio);

    ui->setupUi(this);
}

GameBoard::~GameBoard()
{
    delete ui;
}
