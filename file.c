#include <stdio.h>
#include <string.h>
/*文件的创建*/
int create_file(char *name)
{
	FILE *fd;
	fd=fopen(name,"w+");
	if(fd==NULL)
		return 2;
	else
		fclose(fd);
	return 0;
}
int write_file(char *buf,FILE *fd)
{
	int ans;
	if(1!=fwrite(buf,strlen(buf),1,fd))
		return 4;
	return 0;
}
