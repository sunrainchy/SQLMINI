#include <stdio.h>
#include <stdlib.h>
#include "type.h"
#include "create.h"
#include "mystring.h"
int parse_one_res(int or_and,restrict_colum *res,char *str,table_info *ta);
int parse_where(char *sql_command,restrict_colum *head,table_info *ta);
//int parse_one_res(int or_and,restrict_colum *res,char *str);
extern int is_choose_database;
static char my_select[500];//存放前面select部分的语句
static char where[500];//存放后面的where部分的语句
static char real[500];//存放要选择的列，包括列和*
/**下面的对应的几种类型定义，用来暂时存放不同类型的值的*/
static char sp[500];
static int int_type;
static double double_type;
static float float_type;
static char char_type[200];
static char all[500];
static char my_name[30][50];//存放列名称
static restrict_colum res;//存放约束的头指针,头永远不起真正作用，只是一个头
int release_res(restrict_colum *head)
	/*释放类型为restrict_colum的链表资源*/
{
	restrict_colum *temp=head;
	while(head)
	{
		temp=head;
		head=head->next;
		free(temp);
	}
	return 0;
}
int run_select(char *sql_command)//目前实现的功能就是where前面的部分，where后面的暂时不支持,目前也还是支持单表查询
	/*这个函数总体负责select语句的解析和执行*/
{
	int count=0;//记录满足条件的记录的条数
	res.next=NULL;
	sp[0]=0,char_type[0]=0,all[0]=0;//初始化一下
	my_select[0]=0,where[0]=0,real[0]=0;//同上
	if(is_choose_database==0)
	{
		p_error(151);
		return 0;
	}
	int n=0;
	int colum[50];//存放满足条件要输出的列号，从0开始
	int len=0;
	int k=0;
	int i=0,j;
	char row_colum[50][100];
	table_info *table;
	colum_info *head;
	my_split_end(sql_command,my_select,' ',4,1);
	if(strlen(my_select)==0)//如果选择出来的是空值，说明不存在where后面的部分
	{
		strcpy(my_select,sql_command);
	}
	my_split(my_select,real,' ',2);
	my_split(my_select,sp,' ',3);
	if(strcmp(sp,"from")!=0)
	{
		p_error(150);
		return 0;
	}
	my_split_end(my_select,sp,' ',3,0);//分离出表名称
	table=find_table_head(sp);
	if(table==NULL)
	{
		printf("表名为%s的表不存在\n",sp);
		return 0;
	}
	my_split_end(sql_command,where,' ',4,0);//分离出where部分
	//printf(":%s:\n",where);
	if(parse_where(where,&res,table)!=0)
	{
		return 0;
	}
	head=table->next;
	i=0;
	while(head)
	{
		strcpy(my_name[i],head->colum_name);
		i++;
		head=head->next;
	}
	strcat(sp,".db");
	FILE *fd=fopen(sp,"r+");
	if(fd==NULL)
	{
		p_error(-4);
		return 0;
	}
	n=1;
	len=0;
	while(1)
	{
		k=0;
		my_enhance_split(real,sp,',',n);
		n++;
		//printf("sp:%s\n",sp);
		if(strlen(sp)==0)
			break;
		if(strcmp(sp,"*")==0)//如果是*那么就全部都放入，所有序号都放入
		{
			for(i=0;i<table->num;i++)
				colum[len++]=i;
			continue;
		}
		head=table->next;
		while(head)
		{
			if(strcmp(head->colum_name,sp)==0)
			{
				colum[len++]=k++;
				break;
			}
			k++;
			head=head->next;
		}
		if(head==NULL)
		{
			printf("选择的列名%s不存在\n",sp);
			return 0;
		}
	}
	printf("\033[33m");
	//输出要选择的列名
	for(i=0;i<len;i++)
	{
		printf("+---------------");
	}
	printf("+\n");
	for(i=0;i<len;i++)
		printf("|%-15s",my_name[colum[i]]);
	printf("|\n");
	for(i=0;i<len;i++)
	{
		printf("+---------------");
	}
	printf("+\n");

	//现在都有要选择的列号全部都在colum里面，长度为len
	while(feof(fd)==0)
	{
		for(i=0;i<table->num;i++)
			if(fscanf(fd,"%s",row_colum[i])==EOF)
			{
				j=100000;//如果不行就让i变的很大就跳过下一次的执行
				break;
			}
			else
				j=0;
		/*现在把每一行的每个段依次放在row_colum里面，现在来检测次列是否满足约束条件*/
		if(is_row_ok(row_colum,&res)!=0)////////////////////////////////////////////////////////////
			continue;
		//////////////////////////////////////
		/*for(i=0;i<len;i++)
		  {
		  printf("+---------------");
		  }
		  printf("+\n");
		  */
		for( ;j<len;j++)
			printf("|%-14s ",row_colum[colum[j]]);
		if(feof(fd)==0)
		{
			count++;
			printf("|\n");
		}
	}
	if(count>0)
	{
		for(i=0;i<len;i++)
		{
			printf("+---------------");
		}
		printf("+\n");
	}
	printf("\033[m");
	printf("\033[31m共选出%d条满足条件的记录\033[m\n",count);
	printf("select 成功\n");
	release_res(res.next);
	res.next=NULL;
	return 0;
}

