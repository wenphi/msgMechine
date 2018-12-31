#include <iostream>
#include "message/msgNode.hpp"
#include "message/msgTransmit.hpp"
#include <csignal>
#include <thread>

using namespace std;
bool stop_flag = false;
// string address = "ipc:///home/ywh/robotStateMechine.ipc";
string address = "tcp://127.0.0.1:9001";

void stop(int sig)
{
    cout << "<stop> get signal abort!" << endl;
    if (sig == SIGINT)
        stop_flag = true;
}
void motionHook()
{
    messageNode *msgMotion = new messageNode(address, "motion");
    Json::Value jsonData;
    while (!stop_flag)
    {
        if (msgMotion->recvJsonMsg(jsonData))
        {
            cout << "<motionHook>:" << jsonData << endl;
            jsonData.clear();
            jsonData["param1"] = "i\'m motionHook";
            usleep(500);
            msgMotion->repJsonMsg(jsonData);
        }
        else
            usleep(10);
    }
    delete msgMotion;
    cout << "<moitonHook> exit done!" << endl;
}
int main()
{
    signal(SIGINT, stop);
    messageTransmit *msgtr = new messageTransmit(address);

    thread tt(motionHook);
    Json::Value jsonData;
    while (!stop_flag)
    {
        if (msgtr->recvJson(jsonData))
        {
            cout << "<mainHook>:" << jsonData << endl;
            msgtr->sendJson();
        }
        else
            usleep(10);
    }
    tt.join();
    delete msgtr;
    cout << "<main> exit done!" << endl;
    return 0;
}