#include "GameSystem.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>


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
    else if(str[0] == 's')answer.action = GameSystem::Method::ACTION::SEACH;
    else if(str[0] == 'p')answer.action = GameSystem::Method::ACTION::PUT;
    else                  answer.action = GameSystem::Method::ACTION::UNKNOWN;
    if     (str[1] == 'u')answer.rote   = GameSystem::Method::ROTE::UP;
    else if(str[1] == 'd')answer.rote   = GameSystem::Method::ROTE::DOWN;
    else if(str[1] == 'r')answer.rote   = GameSystem::Method::ROTE::RIGHT;
    else if(str[1] == 'l')answer.rote   = GameSystem::Method::ROTE::LEFT;
    else                  answer.rote   = GameSystem::Method::ROTE::UNKNOWN;
    return answer;
}

bool GameSystem::Map::Export(QString Filename){
    QFile file(Filename);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(nullptr, "ファイルを開けません",
            file.errorString());
        return false;
    }else{

        QString outname(Filename.split("/").last().remove(".map"));

        QTextStream stream( &file );
        stream << "N:" << outname << "\n";
        stream << "T:" << QString::number(this->turn) << "\n";
        for(auto v1 : field){
            stream << "D:";
            for(auto it = v1.begin();it != v1.end();it++){
                stream << QString::number(static_cast<int>(*it));
                if(it != v1.end()-1)stream << ",";
            }
            stream << "\n";
        }
        stream << "H:" << QString::number(hot_first_point .x()) << "," << QString::number(hot_first_point .y()) << "\n";
        stream << "C:" << QString::number(cool_first_point.x()) << "," << QString::number(cool_first_point.y()) ;
        file.close();
        return true;
    }

}

void GameSystem::Map::CreateRandomMap(){
    const int BLOCK_NUM = 100;
    const int ITEM_NUM = 10;

    turn = 300;
    name = "[RANDOM MAP]";

    hot_first_point  = QPoint(qrand()%MAP_WIDTH,qrand()%MAP_HEIGHT);
    cool_first_point = QPoint(qrand()%MAP_WIDTH,qrand()%MAP_HEIGHT);

    //ブロック配置
    for(int i=0;i<BLOCK_NUM;i++){
        QPoint pos(qrand()%MAP_WIDTH,qrand()%MAP_HEIGHT);
        if((hot_first_point  - pos).manhattanLength() > 1 &&
           (cool_first_point - pos).manhattanLength() > 1 &&
           field[pos.y()][pos.x()] != GameSystem::MAP_OBJECT::BLOCK){
            field[pos.y()][pos.x()] = GameSystem::MAP_OBJECT::BLOCK;
        }else{
            i--;
            continue;
        }
    }

    //アイテム配置
    for(int i=0;i<ITEM_NUM;i++){
        QPoint pos(qrand()%MAP_WIDTH,qrand()%MAP_HEIGHT);
        if((hot_first_point  - pos).manhattanLength() > 1 &&
           (cool_first_point - pos).manhattanLength() > 1 &&
            field[pos.y()][pos.x()] != GameSystem::MAP_OBJECT::ITEM){
            field[pos.y()][pos.x()] = GameSystem::MAP_OBJECT::ITEM;
        }else{
            i--;
            continue;
        }
    }
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