int parse_where(char *sql_command,restrict_colum *head,table_info *ta)
	/*这个函数负责解析where 部分，解析出来每个字段的要求放在head这个链表中，sql_comand 是存放sql语句的where和后面的部分*/
{
	char my_sp[500];
	char my_colum_f[100];//表示一对关系的前面一个
	char and_or[100];//关系
	char my_colum_n[100];//一对关系后面的一个 
	int n;
	head->is_res=0;
	head->next=NULL;
	if(strlen(sql_command)==0)
		return 0;
	my_split(sql_command,my_sp,' ',1);
	if(strcmp(my_sp,"where")!=0)//如果where子句第一个字段不是where的话，那么返回语法错误
	{
		printf("161\n");
		p_error(150);
		return -1;
	}
	my_split_end(sql_command,my_sp,' ',1,0);//将where后面的那一部分分离出来
	if(strlen(my_sp)==0)//如果where后面什么都没有了的话，那么一定是语法错误
	{
		//printf("168\n");
		p_error(150);
		return -1;
	}
	n=1;
	restrict_colum *res_head=&res;
	/*下面的是对第一个进行处理*/
	my_enhance_split(my_sp,my_colum_f,' ',n);
	my_enhance_split(my_sp,and_or,' ',2);
	int p,q,r;//临时变量
	//printf(":%s:\n",and_or);
	if(strlen(and_or)==0 || strcmp(and_or,"and")==0)//如果约束就一句，或者and;
	r=0;
	else if(strcmp(and_or,"or")==0)
		r=1;
	else//不为空也不是and也不是or那么一定是语法错误
	{
		//printf("184\n");
		p_error(150);
		return -1;
	}
	res_head->next=(restrict_colum*)malloc(sizeof(restrict_colum));
	res_head->next->next=NULL;
	res_head=res_head->next;
	if(parse_one_res(r,res_head,my_colum_f,ta)!=0)
	{
		free(res.next);
		res.next=NULL;
		//p_error(150);
		//printf("196\n");
		return -1;
	}
	n++;
	while(1)
	{
		my_enhance_split(my_sp,and_or,' ',n);
		my_enhance_split(my_sp,my_colum_f,' ',n+1);
		/*
		   {
		   my_enhance_split(my_sp,and_or,' ',n+1);
		   my_enhance_split(my_sp,my_colum_f,' ',n);
		   }*/				
		p=strlen(and_or);
		printf("%s\n",and_or);
		q=strlen(my_colum_f);
		if(p==0&&q==0)
		{
			break;
		}
		if(p!=0&&q==0)
		{
			release_res(res.next);
			res.next=NULL;
			//printf("209\n");
			p_error(150);
			return -1;
		}
		//printf(":%s:\n",my_colum_f);
		res_head->next=(restrict_colum*)malloc(sizeof(restrict_colum));
		res_head->next->next=NULL;
		res_head=res_head->next;
		if(strlen(and_or)==0 || strcmp(and_or,"and")==0)//如果约束就一句，或者and;
		r=0;
		else if(strcmp(and_or,"or")==0)
			r=1;
		else//不为空也不是and也不是or那么一定是语法错误
		{/////////////////////////////////////////////////释放内存
			//printf("223\n");
			p_error(150);
			return -1;
		}
		if(parse_one_res(r,res_head,my_colum_f,ta)!=0)
		{
			//printf("229\n");
			return -1;
		}
		n+=2;
	}
	/*res_head=res.next;
	  while(res_head)
	  {
	  printf("->%s->\n",res_head->demand);
	  res_head=res_head->next;
	  }*/
	return 0;
}
int parse_one_res(int or_and,restrict_colum *res,char *str,table_info *ta)
	/*这个函数负责解析一段约束的具体要求，解析出来的结果存放在res中，or_and 参数提供是is还是or的参数，1 or 0 and
	 *res就是解析出来的结果存放处，str就是要解析的那么约束,成功返回0，不成功返回非0
	 */
{
	int pos;
	res->or_and=or_and;
	char first[50];//存放符号前面的部分
	pos=is_have_ch(str,'=');
	if(pos!=-1)/////下面全部是有=的处理
	{
		if(pos==0)//如果是第一个位置有=那么一定出错
		{
			//printf("260\n");
			p_error(150);
			return -1;
		}
		if(str[pos-1]=='>')//>=的情况
		{
			if(pos==1){p_error(150);return -1;}
			res->is_res=4;
			my_split_end(str,first,'>',1,1);//分离出前面约束列
			my_split_end(str,res->demand,'=',1,0);//分离出后面的约束值,直接放在res里面了
		}
		else if(str[pos-1]=='<')//<=的情况
		{
			if(pos==1){p_error(150);return -1;}
			res->is_res=5;
			my_split_end(str,first,'<',1,1);
			my_split_end(str,res->demand,'=',1,0);
		}
		else //只有一个=的情况
		{
			res->is_res=1;
			my_split_end(str,first,'=',1,1);
			my_split_end(str,res->demand,'=',1,0);
		}
	}
	else //////////////////////////////////////////////////////////////////////////////////整个大框架的else,上面判断了= >= <=三种，现在只能是<和>
	{
		pos=is_have_ch(str,'>');
		if(pos!=-1)
		{
			if(pos==0) { p_error(150);return -1;}
			res->is_res=2;
			my_split_end(str,first,'>',1,1);
			my_split_end(str,res->demand,'>',1,0);
		}
		else
		{
			pos=is_have_ch(str,'<');
			if(pos!=-1)
			{
				if(pos==0) { p_error(150);return -1;}
				res->is_res=3;
				my_split_end(str,first,'<',1,1);
				my_split_end(str,res->demand,'<',1,0);
			}
		}
	}
	if(pos==-1)//如果pos最后还是为-1，说明没有 > < 和 =
	{
		//printf("306\n");
		p_error(150);
		return -1;
	}
	if(strlen(res->demand)==0)//如果分离出来后约束条件的标杆为空一定是语法错误
	{
		//printf("312\n");
		p_error(150);
		return -1;
	}
	int num;
	num=find_num_colum_name(ta,first,&res->type);
	if(num==-1)
	{
		printf("列名%s 不存在\n",first);
		return -1;
	}
	else
		res->num=num;
	//printf("type:%d is %d num:%d som:%s\n",res->is_res,res->or_and,res->num,res->demand);
	return 0;
}

