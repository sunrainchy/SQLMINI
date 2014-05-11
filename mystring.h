#ifndef MYSTRING_H
#define MYSTRING_H
#include <stdio.h>
#include <string.h>
int get_string(char *first,char del);
/****函数接受以del结尾的一串字符并返回实际接收的字符数目****/
int my_lower(char *first);
/****把所有字母转换成为小写****/
int compress(char *first);
/*
 *
 *把所有回车 tab还有多余的空格压缩变换还有实现语句最后的空字符的删除 
 *现在感觉这个函数有点强大了，去除所有不必要的符合is_space的，还有去除
 *符合is_marry的且前后符合is_space 的所有不必要的类空格符
 *
 */
int is_space(char ch);
/****判断字符是不是'\n' '\t' ' '****/
int my_split(const char *src,char *dec,char del,int k);
/****分离一个以del为分割符的第k个字段存放在dec中 ****/
int my_enhance_split(const char *src,char *dec,char del,int k);
/*
 *这个函数是my_split的加强版本，前面用my_split都没出现问题，现在出现问题了
 */
int my_upper(char *first);
/****能转换成大写字母的小写字母就转换成大写字母****/
int change_case(char * first,int b);
/****改变字母大小写，b==0小写转换成大写，b==1大写转换成小写，上面两个函数都是调用这个函数实现的****/
int check_syntax_for_match(const char *ch);
/*这个函数用来检查语法错误，检测点为括号，引号等是否匹配等，传入参数为要检查的语句串
 *还会检查sql语句中是否有非法字符（中文字符）
 *返回值为0表示成功
 *返回值为50表示失败 p_error定义为语法错误
 */
int is_name(char *name);
/*
 *判断一个给定的字符串能否为名称，也就是只能由数字，字母和下划线组成
 *成功返回1，失败返回0
 */
int my_split_end(char *src,char *dec,char del,int k,int flag);
/*flag 为0 表示要后面的部分，1表示前面的部分,默认为0
 *这个函数实现找到第k个del位置之后的字符串全部放在dec中，参数与my_split相同
 */
int reverse(char *str);
/*反转字符串*/
int my_split_end_reverse(char *src,char *dec,char del,int k,int flag);
/*
 *前面的函数是从前面开始向后找，现在是从后面向前找
 *当flag=0的时候，从后面第k个del开始把前面的全部复制到dec中
 *当flag=1的时候，从后面开始第k个del之后的全部复制到dec中
 */
int check_is_type(char *str,int t);
/*
 *检查str是否符合t类型，t代表什么请参照global.c的find_type()函数,成功返回0，失败返回1
 */
int is_have_ch(char *str,char ch);
/*
 *检查str中是否存在ch字符，如果存在返回字符的位置，从0开始，如果不存在返回-1
 */
#endif
