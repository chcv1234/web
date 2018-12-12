 #include <stdio.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <arpa/inet.h>
 #include <netinet/in.h>
 #include <pthread.h>
 #include <string.h>
 
 
 
 typedef struct sockaddr *sockaddrp;
 int sockfd , FF=0 , numbytes , ac=0;
 char *data , fn[255]={'\0'};
 FILE *fp ;
 
 void *recv_other(void *arg)
 {
     char buf[255]= {'\0'};
     char buf2[1000]={'\0'};
     while(1)
     {
	 int i=0 ;
	 memset(buf,'\0',sizeof(buf));
         int ret = recv(sockfd,buf,sizeof(buf),0);
         if(0 > ret)
         {
             perror("recv");
             return;
         }
	 

	 if(ac==1)
	 {
		if((numbytes=recv(sockfd,buf2,sizeof(buf2),0))<=0)
		{
			FF=0;
			ac=0;
			fclose(fp);
		}
		else
		{
			fwrite(buf2,sizeof(char),numbytes,fp);
		}
	 }
	 
	 if(FF==0)
	 {
         	printf("%s\n",buf);
	 }


	 if(strstr(buf,"(Y/N)")!=0)
	 {
	 	data=strtok(buf,"|");

		while(data!=NULL)
		{
			if(i==1)
			{
				strcpy(fn,data);
			}

			data=strtok(NULL,"|");

			i++;
		}
		
		fp=fopen(fn,"wb");
		FF=1;
	 }
     }
 }
 
 
 
 
 int main(int argc,char **argv)
 {
     if(3 != argc)
     {
         perror("參數錯誤");
         return -1;
     }
 
     //建立socket對象
     sockfd = socket(AF_INET,SOCK_STREAM,0);
     if(0 > sockfd)
     {
         perror("socket");
         return -1;
     }
 
     //準備連接地址
     struct sockaddr_in addr = {AF_INET};
     addr.sin_port = htons(atoi(argv[1]));
     addr.sin_addr.s_addr = inet_addr(argv[2]);
 
     socklen_t addr_len = sizeof(addr);
 
 
     //連接
     int ret = connect(sockfd,(sockaddrp)&addr,addr_len);
     if(0 > ret)
     {
         perror("connect");
         return -1;
     }

     //發送名字
     char buf[255] = {};
     char name[255] = {};
     printf("請輸入您的昵稱：");
     scanf("%s",name);
     ret = send(sockfd,name,strlen(name),0);
     if(0 > ret)
     {
         perror("connect");
         return -1;
     }
 
     //創建接收子線程
     pthread_t tid;
     ret = pthread_create(&tid,NULL,recv_other,NULL);
     
     if(0 > ret)
     {
         perror("pthread_create");
         return -1;
     }
     //循環發送
     while(1)
     {
         //printf("%s:",name);
         //scanf("%s",buf);

		 fgets(buf,sizeof(buf),stdin);

		 for(int i=0;i<255;i++)
		 {
			 if(buf[i]=='\n')
			 {
				 buf[i]='\0';
			 }
		 }

	 
         int ret = send(sockfd,buf,sizeof(buf),0);

         if(0 > ret)
         {
             perror("send");
             return -1;
         }


	 //發送檔案
	 char *test2;
	 int j=0;
	 char fs[255]={'\0'};

	 char buf2[1000]={'\0'};

	 if(strstr(buf,"file::")!=0)
	 {
		test2=strtok(buf,":");

	 	while(test2!=NULL)
	 	{
			if(j==2)
			{
				strcpy(fs,test2);
				//printf("%s\n",fs);
			}	

			test2=strtok(NULL,":");

			j++;
	 	}

	 	fp=fopen(fs,"rb");

		while(!feof(fp))
		{
			numbytes=fread(buf2,sizeof(char),sizeof(buf2),fp);

			send(sockfd,buf2,numbytes,0);
		}
		fclose(fp);

	 }

	 //接收檔案
	 
	 //char buf2[1000]={'\0'};

	 memset(buf2,'\0',sizeof(buf2));

	 if(FF==1)
	 {
		if(strcmp("Y",buf)==0)
		{
			/*
			while(1)
			{
				numbytes = read(sockfd,buf2,sizeof(buf2));
				printf("read %d bytes, ",numbytes);
				if(numbytes == 0)
				{
					break;
				}
				numbytes = fwrite(buf2,sizeof(char),numbytes,fp);	
				printf("fwrite %d bytes\n",numbytes);
				break ;

			}
			*/
		   	//sleep(1);
			printf("start!\n");	
			while(1)
			{
				printf("in\n");
				numbytes=recv(sockfd,buf2,sizeof(buf2),0);
				printf("%d\n",numbytes);
				
			//	if(strstr(buf2,"end")!=0)
			//	{
			//		printf("%s\n",buf2);
			//		break;
			//	}

				fwrite(buf2,sizeof(char),numbytes,fp);
				printf(".");

				if(numbytes<1000)
				{
					break ;
				}
			}
			printf("\nend\n");
		
			//ac=1;
			printf("file\n");
		}
		else if(strcmp("N",buf)==0)
		{
			printf("%s,您拒絕了要求\n",name);
			FF=0;
			fclose(fp);
		}
		
		FF=0;
		printf("end\n");
		fclose(fp);
		printf("%d\n",FF);

	 }

         //輸入quit退出
         if(0 == strcmp("quit",buf))
         {
            printf("%s,您已經退出了聊天室\n",name);
            return 0;
         }

    }

}

