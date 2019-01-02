#include <iostream>
#include "message/msgNode.hpp"
#include "message/msgTransmit.hpp"
#include <thread>
#include <csignal>
using namespace std;
bool stop_flag = false;
// string address = "ipc:///home/ywh/robotStateMechine.ipc";
string address = "tcp://127.0.0.1:9001";
string name = "api";
vector<string> all_msg;
void stop(int sig)
{
    cout << "<stop> get signal abort!" << endl;
    if (sig == SIGINT)
        stop_flag = true;
}
void printHook()
{
    cout << "\033[2J";
    for (auto msg : all_msg)
        cout << msg << endl;
}

bool Hook(messageNode *msgNode)
{
    Json::Value jsonData;
    Json::Value::Members member;
    while (!stop_flag)
    {
        if (msgNode->recvJsonMsg(jsonData))
        {
            /*------*/
            member = jsonData.getMemberNames();
            for (auto a : member)
            {
                all_msg.emplace_back("|RECV|:<" + a + ">:" + jsonData[a].asString());
                // cout << "|RECV|:<" << a << ">:" << jsonData[a] << endl;
                printHook();
            }
            /*------*/
            jsonData.clear();
        }
        else
            usleep(10);
    }
    cout << "<" << name << "Hook> exit done!" << endl;
}
int main(int argc, char **argv)
{
    signal(SIGINT, stop);
    /*------*/
    if (argc != 3)
    {
        cout << "you need input 2 argument:<name> <address>" << endl;
    }
    name = argv[1];
    address = argv[2];
    all_msg.emplace_back("ID:" + name + " | " + "address:" + address);
    // /*------*/
    Json::Value jsonData, jsonRep;
    messageNode *msgApi = new messageNode(address, name);
    thread tt(Hook, msgApi);
    string targetId, str;
    while (!stop_flag)
    {
        cin >> targetId;
        if (stop_flag)
            break;
        cin >> str;
        if (stop_flag)
            break;
        cout << endl;
        jsonData[name] = str;
        all_msg.emplace_back("|SEND|:<" + name + ">:" + str);
        // cout << "|SEND|:<" << targetId << ">:" << str << endl;
        printHook();
        msgApi->postJsonMsg(targetId, jsonData);
    }
    tt.join();
    delete msgApi;
    cout << "<main> exit done!" << endl;
    return 0;
}