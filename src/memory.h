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
    int TLB_num;//TLB���
    page_set pageNO[TLBSIZE];//�����ڴ��ַҳ�������ΪTLBSIZE��
    page_set frameNO[TLBSIZE];
	int is_used[TLBSIZE]={0};
    int USED_TIME[TLBSIZE]={0};
}TLB;

struct pagetable{
    int page_num;//ҳ��
    bool IsUsed;//��ҳ�Ƿ���
    bool IsReadonly;//��ҳ�Ƿ�ֻ��
    bool IsDirty ; //��ҳ�Ƿ��޸�
    page_set virtual_page[PAGESIZE];//�����ڴ��ַҳ�������Ϊpagesize��
    page_set physical_page[PAGESIZE];//�����ڴ��ַҳ
    pagetable *pagenext;
};

void init();
int read(int page_num,bool IsUsed,v_address address,pagetable *page);
int write(int page_num,bool IsUsed,bool IsReadonly,bool IsDirty, pagetable *page);
int Findphyaddr(int page_num,bool IsUsed,v_address address,pagetable *page);//���Ѱַ�㷨
int Free(int page_num,pagetable *page);
int TLB_renew(TLB tlb);


