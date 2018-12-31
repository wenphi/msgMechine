#pragma once
#include <vector>
#include "jsoncpp/json/json.h"
class moduleBase;

class actionBase
{
  public:
    actionBase() {}
    virtual ~actionBase() {}
    //子类调用
    void start()
    {
        run(jsonData);
    }
    bool setParams(Json::Value &jsonData_, moduleBase *baseInstance_)
    {
        baseInstance = baseInstance_;
        return true;
    }

  private:
    virtual bool run(Json::Value &cmdData) = 0;
    moduleBase *baseInstance;
    Json::Value jsonData;
};