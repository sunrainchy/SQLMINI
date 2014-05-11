/*
 *cat ans | sort | tr '\n' ',' | sed s/,/\",\"/g | tr '[A-Z]' '[a-z]'
 *全部转换成小写
 */
//#include "global.h"
#include "type.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include "mystring.h"
extern struct table_info *table_list;
extern int is_choose_database;
extern struct database *my_database;
int CHY_KEY_WORD_NUM=225;
char *CHY_KEY_WORD[255]={"add","all","alter","analyze","and","as","asc","asensitive","before","between","bigint","binary","blob","both","by","call","cascade","case","change","char","character","check","collate","column","condition","connection","constraint","continue","convert","create","cross","current_date","current_time","current_timestamp","current_user","cursor","database","databases","day_hour","day_microsecond","day_minute","day_second","dec","decimal","declare","default","delayed","delete","desc","describe","deterministic","distinct","distinctrow","div","double","drop","dual","each","else","elseif","enclosed","escaped","exists","exit","explain","false","fetch","float","float4","float8","for","force","foreign","from","fulltext","goto","grant","group","having","high_priority","hour_microsecond","hour_minute","hour_second","if","ignore","in","index","infile","inner","inout","insensitive","insert","int","int1","int2","int3","int4","int8","integer","interval","into","is","iterate","join","key","keys","kill","label","leading","leave","left","like","limit","linear","lines","load","localtime","localtimestamp","lock","long","longblob","longtext","loop","low_priority","match","mediumblob","mediumint","mediumtext","middleint","minute_microsecond","minute_second","mod","modifies","natural","not","no_write_to_binlog","null","numeric","on","optimize","option","optionally","or","order","out","outer","outfile","precision","primary","procedure","purge","raid0","range","read","reads","real","references","regexp","release","rename","repeat","replace","require","restrict","return","revoke","right","rlike","schema","schemas","second_microsecond","select","sensitive","separator","set","show","smallint","spatial","specific","sql","sql_big_result","sql_calc_found_rows","sqlexception","sql_small_result","sqlstate","sqlwarning","ssl","starting","straight_join","table","terminated","then","tinyblob","tinyint","tinytext","to","trailing","trigger","true","undo","union","unique","unlock","unsigned","update","usage","use","using","utc_date","utc_time","utc_timestamp","values","varbinary","varchar","varcharacter","varying","when","where","while","with","write","x509","xor","year_month","zerofill"};
int is_key_word(char *str)
	/*
	 *判断str这个串是不是关键字中的某一个
	 *这个函数仅仅是提供给用户使用的函数，具体实现还是看find_key_word函数
	 */
{
	return find_key_word(CHY_KEY_WORD,0,CHY_KEY_WORD_NUM-1,str);
}
int find_key_word(char **str,int L,int R,char *ch)
	/**** 在字符串数组str中的L-R区间查找串ch 这里用的是二分法****/
{
	if(L>R)
		return 0;
	int k=strcmp(ch,str[(L+R)/2]);
	if(k==0)
		return 1;
	if(k==1)
		return find_key_word(str,(L+R)/2+1,R,ch);
	else
		return find_key_word(str,L,(L+R)/2-1,ch);
	return 0;
}
int find_function(const char *name)
	/*
	 *
	 ** 解析关键子name的功能，返回对应九个动词的功能代号*
	 ** 1 create 2 select 3 update 4 insert 5 describe 6 use 7 drop......
	 */
{
	if(strcmp(name,"create")==0)
		return 1;
	else if(strcmp(name,"select")==0)
		return 2;
	else if(strcmp(name,"update")==0)
		return 3;
	else if(strcmp(name,"insert")==0)
		return 4;
	else if(strcmp(name,"describe")==0)
		return 5;
	else if(strcmp(name,"use")==0)
		return 6;
	else if(strcmp(name,"drop")==0)
		return 7;
	return 100;
	///////////and so on
}


