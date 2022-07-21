#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<errno.h>
#include<stdlib.h>
#include <pthread.h>
extern double bme();
pthread_cond_t cvar;

int count;

void *func(void *arg){
    int new_sockfd=(int) arg;
    FILE *istream;
    if(new_sockfd<0){
            perror("accept error\n");
            exit(1);
    }
    istream=fdopen(new_sockfd,"r+");    // クライアントからデータを受け取る
    if(istream==NULL){
        perror("file open error\n");
        exit(1);
    }
    int val;
    val=setvbuf(istream,NULL,_IONBF,0); //バッファーの制御
    if(val!=0){
        perror("setvbuf error\n");
        exit(1);
    }

    //温度を測る
    int temp;
    temp=bme();
    fprintf(istream,"HTTP/1.1 200 OK\r\nContet-Type:text/html\r\n\r\n温度: %f\r\n",temp);
}

int main(){
    pthread_cond_init(&cvar,NULL);
    int val;
    struct sockaddr_in serv_addr;
    // ソケットを作る
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd<0){
        perror("socket error\n");
        exit(1);
    }
    // アドレスを作る
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(80);

    int flag=1;
    val=setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
    if(val<0){
        perror("setsocketopt error\n");
        exit(1);
    }
    // ソケットにアドレスを割り当てる
    val=bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
    if(val<0){
        perror("bind error\n");
        exit(1);
    }
    // コネクション要求を待ち始めるよう指示
    val=listen(sockfd, 5);
    if(val<0)
    {
        perror("listen error\n");
        exit(1);
    }
    int new_sockfd;
    pthread_t th0;
    while(1){
        new_sockfd=accept(sockfd,NULL,NULL);
        if(new_sockfd<0){
            perror("accept error\n");
            exit(1);
        }
        pthread_create(&th0, NULL, func, (void *)&new_sockfd);
        pthread_detach(th0);
    }
}
