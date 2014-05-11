#include <stdio.h>
#include "file.h"
#include "type.h"
#include "errno.h"
#include "mystring.h"
#include "create.h"
static char sp[500];
static char str[500];
static char table_name[100];//存放要插入的表的.db文件名称
static char my_sql[500];
static char str_next[500];
static table_info *this_table;//指向当前插入这个表的表头节点
static char name[50];
static char values[50];
static char storage[1000];
extern int is_choose_database;
int insert_values(char *sql_command)
	//负责插入数据的解析和执行
{
	storage[0]=0;
	int n;
	colum_info *head;
	int flag;
	int len;
	int len1,len2;//就是两个变量
	if(is_choose_database==0)
	{
		p_error(151);
		return 0;
	}
	my_split(sql_command,my_sql,')',1);//分离出values前面的所有内容开始解析
	my_split(my_sql,sp,' ',2);//分离出空格分出的第二个字段
	if(strcmp(sp,"into")!=0)
	{ p_error(150);return 0;}
	my_split_end(my_sql,sp,' ',2,0);//分离出第二个空格后面的所有
	my_split(sp,str,'(',1);//在上面分离的基础上进行再次分离出要插入的表名
	this_table=find_table_head(str);
	if(this_table==NULL)//如过要插入的表名不存在则提示并退出
	{
		printf("表名为%s的表不存在\n",str);
		return 0;
	}
	strcat(str,".db");//打开.db文件
	strcpy(table_name,str);
	FILE *fd=fopen(str,"a+");
	if(fd==NULL)
	{
		p_error(-4);
		return 0;
	}
	my_split_end(sp,str,'(',1,0);//分离出来后，现在str里面就是插入的表项目了
	if(strlen(str)==0)//如果str里面什么也没有，那么就是插入表的所有列
		flag=0;
	else
		flag=1;
	//前面的解析完成，需要的结果现在放在str里面
	my_split_end(sql_command,str_next,')',1,0);//分离出后面的部分，也就是values的部分
	my_split(str_next,sp,'(',1);
	if(strcmp(sp,"values")!=0)
	{
		p_error(150);
		return 0;
	}
	my_split_end(str_next,sp,'(',1,0);
	len=strlen(sp);
	if(len!=0)
		sp[len-1]=0;//在长度不为0的情况下，去除后面的')'
	//现在前面插入表列全部在str里面，每个列的值全部在sp里面
	head=this_table->next;
	if(flag==0)//如果flag==0，那么就全部把名称放入到str里面，然后按照下面的处理（flag=1）的情况
	{
		str[0]=0;
		flag=1;
		while(head)
		{
			strcat(str,head->colum_name);
			strcat(str,",");
			head=head->next;
		}
		len=strlen(str);
		if(len!=0)
			str[len-1]=0;
	}
	if(flag==1)//////////////////////////////////////////////////////////////////////flag=1
	{
		head=this_table->next;
		n=1;
		while(head)
		{
			my_split(str,name,',',n);//分离出要插入的第n个列名
			my_split(sp,values,',',n);//分离出第n个插入的列的数值
			len1=strlen(name);
			//printf("name%s\n",name);
			len2=strlen(values);
			/*下面这个操作还是有必要解释一下的，如果有插入的列或者值其中之一为空的话才出错，两个都不为空，或者两个都为空属于正常情况 */
			if((len1==0&&len2!=0)||(len1!=0&&len2==0))
			{
				//printf("haha\n");
				p_error(155);
				return 0;
			}
			if(len1==0&&len2==0)//如果插入名称为空和插入值也为空，那么说明插入的就是当前head的列名,值为空
			{
				if(head->type==2)//如果是char()类型，那么就这样，保持和后面的兼容
					strcpy(values,"\"null\"");
				else
					strcpy(values,"null");
				strcpy(name,head->colum_name);
			}
			while(head)
			{
				if(strcmp(name,head->colum_name)==0&&len1!=0)//如果开始插入的那么名称默认，不跳出，这里一定能匹配
					break;
				/*
				 * 注意这里没考虑默认值的设置，系统暂时还不支持
				 */
				if(head->is_primary==1)
				{
					//printf("%s %s %d %d\n",name,head->colum_name,len1,head->is_primary);
					p_error(152);
					//return 0;
				}
				if(head->is_null==0)
				{
					p_error(153);
					return 0;
				}
				strcat(storage,"null ");
				if(len1==0)//如果名字等于0，那么说明这次匹配成功了，所以要退出，如过不退出那么会一直匹配到结束，那么后面循环外的head->next 就core了
					break;
				head=head->next;
			}
			if(len2!=0&&check_is_type(values,head->type)!=0)
			{
				p_error(154);
				return 0;
			}
			if(head->is_primary==1)
			{
				if(is_appear(values,table_name,find_key_num(this_table),this_table->num)!=0)//注意这个is_appear函数，传进去的values最后面那个双引号被去掉了，去掉是在前面成对出现的情况下把最后那位清零，而没恢复，反正后面是要压缩的
				{
					p_error(156);
					return 0;
				}
			}
			/*modify in 2013-06-18 */
			if(head&&len1!=0)//如果head为空跳出来的话，那么最后一个插入值且是为空默认插入的那就完了，NULL又插入一次，这样就保证空值不多次插入
			{
				strcat(values," ");
				strcat(storage,values);
			}
			if(head)//防止这样一种情况，最后一个都没匹配上，那么就到结束了，那么就死菜了，所以这里要保证这样情况不发生，语义错误交给后面代码处理
				head=head->next;
			n++;
		}

	}//////////////////////////////////////////////flag=1;
	my_enhance_split(str,name,',',n);
	my_enhance_split(sp,values,',',n);
	if(strlen(name)!=0||strlen(values)!=0)
	{
		p_error(155);
		return 0;
	}
	int i=0;
	len=0;
	for(i=0;storage[i];i++)
	{
		if(storage[i]!=34)
			storage[len++]=storage[i];
	}
	storage[len-1]=10;
	storage[len]=0;
	write_file(storage,fd);
	fclose(fd);
	printf("插入成功\n");
	return 0;
}