int find_type(char *str)
	/*
	 *专门为create table定制的 char()还需要重新定制
	 *检测str代表的是那种数据类型，并返回对应代码
	 *表示这个字段的字节类型1 int 2 char 3 double 4 float 
	 *如果是primary 返回9
	 *如果是not 返回10
	 *返回0表示不是任何类型
	 */
{
	if(strcmp(str,"int")==0)
		return 1;
	else if(strcmp(str,"double")==0)
		return 3;
	else if(strcmp(str,"float")==0)
		return 4;
	else if(strcmp(str,"primary")==0)
		return 9;
	else if(strcmp(str,"not")==0)
		return 10;
	else
	{
		if(str[0]=='c'&&str[1]=='h'&&str[2]=='a'&&str[3]=='r')//char类型初步检测
			return 2;
	}
	return 0;
}
/*加载表*/
int load_table()
{
	int i,j,k;
	table_list=NULL;
	table_info *temp=NULL;
	colum_info *head;
	FILE *fd=fopen("table_info","r");
	if(fd==NULL)
		return -4;
	if(fgetc(fd)==EOF)
	{
		fclose(fd);
		return 0;
	}
	table_list=(table_info*)malloc(sizeof(table_info));
	temp=table_list;
	temp->next_table=NULL;
	temp->next=NULL;
	while(feof(fd)==0)
	{
		fscanf(fd,"%d%s",&temp->num,temp->table_name);
		head=(colum_info*)malloc(sizeof(colum_info));
		temp->next=head;
		head->next=NULL;
		for(i=0;i<temp->num;i++)
		{
			fscanf(fd,"%s%d%d%d%d%d%d",head->colum_name,&head->type,&head->length,&head->is_primary,&head->is_foreign,&head->is_null,&head->default_num);
			if(i+1!=temp->num)
			{
				head->next=(colum_info*)malloc(sizeof(colum_info));
				head=head->next;
				head->next=NULL;
			}
		}
		if(feof(fd)==0)
		{
			temp->next_table=(table_info*)malloc(sizeof(table_info));
			temp=temp->next_table;
			temp->next_table=NULL;
		}
	}
	fclose(fd);
	return 0;
}
/*完成一系列初始化工作*/
int init()
{
	printf("\033[31m         N     N\033[32m NNNNN\033[33m N    \033[34m  NNNN\033[35m  NNNN \033[36m N    N\033[32m NNNNN    \033[33m NNNNNN\033[32m NNNNN  \033[35m N    \033[34m N    N \033[33mNNNNN \033[32mNN   N \033[31mNNNNN\033[0m\n");
	printf("\033[31m         N  N  N\033[32m N    \033[33m N    \033[34m N    \033[35m N    N \033[36mNN  NN\033[32m N         \033[33mN     \033[32m N   N  \033[35m N    \033[34m NN  NN \033[33m  N  \033[32m NN   N \033[31m  N  \033[0m\n");
	printf("\033[31m         N N N N\033[32m NNNNN\033[33m N    \033[34m N    \033[35m N    N \033[36mN NN N\033[32m NNNNN    \033[33m  NNNN \033[32m N   N  \033[35m N    \033[34m N NN N \033[33m  N  \033[32m N N  N \033[31m  N  \033[0m\n");
	printf("\033[31m         NN   NN\033[32m N    \033[33m N    \033[34m N    \033[35m N    N \033[36mN    N\033[32m N        \033[33m      N\033[32m NNNNNN \033[35m N    \033[34m N    N \033[33m  N  \033[32m N  N N \033[31m  N  \033[0m\n");
	printf("\033[31m         N     N\033[32m NNNNN\033[33m NNNNN \033[34m NNNN \033[35m NNNN  \033[36mN    N\033[32m NNNNN    \033[33m NNNNNN\033[32m NNNNN N\033[35m NNNNN\033[34m N    N \033[33mNNNNN\033[32m N   NN \033[31mNNNNN\033[0m\n");

	is_choose_database=0;//
	load_database();
	//load_table();
	return 0;
}
/*一切结尾相关工作会在这里完成*/
int distory()
{
	release_table_list();
	return 0;
}
release_table_list()
{
	//释放table_list链表
	table_info *head=table_list,*head1;
	colum_info *temp,*temp1;
	while(head)
	{
		head1=head;
		temp=head->next;
		while(temp)
		{
			temp1=temp;
			temp=temp->next;
			free(temp1);
		}
		head=head->next_table;
		free(head1);
	}
	table_list=NULL;
	return 0;
}
/*专门为describe_table定制的函数，根据d设置name*/
static int get_type(int d,char *name)
{
	switch(d)
	{
		case 1:strcpy(name,"int");break;
		case 2:strcpy(name,"char");break;
		case 3:strcpy(name,"double");break;
		case 4:strcpy(name,"float");break;
		default:name[0]=0;
	}
	return 0;
}
/*描述表信息，提供describe tables的功能*/
int describe_table(char *name)
{
	char type[20];
	table_info *temp=table_list;
	colum_info *head;
	while(temp)
	{
		if(strcmp(name,temp->table_name)==0)
			break;
		temp=temp->next_table;
	}
	if(temp==NULL)
	{
		printf("此表不存在\n");
		return 0;
	}
	head=temp->next;
	printf("\033[32m+--------------------+--------------------+------------+-------------+----------+-----------+\n");
	printf("|name                |type                |is_primary  |is_foreign   |is_null   |default_num|\n");
	printf("+--------------------+--------------------+------------+-------------+----------+-----------+\n");
	while(head)
	{
		get_type(head->type,type);
		if(head->type==2)//char
			sprintf(type,"%s(%d)","char",head->length);
		printf("|%-20s|%-20s|%-12s|%-13s|%-10s|%-11d|\n",head->colum_name,type,head->is_primary==1 ? "YES" : "NO",head->is_foreign==1 ? "YES" : "NO",head->is_null==1 ? "YES" : "NO",head->default_num);
		head=head->next;
	}
	printf("+--------------------+--------------------+------------+-------------+----------+-----------+\n\033[0m");
	return 0;
}

