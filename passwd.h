#ifndef PASSWD_H
#define PASSWD_H
#include <stdio.h>
#include <string.h>
/*接受字符串密码存放在str中*/
int get_passwd(char *str);
/*给密码加密*/
int get_encryption(char *str);
/*密码比较 真确返回0，错误返回-1*/
int passwd_compare(char *str);
/*密码检测0 success -1 failed*/
int passwd_check();
/*修改密码,成功返回0失败返回-1*/
int change_passwd();
#endif
