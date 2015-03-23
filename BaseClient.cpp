#include "BaseClient.h"

BaseClient::BaseClient(QObject *parent) :
    QObject(parent),
    Name(""),
    disconnected_flag(false)
{

}

BaseClient::~BaseClient()
{

}

