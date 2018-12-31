#include <iostream>
#include "message/msgNode.hpp"
#include "message/msgTransmit.hpp"
#include <csignal>
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
int main()
{
    signal(SIGINT, stop);

    messageNode *msgApi = new messageNode(address, "api");
    Json::Value jsonData, jsonRep;
    jsonData["param1"] = "i\' api";

    msgApi->sendJsonMsg("motion", jsonData, jsonRep);
    cout << "<api>:" << jsonRep << endl;

    delete msgApi;
    cout << "<main> exit done!" << endl;
    return 0;
}