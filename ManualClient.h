#ifndef MANUALCLIENT_H
#define MANUALCLIENT_H

#include "BaseClient.h"
#include "ManualClientDialog.h"

class ManualClient : public BaseClient
{
    Q_OBJECT
public:
    ManualClientDialog* diag;

    bool WaitGetReady()override;
    GameSystem::Method WaitReturnMethod(GameSystem::AroundData data)override;
    bool WaitEndSharp(GameSystem::AroundData data)override;

    void Startup();

    ManualClient(QWidget *parent=0);
    ~ManualClient();
};

#endif // MANUALCLIENT_H
