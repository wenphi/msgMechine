- 节点发送格式
```json
{
    "msgOriginId":"joints.client",
    "msgTargetId":"api.server",
    "msgNeedRep":true,
    "msgIsReq":false,
    "msgData":{},
}
```
- 转发接收
```json
{
    "msgOriginID":"joints.client",
    "msgTargetID":"api.server",
    "msgNeedRep":true,
    "msgIsReq":false,
    "msgData":{},
}
```
- 节点回复格式
```json
{
    "msgOriginID":"api.client",
    "msgTargetID":"joints.clinet",
    "needReply":true,
    "msgIsReq":true,
    "msgData":{},
}
```