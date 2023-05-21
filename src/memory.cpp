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

    this->page_phyaddr = new int[pagetablesize];
    for(int i = 0;i < pagetablesize;i++){
        page_phyaddr[i] = pid * 64 + i;
        if(page_phyaddr[i] >1023||page_phyaddr[i]<0){
            cout << "Error:Out of paging range";
            //INTERRUPT:STORTAGE OF PAGES??
        } 
    }
}

void Pagetable::release_pagetable(Pagetable pt){
    delete[] pt.page_phyaddr;
}


TLB::TLB(int pid){
    this->tlb_id = pid;
    this->TLBitem = new int[TLBsize];
    for(int i = 0;i<TLBsize;i++){
        TLBitem[i] = pid * 64 + i;
    }
}
void TLB::release_tlb(TLB tlb){
    delete[] tlb.TLBitem;
}


MMU::MMU(){
    this->framearray = new int[total_frame] ;
    std::memset(framearray, 0, sizeof(framearray));

    //cout<<"MMU START"<<endl;
}

void MMU::Memory_allocate(int pid){ 




}
void MMU::Memory_release(int pid){



}

void Page_replace(int pid){
    /*
    opt 最佳页面置换算法
    */


}


void Find_paddress(int pid,TLB tlb,Pagetable pt){




}

void MMU::Query_memory(){
    std::cout << this->total_memory << "B" << std::endl;    
}