int is_row_ok(char row_colum[][100],restrict_colum *head)
	/*此函数检测行是否满足链表约束head,符合返回0，否则返回非0*/
{
	/*下面分别作用是or条件成立次数，or条件个数 ，res_and 好and_num意思一样*/
	int rec_or=0,or_num=0,rec_and=0,and_num=0;
	head=head->next;//用下一个，第一个只是一个表头
	while(head)
	{
		//printf("haha->%d\n",head->num);
		//printf("%s\n",row_colum[head->num]);
		if(head->or_and==1)//or
		{
			or_num++;
			if(is_one_colum_ok(row_colum[head->num],head->demand,head->type,head->is_res))
				rec_or++;
		}
		if(head->or_and==0)//and
		{
			and_num++;
			if(is_one_colum_ok(row_colum[head->num],head->demand,head->type,head->is_res))
				rec_and++;
			else 
				return -1;
		}
		head=head->next;
	}
	if((or_num==0&&rec_or==0)||rec_or>0)
		return 0;
	else 
		return -1;
	return 0;
}

/* 
 *这个函数是检测一行中某个约束是否满足，满足返回1，否则返回非0
 *参数说明 first是第一个 比如说约束是 a > b int 型，那么first是a，second是b type是int 1，logical是 > 2
 */
