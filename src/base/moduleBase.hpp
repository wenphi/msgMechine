#pragma once
#include <iostream>
#include "message/msgNode.hpp"
#include <jsoncpp/json/json.h>
#include <string>
#include "actionBase.hpp"

class moduleBase
{
public:
  moduleBase(std::string moduleId, std::string address)
  {
    msgNode = new messageNode(moduleId, address);
  }
  virtual ~moduleBase()
  {
    delete msgNode;
  }
  virtual void operator()()
  {
    cmdBase = praseCmdtoAction(messageHolder);
    if (cmdBase != nullptr)
    {
      cmdBase->setParams(messageHolder, this);
      cmdBase->start();
      delete cmdBase;
    }
  };
  virtual actionBase *praseCmdtoAction(Json::Value JsonData) = 0;
  // bool MsgToParams(Json::Value, std::vector<autoValue> param); //将消息参数提取出来,格式待定,可以是第一个参数命令号...
  //输出
  Json::Value outPutReply()
  {
    return messageReply;
  }
  //输入
  bool setMessage(Json::Value &msg)
  {
    messageHolder = msg;
    return true;
  };
  bool setReply(Json::Value &data)
  {
    messageReply = data;
    return true;
  }

protected:
  // bool getReply(Json::Value &msg);
  messageNode *msgNode;
  actionBase *cmdBase;
  Json::Value messageHolder;
  Json::Value messageReply;
};