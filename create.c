#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include "mystring.h"
#include "global.h"
/*返回name是表的第几个字段，从0开始，没有返回-1*/
int find_num_colum_name(table_info *ta,char *name,int *type)
{
	int k=0;
	colum_info *head=ta->next;
	while(head)
	{
		if(strcmp(head->colum_name,name)==0)
		{
			*type=head->type;
			return k;
		}
		k++;
		head=head->next;
	}
	return -1;
}

/*创建一个文件夹，也就是创建一个名字name的数据库*/

int create_database(const char *name)
{
	if(is_choose_database==1)//如果还在某个数据库中，那么就推出上一层
	{
		chdir("..");
		is_choose_database=0;
	}
	FILE *fd;
	if(access(name,0)==-1)//access函数是查看文件是不是存在
	{
		if (mkdir(name,0777)!=0)//如果不存在就用mkdir函数来创建
		{
			/* 文件夹创建失败*/
			return 19;
		}
		if(chdir(name)==-1)//进入刚创建的这个目录
		{
			printf("进入创建目录失败\n");
			return 0;
		}
		fd=fopen("table_info","w+");//创建一个存放表信息的一个文件，每个数据库中都有
		if(fd==NULL)
		{
			printf("table_info 创建失败\n");
			return 0;
		}
		fclose(fd);
		chdir("..");//返回上一层目录
		fd=fopen("database","a+");
		if(fd==NULL)
		{
			printf("database 名称注册失败\n");
			return 0;
		}
		fprintf(fd," %s",name);//创建的database写入注册
		fclose(fd);
		/*
		   把刚才新建的数据库加入到注册 表
		   */
		database *head=my_database;
		if(head==NULL)
		{
			my_database=(database*)malloc(sizeof(database));
			my_database->next=NULL;
			strcpy(my_database->name,name);
			return 0;
		}
		while(head->next)
		{
			head=head->next;
		}
		head->next=(database *)malloc(sizeof(database));
		head=head->next;
		head->next=NULL;
		strcpy(head->name,name);
	}
	else
	{
		/*文件夹存在<->数据库存在*/
		return 20;
	}
	return 0;
}
/*执行create函数*/
int run_create(char *sql_command)
{
	char sp[100];
	my_split(sql_command,sp,' ',2);
	/*create 两种功能 table 和 databases*/
	if(strcmp(sp,"database")==0)
	{
		my_split(sql_command,sp,' ',4);//不能有超过四个字段
		if(strlen(sp)!=0)
			return 150;//语法错误，执行失败
		my_split(sql_command,sp,' ',3);
		if(strlen(sp)==0)
			return 150;
		if(is_name(sp)==0)//如果第三个字段不符合名称标准那么一定是语法错误
			return 150;
		return create_database(sp);
	}
	else if(strcmp(sp,"table")==0)
	{
		if(is_choose_database==0)
		{
			printf("未选择任何数据库\n");
			return 0;
		}
		return create_table(sql_command,&my_table);
	}
	else
	{
		return 150;
	}
	return 0;
}
/*这个专门为char(n)定制的，负责解析这个，返回整型值 返回-1表示语法错误*/
static int parse_char(char *str)
{
	int len=strlen(str);
	str[len]=')';
	str[len+1]=0;
	int ans=0;
	if(str[4]!='(')
		return -1;
	int offset=5;
	if(str[offset]>'9'||str[offset]<'0')
		return -1;
	while(str[offset]<='9'&&str[offset]>='0')
	{
		offset++;
	}
	if(str[offset]!=')'||strlen(str+offset)!=1)
		return -1;
	offset=5;
	while(str[offset]!=')')
	{
		ans=ans*10+(int)(str[offset]-'0');
		offset++;
	}
	return ans;
}
/*解析其中的一行，也就是一个字段，并根据解析结果填写colum_info,正常返回0 表示信息填写正确，1表示不需要填写信息，150表示语法错误*/
int parse_one_colum(char *str,colum_info *col)
{
	int offset=0;
	int rec;
	int len_type;
	char next[50];
	char next_for_char[50];
	my_split(str,col->colum_name,' ',1);
	if(is_name(col->colum_name)==0)
		return 150;
	offset+=strlen(col->colum_name)+1;
	my_split(str+offset,next,' ',1);
	switch(find_type(next))
	{
		case 0:return 150;
		case 1: col->type=1;len_type=3;break;//int finish
		case 2: col->type=2;my_split(next,next_for_char,')',1); len_type=strlen(next_for_char);
				/* 上面要注意，开始写的时候是len_type=strlen(next_for_char)-1,认为其实不可以，因为这里next_for_char还没有解析，这是我犯的一个错误*/
				col->length=parse_char(next_for_char); if(col->length==-1) { return 150;} break;//
				/*
				 *2的时候要做特殊处理:
				 */
		case 3: col->type=3;len_type=6;break;
		case 4: col->type=4;len_type=5;break;
		default:return 150;
	}
	offset+=len_type+1;
	col->is_null=1;//在初始默认的情况下一定是先允许为空的，再根据后面条件判断具体能否为空
	col->is_primary=0;//开始一定不是主码
	if(strlen(str+offset-1)==0)//结束其它什么条件也没有，直接结束
	{
		col->is_primary=0;
		col->is_null=1;
		return 0;
	}

	my_split(str+offset,next,' ',1);
	rec=find_type(next);
	if(rec==9)
	{
		offset+=8;
		if(strcmp(str+offset,"key")==0)
		{
			col->is_primary=1;
			col->is_null=0;
			return 0;
		} 
		else if(strcmp(str+offset,"key not null")==0)
		{
			col->is_primary=1;
			col->is_null=0;
			return 0;
		}
		else
			return 150;
	}        
	if(rec==10)
		if(strcmp(str+offset,"not null")==0)
		{
			col->is_primary=0;
			col->is_null=0;
			return 0;
		}
		else
			return 150;
	return 0;
}
/*检查为name的表是否存在存在返回1 不存在返回0*/
int check_table_name(char *name)
{
	table_info *temp=table_list;
	while(temp)
	{
		if(strcmp(name,temp->table_name)==0)
			return 1;
		temp=temp->next_table;
	}
	return 0;
}
/*返回表名为name的表头*/
table_info * find_table_head(char *name)
{
	table_info *temp=table_list;
	while(temp)
	{
		if(strcmp(name,temp->table_name)==0)
		{
			return temp;
		}
		temp=temp->next_table;
	}
	return NULL;
}
/*真正执行create_table*/
int create_table(char *sql_command,table_info **my_table)
{
	char buf[300];//提供给sprintf存储表信息，写入文件
	int offset=0,len=strlen(sql_command);
	table_info *my_t;
	colum_info *colum,*temp;
	colum_info *current=(colum_info*)malloc(sizeof(colum_info)),*head=current;
	/*
	 *后面这两句是判断create语句最后一个字符是不是')'如果不是那么就返回语法错误
	 *如果是就把括号变成','，方便while语句的完全分离和解析
	 */
	if(sql_command[len-1]!=')')
		return 150;
	sql_command[len-1]=',';
	*my_table=(table_info*)malloc(sizeof(table_info));
	(*my_table)->num=0;
	(*my_table)->tail=NULL;
	(*my_table)->next=NULL;
	char sp[200];
	char one[50];//存放一个字段的信息，不断解析
	my_split_end(sql_command,sp,' ',2,0);//分离出前面两个固定格式已经确定的
	my_split(sp,(*my_table)->table_name,'(',1);//把名称分离出来，放到name中去
	if(is_name((*my_table)->table_name)==0)
		return 150;
	offset=strlen((*my_table)->table_name)+1;
	/*
	 *现在已经实现所有以','为分割的创建表项的功能代码，下一步就是要解析这些东西
	 *按照我们现在的情况来看，还不能够支持主键在下面定义，和主键有多个的情况!
	 */
	while(1)
	{
		colum=(colum_info*)malloc(sizeof(colum_info));
		colum->next=NULL;
		my_split_end(sp+offset,one,',',1,1);
		if(strlen(one)==0)
			break;
		offset+=strlen(one)+1;
		//printf("->%s<-\n",one);
		if(parse_one_colum(one,colum)==150)
		{
			free(colum);
			return 150;
		}
		current->next=colum;
		(*my_table)->num++;
		(*my_table)->tail=colum;
		current=current->next;
		//printf("\nname:%s\ntype:%d\nlength:%d\nprimary:%d\nis_null:%d\n",colum->colum_name,colum->type,colum->length,colum->is_primary,colum->is_null);
	}
	sql_command[len-1]=')';//防止万一有用这里恢复
	//printf("ok:%s:",sp+offset);
	//printf("name:%s\n",(*my_table)->table_name);
	FILE *fd=fopen("table_info","a+");
	if(fd==NULL)
		return -4;
	(*my_table)->next=head->next;
	free(head);
	colum=(*my_table)->next;
	if(check_table_name((*my_table)->table_name)==1)
	{
		printf("名为 %s 的表已经存在 执行失败\n",(*my_table)->table_name);
		colum=(*my_table)->next;
		while(colum)
		{
			temp=colum;
			colum=colum->next;
			free(temp);
		}
		free(*my_table);
		return 0;
	}
	sprintf(buf," %d %s ",(*my_table)->num,(*my_table)->table_name);
	write_file(buf,fd);
	while(colum)
	{
		sprintf(buf,"%s %d %d %d %d %d %d",colum->colum_name,colum->type,colum->length,colum->is_primary,colum->is_foreign,colum->is_null,colum->default_num);
		//printf("%s \n",buf);
		write_file(buf,fd);
		colum=colum->next;
	}
	fflush(fd);
	fclose(fd);
	/*下面这几步就是把新建立好的表插到table_list上面去，不能释放内存了！ */
	my_t=table_list;
	sprintf(buf,"%s.db",(*my_table)->table_name);
	fd=fopen(buf,"w");
	if(fd==NULL)
	{
		printf("创建物理表文件失败\n");
	}
	else
		fclose(fd);
	if(my_t==NULL)//如果table_list本来就为空那么就要特殊处理
	{
		table_list=(*my_table);
		(*my_table)->next_table=NULL;
		(*my_table)=NULL;
		return 0;
	}
	while(my_t&&my_t->next_table)
	{
		my_t=my_t->next_table;
	}
	my_t->next_table=(*my_table);
	(*my_table)->next_table=NULL;
	(*my_table)=NULL;
	/*
	   colum=(*my_table)->next;
	   while(colum)
	   {
	   temp=colum;
	   colum=colum->next;
	   free(temp);
	   }
	   free(*my_table);*/
	printf("success in create table\n");
	return 0;
}
