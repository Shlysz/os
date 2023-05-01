#include <iostream>
#include "file.h"
#define PAGESIZE 1024
#define MEMORYSIZE 4*1024*PAGESIZE
#define page_set unsigned int
typedef unsigned char BYTE ;
typedef unsigned int v_address;
typedef unsigned int p_address;
//��32ҳ������2ҳ��PCB��1ҳ�ж�������1ҳϵͳ�豸��5ҳҳ��23ҳ�û���
// #define PCBSIZE 2*1024//PCB��
// #define INTERRUPTSIZE 1024 //�ж�������
// #define SYSTEMSIZE 1024 //ϵͳ�豸��
// #define FLAMESIZE 5*1024//ҳ��
// #define USERSIZE 23*1024//�û�����α�


class pagetable{
private:
    int page_num;//ҳ��
    bool IsUsed;//��ҳ�Ƿ���
    bool IsReadonly;//��ҳ�Ƿ�ֻ��
    bool IsDirty ; //��ҳ�Ƿ��޸�
    page_set virtual_page[PAGESIZE];//�����ڴ��ַҳ�������Ϊpagetable��
    page_set physical_page[PAGESIZE];//�����ڴ��ַҳ
    pagetable *pagenext;
public:
    void init();
    int read(int page_num,bool IsUsed,v_address address,pagetable *page);
    int write(int page_num,bool IsUsed,bool IsReadonly,bool IsDirty, pagetable *page);
    int Findphyaddr(int page_num,bool IsUsed,v_address address,pagetable *page);//���Ѱַ�㷨
    int Free(int page_num,pagetable *page);
};




