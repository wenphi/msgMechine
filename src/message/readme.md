- 节点发送格式
```json
{
    "msgTargetID":"api",
    "msgData":{},
}
```
- 转发接收
```json
{
    "msgOriginID":"joints.client",
    "msgTargetID":"api.server",
    "msgData":{},
}
```
- 节点回复格式
```json
{
    "msgOriginID":"api.client",
    "msgTargetID":"joints.clinet",
    "needReply":true,
    "msgData":{},
}
```