#ifndef COMCLIENT_H
#define COMCLIENT_H

#include <QObject>
#include "BaseClient.h"

class ComClient : public BaseClient
{
public:
    virtual bool WaitGetReady();
    virtual GameSystem::Method WaitReturnMethod(GameSystem::AroundData data);
    virtual bool WaitEndSharp(GameSystem::AroundData data);

    explicit ComClient(QObject *parent = 0);
    ~ComClient();
};

#endif // COMCLIENT_H
