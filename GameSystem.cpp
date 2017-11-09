#include "GameSystem.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>



QString GameSystem::TEAM_PROPERTY::getTeamName(GameSystem::TEAM team){
    if(team == GameSystem::TEAM::COOL)return "COOL";
    if(team == GameSystem::TEAM::HOT) return "HOT";
    return "UNKNOWN";
}

QPoint GameSystem::Method::GetRoteVector(){
    //方向ベクトル生成
    switch(this->rote){
        case GameSystem::Method::ROTE::UP:    return QPoint( 0,-1);
        case GameSystem::Method::ROTE::RIGHT: return QPoint( 1, 0);
        case GameSystem::Method::ROTE::DOWN:  return QPoint( 0, 1);
        case GameSystem::Method::ROTE::LEFT:  return QPoint(-1, 0);
        default:                              return QPoint( 0, 0);
    }
}

GameSystem::Method GameSystem::Method::fromString(const QString& str){
    //文字列から抽出
    GameSystem::Method answer;
    if     (str[0] == 'w')answer.action = GameSystem::Method::ACTION::WALK;
    else if(str[0] == 'l')answer.action = GameSystem::Method::ACTION::LOOK;
    else if(str[0] == 's')answer.action = GameSystem::Method::ACTION::SEARCH;
    else if(str[0] == 'p')answer.action = GameSystem::Method::ACTION::PUT;
    else                  answer.action = GameSystem::Method::ACTION::UNKNOWN;
    if     (str[1] == 'u')answer.rote   = GameSystem::Method::ROTE::UP;
    else if(str[1] == 'd')answer.rote   = GameSystem::Method::ROTE::DOWN;
    else if(str[1] == 'r')answer.rote   = GameSystem::Method::ROTE::RIGHT;
    else if(str[1] == 'l')answer.rote   = GameSystem::Method::ROTE::LEFT;
    else                  answer.rote   = GameSystem::Method::ROTE::UNKNOWN;
    return answer;
}


GameSystem::Map::Map():
    //デフォルトの設定
    turn(100),
    name("[DEFAULT MAP]"),
    size(DEFAULT_MAP_WIDTH,DEFAULT_MAP_HEIGHT),
    texture(GameSystem::Texture::Jewel){
}
void GameSystem::Map::SetSize(QPoint size){
    this->size = size;
    this->CreateRandomMap();
}

QPoint GameSystem::Map::MirrorPoint(const QPoint& pos){
    QPoint center(size.x() / 2.0f, size.y() / 2.0f);
    return center * 2 - pos;
}

bool GameSystem::Map::Import(QString Filename){
    //ファイルからマップを読み込む
    QFile file(Filename);
    if (file.open(QIODevice::ReadOnly)){
        char buf[1024]={};
        int str_length;
        int calm=0;
        while((str_length = file.readLine(buf,1024)) != -1){
            QString str = QString::fromLatin1(buf);
            str.replace("\r","");
            str.replace("\n","");

            //マップ名
            if(str[0]=='N')name = str.remove(0,2);
            //ターン数
            if(str[0]=='T')turn = str.remove(0,2).toInt();


            //マップ
            if(str[0]=='D'){
                QStringList list = str.remove(0,2).split(",");
                QVector<GameSystem::MAP_OBJECT> vec;
                foreach(QString s,list){
                    vec.push_back(static_cast<GameSystem::MAP_OBJECT>(s.toInt()));
                }
                size.setX(vec.size());
                field[calm] = vec;
                calm++;
            }

            //チーム初期位置
            for(int i=0;i<TEAM_COUNT;i++){
                if(str[0]==GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(i))[0]){
                    QStringList list = str.remove(0,2).split(",");
                    team_first_point[i] = QPoint(list[0].toInt(),list[1].toInt());
                }
            }
        }
        return true;
    }else{
        return false;
    }

}

bool GameSystem::Map::Export(QString Filename){
    QFile file(Filename);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(nullptr, "ファイルを開けません",file.errorString());
        return false;
    }else{
        //Map出力
        QString outname(Filename.split("/").last().remove(".map"));
        QTextStream stream( &file );
        stream << "N:" << outname << "\n";
        stream << "T:" << QString::number(this->turn) << "\n";
        stream << "S:" << QString::number(size.x()) << "," << QString::number(size.y()) << "\n";
        for(auto v1 : field){
            stream << "D:";
            for(auto it = v1.begin();it != v1.end();it++){
                stream << QString::number(static_cast<int>(*it));
                if(it != v1.end()-1)stream << ",";
            }
            stream << "\n";
        }
        for(int i=0;i<TEAM_COUNT;i++){
            stream << GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(i))[0]
                    << ":"
                    << QString::number(team_first_point[i] .x())
                    << ","
                    << QString::number(team_first_point[i] .y());
            //if(i == TEAM_COUNT-1)
            stream << "\n";
        }
        file.close();
        return true;
    }

}

void GameSystem::Map::CreateRandomMap(){
    const int BLOCK_NUM = 100;
    const int ITEM_NUM = 10;

    turn = 100;
    name = "[RANDOM MAP]";
    for(int i=0;i<TEAM_COUNT;i++){
        team_first_point[i]  = QPoint(qrand() % size.x(),qrand() % size.y());
    }
    field.clear();
    for(int i=0;i<size.y();i++){
        field.push_back(QVector<GameSystem::MAP_OBJECT>(size.x()));
    }
    //ブロック配置
    for(int i=0;i<BLOCK_NUM;i++){
        QPoint pos(qrand() % size.x(),qrand() % size.y());
        bool all_of = true;
        for(int j=0;j<TEAM_COUNT;j++){
            if(!((team_first_point[j]  - pos).manhattanLength() > 1))all_of=false;
        }
        if(all_of && field[pos.y()][pos.x()] != GameSystem::MAP_OBJECT::BLOCK){
            field[pos.y()][pos.x()] = GameSystem::MAP_OBJECT::BLOCK;
        }else{
            i--;
            continue;
        }
    }

    //アイテム配置
    for(int i=0;i<ITEM_NUM;i++){
        QPoint pos(qrand() % size.x(),qrand() % size.y());

        bool all_of = true;
        for(int j=0;j<TEAM_COUNT;j++){
            if(!((team_first_point[j]  - pos).manhattanLength() > 1))all_of=false;
        }
        if(all_of && field[pos.y()][pos.x()] != GameSystem::MAP_OBJECT::ITEM){
            field[pos.y()][pos.x()] = GameSystem::MAP_OBJECT::ITEM;
        }else{
            i--;
            continue;
        }
    }
}

void GameSystem::AroundData::finish(){
    this->data[0] = static_cast<GameSystem::MAP_OBJECT>(GameSystem::CONNECTING_STATUS::FINISHED);
    this->connect = GameSystem::CONNECTING_STATUS::FINISHED;
}

QString GameSystem::AroundData::toString(){
    //文字列へ変換
    QString str;
    str.append('0' + static_cast<int>(this->connect));
    for(int i=0;i<9;i++){
        if     (this->data[i] == GameSystem::MAP_OBJECT::NOTHING)str.append('0');
        else if(this->data[i] == GameSystem::MAP_OBJECT::TARGET) str.append('1');
        else if(this->data[i] == GameSystem::MAP_OBJECT::BLOCK)  str.append('2');
        else if(this->data[i] == GameSystem::MAP_OBJECT::ITEM)   str.append('3');
    }
    return str;
}
