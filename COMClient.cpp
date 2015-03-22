#include "ComClient.h"

bool ComClient::WaitGetReady(){
    return true;
}
GameSystem::Method ComClient::WaitReturnMethod(GameSystem::AroundData data){
    return GameSystem::Method();
}

bool ComClient::WaitEndSharp(GameSystem::AroundData data){
    return true;
}

ComClient::ComClient(QObject *parent):
    BaseClient(parent)
{

}

ComClient::~ComClient()
{

}

