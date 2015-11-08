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

    for(int i=0;i<TEAM_COUNT;i++)team_score[i] = 0;

    //ServerSetting読み込み
    QString path;
    QSettings* mSettings;
    QVariant v;
    mSettings = new QSettings( "setting.ini", QSettings::IniFormat ); // iniファイルで設定を保存
    mSettings->setIniCodec( "UTF-8" ); // iniファイルの文字コード
    v = mSettings->value( "LogFilepath" );
    if (v.type() != QVariant::Invalid)path = v.toString();
    v = mSettings->value( "Gamespeed" );
    if (v.type() != QVariant::Invalid)FRAME_RATE = v.toInt();

    //ログファイルオープン
    file = new QFile(QString(path + "/log" + getTime() + ".txt"),this);
    file->open(QIODevice::WriteOnly);
    log.setDevice(file);

    //スタートアップダイアログ開始
    if(this->startup->exec()){
        //マップ初期化
        for(int i=0;i<TEAM_COUNT;i++){
            this->ui->Field->team_pos[i] = this->startup->map.team_first_point[i];
            qDebug() << this->ui->Field->team_pos[i];
        }
        this->ui->Field->setMap(this->startup->map);

        //ui初期化
        this->ui->Field  ->setMap(this->startup->map);
        this->ui->TimeBar->setMaximum(this->startup->map.turn);
        this->ui->TimeBar->setValue  (this->startup->map.turn);
        this->ui->TurnLabel     ->setText("Turn : " + QString::number(this->ui->TimeBar->value()));
        //this->ui->HotNameLabel  ->setText(this->startup->hot_client ->Name == "" ? "Hot"  : this->startup->hot_client ->Name);
        //this->ui->CoolNameLabel ->setText(this->startup->cool_client->Name == "" ? "Cool" : this->startup->cool_client->Name);
        this->ui->HotScoreLabel ->setText("Hot : 0");
        this->ui->CoolScoreLabel->setText("Cool : 0");

        //タイマー開始
        this->clock = new QTimer();
        connect(this->clock,SIGNAL(timeout()),this,SLOT(StepGame()));
        this->clock->start(FRAME_RATE);
    }else{
        exit(0);
    }
    player = 0;
    log << getTime() + "セットアップ完了　ゲームを開始します。\n";
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::StepGame(){
    //ゲーム進行
    static GameSystem::Method team_mehod[TEAM_COUNT];
    this->ui->Field->RefreshOverlay();
    static int turn_count;
    static bool getready_flag=true;


    //ターンログ出力
    if(ui->TimeBar->value() != turn_count){
       turn_count = ui->TimeBar->value();
       log << QString("-----第") + QString::number(ui->TimeBar->value()) + "ターン-----" + "\n";
    }
    if(getready_flag){
        // GetReady
        if(!startup->team_client[player]->client->WaitGetReady()){
            log << getTime() + "[停止]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(player)) + "が正常にGetReadyを返しませんでした!" << "\n";
        }
        team_mehod[player] = startup->team_client[player]->client->WaitReturnMethod(ui->Field->FieldAccessAround(GameSystem::Method{static_cast<GameSystem::TEAM>(player),
                                                                                                                                    GameSystem::Method::ACTION::GETREADY,
                                                                                                                                    GameSystem::Method::ROTE::UNKNOWN},
                                                                                                                 ui->Field->team_pos[player]));
        team_mehod[player].team = static_cast<GameSystem::TEAM>(player);
    }else{
        // Method
        startup->team_client[player]->client->WaitEndSharp(ui->Field->FieldAccessMethod(team_mehod[player]));
        PickItem(team_mehod[player]);

        if(team_mehod[player].action == GameSystem::Method::ACTION::GETREADY)log << getTime() + "[停止]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(player)) + "が行動メソッドを呼ぶべき位置にGetReadyを呼んでいます！" << "\n";
        if(team_mehod[player].action == GameSystem::Method::ACTION::UNKNOWN) log << getTime() + "[停止]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(player)) + "が不正なメソッドを呼んでいます！" << "\n";
        if(team_mehod[player].rote   == GameSystem::Method::ROTE::UNKNOWN)   log << getTime() + "[停止]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(player)) + "の行動メソッドが不正な方向を示しています！" << "\n";

        log << getTime() + "[行動]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(player)) + "が" + convertString(team_mehod[player]) + "を行いました。" << "\n";


        //refresh
        ui->TimeBar->setValue(this->ui->TimeBar->value() - 1);
        this->ui->TurnLabel->setText("Turn : " + QString::number(ui->TimeBar->value()));
        repaint();

        //End
        GameSystem::WINNER win = Judge();
        if(win != GameSystem::WINNER::CONTINUE)Finish(win);
        player++;
        player %= TEAM_COUNT;
    }
    ui->Field->repaint();
    getready_flag = !getready_flag;
}

