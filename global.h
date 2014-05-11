/* 
 *  cat ans | sort | tr '\n' ',' | sed s/,/\",\"/g 
 *  就是用上面这个脚本实现从散乱的一行一行的关键转换成需要的格式
 *  还要注意一点就是这里面的关键字是排序了的(从脚本可以看出)，所以一会在判断一个串是不是关键字的时候就可以用二分法了
 */
#ifndef GLOBAL_H
#define GLOBAL_H
#include "type.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int is_key_word(char *str);
/****判断str这个串是不是关键字中的某一个****/
int init();
/****一切预处理相关的工作全部会在这里完成****/
int distory();
/*一切收尾相关的工作在这里完成*/
int find_key_word(char **str,int L,int R,char *ch);
/**** 在字符串数组str中的L-R区间查找串ch 这里用的是二分法****/
int find_function(const char *name);
/*
 *
 * 解析关键子name的功能，返回对应九个动词的功能代号*
 * 1 create 2 select 3 update 4 insert......
 */
int find_type(char *str);
/*
 *检测str代表的是那种数据类型，并返回对应代码
 *表示这个字段的字节类型1 int 2 char 3 double 4 float 
 *如果是primary 返回9
 *如果是not 返回10
 *返回0表示不是任何类型
 */
/*用下面两的全局变量加载table 常驻内存*/
int load_table();
/*描述表信息，提供describe tables的功能*/
int describe_table(char *name);
/*实现show_table的功能*/
int show_table();
/*用my_database加载已经存在的数据库*/
int load_database();
/*实现show_databases功能*/
int show_databases();
/*实现use database功能*/
int use_database(char *name);
/*判断name是不是在系统my_database注册表中,在返回1，不在返回0*/
int is_database(char *name);
/*删除一个数据库，就是连同文件夹及其内容一并删除*/
int drop_database(char *name);
/*删除一个表，连同注册表里面的表和物理文件一同删除*/
int drop_table(char *name);
#ifndef MY_TABLE 
#define MY_TABLE
table_info *my_table=NULL;//定义的一个全局的table_info 节点，供创建表时候使用，注意用完要整体释放
#endif
#ifndef TABLE_LIST
#define TABLE_LIST
table_info *table_list;//这个是存放所有表的信息的一个链表的头部，每次在数据库启动的时候加载，常驻内存
int is_choose_database;//这个东西是来标志是否选择某个数据库，0没有，1 有
database *my_database;
#endif
#endif
