#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

#define MYPORT 8888				/* 监听的端口 */
#define BACKLOG 10				/*listen的请求接收队列长度*/
#define LEN	512	

struct FILEINFO
{
    char filename[256];
    int szofile;
};

int main()
{	//
	int lisock, clisock;			/* 监听socket，数据收发socket*/
	struct sockaddr_in sa;			/* 自身的地址信息，bind函数使用*/
	struct sockaddr_in cli_addr; 	/* 连接客户端的地址信息 */
	int sin_size, ret;
    int lenofrec = 0;
	char buf[LEN] = {0};
    struct FILEINFO fileinf;

    FILE* fp = NULL;
	
	//1,创建tcp socket,
	if ((lisock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	
	//2,填充服务器的地址端口信息
	memset(&sa, 0, sizeof(struct sockaddr_in));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(MYPORT);		/* 网络字节顺序 */
	sa.sin_addr.s_addr = INADDR_ANY;	/* 自动填本机IP */
	
	//3,绑定地址和端口号
	if (bind(lisock, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
		perror("bind");
		exit(1);
	}
	
	int on=1;
    //IP可重用
    if(setsockopt(lisock,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))==-1)
    {
        printf("setsockopt fail %s\n",strerror(errno));
        return 0;
    }
	
	//4,监听客户端的连接
	if(listen(lisock, 5) == -1){
		perror("listen");
		exit(1);
	}
	

	/* 主循环 */
	while(1) {
		//5,接受客户端连接
		sin_size = sizeof(struct sockaddr_in);
		clisock = accept(lisock, (struct sockaddr *)&cli_addr, &sin_size);
		if (clisock == -1) {
			perror("accept");
			continue;
		}
		printf("Got connection from %s\n", inet_ntoa(cli_addr.sin_addr));
		break;
	}
	//接收文件信息
    while(1)
    {
	    recv(clisock, &fileinf, sizeof(fileinf), 0);
        break;
    }

    if(NULL == (fp = fopen(fileinf.filename,"wb")))
    {
        printf("Receive file failed");
		return -2;
    }
    
	while(1)
	{
		ret = recv(clisock, (void *)buf, LEN-1, 0);
        if(0 == strcmp(buf,"-END-"))
        {
            printf("Done!\n");
            break;
        }
        else
        {
            lenofrec += ret;
            printf("Received %dByte/%dByte\n",lenofrec,fileinf.szofile);
        }

		if (ret <= 0)
		{
			printf("recv error: %s\n", buf);
			break;
		}
	    fwrite(buf, sizeof(char) , strlen(buf), fp);	
		memset(buf, 0, sizeof(buf));
	}
    fclose(fp);	
	close(clisock);
	
	return 0;
}
