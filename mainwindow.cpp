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
    v = mSettings->value( "Silent" );
    if (v.type() != QVariant::Invalid)silent = v.toBool();
    else silent = true;


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
        this->ui->CoolNameLabel ->setText(this->startup->team_client[static_cast<int>(GameSystem::TEAM::COOL)]->client->Name == "" ? "Cool" : this->startup->team_client[static_cast<int>(GameSystem::TEAM::COOL)]->client->Name);
        this->ui->HotNameLabel  ->setText(this->startup->team_client[static_cast<int>(GameSystem::TEAM::HOT )]->client->Name == "" ? "Hot"  : this->startup->team_client[static_cast<int>(GameSystem::TEAM::HOT )]->client->Name);
        this->ui->HotScoreLabel ->setText("0");
        this->ui->CoolScoreLabel->setText("0");

    }else{
        exit(0);
    }

    player = 0;

    startup_anime = new QTimer();
    connect(startup_anime,SIGNAL(timeout()),this,SLOT(StartAnimation()));
    startup_anime->start(4000.0 / (startup->map.size.x()*startup->map.size.y()));

    music = new QSound(QString(":/Music/Music/") + this->startup->music_text + ".wav");
    if(!silent)music->play();

    for(int i=0;i<TEAM_COUNT;i++){
        ui->Field->team_pos[i].setX(-1);
        ui->Field->team_pos[i].setY(-1);
    }
    
    //アイテム数ラベルセット
    for(int i=0;i<startup->map.size.y();i++){
       for(int j=0;j<startup->map.size.x();j++){
            if(startup->map.field[i][j] == GameSystem::MAP_OBJECT::ITEM)leave_items++;
       }
    }
    ui->ItemLeaveLabel->setText(QString::number(leave_items));

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
       log << QString("-----第") + QString::number(ui->TimeBar->value()) + "ターン-----" + "\r\n";
    }

    if(getready_flag){
        // GetReady
        if(!startup->team_client[player]->client->WaitGetReady()){
            log << getTime() + "[停止]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(player)) + "が正常にGetReadyを返しませんでした!" << "\r\n";
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

        if(team_mehod[player].action == GameSystem::Method::ACTION::GETREADY)log << getTime() + "[停止]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(player)) + "が行動メソッドを呼ぶべき位置にGetReadyを呼んでいます！" << "\r\n";
        if(team_mehod[player].action == GameSystem::Method::ACTION::UNKNOWN) log << getTime() + "[停止]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(player)) + "が不正なメソッドを呼んでいます！" << "\r\n";
        if(team_mehod[player].rote   == GameSystem::Method::ROTE::UNKNOWN)   log << getTime() + "[停止]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(player)) + "の行動メソッドが不正な方向を示しています！" << "\r\n";

        log << getTime() + "[行動]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(player)) + "が" + convertString(team_mehod[player]) + "を行いました。" << "\r\n";

        //refresh
        ui->TimeBar->setValue(this->ui->TimeBar->value() - 1);
        this->ui->TurnLabel->setText("Turn : " + QString::number(ui->TimeBar->value()));

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
        leave_items--;
        ui->ItemLeaveLabel->setText(QString::number(leave_items));
        log << getTime() + "[取得]" + GameSystem::TEAM_PROPERTY::getTeamName(method.team) + "がアイテムを取得しました。" << "\r\n";
        ui->CoolScoreLabel->setText(QString::number(team_score[static_cast<int>(GameSystem::TEAM::COOL)]));
        ui->HotScoreLabel ->setText(QString::number(team_score[static_cast<int>(GameSystem::TEAM::HOT)]));
    }

}
void MainWindow::Finish(GameSystem::WINNER winner){
    this->clock->stop();
    QString append_str = "";

    //disconnect
    for(int i=0;i<TEAM_COUNT;i++){
        if(startup->team_client[i]->client->disconnected_flag){
            append_str.append("[" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(i)) + " Disconnected...]");
            log << getTime() + "[終了]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(i)) + "との通信が切断されています。" << "\r\n";
        }
    }
    log << this->ui->WinnerLabel->text() << "\r\n";
    if(!silent)music->stop();
    QSound::play(":/Music/Music/ji_023.wav");

    if(winner == GameSystem::WINNER::COOL){
        this->ui->WinnerLabel->setText("COOL WIN!" + append_str);
        log << getTime() + "[決着]COOLが勝利しました。" << "\r\n";
    }
    if(winner == GameSystem::WINNER::HOT){
        this->ui->WinnerLabel->setText("HOT WIN!"  + append_str);
        log << getTime() + "[決着]HOTが勝利しました。" << "\r\n";
    }
    if(winner == GameSystem::WINNER::DRAW){
        this->ui->WinnerLabel->setText("DRAW");
        log << getTime() + "[決着]引き分けです。" << "\r\n";
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
            log << getTime() + "[死因]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(i)) + "ブロック下敷き" << "\r\n";
            team_lose[i]=true;
        }

        //ブロック囲まれ死
        if(team_around.data[1] == GameSystem::MAP_OBJECT::BLOCK &&
           team_around.data[3] == GameSystem::MAP_OBJECT::BLOCK &&
           team_around.data[5] == GameSystem::MAP_OBJECT::BLOCK &&
           team_around.data[7] == GameSystem::MAP_OBJECT::BLOCK){
            log << getTime() + "[死因]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(i)) + "ブロック囲まれ" << "\r\n";
            team_lose[i]=true;
        }

        //切断死
        if(startup->team_client[i]->client->disconnected_flag){
            log << getTime() + "[死因]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(i)) + "通信切断" << "\r\n";
            team_lose[i]=true;
        }
    }

    //相打ち、または時間切れ時はアイテム判定とする
    if(!qFind(team_lose,team_lose+TEAM_COUNT,false) || ui->TimeBar->value()==0){
        log << getTime() + "[情報]相打ちまたは、タイムアップのためアイテム判定を行います" + "\r\n";
        log << getTime() + "[得点]";
        for(int i=0;i<TEAM_COUNT;i++){
            log << GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(i)) + ":" + team_score[i] + "  ";
            team_lose[i] = false;
        }
        log << "\r\n";

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


