#pragma once
#include <iostream>
#include <jsoncpp/json/json.h>
// #include "../robotStateMechine/robotStateMechine.hpp"

class robotStateMechine;
enum msgState_t
{
    MSG_NONE,
    MSG_EMERGENCY,
    MSG_HOMED,
};

enum msgtype_t
{
    MSG_CMD,
    MSG_STATE
};
struct stateHolder_t
{
    bool emergencyStop;
    bool drag;
    bool jointHomed[6];
};

//命令消息执行动作
//状态消息更新状态
struct msgHolder_t
{
    bool msgIsValid;     //消息是否有效
    bool msgIsNull;      //是否获取到消息
    int msgType;         //消息类型
    int modules;         //消息执行模块
    Json::Value msgData; //消息主体
    bool needReply;      //是否需要返回
};
struct replyHolder_t
{
    Json::Value moduleReply;
    Json::Value stateMechineReply;
};

class stateBase
{
  public:
    stateBase(){};
    virtual ~stateBase(){};

    virtual void filterHook(msgHolder_t &msgHolder) = 0;          //消息过滤
    virtual void updateHook(robotStateMechine *rsm) = 0;          //执行
    virtual stateBase *updateState(const Json::Value &state) = 0; //状态更新
};