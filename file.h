#ifndef FILE_H
#define FILE_H
/*创建一个名为name的文件成功返回0，不成功返回2 */
int create_file(char *name);
/*将buf里面的内容写如fd代表的文件中成功返回0，失败返回4*/
int write_file(char *buf,FILE *fd);
#endif
