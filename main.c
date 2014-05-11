//#include "errno.h"
//#include "mystring.h"
//#include "global.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "passwd.h"
#include "insert.h"
static int stat=0;//记录函数执行返回状态
int main()
{
	clock_t start,finish;
	int k=0,i;
	char sql_command[2000];
	char str[500];
	char sp[500];
	init();
	if(passwd_check()==-1)
		return 0;
	while(1)
	{
		printf("SqlMiNi>");
		get_string(sql_command,';');
		getchar();
		start=clock();
		//printf("new:%s\n",sql_command);
		my_lower(sql_command);
		compress(sql_command);
		my_lower(sql_command);
		printf("AFTER PARSE:%s;\n",sql_command);
		/*用户输入空语句提示 也就是输入的语句除了; 之外其他的都是符合is_space()的*/
		if(strlen(sql_command)==0)
		{
			printf("没有任何语句可以执行\n");
			continue;
		}
		p_error(stat=check_syntax_for_match(sql_command));
		if(stat!=0)//语法出问题
			continue;
		if(strcmp(sql_command,"exit")==0)
		{
			distory();
			return 0;
		}
		if(strcmp(sql_command,"show tables")==0)
		{
			show_table();
			//printf("over\n");
			finish=clock();
			printf("run time :%f seconds\n",(double)(finish-start)/CLOCKS_PER_SEC);
			continue;
		}
		if(strcmp(sql_command,"show databases")==0)
		{
			show_databases();
			finish=clock();
			printf("run time :%f seconds\n",(double)(finish-start)/CLOCKS_PER_SEC);
			continue;
		}
		if(strcmp(sql_command,"help")==0)
		{
			printf("详细请参见user.doc and my_std.doc 或者联系15827262625 获得技术支持\n");
			continue;
		}
		if(strcmp(sql_command,"passwd")==0)
		{
			p_error(stat=change_passwd());
			if(stat!=0)
				continue;
		}
		/*main function*/
		my_split(sql_command,sp,' ',1);
		switch(find_function(sp))
		{
			case 1:printf("执行创建功能\n");p_error(run_create(sql_command));break;
			case 5:
				   my_split(sql_command,sp,' ',3);
				   if(strlen(sp)!=0)
				   { p_error(150);break;}
				   my_split(sql_command,sp,' ',2); 
				   if(is_name(sp)==0)
				   {p_error(150);break;}
				   describe_table(sp);break;
			case 6:
				   my_split(sql_command,sp,' ',3);
				   if(strlen(sp)!=0)
				   { p_error(150);break;}
				   my_split(sql_command,sp,' ',2);
				   if(is_name(sp)==0)
				   {p_error(150);break;}
				   use_database(sp);
				   break;
			case 7:
				   my_split(sql_command,sp,' ',2);
				   if(strcmp(sp,"table")==0)
				   {
					   my_split(sql_command,sp,' ',4);
					   if(strlen(sp)!=0)
					   {p_error(150); break;};
					   my_split(sql_command,sp,' ',3);
					   if(is_name(sp)==0)
					   {p_error(150); break;};
					   drop_table(sp);
					   printf("drop tables\n");
					   break;
				   }
				   my_split(sql_command,sp,' ',3);
				   if(strlen(sp)!=0)
				   { p_error(150);break;}
				   my_split(sql_command,sp,' ',2);
				   if(is_name(sp)==0)
				   {p_error(150);break;}
				   drop_database(sp);
				   break;
			case 4://insert
				   insert_values(sql_command);
				   break;
			case 2:
				   run_select(sql_command);
				   break;
			default:printf("不能识别的语句，或此功能暂不支持\n");
		}
		finish=clock();
		printf("run time :%f seconds\n",(double)(finish-start)/CLOCKS_PER_SEC);
		memset(sql_command,0,sizeof(sql_command));
	}
	return 0;
}
