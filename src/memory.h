#include <iostream>
#include "file.h"
#define PAGESIZE 1024
#define TLBSIZE 64 
#define MEMORYSIZE 4*1024*PAGESIZE
#define page_set unsigned int
typedef unsigned char BYTE ;
typedef unsigned int v_address;
typedef unsigned int p_address;


typedef struct TLBTHING{
    int TLB_num;//TLB序号
    page_set pageNO[TLBSIZE];//虚拟内存地址页，最大储量为TLBSIZE；
    page_set frameNO[TLBSIZE];
	int is_used[TLBSIZE]={0};
    int USED_TIME[TLBSIZE]={0};
}TLB;

struct pagetable{
    int page_num;//页号
    bool IsUsed;//该页是否被用
    bool IsReadonly;//该页是否只读
    bool IsDirty ; //该页是否被修改
    page_set virtual_page[PAGESIZE];//虚拟内存地址页，最大储量为pagesize；
    page_set physical_page[PAGESIZE];//物理内存地址页
    pagetable *pagenext;
};

int init();//ldy:修改返回值类型
int read(int page_num,bool IsUsed,v_address address,pagetable *page);
int write(int page_num,bool IsUsed,bool IsReadonly,bool IsDirty, pagetable *page);
int Findphyaddr(int page_num,bool IsUsed,v_address address,pagetable *page);//设计寻址算法
int Free(int page_num,pagetable *page);
int TLB_renew(TLB tlb);


