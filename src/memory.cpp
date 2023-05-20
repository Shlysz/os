#include "memory.h"
using namespace std;
BYTES mem[memory_size];

int pagetableitem = 1024;//It means a pagetable has less than 1024 items ,we use array[] to store it 
int tlbitem = 32; //It means a TLB has less than 1024 items ,we use array[] to store it
int remain_mem = memory_size;// to calculate the memory that can be used
int tlb_id = 0;
int pagetable_id = 0;
//byte disk[disk_size];

struct v_address{
    int pagenum;
    int offset;
};

struct p_address{
    int framenum;
    int offfset;
};

Pagetable::Pagetable(int pid){
    this->pagetable_id = pid;
    /*
    get pid, set pagetable_id = pid;
    randomly create pageitems;
    */


}

TLB::TLB(int pid){
    this->tlb_id = pid;
    /*
    get pid,set tlb_id = pid;
    randomly create tlb items;
    */

}









int Query_memory(){
    return remain_mem;
}