void MainWindow::PickItem(GameSystem::Method method){

    //移動先がアイテムの場合は壁を置く
    QPoint pos = ui->Field->team_pos[static_cast<int>(method.team)];
    if(ui->Field->FieldAccess(method,pos) == GameSystem::MAP_OBJECT::ITEM){
        ui->Field->field.field[ pos                        .y()][ pos                        .x()] = GameSystem::MAP_OBJECT::NOTHING;
        ui->Field->field.field[(pos-method.GetRoteVector()).y()][(pos-method.GetRoteVector()).x()] = GameSystem::MAP_OBJECT::BLOCK;

        team_score[static_cast<int>(method.team)]++;
        log << getTime() + "[取得]" + GameSystem::TEAM_PROPERTY::getTeamName(method.team) + "がアイテムを取得しました。" << "\n";
        ui->CoolScoreLabel->setText("Cool : " + QString::number(team_score[static_cast<int>(GameSystem::TEAM::COOL)]));
        ui->HotScoreLabel ->setText("Hot : "  + QString::number(team_score[static_cast<int>(GameSystem::TEAM::HOT)]));
    }

}
void MainWindow::Finish(GameSystem::WINNER winner){
    this->clock->stop();
    QString append_str = "";

    //disconnect
    for(int i=0;i<TEAM_COUNT;i++){
        if(startup->team_client[i]->client->disconnected_flag){
            append_str.append("[" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(i)) + " Disconnected...]");
            log << getTime() + "[終了]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(i)) + "との通信が切断されています。" << "\n";
        }
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
    bool team_lose[TEAM_COUNT];
    GameBoard*& board = this->ui->Field;

    for(int i=0;i<TEAM_COUNT;i++){

        GameSystem::AroundData team_around = board->FieldAccessAround(static_cast<GameSystem::TEAM>(i));

        //ブロック置かれ死
        if(team_around.data[4] == GameSystem::MAP_OBJECT::BLOCK){
            log << getTime() + "[死因]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(i)) + "ブロック置かれ死" << "\n";
            team_lose[i]=true;
        }

        //ブロック囲まれ死
        if(team_around.data[1] == GameSystem::MAP_OBJECT::BLOCK &&
           team_around.data[3] == GameSystem::MAP_OBJECT::BLOCK &&
           team_around.data[5] == GameSystem::MAP_OBJECT::BLOCK &&
           team_around.data[7] == GameSystem::MAP_OBJECT::BLOCK){
            log << getTime() + "[死因]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(i)) + "ブロック囲まれ死" << "\n";
            team_lose[i]=true;
        }

        //切断死
        if(startup->team_client[i]->client->disconnected_flag){
            log << getTime() + "[死因]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(i)) + "通信切断死" << "\n";
            team_lose[i]=true;
        }
    }

    //相打ち、または時間切れ時はアイテム判定とする
    if(!qFind(team_lose,team_lose+TEAM_COUNT,false) || ui->TimeBar->value()==0){
        log << getTime() + "[情報]相打ちまたは、タイムアップのためアイテム判定を行います" + "\n";
        log << getTime() + "[得点]";
        for(int i=0;i<TEAM_COUNT;i++){
            log << GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(i)) + ":" + team_score[i] + "  ";
            team_lose[i] = false;
        }
        log << "\n";

        //引き分けかな？
        QSet<int> team_score_set; //スコア
        for(int i=0;i<TEAM_COUNT;i++){
            team_score_set.insert(team_score[i]);
        }
        if(team_score_set.size()==1)return GameSystem::WINNER::DRAW;

        int index=0;
        for(int i=0;i<TEAM_COUNT;i++){
            if(team_score[index] < team_score[i])index = i;
        }
        //勝者判定
        return static_cast<GameSystem::WINNER>(index);
    }
    if(team_lose[0])return GameSystem::WINNER::HOT;
    else if(team_lose[1])return GameSystem::WINNER::COOL;
    else return GameSystem::WINNER::CONTINUE;
}
