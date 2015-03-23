#include "BaseClient.h"

void BaseClient::Startup(){

}

BaseClient::BaseClient(QObject *parent) :
    QObject(parent),
    Name(""),
    disconnected_flag(false)
{

}

BaseClient::~BaseClient()
{

}

