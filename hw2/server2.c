 #include <stdio.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <pthread.h>
 #include <arpa/inet.h>
 #include <netinet/in.h>
 #include <string.h>
 #include <unistd.h>
 #include <stdlib.h>
// #include <winsock2.h> 
 typedef struct sockaddr *sockaddrp;
 
 //存儲客戶端地址的結構體數組
 struct sockaddr_in src_addr[50];
 socklen_t src_len = sizeof(src_addr[0]);
 
 
 
 //連接後記錄confd數組
 int confd[50] = {};
 char username[50][25]={'\0'};

 
 //設置連接人數
 int count = 0;
 
 //檔案相關
 int FF=0 , uid=0 , ac=0 ;
 FILE *fp ;
 char un[255]={'\0'};
 char buf[1000] ;
 char mes[100]={'\0'} ;
 void *broadcast(void *indexp)
 {
     int index = *(int *)indexp ;
	int numbytes ;
	 //char buf[1000];
	 char *test , *test2;
     char buf_rcv[255] = {'\0'};
	 //char un[255]={'\0'};
	 //char mes[255]={'\0'};
     char buf_snd[255] = {'\0'};
     //第一次讀取用戶姓名
     char name[20] = {'\0'};
	 //FILE *fp;
     int ret = recv(confd[index],name,sizeof(name),0);
     if(0 > ret)
     {
         perror("recv");
         close(confd[index]);
         return;
     }
 
	 strcpy(username[index],name);

     while(1)
     {
         //bzero(buf_rcv,sizeof(buf_rcv));
		 memset(buf_rcv,'\0',sizeof(buf_rcv));
         recv(confd[index],buf_rcv,sizeof(buf_rcv),0);
		
		 memset(buf_snd,'\0',sizeof(buf_snd));
 
         //判斷是否退出
         if(0 == strcmp("quit",buf_rcv))
         {
             sprintf(buf_snd,"%s已經退出聊天室",name);
             for(int i = 0;i <= count;i++)
             {
                 if(i == index || 0 == confd[i])
                 {
                     continue;
                 }
 
                 send(confd[i],buf_snd,sizeof(buf_snd),0);
             }
	     printf("%d\n",count);
	     strcpy(username[index],"\0");
             confd[index] = -1;
             pthread_exit(0);
                     
         }
 		 
	 //查看上線人員
	 else if(0 == strcmp("list",buf_rcv))
         {
			 sprintf(buf_snd,"======上線人員======");
			 send(confd[index],buf_snd,sizeof(buf_snd),0);
             for(int i = 0;i < count;i++)
             {
				 /*
                 if(i == index || 0 == confd[i])
                 {
                     continue;
                 }
 				 */
				 memset(buf_snd,'\0',sizeof(buf_snd));

				 sprintf(buf_snd,"%s",username[i]);		
				 printf("%d:%s\n",i,username[i]); 
                 send(confd[index],buf_snd,sizeof(buf_snd),0);
             }
			
			 memset(buf_snd,'\0',sizeof(buf_snd));
             sprintf(buf_snd,"====================");
			 send(confd[index],buf_snd,sizeof(buf_snd),0);
         }
		
 		 
	 else if(strstr(buf_rcv,":::")!=0)
		 {
			//char *test2 ;
			//char un[255]={'\0'} ;
			//char mes[255]={'\0'};

			memset(un,'\0',sizeof(un));
			memset(mes,'\0',sizeof(mes));
			int j=0 ;
			test=strstr(buf_rcv,":::");
			printf("test\n");
			int mg ;
			mg = strlen(test);
			int ID ;
			ID = strlen(buf_rcv) - mg ;
			printf("%d %d\n",strlen(test),mg);
			printf("%d %d\n",strlen(buf_rcv),ID);

			test2=strtok(buf_rcv,":");
			printf("test2:%s\n",test2);
			while(test2!=NULL)
			{
				if(j==0)
				{
					printf("0\n");
					strcpy(un,test2);
					printf("un:%s\n",un);
				}
				else if(j==1)
				{
					strcpy(buf_snd,test2);
				}
					
				j++;

				test2=strtok(NULL,":");
			}

			printf("%s ",un);
			printf("%s\n",buf_snd);
			strcpy(mes,buf_snd);
			printf("mes:%s\n",mes);

			for(int i=0 ; i<=count ; i++)
			{
				if(strcmp(username[i],un)==0)
				{
					sprintf(buf_snd,"#來自%s的訊息# ==> %s",name,mes);
					send(confd[i],buf_snd,sizeof(buf_snd),0);
					printf("succsee\n");
					break;
				}
			}

			//send(confd[index],test,sizeof(test),0);
			printf("end\n");
		 }

	 else if(strstr(buf_rcv,"file::")!=0)
	 {
		printf("filetest\n");
		printf("%s\n",buf_rcv);
		int j=0 ;
		test2=strtok(buf_rcv,":");
	 	memset(mes,'\0',sizeof(mes));

		while(test2!=NULL)
		{
			if(j==1)
			{
				strcpy(un,test2);
			}
			else if(j==2)
			{
				strcpy(mes,test2);
				printf("%s\n",mes);
			}
			
			test2=strtok(NULL,":");			

			j++;
		}

		
		fp=fopen(mes,"wb");

		printf("%s\n",mes);

		printf("檔案傳輸功能啟動,建立暫存檔中\n");
		
		memset(buf,'\0',sizeof(buf));

		while(1)
		{
			numbytes=recv(confd[index],buf,sizeof(buf),0);
			printf("%d\n",numbytes);
			fwrite(buf,sizeof(char),numbytes,fp);

			if(numbytes<1000)
			{
				break;
			}
		}

		fclose(fp);
		printf("暫存檔建立完成\n");

		for(int i=0 ; i<=count ; i++)
		{
			if(strcmp(username[i],un)==0)
			{
				sprintf(buf_snd,"#來自%s的檔案傳輸,是否接受(Y/N)|%s|#",name,mes);
				uid=i;
				send(confd[i],buf_snd,sizeof(buf_snd),0);
				FF=1;
				printf("succsee\n");

//				while((numbytes=fread(buf,1,sizeof(buf),fp))>0)
//				{
//					send(confd[uid],buf,numbytes,0);
//				}
//
//				break;
			}
		}

		

	 }
//	 else if(FF==1&&(strcmp(name,un)==0)&&ac==1)
//	 {
//		printf("start!!!\n");
//
//		if((numbytes=fread(buf,sizeof(char),sizeof(buf),fp))<=0)
//		{
//			FF=0;
//			ac=0;
//			fclose(fp);
//		}
//		else
//		{
//			send(confd[index],buf,numbytes,0);
//		}
//	 }
	 else if(FF==1&&(strcmp(name,un)==0))
	 {
		printf("filetrans\n");
		if(strcmp("Y",buf_rcv)==0)
		{
			printf("YES\n");
			ac=1;
			//printf("%d\n",numbytes);
			/*
			while(!feof(fp))
			{
				printf("start\n");
			 	numbytes = fread(buf, sizeof(char), sizeof(buf), fp);
				printf("fread %d bytes, ", numbytes);
				numbytes = write(confd[index], buf, numbytes);
				printf("Sending %d bytesn",numbytes);
			}
			sprintf(buf_snd,"傳輸結束");
			send(confd[index],buf_snd,sizeof(buf_snd),0);
			*/
			
			fp=fopen(mes,"rb");
			//rewind(fp);
			printf("%s\n",mes);	
			//sleep(5);
			printf("start!\n");
			memset(buf,0,sizeof(buf));

			while(!feof(fp))
			{
				if(fp!=NULL)
				 	printf("???\n");

				numbytes=fread(buf,sizeof(char),sizeof(buf),fp);
				ferror(fp);
				printf("%s\n",buf);
				printf("%d\n",numbytes);
				//if(numbytes==0)
				//	break;

				send(confd[index],buf,numbytes,0);
				
				if(numbytes<1000)
					break;

				printf(".");
			}
			

			//shutdown(confd[index],SD_SEND);
			//recv(confd[index],buf,sizeof(buf),0);
			//fclose(fp);

			printf("end\n");
			//sleep(1);
			//sprintf(buf_snd,"end");
			//send(confd[index],buf_snd,sizeof(buf_snd),0);

		}
		else if(strcmp("N",buf_rcv)==0)
		{
			sprintf(buf_snd,"要求遭拒,傳輸失敗");
			send(confd[index],buf_snd,sizeof(buf_snd),0);
			FF=0;
			
		}

		FF=0;
		fclose(fp);
		remove(mes);
	 }
	 else if(strcmp("\0",buf_rcv)==0)
	 {
		continue ;
	 }
	 else
	 {
         	sprintf(buf_snd,"%s:%s",name,buf_rcv);
         	printf("%s\n",buf_snd);
         	for(int i = 0;i <= count;i++)
         	{
            		if(i == index || 0 == confd[i])
             		{
                 		continue;
             		}
 
             		send(confd[i],buf_snd,sizeof(buf_snd),0);
         	}
	 }
    }
 
 }
 
 
 
 
 
 int main(int argc,char **argv)
 {
     printf("聊天室服務器端開始運行\n");
 
 
     //創建通信對象
     int sockfd = socket(AF_INET,SOCK_STREAM,0);
     if(0 > sockfd)
     {
         perror("socket");
         return -1;
     }
 
     //準備地址
    struct sockaddr_in addr = {AF_INET};
    addr.sin_port = htons(atoi(argv[1]));
    addr.sin_addr.s_addr = inet_addr(argv[2]);

    socklen_t addr_len = sizeof(addr);



    //綁定
    int ret = bind(sockfd,(sockaddrp)&addr,addr_len);
    if(0 > ret)
    {
        perror("bind");
        return -1;
    }


    //設置最大排隊數
    listen(sockfd,50);

    int index = 0;


    while(count <= 50)
    {
        confd[count] = accept(sockfd,(sockaddrp)&src_addr[count],&src_len);
        ++count;
        //保存此次客戶端地址所在下標方便後續傳入
        index = count-1;

        pthread_t tid;
        int ret = pthread_create(&tid,NULL,broadcast,&index);
        if(0 > ret)
        {
            perror("pthread_create");
            return -1;
        }


    }


}
