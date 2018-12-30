#pragma once
#include "zhelpers.h"
#include <jsoncpp/include/json.h>
#include <iostream>
//客户段可以选择一次发送接收多条
class messageNode
{
  public:
    messageNode(std::string address_, std::string id, int blockTime = 1000)
    {
        int ret;
        address = address_;
        nodeID = id;
        idDealerServer = id + std::string(".server");
        idDealerClient = id + std::string(".client");
        std::cout << "debug:msgNode:identify: " << idDealerClient << "||" << idDealerServer << std::endl;
        std::cout << "debug::msgNode:address: " << address << std::endl;
        if (initMsgNode())
            msgReady = true;
    };
    ~messageNode()
    {
        zmq_close(pSockDealerClient);
        zmq_close(pSockDealerServer);
        zmq_ctx_destroy(context);
        std::cout << "msgNode:" << nodeID << " exit done!" << std::endl;
    };
    //初始化
    bool initMsgNode()
    {
        //创建环境
        context = zmq_ctx_new();
        if (context == NULL)
        {
            std::cout << "msgNode:" << nodeID << " create context Failed!" << std::endl;
            return false;
        }
        //创建socket
        pSockDealerServer = zmq_socket(context, ZMQ_DEALER);
        pSockDealerClient = zmq_socket(context, ZMQ_DEALER);
        if (pSockDealerClient == NULL || pSockDealerServer == NULL)
        {
            std::cout << "msgNode:" << nodeID << " create socket Failed!" << std::endl;
            return false;
        }
        //设置阻塞时间
        ret = zmq_setsockopt(pSockDealerClient, ZMQ_SNDTIMEO, &blockTime, sizeof(blockTime));
        if (ret < 0)
        {
            std::cout << "msgNode:" << nodeID << " set client \'send blockTime\' Failed!" << std::endl;
            return false;
        }
        ret = zmq_setsockopt(pSockDealerClient, ZMQ_RCVTIMEO, &blockTime, sizeof(blockTime));
        if (ret < 0)
        {
            std::cout << "msgNode:" << nodeID << " set client \'recv blockTime\' Failed!" << std::endl;
            return false;
        }
        int routerBlockTime = 1;
        ret = zmq_setsockopt(pSockDealerServer, ZMQ_RCVTIMEO, &routerBlockTime, sizeof(routerBlockTime));
        if (ret < 0)
        {
            std::cout << "msgNode:" << nodeID << " set server \'recv blockTime\' Failed!" << std::endl;
            return false;
        }
        //设置退出时的等待时间
        int linger_time = 10;
        ret = zmq_setsockopt(pSockDealerClient, ZMQ_LINGER, &linger_time, sizeof(linger_time));
        if (ret < 0)
        {
            std::cout << "msgNode:" << nodeID << " set client \'lingerTime\' Failed!" << std::endl;
            return false;
        }
        ret = zmq_setsockopt(pSockDealerServer, ZMQ_LINGER, &linger_time, sizeof(linger_time));
        if (ret < 0)
        {
            std::cout << "msgNode:" << nodeID << " set server \'lingerTime\' Failed!" << std::endl;
            return false;
        }
        //设置socket缓存的最大消息条数
        int sendHwm = 10;
        int recvHwm = 10;
        ret = zmq_setsockopt(pSockDealerClient, ZMQ_SNDHWM, &sendHwm, sizeof(sendHwm));
        if (ret < 0)
        {
            std::cout << "msgNode:" << nodeID << " set client \'sendHwm\' Failed!" << std::endl;
            return false;
        }
        ret = zmq_setsockopt(pSockDealerClient, ZMQ_RCVHWM, &recvHwm, sizeof(recvHwm));
        if (ret < 0)
        {
            std::cout << "msgNode:" << nodeID << " set client \'recvHwm\' Failed!" << std::endl;
            return false;
        }
        ret = zmq_setsockopt(pSockDealerServer, ZMQ_RCVHWM, &recvHwm, sizeof(recvHwm));
        if (ret < 0)
        {
            std::cout << "msgNode:" << nodeID << " set server \'recvHwm\' Failed!" << std::endl;
            return false;
        }
        //设置id
        ret = zmq_setsockopt(pSockDealerClient, ZMQ_IDENTITY, idDealerClient.c_str(), idDealerClient.size());
        if (ret < 0)
        {
            std::cout << "msgNode:" << nodeID << " set Dealer \'identify\' Failed!" << std::endl;
            return false;
        }
        ret = zmq_setsockopt(pSockDealerServer, ZMQ_IDENTITY, idDealerServer.c_str(), idDealerServer.size());
        if (ret < 0)
        {
            std::cout << "msgNode:" << nodeID << " set router \'identify'\' Failed!" << std::endl;
            return false;
        }
        //连接
        ret = zmq_connect(pSockDealerClient, address.c_str());
        if (ret < 0)
        {
            std::cout << "msgNode:" << nodeID << " client connect address Failed!" << std::endl;
            return false;
        }
        ret = zmq_connect(pSockDealerServer, address.c_str());
        if (ret < 0)
        {
            std::cout << "msgNode:" << nodeID << " client connect address Failed!" << std::endl;
            return false;
        }
        return true;
    }
    //发送json格式的消息
    bool postJsonMsg(std::string const &targetId, Json::Value const &jsonData)
    {
        Json::Value postValue;
        postValue["msgTargetId"] = std::string(targetId + ".server");
        postValue["msgData"] = jsonData;
        if (s_send(pSockDealerClient, const_cast<char *>(postValue.toStyledString().c_str())) < 0)
        {
            std::cout << "msgNode:" << nodeID << " postJsonMsg Failed!" << std::endl;
            return false;
        }
        return true;
    };
    bool sendJsonMsg(std::string const &targetId, Json::Value const &JsonData, Json::Value &jsonRep)
    {
        //send
        Json::Value sendValue;
        sendValue["msgTargetId"] = std::string(targetId + ".server");
        if (s_send(pSockDealerClient, const_cast<char *>(postValue.toStyledString().c_str())) < 0)
        {
            std::cout << "msgNode:" << nodeID << " sendJsonMsg:send Failed!" << std::endl;
            return false;
        }
        //recv
        Json::Reader reader;
        char *charData = s_recv(pSockDealer);
        if (charData == NULL)
        {
            std::cout << "msgNode" : << nodeID << " sendJsonMsg:recv overTime!" << std::endl;
            return false;
        }

        reader.parse(charData, jsonData);
        free(charData);
        return true;
    };
    bool repJsonMsg(Json::Value &jsonData)
    {
    }
    //接收json格式的消息
    bool recvJsonMsg(Json::Value &jsonData)
    {
        Json::Reader reader;
        Json::Value jsonData;
        char *msgAddr = s_recv(pSockDealerServer);
        if (msgAddr == NULL)
            return false;
        reader.parse(charData, jsonData);
        free(charData);
        return true;
    };

  private:
    bool msgReady = false;

    std::string nodeID;
    std::string idDealerServer; //zmq的sock标识
    std::string idDealerClient;
    std::string address; //zmq链接的通讯地址

    void *context;           //zmq的环境/上下文
    void *pSockDealerServer; //zmq创建Dealer的sock
    void *pSockDealerClient; //zmq创建Router的sock
};

id1->router->idd1
    idd1->idd2
        idd2->router->id1
