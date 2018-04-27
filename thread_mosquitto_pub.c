
#include <stdio.h>
#include <stdlib.h>
#include <mosquitto.h>
#include <string.h>

#define PORT  1883
#define KEEP_ALIVE 60
#define MSG_MAX_SIZE  1024

bool session = true;
extern int sock;
extern char gatewayID[16];
extern struct BACKENDCONFIG backendconfig;

/*MQTT发布主题线程，从网关接收数据并根据消息类型发布MQTT主题*/
int thread_mosquitto_pub()
{
	int num;
	char buff[MSG_MAX_SIZE];
	char topic[256];
	struct sockaddr_in clientAddr;
    int len = sizeof(clientAddr);

	
    struct mosquitto *mosq = NULL;
    //libmosquitto 库初始化
    mosquitto_lib_init();
    //创建mosquitto客户端
    mosq = mosquitto_new(NULL,session,NULL);
    if(!mosq){
        printf("create client failed..\n");
        mosquitto_lib_cleanup();
        return 1;
    }
    //连接服务器
    if(mosquitto_connect(mosq, backendconfig.server, PORT, KEEP_ALIVE)){
        fprintf(stderr, "Unable to connect.\n");
        return 1;
    }
    //开启一个线程，在线程里不停的调用 mosquitto_loop() 来处理网络信息
    int loop = mosquitto_loop_start(mosq);
    if(loop != MOSQ_ERR_SUCCESS)
    {
        printf("mosquitto loop error\n");
        return 1;
    }

    while(true)
    {
    	memset(topic,0,sizeof(topic));
    	strcpy(topic,"gateway/");
		strcat(topic,gatewayID);
		strcat(topic,"/");
    	num = recvfrom(sock, buff, sizeof(buff)-1, 0, (struct sockaddr*)&clientAddr, &len);
		if(num>0){
			if(strstr(buff,"stat"))
				strcat(topic,"stats");
			else
			if(strstr(buff,"rxpk"))
				strcat(topic,"rx");

				
        /*发布消息*/
        mosquitto_publish(mosq,NULL,topic,strlen(buff)+1,buff,0,0);
        memset(buff,0,sizeof(buff));
		}
    }
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return 0;
}

