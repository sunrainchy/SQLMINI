#include <stdio.h>
#include <time.h>
#include <stdlib.h>
char ch[30]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
char num[20]={'0','1','2','3','4','5','6','7','8','9'};
char str[1024];
char name[1024];
int main()
{
	srand((unsigned)time(NULL));
	int i,j,k,l,m,n;
	printf("请输入文件名\n");
	scanf("%s",name);
	printf("请输入要生成元组条数\n");
	scanf("%d",&k);
	FILE *fd=fopen(name,"a+");
	if(fd==NULL)
	{
		printf("文件打开错误\n");
		return 0;
	}
	while(k--)
	{
		m=0;
		for(i=0;i<10;i++)
			str[m++]=ch[rand()%26];
		str[m++]=' ';
		for(i=0;i<13;i++)
			str[m++]=num[rand()%10];
		str[m++]=' ';
		for(i=0;i<2;i++)
			str[m++]=num[rand()%10];
		str[m++]=' ';
		for(i=0;i<2;i++)
			str[m++]=num[rand()%10];
		str[m++]=' ';
		for(i=0;i<10;i++)
			str[m++]=num[rand()%10];
		str[m]=0;
		printf("%d ",k);
		fprintf(fd,"%s\n",str);
	}
	fclose(fd);
	return 0;
}

