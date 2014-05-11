#include "errno.h"
int p_error(int err_no)
{
	switch (err_no)
	{
		case 0:printf("running success                 \n");   break;
		case 1:printf("不能够识别                      \n");   break;
		case -1:printf("failed running                 \n");   break; 
		case 2:printf("文件创建失败                    \n");   break;
		case 3:printf("找不到                          \n");   break;
		case 4:printf("文件写入错误                    \n");   break;       
		case -4:printf("文件打开错误                   \n");   break;
		case 19:printf("文件夹创建失败                 \n");   break;
		case 20:printf("文件夹已存在    该数据库已存在 \n");   break;  
		case 50:printf("syntax errno                   \n");   break;
		case -100:printf("密码修改失败                 \n");   break;
		case 100:printf("密码修改成功                  \n");   break;
		case 150:printf("语法错误，执行失败            \n");   break;
		case 151:printf("您未选择任何数据库            \n");   break;
		case 152:printf("主键不能为空                  \n");   break;
		case 153:printf("有列不允许为空值              \n");   break;
		case 154:printf("插入的值和类型不匹配          \n");   break;
		case 155:printf("插入的值匹配混乱              \n");   break;
		case 156:printf("主键重复，插入失败            \n");   break;
		default:
				 printf("未定义的错误类型                       \n"); return 2;    
	}
	return 0;
}
