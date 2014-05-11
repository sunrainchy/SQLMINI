#include "passwd.h"
#include <stdio.h> 
/*获取用户输入密码*/
int get_passwd(char *str)
{
	char ch;
	int i=0;
	system("stty raw");   /*使终端驱动处于一次一字符模式*/
	while(ch=getchar())
	{
		if(ch==13)
			break;
		/*输入backspace时候保证能工作*/
		if(ch==127)
		{
			if(i>0)
			{
				printf("%s","\b\b\b   \b\b\b");
				i--;
				continue;
			}
			else
			{
				printf("%s","\b\b  \b\b");
				continue;
			}
		}
		str[i++]=ch;
		printf("%s%c","\b",'*');
	}
	str[i]=0;
	printf("%s","\b\b  ");
	system("stty cooked");   /*使终端驱动回到一次一行模式*/
	printf("\n");
	return 0;
}
/*密码加密*/
int get_encryption(char *str)
{
	int i;
	for(i=0;str[i];i++)
	{
		str[i]=((int)str[i]/3+1)*2;//这个加密方式不好，因为不同的密码可能算出相同的值，下次有空换一个好点的加密方法
	}
	return 0;
}
/*密码比较 真确返回0，错误返回-1*/
int passwd_compare(char *str)
{
	char pa[100];
	FILE  *fd;
	fd=fopen("passwd","r");
	if(fd==NULL)
	{ 
		printf("密码匹配出错\n");
		return -1;
	}
	fscanf(fd,"%s",pa);
	if(fclose(fd)==-1)
	{
		printf("close errno\n");
		return -1;
	}
	if(strcmp(str,pa)==0)
		return 0;
	else
		return -1;
}

/*密码输入检测 0 success -1 failed*/
int passwd_check()
{
	char pass[100];
	int k=0;
	while(1)
	{
		printf("passwd : ");
		get_passwd(pass);
		get_encryption(pass);
		if(passwd_compare(pass)==0)
		{
			printf("yes accepted correctly\n");
			break;
		}
		else
		{ 
			k++;
			printf("wrong please try again\n");
		}
		if(k>=3)
		{
			printf("three time system will exit\n");
			return -1;
		}
	}
	return 0;
}
/*修改密码,成功返回0失败返回-1*/
int change_passwd()
{
	if(passwd_check()==-1)
		return -100;
	char pass_f[200];
	char pass_s[200];
	FILE *fd;
	printf("please input new password:");
	get_passwd(pass_f);
	printf("please input new password again:");
	get_passwd(pass_s);
	if(strcmp(pass_f,pass_s)!=0||strlen(pass_f)==0)
	{
		printf("两次不一致\n");
		return -100;
	}
	get_encryption(pass_f);
	fd=fopen("passwd","wt");
	if(fd==NULL)
		return -1;
	fprintf(fd,"%s",pass_f);
	if(fclose(fd)==-1)
		printf("errno in close file in change passwd\n");
	return 100;
}