int is_one_colum_ok(char *first,char *second,int type,int logical)
{
	//return 1;
	if(logical==1&&type!=2)//防止为空且为NULL的情况
		return strcmp(first,second)==0;
	if(strcmp(first,"null")==0)//如果first是NULL的话，那么就不支持任何操作，这个约束一定不满足
		return 0;
	int rec_int_first,rec_int_second;
	double rec_double_first,rec_double_second;
	float rec_float_first,rec_float_second;
	char rec_char[50];
	switch(type)
	{
		case 1://int
			sscanf(first,"%d",&rec_int_first);
			sscanf(second,"%d",&rec_int_second);
			if(logical==1)
				return rec_int_first==rec_int_second;
			else if(logical==2)
				return rec_int_first>rec_int_second;
			else if(logical==3)
				return rec_int_first<rec_int_second;
			else if(logical==4)
				return rec_int_first>=rec_int_second;
			else if(logical==5)
				return rec_int_first<=rec_int_second;
			else 
				return 0;
		case 3://double
			sscanf(first,"%lf",&rec_double_first);
			sscanf(second,"%lf",&rec_double_second);
			if(logical==1)
				return rec_double_first==rec_double_second;
			else if(logical==2)
				return rec_double_first>rec_double_second;
			else if(logical==3)
				return rec_double_first<rec_double_second;
			else if(logical==4)
				return rec_double_first>=rec_double_second;
			else if(logical==5)
				return rec_double_first<=rec_double_second;
			else
				return 0;
		case 4://float
			sscanf(first,"%f",&rec_float_first);
			sscanf(second,"%f",&rec_float_second);
			if(logical==1)
				return rec_float_first==rec_float_second;
			else if(logical==2)
				return rec_float_first>rec_float_second;
			else if(logical==3)
				return rec_float_first<rec_float_second;
			else if(logical==4)
				return rec_float_first>=rec_float_second;
			else if(logical==5)
				return rec_float_first<=rec_float_second;
			else
				return 0;
		case 2://char()
			rec_char[0]=34;
			rec_char[1]=0;
			strcat(rec_char,first);
			strcat(rec_char,"\"");
			if(logical==1)
				return strcmp(rec_char,second)==0;
			else if(logical==2)
				return strcmp(rec_char,second)<0;
			else if(logical==3)
				return strcmp(rec_char,second)>0;
			else if(logical==4)
				return strcmp(rec_char,second)>=0;
			else if(logical==5)
				return strcmp(rec_char,second)<=0;
			else
				return 0;
	}
	return 0;
}
