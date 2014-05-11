#ifndef TYPE_H
#define TYPE_H
typedef struct colum_info
{
char colum_name[50];//存放列名称
int type;//表示这个字段的字节类型1 int 2 char 3 double 4 float ....
int length;//如果前面的是2 也就是char类型的，那么这个就表示char类型的长度
int is_primary;//是否为主键，1表示这个字段是主键，0表示不是
int is_foreign;//是否为外键
int is_null;//允许为空不 1允许，0 不允许
int default_num;//默认值 前面类型为char 那么默认值为空，默认值这一项暂时还不支持
struct colum_info *next;
}colum_info;
typedef struct table_info
{
int num;//表示字段的数目
char table_name[50];//存放表的名称
struct colum_info *tail;//存放链表尾部节点指针
struct colum_info *next;//存放下一个列信息指针
struct table_info *next_table;//
}table_info;
typedef struct database
{
char name[50];
struct database *next;
}database;
typedef struct restrict_colum//表示一个表每个字段约束的结构
{
int is_res;//是否有约束，没有约束那么就是0，有约束1 代表=,   2 代表 > ,3代表 <,   4 代表>= ,5 代表 <=
int num;//表示有约束的那个字段号-1,表示没有任何字段 
int or_and;//判断这个是or成立还是and成立
/*如果是or成立，那么在所有or中成立一个就OK，如果是and那么一定是要成立
 * 1表示or，0表示and，我们目前这样支持，
 *or的一定在一起成为一个or整体，and一定独立开来，也就是where条件子句一定不能有括号表示的逻辑条件
 */
int type;//约束字段型，int float等
char demand[100];//如果有约束，那么约束的条件,也就是该字符串大小的比较，和这个比
struct restrict_colum *next;
}restrict_colum;
#endif
