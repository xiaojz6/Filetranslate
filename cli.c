#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 8888
#define LEN  512 

struct FILEINFO
{
    char filename[256];
    int szofile;
};

int main(int argc,char *argv[])
{
	char buf[LEN] = {0};
    FILE* fp = NULL;
    
    struct FILEINFO fileinf;//文件信息
    int lenofrd = 0;//已读取长度 
    int lenofsd = 0;//已发送字长度
    
    if(argc < 3)
    {
        printf("argument: Promgram Hostname Port\n");
        return EXIT_FAILURE;
    }
	//1,创建socket
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd==-1)
    {
        printf("sockfd open error: %s\n",strerror(errno));
        return -1;
    }
	
	//2,填充服务器端的地址信息
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    
    //打开文件
    if(NULL == (fp = fopen(argv[3],"rb")))
    {
        printf("Open file %s failed\n",argv[3]);
        return -2;
    }
    strcpy(fileinf.filename,argv[3]);

	//3,连接服务器
    if(connect(sockfd,(struct sockaddr *)&addr,sizeof(addr))==-1)
    {
        printf("connect fail: %s\n",strerror(errno));
        return -1;
    }
    
    
    //求文件大小
    fseek(fp,0,SEEK_END);
    fileinf.szofile = ftell(fp);
    rewind(fp);
    
    //发送文件信息
    send(sockfd,&fileinf,sizeof(fileinf),0);
    
    while (1)
    {
        memset(buf, 0, sizeof(buf));
		//4,发送数据
		//从文件获取输入
		lenofrd = fread(buf,sizeof(char), LEN - 1,fp);
        ssize_t sd_size = send(sockfd, buf, strlen(buf), 0);		
		lenofsd += sd_size;
        if(lenofrd == 0)
		{
            send(sockfd,"-END-",strlen("-END-"),0);
            printf("OK!\n");
			break;
		}
        else
            printf("Sent %dByte/%dByte\n",lenofsd,fileinf.szofile);
        if(sd_size == -1)
        {
            printf("send fail: %s\n", strerror(errno));
			break;
        }
        sleep(1);
    }

    fclose(fp);

    return 0;
}
