/*
北斗易通 BDGW470型lora基站网桥程序
功能:		UDP转MQTT协议
环境依赖:	已安装mosquitto_client
author:		zhaoshichao
datetime:	2018-4-27
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <uci.h>

const char* conf_file="/etc/config/lora_config";
int  udpport=1680;
/*
通信协议类型: 1:UDP;2:MQTT;3:HTTPS
*/
int protol_type=0;  
struct BACKENDCONFIG backendconfig;
unsigned long long gwid;
char gatewayID[16];
int sock;


int parse_UCI_gateway_configuration(const char * conf_file)
{
	static struct uci_context * ctx = NULL; //定义一个UCI上下文的静态变量.
    struct uci_package * pkg = NULL;
    struct uci_element *e=NULL;
    const char *value;
	unsigned long long ull;
	ctx = uci_alloc_context(); // 申请一个UCI上下文.
    if (UCI_OK != uci_load(ctx, conf_file, &pkg))
        goto cleanup; //如果打开UCI文件失败,则跳到末尾 清理 UCI 上下文.


    /*遍历UCI的每一个节*/
	uci_foreach_element(&pkg->sections, e){
    
        struct uci_section *s = uci_to_section(e);
    	MSG("Parse UCI Gateway section s's type is %s.\n",s->type);
		if (NULL != (value = uci_lookup_option_string(ctx, s, "GW_ID")))
        	{
                /* gateway unique identifier (aka MAC address) (optional) */
			    if (value != NULL) {
					strcpy(gatewayID,value);
			        sscanf(value, "%llx", &ull);
			        gwid = ull;
			        MSG("INFO: gateway MAC address is configured to %016llX\n", ull);
			    }
        	}
    	if(!strcmp("lora",s->type)) //this section is a lora
    	{
        	MSG("This seciton is a LoRa  GW Configure.\n");
			/* 获取通信协议类型 */
			if (NULL != (value = uci_lookup_option_string(ctx, s, "protol")))
        	{
                
			    if (value != NULL) {
			        protol_type=atoi(value);
			        printf("protocol type: %d\n", protol_type);
			    }
        	}
			if (NULL != (value = uci_lookup_option_string(ctx, s, "server")))
        	{
                /* server hostname or IP address (optional) */
			    if (value != NULL) {
			        strncpy(backendconfig.server, value, sizeof int);
			        printf("INFO: server hostname or IP address is configured to \"%s\"\n", serv_addr);
			    }
        	}
			
        	}
    	}
    uci_unload(ctx, pkg); // 释放 pkg 
cleanup:
    uci_free_context(ctx);
    ctx = NULL;
	return 0;
}

int init(char *config_file){
	int ret=0;
	ret=parse_UCI_gateway_configuration(config_file);
	ret=udp_server_init();
	return ret;
}



/*UDP服务端初始化*/
int udp_server_init()
{

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(udpport);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

//    int sock;
    if ( (sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("creat udp socket error ");
        exit(1);
    }
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind udp error ");
        exit(1);
    }
    return 0;
}

int main(){

	pthread_t thread_pub;
	pthread_t thread_sub;
	int threadret_pub;
	int threadret_sub;
 	init();
	threadret_pub=pthread_create(&thread_pub,NULL,thread_mosquitto_pub,NULL);
	threadret_sub=pthread_create(&thread_sub,NULL,thread_mosquitto_sub,NULL);

	while(1){

	sleep(1);

		}
	
	pthread_join(thread_pub,NULL);
    pthread_join(thread_sub,NULL);

return 0;
}
