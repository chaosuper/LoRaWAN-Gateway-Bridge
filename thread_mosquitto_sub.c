#include <stdio.h>
#include <stdlib.h>
#include <mosquitto.h>
#include <string.h>

#define HOST "localhost"
#define PORT  1883
#define KEEP_ALIVE 60



void sub_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
	struct sockaddr_in clientAddr;
	int n;
	int len = sizeof(clientAddr);

    if(message->payloadlen){
        printf("%s %s", message->topic, message->payload);
		n = sendto(sock, message->payload, message->payloadlen, 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));
    }else{
        printf("%s (null)\n", message->topic);
    }
    fflush(stdout);
}

void sub_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
    int i;
    if(!result){
        /* Subscribe to broker information topics on successful connect. */
        mosquitto_subscribe(mosq, NULL, "topics", 2);
    }else{
        fprintf(stderr, "Connect failed\n");
    }
}

void sub_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
    int i;
    printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
    for(i=1; i<qos_count; i++){
        printf(", %d", granted_qos[i]);
    }
    printf("\n");
}

void sub_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
    /* Pring all log messages regardless of level. */
    printf("%s\n", str);
}
/*MQTT��Ϣ����ת���̣߳���MQTT��������ȡָ���������Ϣ��ͨ��UDPת��������*/
int thread_mosquitto_sub()
{
    struct mosquitto *mosq = NULL;
	bool session = true;
    //libmosquitto ���ʼ��
    mosquitto_lib_init();
    //����mosquitto�ͻ���
    mosq = mosquitto_new(NULL,session,NULL);
    if(!mosq){
        printf("create client failed..,check your host if installed mosquitto_clients libmosquitto-dev\n");
        mosquitto_lib_cleanup();
        return 1;
    }
    //���ûص���������Ҫʱ��ʹ��
    //mosquitto_log_callback_set(mosq, sub_log_callback);
    mosquitto_connect_callback_set(mosq, sub_connect_callback);
    mosquitto_message_callback_set(mosq, sub_message_callback);
  //mosquitto_subscribe_callback_set(mosq, sub_subscribe_callback);
    //�ͻ������ӷ�����
    if(mosquitto_connect(mosq, HOST, PORT, KEEP_ALIVE)){
        fprintf(stderr, "Unable to connect.\n");
        return 1;
    }
    //ѭ������������Ϣ
    mosquitto_loop_forever(mosq, -1, 1);

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}

