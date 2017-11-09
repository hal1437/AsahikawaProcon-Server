#include "ComClient.h"

bool ComClient::WaitGetReady(){
    return true;
}
GameSystem::Method ComClient::WaitReturnMethod(GameSystem::AroundData data){

    return GameSystem::Method{GameSystem::TEAM::UNKNOWN, GameSystem::Method::ACTION::SEARCH,GameSystem::Method::ROTE::UP};
}

bool ComClient::WaitEndSharp(GameSystem::AroundData data){
    return true;
}
void ComClient::Startup(){
    emit Connected();
    emit WriteTeamName();
    emit Ready();
}

ComClient::ComClient(QObject *parent):
    BaseClient(parent)
{
    this->Name = "自動くん";
    this->IP   = "ローカル";
}

ComClient::~ComClient()
{

}

