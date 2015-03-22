#include "GameSystem.h"


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

QString GameSystem::AroundData::toString(){
    //文字列へ変換
    QString str;
    str.append('0' + static_cast<int>(this->connect));
    for(int i=0;i<9;i++){
        if(i==5)str.append('0');
        else if(this->data[i] == GameSystem::MAP_OBJECT::NOTHING)str.append('0');
        else if(this->data[i] == GameSystem::MAP_OBJECT::HOT)    str.append('1');
        else if(this->data[i] == GameSystem::MAP_OBJECT::COOL)   str.append('1');
        else if(this->data[i] == GameSystem::MAP_OBJECT::BLOCK)  str.append('2');
        else if(this->data[i] == GameSystem::MAP_OBJECT::ITEM)   str.append('3');
    }
    return str;
}
