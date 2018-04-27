
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

/*MQTT���������̣߳������ؽ������ݲ�������Ϣ���ͷ���MQTT����*/
int thread_mosquitto_pub()
{
	int num;
	char buff[MSG_MAX_SIZE];
	char topic[256];
	struct sockaddr_in clientAddr;
    int len = sizeof(clientAddr);

	
    struct mosquitto *mosq = NULL;
    //libmosquitto ���ʼ��
    mosquitto_lib_init();
    //����mosquitto�ͻ���
    mosq = mosquitto_new(NULL,session,NULL);
    if(!mosq){
        printf("create client failed..\n");
        mosquitto_lib_cleanup();
        return 1;
    }
    //���ӷ�����
    if(mosquitto_connect(mosq, backendconfig.server, PORT, KEEP_ALIVE)){
        fprintf(stderr, "Unable to connect.\n");
        return 1;
    }
    //����һ���̣߳����߳��ﲻͣ�ĵ��� mosquitto_loop() ������������Ϣ
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

				
        /*������Ϣ*/
        mosquitto_publish(mosq,NULL,topic,strlen(buff)+1,buff,0,0);
        memset(buff,0,sizeof(buff));
		}
    }
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return 0;
}

