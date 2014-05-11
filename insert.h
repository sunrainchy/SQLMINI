#ifndef INSERT_H
#define INSERT_H
#include "type.h"
int insert_values(char *sql_command);
//负责插入数据的解析和执行
int is_appear(char *str,char *name,int k,int p);
/*主键化为字符串后是str，判断str在name(.db)的文件中有没有出现，K是主码中的第几个字段,p是总共有多少个字段*/
/*返回一个表的主键是第几个字段,参数ta是指向这个表的表头*/
int find_key_num(table_info *ta);

#endif