/*实现show_table的功能*/
int show_table()
{
	//printf("show tables\n");
	if(is_choose_database==0)
	{
		printf("未选择任何数据库\n");
		return 0;
	}
	table_info *temp=table_list;
	if(temp==NULL)
	{
		printf("没有任何表的存在\n");
		return 0;
	}
	printf("\033[31m+-----------------+\n");
	printf("|table_name       |\n");
	while(temp)
	{
		printf("+-----------------+\n");
		printf("|%-17s|\n",temp->table_name);
		temp=temp->next_table;
	}
	printf("+-----------------+\n\033[0m");
	return 0;
}


/*用my_database加载已经存在的数据库*/
int load_database()
{
	FILE *fd=fopen("database","r");
	if(fd==NULL)
	{
		printf("数据库加载失败\n");
		return 0;
	}
	my_database=NULL;
	database *head;
	if(fgetc(fd)==EOF)
	{
		fclose(fd);
		return 0;
	}
	my_database=(database *)malloc(sizeof(database));
	head=my_database;
	head->next=NULL;
	while(feof(fd)==0)
	{
		fscanf(fd,"%s",head->name);
		if(feof(fd)==0)
		{
			head->next=(database *)malloc(sizeof(database));
			head=head->next;
			head->next=NULL;
		}
	}
	fclose(fd);
	return 0;
}

/*实现show_databases功能*/
int show_databases()
{
	if(my_database==NULL)
	{
		printf("没有任何数据库\n");
		return 0;
	}
	printf("\033[33m+-------------------------+\n");
	printf("|Databases                |\n");
	database *head=my_database;
	while(head)
	{
		printf("+-------------------------+\n");
		printf("|%-25s|\n",head->name);
		head=head->next;
	}
	printf("+-------------------------+\n\033[0m");
	return 0;
}
/*判断name是不是在系统my_database注册表中返回 1在 0 不在*/
int is_database(char *name)
{
	database *head=my_database;
	while(head)
	{
		if(strcmp(head->name,name)==0)
			return 1;
		head=head->next;
	}
	return 0;
}
/*实现use_databse 功能*/
int use_database(char *name)
{
	if(is_database(name)==0)
	{
		printf("该数据库不存在\n");
		return 0;
	}
	release_table_list();//先释放先前的table_list
	if(is_choose_database==1)//如果当前不是主目录，则一定在某个数据库中，那么出来
	{
		chdir("..");
		is_choose_database=0;
	}
	chdir(name);
	is_choose_database=1;
	load_table();
	printf("database changed\n");
	return 0;
}

