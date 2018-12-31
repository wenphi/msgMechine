#pragma once
#include "zhelpers.h"
#include <jsoncpp/json/json.h>
#include <iostream>
//客户段可以选择一次发送接收多条
// struct msgHolder_t
// {
//     std::string msgOriginId;
//     std::string msgTragetId;
//     bool msgNeedRep;
//     bool msgIsReq;
//     Json::Value msgData;
// };

class messageNode
{
  public:
    messageNode(std::string const &address_, std::string const &id)
    {
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
        int ret;
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
        int blockTime = 1000;
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
        /*check*/
        if (!msgReady)
        {
            // std::cout << "msgNode:" << nodeID << " is not ready!" << std::endl;
            return false;
        }
        /*---send---*/
        Json::Value postValue;
        //设置来源节点
        postValue["msgOriginId"] = idDealerClient;
        //设置目标节点
        postValue["msgTargetId"] = std::string(targetId + ".server");
        //设置返回
        postValue["msgNeedRep"] = false;
        //消息类型
        postValue["msgIsRep"] = false;
        //消息内容
        postValue["msgData"] = jsonData;
        //发送
        if (s_send(pSockDealerClient, const_cast<char *>(postValue.toStyledString().c_str())) < 0)
        {
            std::cout << "msgNode:" << nodeID << " postJsonMsg Failed!" << std::endl;
            return false;
        }
        return true;
    };
    bool sendJsonMsg(std::string const &targetId, Json::Value const &JsonData, Json::Value &jsonRep)
    {
        /*check*/
        if (!msgReady)
        {
            // std::cout << "msgNode:" << nodeID << " is not ready!" << std::endl;
            return false;
        }
        /*---send---*/
        Json::Value sendValue;
        Json::Value recvValue;
        //设置来源节点
        sendValue["msgOriginId"] = idDealerClient;
        //设置目标节点
        sendValue["msgTargetId"] = std::string(targetId + ".server");
        //设置返回
        sendValue["msgNeedRep"] = true;
        //消息类型
        sendValue["msgIsRep"] = false;
        //设置消息编号
        int intNum = msgSerialNum++;
        sendValue["msgSerialNum"] = intNum;
        //消息内容
        sendValue["msgData"] = JsonData;
        //发送
        if (s_send(pSockDealerClient, const_cast<char *>(sendValue.toStyledString().c_str())) < 0)
        {
            std::cout << "msgNode:" << nodeID << " <sendJsonMsg>:send msg Failed!" << std::endl;
            return false;
        }
        /*---recv---*/
        Json::Reader reader;
        while (1)
        {
            char *charData = s_recv(pSockDealerClient);
            if (charData == NULL)
            {
                std::cout << "msgNode:" << nodeID << " <sendJsonMsg>:recv msg overTime!" << std::endl;
                return false;
            }
            if (!reader.parse(charData, recvValue))
            {
                std::cout << "msgNode:" << nodeID << " <sendJsonMsg>:parse msgToJson Failed!" << std::endl;
                free(charData);
                return false;
            }
            free(charData);
            if (jsonRep["msgSerialNum"].asInt() == intNum)
            {
                jsonRep = recvValue["msgData"];
                break;
            }
            else
                std::cout << "msgNode:" << nodeID
                          << "<sendJsonMsg>:recv msgSerialNum:"
                          << jsonRep["msgSerialNum"].asInt() << " is not equal to send msgSerialNum:" << intNum
                          << std::endl;
        }
        return true;
    };
    bool repJsonMsg(Json::Value &jsonData)
    {
        /*check*/
        if (!msgReady)
        {
            // std::cout << "msgNode:" << nodeID << " is not ready!" << std::endl;
            return false;
        }
        if (!repIsReady)
        {
            std::cout << "msgNode:" << nodeID << " reply msg is not ready!" << std::endl;
            return false;
        }
        repIsReady = false;
        /*---rep---*/
        Json::Value repValue;
        //交换地址
        std::string strTemp;
        repValue = msgHolder;
        strTemp = msgHolder["msgOriginId"].asString();
        repValue["msgOriginId"] = idDealerServer;
        repValue["msgTargetId"] = strTemp;

        repValue["msgIsRep"] = true;
        repValue["msgData"] = jsonData;
        //发送
        if (s_send(pSockDealerServer, const_cast<char *>(repValue.toStyledString().c_str())) < 0)
        {
            std::cout << "msgNode:" << nodeID << " repJsonMsg Failed!" << std::endl;
            return false;
        }
        return true;
    }
    //接收json格式的消息
    bool recvJsonMsg(Json::Value &jsonData)
    {
        /*check*/
        if (!msgReady)
        {
            // std::cout << "msgNode:" << nodeID << " is not ready!" << std::endl;
            return false;
        }
        if (repIsReady)
        {
            std::cout << "msgNode:" << nodeID << " need reply First!" << std::endl;
        }
        repIsReady = false;
        Json::Reader reader;
        Json::Value recvData;
        char *charData = s_recv(pSockDealerServer);
        if (charData == NULL)
            return false;
        if (!reader.parse(charData, recvData))
        {
            std::cout << "msgNode:" << nodeID << " <recvJsonMsg>:parse msgToJson Failed!" << std::endl;
            free(charData);
            return false;
        }
        free(charData);
        /*解析*/
        msgHolder = recvData;
        jsonData = recvData["msgData"];
        if (recvData["msgNeedRep"].asBool())
            repIsReady = true;
        return true;
    };

  private:
    bool msgReady = false;
    int msgSerialNum = 0;
    bool repIsReady = false;

    std::string nodeID;
    std::string idDealerServer; //zmq的sock标识
    std::string idDealerClient;
    std::string address; //zmq链接的通讯地址
    Json::Value msgHolder;

    void *context;           //zmq的环境/上下文
    void *pSockDealerServer; //zmq创建Dealer的sock
    void *pSockDealerClient; //zmq创建Router的sock
};
