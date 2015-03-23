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
    hot_score = 0;
    cool_score = 0;

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
        PickItem(GameSystem::TEAM::COOL,cool_mehod,ui->Field->cool_pos);
        process = HOT_GETREADY;
    }
    else if(process == HOT_GETREADY){
        startup->hot_client->WaitGetReady();
        hot_mehod = startup->hot_client->WaitReturnMethod(this->ui->Field->FieldAccessAround(GameSystem::TEAM::HOT,ui->Field->hot_pos));
        process = HOT_ACTION;
    }
    else if(process == HOT_ACTION){
        startup->hot_client->WaitEndSharp(this->ui->Field->FieldAccessMethod(GameSystem::TEAM::HOT,hot_mehod,ui->Field->hot_pos));
        PickItem(GameSystem::TEAM::HOT,hot_mehod,ui->Field->hot_pos);
        this->ui->TimeBar->setValue(this->ui->TimeBar->value() - 1);
        process = COOL_GETREADY;
    };

    this->ui->TurnLabel->setText("Turn : " + QString::number(ui->TimeBar->value()));
    repaint();

    //End
    GameSystem::WINNER win = Judge();
    if(win != GameSystem::WINNER::CONTINUE)Finish(win);

}

void MainWindow::PickItem(GameSystem::TEAM team, GameSystem::Method method, QPoint &pos){

    if(ui->Field->FieldAccess(team,pos) == GameSystem::MAP_OBJECT::ITEM){
        ui->Field->field.field[ pos                        .y()][ pos                        .x()] = GameSystem::MAP_OBJECT::NOTHING;
        ui->Field->field.field[(pos-method.GetRoteVector()).y()][(pos-method.GetRoteVector()).x()] = GameSystem::MAP_OBJECT::BLOCK;
        if(team == GameSystem::TEAM::COOL)cool_score++;
        if(team == GameSystem::TEAM::HOT) hot_score++;
        ui->CoolScoreLabel->setText("Cool : " + QString::number(cool_score));
        ui->HotScoreLabel ->setText("Hot : "  + QString::number(hot_score));
    }

}
void MainWindow::Finish(GameSystem::WINNER winner){
    this->clock->stop();
    QString append_str = "";

    if(startup->cool_client->disconnected_flag)append_str.append("[Cool Disconnected...]");
    else if(startup->hot_client ->disconnected_flag)append_str.append("[Hot Disconnected...]");

    if(winner == GameSystem::WINNER::COOL)this->ui->WinnerLabel->setText("COOL WIN!" + append_str);
    if(winner == GameSystem::WINNER::HOT) this->ui->WinnerLabel->setText("HOT WIN!"  + append_str);
    if(winner == GameSystem::WINNER::DRAW)this->ui->WinnerLabel->setText("DRAW");
}
GameSystem::WINNER MainWindow::Judge(){
    bool cool_lose;
    bool hot_lose;
    GameBoard*& board = this->ui->Field;
    GameSystem::AroundData cool_around;
    GameSystem::AroundData hot_around;

    cool_around = board->FieldAccessAround(GameSystem::TEAM::COOL,board->cool_pos);
    hot_around  = board->FieldAccessAround(GameSystem::TEAM::HOT ,board->hot_pos );

    //ブロック置かれ死
    if(cool_around.data[4] == GameSystem::MAP_OBJECT::BLOCK)cool_lose=true;
    if(hot_around .data[4] == GameSystem::MAP_OBJECT::BLOCK)hot_lose=true;

    //ブロック囲まれ死
    if(cool_around.data[1] == GameSystem::MAP_OBJECT::BLOCK &&
       cool_around.data[3] == GameSystem::MAP_OBJECT::BLOCK &&
       cool_around.data[5] == GameSystem::MAP_OBJECT::BLOCK &&
       cool_around.data[7] == GameSystem::MAP_OBJECT::BLOCK)cool_lose=true;
    if(hot_around .data[1] == GameSystem::MAP_OBJECT::BLOCK &&
       hot_around .data[3] == GameSystem::MAP_OBJECT::BLOCK &&
       hot_around .data[5] == GameSystem::MAP_OBJECT::BLOCK &&
       hot_around .data[7] == GameSystem::MAP_OBJECT::BLOCK)hot_lose=true;

    //切断死
    if(startup->cool_client->disconnected_flag)cool_lose=true;
    if(startup->hot_client ->disconnected_flag)hot_lose =true;

    //相打ち、または時間切れ時はアイテム判定とする
    if((cool_lose && hot_lose) || ui->TimeBar->value()==0){
        cool_lose = false;
        hot_lose  = false;

        if(hot_score == cool_score)return GameSystem::WINNER::DRAW;
        else if(hot_score > cool_score)return GameSystem::WINNER::HOT;
        else if(hot_score < cool_score)return GameSystem::WINNER::COOL;

    }
    if(cool_lose)return GameSystem::WINNER::HOT;
    else if(hot_lose)return GameSystem::WINNER::COOL;
    else return GameSystem::WINNER::CONTINUE;
}