/*删除一个数据库，就是连同文件夹及其内容一并删除*/
/*实现过程就是先删除注册表中的，然后删除物体实体，再将注册表写回文件*/
int drop_database(char *name)
{
	char buf[50];//存放系统调用的命令
	if(is_choose_database==1)
	{
		chdir("..");
		is_choose_database=0;
	}
	if(is_database(name)==0)
	{
		printf("要删除的数据库不存在\n");
		return 0;
	}
	database *head=my_database,*temp;
	if(head==NULL)
		return 0;
	if(strcmp(name,head->name)==0)
	{
		my_database=head->next;
		free(head);
		head=NULL;
	}
	while(head&&head->next)
	{
		if(strcmp(head->next->name,name)==0)
		{
			temp=head->next;
			head->next=head->next->next;
			free(temp);
			break;
		}
		head=head->next;
	}
	FILE *fd=fopen("database","w+");
	if(fd==NULL)
	{
		printf("删除错误，请重新加载\n");
		return 0;
	}
	head=my_database;
	while(head)
	{
		fprintf(fd," %s",head->name);
		head=head->next;
	}
	fclose(fd);
	sprintf(buf,"rm -r %s",name);
	if(system(buf)==-1)//执行系统调用删除文件夹及其里面的内容
	{
		printf("数据库物理文件删除失败\n");
		return 0;
	}
	printf("数据库删除成功\n");
	return 0;
}

/*删除一个表，连同注册表里面的表和物理文件一同删除*/
int drop_table(char *name)
{
	char buf[1000];//存放重新写入的一些信息
	if(is_choose_database==0)
	{
		printf("没有选择任何数据库\n");
		return 0;
	}
	if(check_table_name(name)==0)
	{
		printf("要删除的表不存在\n");
		return 0;
	}
	table_info *temp=table_list,*temp1;
	colum_info *head,*head1;
	//注意这里的temp不可能为NULL，否则前面的条件不可能通过
	if(strcmp(temp->table_name,name)==0)
	{
		head=table_list->next;
		table_list=temp->next_table;
		free(temp);////////////////////////////////////////////////////////////////////
		temp=NULL;
	}
	while(temp&&temp->next_table)
	{
		if(strcmp(temp->next_table->table_name,name)==0)
		{
			temp1=temp->next_table;
			head=temp->next_table->next;
			temp->next_table=temp->next_table->next_table;
			//head=temp1->next_table->next;
			free(temp1);
			break;
		}
		temp=temp->next_table;
	}
	while(head)
	{
		head1=head;
		head=head->next;
		free(head1);
	}
	/////////删除后重新写入文件
	temp=table_list;
	FILE *fd=fopen("table_info","w+");
	if(fd==NULL)
	{
		printf("表删除信息写入失败\n");
		return 0;
	}
	while(temp)
	{
		head=temp->next;
		sprintf(buf," %d %s ",temp->num,temp->table_name);
		write_file(buf,fd);
		while(head)
		{
			sprintf(buf,"%s %d %d %d %d %d %d",head->colum_name,head->type,head->length,head->is_primary,head->is_foreign,head->is_null,head->default_num);
			//printf("%s \n",buf);
			write_file(buf,fd);
			head=head->next;
		}
		temp=temp->next_table;
	}
	fclose(fd);
	sprintf(buf,"rm %s.db",name);
	if(system(buf)==-1)
	{
		printf("物理表删除失败\n");
		return 0;
	}
	printf("%s 删除成功\n",name);
	///////////
	return 0;
}
