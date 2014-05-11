#ifndef SELECT_H
#define SELECT_H
int run_select(char *sql_command);
/*这个函数总体负责select语句的解析和执行*/
int parse_where(char *sql_command,restrict_colum *head,table_info *ta);
/*
 *这个函数负责解析where 部分，解析出来每个字段的要求放在head这个链表中，sql_comand 是存放sql语句的where和后面的部分，参数ta是对应于哪个表
 *函数负责将有约束的列串联成一个链表，方便select等使用
 */
int release_res(restrict_colum *head);
/*释放类型为restrict_colum的链表资源*/
int parse_one_res(int or_and,restrict_colum *res,char *str,table_info *ta);
/*这个函数负责解析一段约束的具体要求，解析出来的结果存放在res中，or_and 参数提供
是is还是or的参数，1 or 0 and
 *res就是解析出来的结果存放处，str就是要解析的那么约束,成功返回0，不成功返回非0
 */
int is_row_ok(char row_colum[][100],restrict_colum *head);
/*此函数检测行是否满足链表约束head,符合返回0，否则返回非0*/


/* 
 *这个函数是检测一行中某个约束是否满足，满足返回1，否则返回非0
 *参数说明 first是第一个 比如说约束是 a > b int 型，那么first是a，second是b type是int 1，logical是 > 2
 */
int is_one_colum_ok(char *first,char *second,int type,int logical);


#endif

