#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>

QString getTime(){
    return QString("[") + QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + QString("]");
}
QString convertString(GameSystem::Method method){
    QString str;
    if(method.action == GameSystem::Method::ACTION::GETREADY)str += "GetReady";
    if(method.action == GameSystem::Method::ACTION::LOOK)    str += "Look";
    if(method.action == GameSystem::Method::ACTION::PUT)     str += "Put";
    if(method.action == GameSystem::Method::ACTION::SEACH)   str += "Seach";
    if(method.action == GameSystem::Method::ACTION::WALK)    str += "Walk";

    if(method.rote == GameSystem::Method::ROTE::UP)str += "Up";
    if(method.rote == GameSystem::Method::ROTE::RIGHT)str += "Right";
    if(method.rote == GameSystem::Method::ROTE::LEFT)str += "Left";
    if(method.rote == GameSystem::Method::ROTE::DOWN)str += "Down";

    return str;
}


void MainWindow::keyPressEvent(QKeyEvent * event){
    //縦に比率を合わせる
    if(event->key()==Qt::Key_F){
        int left_margin=0,right_margin=0;
        this->ui->centralWidget->layout()->getContentsMargins(&left_margin,nullptr,&right_margin,nullptr);
        this->ui->Field->resize((static_cast<float>(this->ui->Field->size().height())/ui->Field->field.size.y())*ui->Field->field.size.x(),this->ui->Field->size().height());
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

    //ServerSetting読み込み
    QString path;
    QSettings* mSettings;
    mSettings = new QSettings( "setting.ini", QSettings::IniFormat ); // iniファイルで設定を保存
    mSettings->setIniCodec( "UTF-8" ); // iniファイルの文字コード
    QVariant v = mSettings->value( "LogFilepath" );
    if (v.type() != QVariant::Invalid){
        path = v.toString();
    }
    v = mSettings->value( "Gamespeed" );
    if (v.type() != QVariant::Invalid){
        FRAME_RATE = v.toInt();
    }

    //ログファイルオープン
    file = new QFile(QString(path + "/log" + getTime() + ".txt"),this);
    file->open(QIODevice::WriteOnly);
    log.setDevice(file);

    //スタートアップダイアログ開始
    if(this->startup->exec()){
        this->ui->Field->cool_pos = this->startup->map.cool_first_point;
        this->ui->Field->hot_pos = this->startup->map.hot_first_point;
        qDebug() << this->ui->Field->cool_pos;
        qDebug() << this->ui->Field->hot_pos;
        this->ui->Field->setMap(this->startup->map);
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
    log << getTime() + "セットアップ完了　ゲームを開始します。\n";
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
    static int turn_count;

    if(ui->TimeBar->value() != turn_count){
       turn_count = ui->TimeBar->value();
       log << QString("-----第") + QString::number(ui->TimeBar->value()) + "ターン-----" + "\n";
    }

    if(process == COOL_GETREADY){
        if(!startup->cool_client->WaitGetReady()){
            log << getTime() + "[停止]COOLが正常にGetReadyを返しませんでした!" << "\n";
        }
        cool_mehod = startup->cool_client->WaitReturnMethod(this->ui->Field->FieldAccessAround(GameSystem::TEAM::COOL,ui->Field->cool_pos,GameSystem::Method::ACTION::GETREADY));

        process = COOL_ACTION;
    }
    else if(process == COOL_ACTION){
        startup->cool_client->WaitEndSharp(this->ui->Field->FieldAccessMethod(GameSystem::TEAM::COOL,cool_mehod,ui->Field->cool_pos));
        this->ui->TimeBar->setValue(this->ui->TimeBar->value() - 1);
        PickItem(GameSystem::TEAM::COOL,cool_mehod,ui->Field->cool_pos);

        if(cool_mehod.action == GameSystem::Method::ACTION::GETREADY){
            log << getTime() + "[停止]COOLが行動メソッドを呼ぶべき位置にGetReadyを呼んでいます！" << "\n";
        }else if (cool_mehod.rote   == GameSystem::Method::ROTE::UNKNOWN){
            log << getTime() + "[停止]COOLの行動メソッドが不正な方向を示しています！" << "\n";
        }

        process = HOT_GETREADY;
        log << getTime() + "[行動]COOLが" + convertString(cool_mehod) + "を行いました。" << "\n";
    }
    else if(process == HOT_GETREADY){
        if(!startup->hot_client->WaitGetReady()){
            log << getTime() + "[停止]HOTが正常にGetReadyを返しませんでした!" << "\n";
        }
        hot_mehod = startup->hot_client->WaitReturnMethod(this->ui->Field->FieldAccessAround(GameSystem::TEAM::HOT,ui->Field->hot_pos,GameSystem::Method::ACTION::GETREADY));

        process = HOT_ACTION;
    }
    else if(process == HOT_ACTION){
        startup->hot_client->WaitEndSharp(this->ui->Field->FieldAccessMethod(GameSystem::TEAM::HOT,hot_mehod,ui->Field->hot_pos));
        PickItem(GameSystem::TEAM::HOT,hot_mehod,ui->Field->hot_pos);
        this->ui->TimeBar->setValue(this->ui->TimeBar->value() - 1);

        if(hot_mehod.action == GameSystem::Method::ACTION::GETREADY){
            log << getTime() + "[停止]HOTが行動メソッドを呼ぶべき位置にGetReadyを呼んでいます！" << "\n";
        }else if (hot_mehod.rote   == GameSystem::Method::ROTE::UNKNOWN){
            log << getTime() + "[停止]HOTの行動メソッドが不正な方向を示しています！" << "\n";
        }
        process = COOL_GETREADY;
        log << getTime() + "[行動]HOTが" + convertString(hot_mehod) + "を行いました。" << "\n";
    }

    this->ui->TurnLabel->setText("Turn : " + QString::number(ui->TimeBar->value()));
    repaint();

    //End
    GameSystem::WINNER win = Judge();
    if(win != GameSystem::WINNER::CONTINUE)Finish(win);

}

void MainWindow::PickItem(GameSystem::TEAM team, GameSystem::Method method, QPoint &pos){

    if(ui->Field->FieldAccess(team,pos,GameSystem::Method::ACTION::UNKNOWN) == GameSystem::MAP_OBJECT::ITEM){
        ui->Field->field.field[ pos                        .y()][ pos                        .x()] = GameSystem::MAP_OBJECT::NOTHING;
        ui->Field->field.field[(pos-method.GetRoteVector()).y()][(pos-method.GetRoteVector()).x()] = GameSystem::MAP_OBJECT::BLOCK;
        if(team == GameSystem::TEAM::COOL){
            cool_score++;
            log << getTime() + "[取得]COOLがアイテムを取得しました。" << "\n";
        }
        if(team == GameSystem::TEAM::HOT){
            hot_score++;
            log << getTime() + "[取得]HOTがアイテムを取得しました。" << "\n";
        }
        ui->CoolScoreLabel->setText("Cool : " + QString::number(cool_score));
        ui->HotScoreLabel ->setText("Hot : "  + QString::number(hot_score));
    }

}
void MainWindow::Finish(GameSystem::WINNER winner){
    this->clock->stop();
    QString append_str = "";

    if(startup->cool_client->disconnected_flag){
        append_str.append("[Cool Disconnected...]");
        log << getTime() + "[終了]COOLとの通信が切断されています。" << "\n";
    }else if(startup->hot_client ->disconnected_flag){
        append_str.append("[Hot Disconnected...]");
        log << getTime() + "[終了]HOTとの通信が切断されています。" << "\n";
    }
    log << this->ui->WinnerLabel->text() << "\n";

    if(winner == GameSystem::WINNER::COOL){
        this->ui->WinnerLabel->setText("COOL WIN!" + append_str);
        log << getTime() + "[決着]COOLが勝利しました。" << "\n";
    }
    if(winner == GameSystem::WINNER::HOT){
        this->ui->WinnerLabel->setText("HOT WIN!"  + append_str);
        log << getTime() + "[決着]HOTが勝利しました。" << "\n";
    }
    if(winner == GameSystem::WINNER::DRAW){
        this->ui->WinnerLabel->setText("DRAW");
        log << getTime() + "[決着]引き分けです。" << "\n";
    }
    //log.close();
}
GameSystem::WINNER MainWindow::Judge(){
    bool cool_lose = false;
    bool hot_lose  = false;
    GameBoard*& board = this->ui->Field;
    GameSystem::AroundData cool_around;
    GameSystem::AroundData hot_around;

    cool_around = board->FieldAccessAround(GameSystem::TEAM::COOL,board->cool_pos,GameSystem::Method::ACTION::UNKNOWN);
    hot_around  = board->FieldAccessAround(GameSystem::TEAM::HOT ,board->hot_pos ,GameSystem::Method::ACTION::UNKNOWN);

    //ブロック置かれ死
    if(cool_around.data[4] == GameSystem::MAP_OBJECT::BLOCK){
        log << getTime() + "[死因]COOLブロック置かれ死" << "\n";
        cool_lose=true;
    }
    if(hot_around .data[4] == GameSystem::MAP_OBJECT::BLOCK){
        log << getTime() + "[死因]HOTブロック置かれ死" << "\n";
        hot_lose=true;
    }

    //ブロック囲まれ死
    if(cool_around.data[1] == GameSystem::MAP_OBJECT::BLOCK &&
       cool_around.data[3] == GameSystem::MAP_OBJECT::BLOCK &&
       cool_around.data[5] == GameSystem::MAP_OBJECT::BLOCK &&
       cool_around.data[7] == GameSystem::MAP_OBJECT::BLOCK){
        log << getTime() + "[死因]COOLブロック囲まれ死" << "\n";
        cool_lose=true;
    }
    if(hot_around .data[1] == GameSystem::MAP_OBJECT::BLOCK &&
       hot_around .data[3] == GameSystem::MAP_OBJECT::BLOCK &&
       hot_around .data[5] == GameSystem::MAP_OBJECT::BLOCK &&
       hot_around .data[7] == GameSystem::MAP_OBJECT::BLOCK){
        log << getTime() + "[死因]HOTブロック囲まれ死" << "\n";
        hot_lose=true;
    }

    //切断死
    if(startup->cool_client->disconnected_flag){
        log << getTime() + "[死因]COOL通信切断死" << "\n";
        cool_lose=true;
    }
    if(startup->hot_client ->disconnected_flag){
        log << getTime() +"[死因]HOT通信切断死" << "\n";
        hot_lose =true;
    }

    //相打ち、または時間切れ時はアイテム判定とする
    if((cool_lose && hot_lose) || ui->TimeBar->value()==0){
        log << getTime() + "[情報]相打ちまたは、タイムアップのためアイテム判定を行います" + "\n";
        log << getTime() + "[得点]COOL" + QString::number(cool_score) + "点 HOT" + QString::number(hot_score) + "点" + "\n";
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
