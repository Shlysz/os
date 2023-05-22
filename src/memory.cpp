#include "memory.h"
#include "global.h"
using namespace std;
BYTES mem[memory_size];

int pagetableitem = 1024;//It means a pagetable has less than 1024 items ,we use array[] to store it 
int tlbitem = 32; //It means a TLB has less than 1024 items ,we use array[] to store it
int mem_available = memory_size;// to calculate the memory that can be used
int tlb_id = 0;
int pagetable_id = 0;
MMU::Mlist *mlist = nullptr;
//byte disk[disk_size];

struct v_address{
    int pagenum;
    int offset;
};
struct p_address{
    int framenum;
    int offfset;
};

Pagetable Pagetable::initPagetable(int pid){
    Pagetable pt; 
    pt.pagetable_id = pid;
    pt.page_phyaddr = new int[pagetablesize];
    pt.is_changed = new int[pagetablesize];
    pt.is_inmemory = new int[pagetablesize];
    pt.last_used = new int[pagetablesize];
    memset(is_changed,0,pagetablesize);
    memset(is_inmemory,0,pagetablesize);
    memset(last_used,0,pagetablesize);
    for(int i = 0;i < pagetablesize;i++) page_phyaddr[i] = pid * 64 + i;
    return pt;
}

void Pagetable::release_pagetable(Pagetable pt){
    delete[] pt.page_phyaddr;
    delete[] pt.is_changed;
    delete[] pt.is_inmemory;
    delete[] pt.last_used;
}


TLB TLB::initTLB(int pid){
    TLB tlb;
    tlb.tlb_id = pid;
    tlb.page_phyaddr = new int[tlbsize];
    tlb.is_changed = new int[tlbsize];
    tlb.is_inmemory = new int[tlbsize];
    tlb.last_used = new int[tlbsize];
    memset(is_changed,0,tlbsize);
    memset(is_inmemory,0,tlbsize);
    memset(last_used,0,tlbsize);
    for(int i = 0;i < tlbsize;i++){
        page_phyaddr[i] = pid * 64 + i;
    }
    return tlb;
}
void TLB::release_tlb(TLB tlb){
    delete[] tlb.page_phyaddr;
    delete[] tlb.is_changed;
    delete[] tlb.is_inmemory;
    delete[] tlb.last_used;
}




MMU::MMU(){
    this->framearray = new int[total_frame] ;
    std::memset(framearray, 0, sizeof(framearray));
    //cout<<"MMU START"<<endl;
}

void MMU::Memory_allocate(int pid){ 
    mlist->mid = pid;
    mlist->PT = initpt(pid);
    mlist->TLb = initlb(pid); 
    mem_available -= 64 * 1024;
    if(mem_available < 1){
        std::cout << "Error:No Enough Memory!" << std::endl;
    }
    //MMU:PT(pid);

}
void MMU::Memory_release(int pid){

    
    while(mlist){
        



    }


}

void MMU::LRU_replace(){
    /*
    LRU least recently used
    */


}


void Find_phyaddr(int pid,TLB tlb,Pagetable pt){




}

void MMU::Query_memory(){
    std::cout << this->total_memory << "B" << std::endl;    
}






