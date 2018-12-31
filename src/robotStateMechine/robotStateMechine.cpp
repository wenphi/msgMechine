#include "robotStateMechine.hpp"
#include <cstdlib>
#include <cstring>
#include <unistd.h>

void robotStateMechine::updateHook()
{
    while (!stopFlag)
    {
        if (!msgTransmit->recvJson(msgHolder))
            usleep(100);
        else
        {

            if (msgTransmit->isReply())
            {
                msgTransmit->sendJson();
                continue;
            }
            if (msgHolder["msgTargetId"] == "StateMechine")
            {
                nextStateBase = preStateBase->updateState(msgHolder["msgData"]);
                if (nextStateBase != nullptr)
                {
                    delete preStateBase;
                    preStateBase = nextStateBase;
                }
            }
        }
        preStateBase->updateHook(this);
    }
    std::cout << "robotStateMechine<updateHook> exit done!" << std::endl;
}
