#include <iostream>
#include "file.h"
#define PAGESIZE 1024
#define MEMORYSIZE 4*1024*PAGESIZE
#define page_set unsigned char
typedef unsigned char BYTE ;
typedef unsigned int v_address;
typedef unsigned int p_address;
//共32页，包括2页的PCB，1页中断向量表，1页系统设备表，5页页表，23页用户表
// #define PCBSIZE 2*1024//PCB表
// #define INTERRUPTSIZE 1024 //中断向量表
// #define SYSTEMSIZE 1024 //系统设备表
// #define FLAMESIZE 5*1024//页表
// #define USERSIZE 23*1024//用户代码段表

class pagetable{
private:
    int page_num;
    bool IsUsed;
    page_set virtual_page[PAGESIZE];
    page_set physical_page[PAGESIZE];
    pagetable *page;
public:
    void init();
    int read(int page_num,bool IsUsed,v_address address,pagetable *page);
    int write(int page_num,bool IsUsed,pagetable *page);
    int Findphyaddr(int page_num,bool IsUsed,v_address address,pagetable *page);
    int Free(int page_num,pagetable *page);
};





