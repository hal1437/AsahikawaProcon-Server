#ifndef COMCLIENT_H
#define COMCLIENT_H

#include <QObject>
#include "BaseClient.h"

class ComClient : virtual public BaseClient
{
public:
    virtual bool WaitGetReady();
    virtual GameSystem::Method WaitReturnMethod(GameSystem::AroundData data);
    virtual bool WaitEndSharp(GameSystem::AroundData data);

    virtual void Startup();

    explicit ComClient(QObject *parent = 0);
    ~ComClient();
};

#endif // COMCLIENT_H
