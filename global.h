/*
lorawan网关网桥转发程序全局变量及结构体定义
zhaoshichao
2018-04-27
*/
/*MQTT服务配置结构体*/
struct BACKENDCONFIG{
	char server[64];
	char name[64];
	char passwd[64];
};
/*UDP 服务配置*/