int is_appear(char *str,char *name,int k,int p)
	/*主键化为字符串后是str，判断str在name(.db)的文件中有没有出现，K是主码中的第几个字段,p是总共有多少个字段,出现返回1，没出现返回0*/
{
	int len=strlen(str);
	if(len==0)//如果为空  处理一下，虽然不可能为空
		return 0;
	if(str[0]==34)//如果是一个串的话，那么一定前面有双引号，不管参数提供的是什么这里提供一个保障
	{
		str++;
		str[len-2]=0;//这个函数其实就帮忙把双引号压缩了一下，后面不恢复了，因为反正在调用这个函数的的后来被压缩了
	}
	//printf("str:%s\n",str);
	char my_ch[500];
	char my_ch1[500];
	int i;
	FILE *fd=fopen(name,"r+");
	if(fd==NULL)
	{
		p_error(-4);
		return 0;
	}
	while(feof(fd)==0)
	{
		for(i=0;i<k-1;i++)
			fscanf(fd,"%s",my_ch1);
		i++;
		fscanf(fd,"%s",my_ch);
		for( ;i<p;i++)
			fscanf(fd,"%s",my_ch1);
		if(strcmp(my_ch,str)==0)
			return 1;
		//printf("%s:\n",my_ch);
	}
	return 0;
}


/*返回一个表的主键是第几个字段,字段号从1开始,参数ta是指向那么表的表头*/
int find_key_num(table_info *ta)
{
	colum_info *head=ta->next;
	int n=1;
	while(head)
	{
		if(head->is_primary==1)
			return n;
		n++;
		head=head->next;
	}
	return 0;
}
