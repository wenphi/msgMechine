#pragma once
#include "zhelpers.h"
#include <jsoncpp/json/json.h>
#include <iostream>

//一次只允许执行一条命令,否则导致发送地址出错
class messageTransmit
{
  public:
    messageTransmit(std::string const address_)
    {
        address = address_;
        std::cout << "debug:msgTransmit:address:" << address_ << std::endl;
        if (initMsgTransmit())
            msgReady = true;
    };
    ~messageTransmit()
    {
        zmq_close(pSockRouter);
        zmq_ctx_destroy(context);
        std::cout << "server:"
                  << "exit done!" << std::endl;
    };
    //初始化
    bool initMsgTransmit()
    {
        int ret;
        //创建环境
        context = zmq_ctx_new();
        if (context == NULL)
        {
            std::cout << "msgTransmit create contex failed!" << std::endl;
            return false;
        }
        //创建socket
        pSockRouter = zmq_socket(context, ZMQ_ROUTER);
        if (pSockRouter == NULL)
        {
            std::cout << "msgTransmit create \'zmq_router\' failed!" << std::endl;
            return false;
        }
        //设置阻塞时间
        rblockTime = 1000;
        sblockTime = 1000;
        ret = zmq_setsockopt(pSockRouter, ZMQ_SNDTIMEO, &sblockTime, sizeof(sblockTime));
        if (ret < 0)
        {
            std::cout << "msgTransmit set sendTimeOut failed!" << std::endl;
            return false;
        }
        ret = zmq_setsockopt(pSockRouter, ZMQ_RCVTIMEO, &rblockTime, sizeof(rblockTime));
        if (ret < 0)
        {
            std::cout << "msgTransmit set recvTimeOut failed!" << std::endl;
            return false;
        }
        //设置socket退出时的阻塞时间--1000ms
        int linger_time = 1000;
        ret = zmq_setsockopt(pSockRouter, ZMQ_LINGER, &linger_time, sizeof(linger_time));
        if (ret < 0)
        {
            std::cout << "msgTransmit set zmqLinger failed!" << std::endl;
            return false;
        }
        //设置socket缓存的最大消息条数
        int recvHwm = 10;
        ret = zmq_setsockopt(pSockRouter, ZMQ_RCVHWM, &recvHwm, sizeof(recvHwm));
        if (ret < 0)
        {
            std::cout << "msgTransmit set recvHwm failed!" << std::endl;
            return false;
        }
        //绑定
        ret = zmq_bind(pSockRouter, address.c_str());
        if (ret < 0)
        {
            std::cout << "msgTransmit bind address failed!" << std::endl;
            return false;
        }
        //完成
        return true;
    }
    //发送
    bool sendJson()
    {
        /*check*/
        if (!msgReady)
        {
            // std::cout << "msgTransmit is not ready!" << std::endl;
            return false;
        }
        if (!repIsReady)
        {
            std::cout << " msgTransmit need recv msg first!" << std::endl;
            return false;
        }
        if (!msgHolder["msgNeedRep"].asBool() && msgHolder["msgIsRep"].asBool())
        {
            std::cout << " msgTransmit:this msg is not need to reply!" << std::endl;
            return false;
        }
        repIsReady = false;
        int ret;
        ret = s_sendmore(pSockRouter, const_cast<char *>(msgHolder["msgTargetId"].asString().c_str()));
        if (ret < 0)
        {
            std::cout << "msgTransmit send address failed!" << std::endl;
            return false;
        }
        ret = s_send(pSockRouter, const_cast<char *>(msgHolder.toStyledString().c_str()));
        if (ret < 0)
        {
            std::cout << "msgTransmit send msgData failed!" << std::endl;
            return false;
        }
        return true;
    };
    //接收
    bool recvJson(Json::Value &jsonData)
    {
        /*check*/
        if (!msgReady)
        {
            // std::cout << "msgTransmit is not ready!" << std::endl;
            return false;
        }
        if (repIsReady)
        {
            // std::cout << "msgTransmit need transmit first!" << std::endl;
            return false;
        }
        repIsReady = false;
        Json::Reader reader;
        std::string strOriginID;
        char *msgAddr = s_recv(pSockRouter);
        if (msgAddr == NULL)
            return false;
        /*开始接收数据*/
        free(msgAddr);
        char *charData = s_recv(pSockRouter);
        if (strcmp(charData, "") == 0)
        {
            free(charData);
            charData = s_recv(pSockRouter);
        }
        if (!reader.parse(charData, msgHolder))
        {
            std::cout << "debug:msgTransmit:recv an empty data!" << std::endl;
            return false;
        }
        free(charData);
        jsonData = msgHolder;
        repIsReady = true;
        return true;
    };
    bool isReply()
    {
        return msgHolder["msgIsRep"].asBool();
    }

  private:
    bool msgReady = false;
    bool repIsReady = false;
    Json::Value msgHolder;
    bool isBlock;         //每次调用recv后刷新,决定了发送类型{地址+""+数据}/{地址+数据}
    std::string clientid; //每次调用recv后刷新,决定了下一次send的发送地址;
    std::string address;  //zmq链接的通讯地址
    void *context;        //zmq的环境/上下文
    void *pSockRouter;    //zmq创建Router的sock
    int rblockTime;       //接收阻塞时间
    int sblockTime;       //发送阻塞时间
};