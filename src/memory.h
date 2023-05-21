#pragma once
#include <cstring>
#include <iostream>
//#include "process.h"
/*
unsigned int ---> virtual address,physical address 32bits,
while the page/frame number is 20 bits and the offset is 12 bits
memory size ---> 4MB
use 0/1 to simulate whether a memory unit is used
TLB item num:32
pagetable item num: 1024
for each process ,init a pagetable and a TLB,and when a process is terminated, release it 
*/
#define TLBsize 8 // 8B
#define pagetablesize 64 // 64B a pagetable has 64 pageitem
#define memory_size 4 * 1024 * 1024 //1 = 1B ,total:4MB 
#define pagesize = 4 * 1024
#define framesize = 4 * 1024

typedef  unsigned char BYTES;
/*
pagetable_id = pid
using Paging-request 
when a process is created, init a pagetable and randomly create a pagetableitem[] and simulate addressing from v to p
when the process is terminated, release it   
*/
class Pagetable{
    public:
    int pagetable_id;  
    int * page_phyaddr = nullptr;
    int * last_used = nullptr;
    int * is_changed = nullptr;
    
    Pagetable(int pid);  //Init a pagetable when a process is created
    void release_pagetable(Pagetable pt);//Release it when the process is terminated
};

/*
TLB = TranslookasideBuffer
tlb_id = pid
when a process is created, init a tlb and randomly create a TLBitem[] and simulate addressing from v to p
when the process is terminated, release it   
*/
class TLB{
    public:
    int tlb_id;
    int *TLBitem = nullptr;

    TLB(int pid);//Init a TLB when a process is created
    void release_tlb(TLB tlb);//Release it when the process is terminated
};

/*
Page_num range: 0~1023
size of each page : 4 * 1024
if a page is allocated, isused = 1,in the meanwhile the virtual memory need - 4*1024
a page need match a frame
*/
// class Pageitem{
//     public:
//     int page_num;//0-1023
//     int last_used;//to calculate opt algo
//     void page_init(int remaining_memory);
//     void page_release(int remaining_memory); 
// };

/*
Frame_num range: 0~1023
size of each Frame : 4 * 1024
if a Frame is allocated, isused = 1,in the meanwhile the memory need - 4*1024
*/
// class Frameitem{
//     public:
//     int frame_num;//0-1023
//     bool isused;
//     void frame_init(int remaining_memory);
//     void frame_release(int remaining_memory); 
// };
/*
MMU: memory manage unit 
Functions: allocate memory for process,release used memory,transfer vad to pad,return memory that can be used 
*/

class MMU{

    public:
    int total_frame = 1024;
    int total_memory = memory_size;
    int *framearray = nullptr;
    


    MMU();

    //void IO_in_memory();
    void Memory_allocate(int pid);
    void Memory_release(int pid);
    void OPT_replace();
    void Find_paddress(int pid,TLB tlb,Pagetable pt);
    void Query_memory();
    
};


