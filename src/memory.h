#include <cstring>
#include <iostream>
/*
unsigned int ---> virtual address,physical address 32位
memory size ---> 4MB
use 0/1 to simulate whether a memory unit is used
TLB item num:32
pagetable item num: 1024
for each process ,init a pagetable and a TLB,and when a process is terminated, release it 
*/

#define unsigned char BYTES
#define TLBsize 32
#define pagetablesize 1024
#define memory_size 4 * 1024 * 1024
//#define disk_size 512 * 1024 * 1024

/*
pagetable_id = pid
when a process is created, init a pagetable and randomly create a pagetableitem[] and simulate addressing from v to p
when the process is terminated, release it   
*/
class Pagetable {
   public:
    int pagetable_id;  
    bool isallocated;
    int pagetableitem[pagetablesize] = {0};
    void init_pagetable();  // todo:当进程新加入时，初始化一个页表给他
};

/*
tlb_id = pid
when a process is created, init a tlb and randomly create a TLBitem[] and simulate addressing from v to p
when the process is terminated, release it   
*/
class TranslookasideBuffer {
    public:
    int tlb_id;
    int TLBitem[TLBsize];





};

/*
Frame_num range: 0~1023
size of each Frame : 4 * 1024
if a Frame is allocated, isused = 1,in the meanwhile the memory need - 4*1024
*/
class Frameitem{
    public:
    int frame_num;//0-1023
    bool isused;



};



typedef TranslookasideBuffer TLB;