#include "MapEditerDialog.h"
#include "ui_MapEditerDialog.h"
#include <cmath>
#include <QFileDialog>
#include <QDesktopServices>

MapEditerDialog::MapEditerDialog(GameSystem::Map map,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MapEditerDialog),
    clicking(false),
    filepath("")
{
    ui->setupUi(this);

    //マウス追跡を有効化
    setMouseTracking(true);

    ComboChanged("決戦(15x17)");
    ui->widget->setMap(map);
    ui->listWidget->addItem(new QListWidgetItem("Nothing"));
    ui->listWidget->addItem(new QListWidgetItem("Target"));
    ui->listWidget->addItem(new QListWidgetItem("Block"));
    ui->listWidget->addItem(new QListWidgetItem("Item"));
    ui->listWidget->setIconSize(QSize(32,32));
    ui->listWidget->item(0)->setIcon(QIcon(GameBoard::GetTexturePath(map.texture) + "/Floor.png"));
    ui->listWidget->item(1)->setIcon(QIcon(GameBoard::GetTexturePath(map.texture) + "/Cool.png"));
    ui->listWidget->item(2)->setIcon(QIcon(GameBoard::GetTexturePath(map.texture) + "/Block.png"));
    ui->listWidget->item(3)->setIcon(QIcon(GameBoard::GetTexturePath(map.texture) + "/Item.png"));

    connect(ui->listWidget,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this,SLOT(SelectItem(QListWidgetItem*,QListWidgetItem*)));
    connect(ui->TurnSpin  ,SIGNAL(valueChanged(int))                                    ,this,SLOT(SpinChanged(int)));
    ui->listWidget->setCurrentRow(0);

    ui->TurnSpin->setValue(ui->widget->field.turn);
}

MapEditerDialog::~MapEditerDialog()
{
    QApplication::setOverrideCursor(Qt::ArrowCursor);
    delete ui;
}
GameSystem::Map MapEditerDialog::GetMap(){
    return ui->widget->field;
}

void MapEditerDialog::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        this->clicking = true;
        FillItem(event->pos());
    }
}
void MapEditerDialog::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton)this->clicking = false;
}
void MapEditerDialog::mouseMoveEvent(QMouseEvent* event){
    if(clicking){
        //設置
        FillItem(event->pos());
    }
}

void MapEditerDialog::FillItem(const QPoint& pos){
    int left_m,top_m;
    this->layout()->getContentsMargins(&left_m,&top_m,nullptr,nullptr);
    QPoint fill_point((pos.x() - left_m)/ui->widget->image_part.width(),(pos.y() - top_m)/ui->widget->image_part.height());

    //有効範囲内でなければスキップ
    if(fill_point.x() < 0 || fill_point.x() >= ui->widget->field.size.x() ||
       fill_point.y() < 0 || fill_point.y() >= ui->widget->field.size.y())return;

    GameSystem::MAP_OBJECT obj;
    if     (ui->listWidget->selectedItems().first()->text() == "Nothing")obj = GameSystem::MAP_OBJECT::NOTHING;
    else if(ui->listWidget->selectedItems().first()->text() == "Block"  )obj = GameSystem::MAP_OBJECT::BLOCK;
    else if(ui->listWidget->selectedItems().first()->text() == "Item"   )obj = GameSystem::MAP_OBJECT::ITEM;
    if(ui->listWidget->selectedItems().first()->text() == "Target" ){
        //初期位置変更
        this->ui->widget->field.team_first_point[static_cast<int>(GameSystem::TEAM::COOL)] = fill_point;
        this->ui->widget->field.team_first_point[static_cast<int>(GameSystem::TEAM::HOT )] = ui->widget->field.MirrorPoint(fill_point);
        this->ui->widget->team_pos[static_cast<int>(GameSystem::TEAM::COOL)] = fill_point;
        this->ui->widget->team_pos[static_cast<int>(GameSystem::TEAM::HOT )] = ui->widget->field.MirrorPoint(fill_point);
    }else{
        this->ui->widget->field.field[fill_point.y()][fill_point.x()] = obj;
        //対称コピー
        if(ui->SymmetryCheck->isChecked()){
            QPoint r_fill_point(ui->widget->field.MirrorPoint(fill_point));
            this->ui->widget->field.field[r_fill_point.y()][r_fill_point.x()] = obj;
        }
    }
    update();
}

void MapEditerDialog::Clear(){
    for(auto& v1 :ui->widget->field.field){
        for(auto& v2 : v1){
            v2 = GameSystem::MAP_OBJECT::NOTHING;
        }
    }
    update();
}
void MapEditerDialog::SpinChanged(int value){
    ui->widget->field.turn = value;
}
void MapEditerDialog::Export(){

    filepath = QFileDialog::getSaveFileName( this,
                                             tr("マップを保存"),
                                             QDir::homePath(),
                                             tr("マップデータ (*.map)") );
    if(filepath != "")ui->widget->field.Export(filepath);
}
void MapEditerDialog::SelectItem(QListWidgetItem *next, QListWidgetItem *old){
    if(next){
        QIcon icon = next->icon();
        QCursor myCursor = QCursor(icon.pixmap(icon.actualSize(QSize(20, 20))));
        QApplication::setOverrideCursor(myCursor);
    }
}

void MapEditerDialog::ComboChanged(QString value){
    if(value=="広域(21x17)"){
        this->ui->widget->field.SetSize(QPoint(21,17));
    }
    if(value=="決戦(15x17)"){
        this->ui->widget->field.SetSize(QPoint(15,17));
    }

    this->ui->widget->team_pos[static_cast<int>(GameSystem::TEAM::COOL)] = this->ui->widget->field.team_first_point[static_cast<int>(GameSystem::TEAM::COOL)];
    this->ui->widget->team_pos[static_cast<int>(GameSystem::TEAM::HOT )] = this->ui->widget->field.team_first_point[static_cast<int>(GameSystem::TEAM::HOT )];
    resize(QSize(ui->widget->field.size.x()*25+134,ui->widget->field.size.y()*25+4));
    ui->widget->setMap(ui->widget->field);
    paintEvent(nullptr);
    update();

}