void MainWindow::StartAnimation(){
    static int timer = 1;
    static Field<GameSystem::MAP_OVERLAY> f(this->startup->map.size.y(),
                                            QVector<GameSystem::MAP_OVERLAY>(this->startup->map.size.x(),GameSystem::MAP_OVERLAY::ERASE));
    static int ANIMATION_SIZE = 2;
    static int ANIMATION_TYPE = qrand() % ANIMATION_SIZE;
    int count = 0;

    ui->Field->RefreshOverlay();

    if(ANIMATION_TYPE == 1){
        //ランダム
        QPoint pos;
        do{
            pos.setX(qrand() % this->startup->map.size.x());
            pos.setY(qrand() % this->startup->map.size.y());
        }while(f[pos.y()][pos.x()] != GameSystem::MAP_OVERLAY::ERASE);
        f[pos.y()][pos.x()] = GameSystem::MAP_OVERLAY::NOTHING;

        for(int i=0;i<this->startup->map.size.y();i++){
            for(int j=0;j<this->startup->map.size.x();j++){
                this->ui->Field->overlay[i][j] = f[i][j];
            }
        }
    }else if(ANIMATION_TYPE == 0){
        //ランダム
        QPoint pos;

        for(int i=0;i<this->startup->map.size.y();i++){
            for(int j=0;j<this->startup->map.size.x();j++){
                if(count > timer){
                    this->ui->Field->overlay[i][j] = f[i][j];
                }
                count++;
            }
        }
    }

    if(timer == startup->map.size.x() * startup->map.size.y()){
        teamshow_anime = new QTimer();
        connect(teamshow_anime,SIGNAL(timeout()),this,SLOT(ShowTeamAnimation()));
        teamshow_anime->start(1500.0/TEAM_COUNT);
        disconnect(startup_anime,SIGNAL(timeout()),this,SLOT(StartAnimation()));
    }else{
        repaint();
        timer++;
    }
}


void MainWindow::ShowTeamAnimation(){
    static int team_count;

    ui->Field->team_pos[team_count] = this->startup->map.team_first_point[team_count];

    if(team_count == TEAM_COUNT){
        clock = new QTimer();
        connect(clock,SIGNAL(timeout()),this,SLOT(StepGame()));
        clock->start(FRAME_RATE);
        disconnect(teamshow_anime,SIGNAL(timeout()),this,SLOT(ShowTeamAnimation()));
    }
    repaint();
    team_count++;
}

