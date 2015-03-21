#include "ComClient.h"

bool ComClient::WaitGetReady(){
}
GameSystem::Method ComClient::WaitReturnMethod(GameSystem::AroundData data){
}
bool ComClient::WaitEndSharp(GameSystem::AroundData data){
}

ComClient::ComClient(QObject *parent):
    BaseClient(parent)
{

}

ComClient::~ComClient()
{

}

