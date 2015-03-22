#include "GraphicField.h"
#include "ui_GraphicField.h"


void GraphicField::resizeEvent(QResizeEvent *event){

    //常に同じアスペクト比になるようにするとか？

    image_part_width  = static_cast<float>(event->size().width ()) / GameSystem::MAP_WIDTH;
    image_part_height = static_cast<float>(event->size().height()) / GameSystem::MAP_HEIGHT;

    //画像読み込み
    this->field_resource[static_cast<int>(GameSystem::MAP_OBJECT::NOTHING)] = QImage(":/Pictures/floor1.jpg");
    this->field_resource[static_cast<int>(GameSystem::MAP_OBJECT::COOL)]    = QImage(":/Pictures/cool1.jpg");
    this->field_resource[static_cast<int>(GameSystem::MAP_OBJECT::HOT)]     = QImage(":/Pictures/hot1.jpg");
    this->field_resource[static_cast<int>(GameSystem::MAP_OBJECT::ITEM)]    = QImage(":/Pictures/item1.jpg");
    this->field_resource[static_cast<int>(GameSystem::MAP_OBJECT::BLOCK)]   = QImage(":/Pictures/block1.jpg");

    //変形
    for(QImage& img:field_resource)img = img.scaled(image_part_width,image_part_height,Qt::IgnoreAspectRatio);

}


void GraphicField::paintEvent(QPaintEvent *)//描画イベント
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);//アンチエイリアスセット
    painter.setPen(QPen(Qt::black, 12, Qt::DashDotLine, Qt::RoundCap));
    painter.setBrush(QBrush(Qt::green, Qt::SolidPattern));
    for(int i = 0;i < GameSystem::MAP_HEIGHT;i++){
        for(int j = 0;j < GameSystem::MAP_WIDTH;j++){
            painter.drawImage(j * image_part_width,i * image_part_height,field_resource[static_cast<int>(field[i][j])]);
        }
    }

}

GraphicField::GraphicField(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphicField)
{
    //range-based-for を使った初期化
    field  .resize(GameSystem::MAP_HEIGHT);
    overlay.resize(GameSystem::MAP_HEIGHT);
    for(auto& v : field  )v = QVector<GameSystem::MAP_OBJECT >(GameSystem::MAP_WIDTH,GameSystem::MAP_OBJECT ::BLOCK);
    for(auto& v : overlay)v = QVector<GameSystem::MAP_OVERLAY>(GameSystem::MAP_WIDTH,GameSystem::MAP_OVERLAY::NOTHING);

    //画像読み込み
    this->field_resource[static_cast<int>(GameSystem::MAP_OBJECT::NOTHING)] = QImage(":/Pictures/floor1.jpg");
    this->field_resource[static_cast<int>(GameSystem::MAP_OBJECT::COOL)]    = QImage(":/Pictures/cool1.jpg");
    this->field_resource[static_cast<int>(GameSystem::MAP_OBJECT::HOT)]     = QImage(":/Pictures/hot1.jpg");
    this->field_resource[static_cast<int>(GameSystem::MAP_OBJECT::ITEM)]    = QImage(":/Pictures/item1.jpg");
    this->field_resource[static_cast<int>(GameSystem::MAP_OBJECT::BLOCK)]   = QImage(":/Pictures/block1.jpg");
    image_part_width  = 32.0;
    image_part_height = 32.0;

    //変形
    for(QImage& img:field_resource)img = img.scaled(image_part_height,image_part_width,Qt::IgnoreAspectRatio);

    ui->setupUi(this);
}

GraphicField::~GraphicField()
{
    delete ui;
}
