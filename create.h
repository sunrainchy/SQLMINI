#ifndef CREATE_H
#define CREATE_H
/*创建一个名字为name的数据库，也就是一个文件夹*/
int create_database(const char *name);
/*执行和发现 create 语句错误*/
int run_create(char *sql_command);
/*真正执行create_table*/
int create_table(char *sql_command,table_info **my_table);
/*检查为name的表是否存在存在返回1 不存在返回0*/
int check_table_name(char *name);
/*返回表名为name的表头*/
table_info * find_table_head(char *name);
/*返回name是表的第几个字段，从0开始，没有返回-1,并把该字段的类型放在type里面*/
int find_num_colum_name(table_info *ta,char *name,int *type);
#endif
