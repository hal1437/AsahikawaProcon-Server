#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::keyPressEvent(QKeyEvent * event){
    //縦に比率を合わせる
    if(event->key()==Qt::Key_F){
        int left_margin=0,right_margin=0;
        this->ui->centralWidget->layout()->getContentsMargins(&left_margin,nullptr,&right_margin,nullptr);
        this->ui->Field->resize((static_cast<float>(this->ui->Field->size().height())/GameSystem::MAP_HEIGHT)*GameSystem::MAP_WIDTH,this->ui->Field->size().height());
        this->resize(QSize(this->ui->Field->width() + left_margin + right_margin,this->size().height()));
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->startup = new StartupDialog(this);
    process = COOL_GETREADY;
    if(this->startup->exec()){
        //ui初期化
        this->ui->Field  ->setMap(this->startup->map);
        this->ui->TimeBar->setMaximum(this->startup->map.turn);
        this->ui->TimeBar->setValue  (this->startup->map.turn);
        this->ui->TurnLabel     ->setText("Turn : " + QString::number(this->ui->TimeBar->value()));
        this->ui->HotNameLabel  ->setText(this->startup->hot_client ->Name == "" ? "Hot"  : this->startup->hot_client ->Name);
        this->ui->CoolNameLabel ->setText(this->startup->cool_client->Name == "" ? "Cool" : this->startup->cool_client->Name);
        this->ui->HotScoreLabel ->setText("Hot : 0");
        this->ui->CoolScoreLabel->setText("Cool : 0");

        //タイマーコネクト
        this->clock = new QTimer();
        connect(this->clock,SIGNAL(timeout()),this,SLOT(StepGame()));
        this->clock->start(FRAME_RATE);
    }else{
        exit(0);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::StepGame(){
    //ゲーム進行
    static GameSystem::Method cool_mehod;
    static GameSystem::Method hot_mehod;
    this->ui->Field->RefreshOverlay();

    if(process == COOL_GETREADY){
        startup->cool_client->WaitGetReady();
        cool_mehod = startup->cool_client->WaitReturnMethod(this->ui->Field->FieldAccessAround(GameSystem::TEAM::COOL,ui->Field->cool_pos));
        process = COOL_ACTION;
    }
    else if(process == COOL_ACTION){
        startup->cool_client->WaitEndSharp(this->ui->Field->FieldAccessMethod(GameSystem::TEAM::COOL,cool_mehod,ui->Field->cool_pos));
        this->ui->TimeBar->setValue(this->ui->TimeBar->value() - 1);
        process = HOT_GETREADY;
    }
    else if(process == HOT_GETREADY){
        startup->hot_client->WaitGetReady();
        hot_mehod = startup->hot_client->WaitReturnMethod(this->ui->Field->FieldAccessAround(GameSystem::TEAM::HOT,ui->Field->hot_pos));
        process = HOT_ACTION;
    }
    else if(process == HOT_ACTION){
        startup->hot_client->WaitEndSharp(this->ui->Field->FieldAccessMethod(GameSystem::TEAM::HOT,hot_mehod,ui->Field->hot_pos));
        this->ui->TimeBar->setValue(this->ui->TimeBar->value() - 1);
        process = COOL_GETREADY;
    };

    this->ui->TurnLabel->setText("Turn : " + QString::number(ui->TimeBar->value()));
    repaint();

    //End

}




