#include "memory.h"
#include "global.h"

//BYTES mem[memory_size];

int oskernel_size = 64 * pagesize;//It means the memory for os code,it cannot be used;
int shell_size = 64 * pagesize;
int schedule_size = 64 * pagesize;
int mem_available = memory_size;
mutex allocamtx;
//int pagetableitem = 1024;//It means a pagetable has less than 1024 items ,we use array[] to store it 
//int tlbitem = 32; //It means a TLB has less than 1024 items ,we use array[] to store it
// to calculate the memory that can be used
// int tlb_id = 0;
// int pagetable_id = 0;
//int disk_size = 512 * 1024 *1024;
int  findMaxIndex(int array[], int size);


void MMU::initMMU(){


    //Mlist *initkernel = new Mlist;
    Mlist *initkernel = new Mlist;
    while(!initkernel){
        Mlist *initkernel = new Mlist;
    }
    Mlist *initshell = new Mlist;
    while(!initshell){
        Mlist *initshell = new Mlist;
    }
    Mlist *initschedule = new Mlist;
    while(!initschedule){
        Mlist *initschedule = new Mlist;
    }
    

    //cout<<"apply node successfully";

    initkernel->mid = -1;
    initkernel->PT = initPagetable(-1);
    initkernel->TLb = initTLB(-1,initkernel->PT);
    initkernel->next = initshell;

    //cout<<"init kernel successfully"<<endl;
    initshell->mid = 0;
    initshell->PT = initPagetable(0);
    initshell->TLb = initTLB(0,initshell->PT);
    initshell->next = initschedule;

    //cout<<"init shell successfully"<<endl;
    initschedule->mid = 1;
    initschedule->PT = initPagetable(1);
    initschedule->TLb = initTLB(1,initschedule->PT);
    initschedule->next = nullptr;

    //cout<<"init sche successfully"<<endl;
    //Mmu->framearray = new int[total_frame]
    Mmu->total_frame -= 3 * 64;
    Mmu->total_memory -= 3 * 64 * 4096;

    //cout<<"init mmu start"<<endl;
    // Mmu->mlist->mid = -1;
    // Mmu->mlist->PT = initPagetable(-1);
    // Mmu->mlist->TLb = initTLB(-1,Mmu->mlist->PT);
    Mmu->mlist = initkernel;

    //std::memset(framearray, -2, sizeof(framearray));
    std::memset(framearray, -1, sizeof(framearray)/16);
    std::memset(framearray+64, 0, sizeof(framearray)/16);
    std::memset(framearray+128, 1, sizeof(framearray)/16);

    //cout<<"mmu inited"<<endl;
    mem_available -= oskernel_size - shell_size - schedule_size;
    free(initkernel);
    free(initshell);
    free(initschedule);
    //return;
}


void MMU::Memory_allocate(int pid){ 

    while(Mmu->mlist->next!=nullptr){
        if(Mmu->mlist->mid = -2){
            break;
        }
        Mmu->mlist = Mmu->mlist->next;
    }
    Mlist *newmmu;
    newmmu->mid = pid;
    newmmu->PT = initPagetable(pid);
    newmmu->TLb = initTLB(pid,newmmu->PT); 
    newmmu->next = nullptr;

    Mmu->mlist->next = newmmu;
    for(int i = pid * 64;i <=pid * 64 + 64;i++ ){
        Mmu->framearray[i] = pid;
    }
    Mmu->mlist->next = newmmu;
    Mmu-> total_frame -= 64;
    Mmu-> total_memory -= 64 * 4 * 1024;
    mem_available -= 64 * 4 * 1024;
    
    if(mem_available < 1){
        std::cout << "Error:No Enough Memory!" << std::endl;
    }
}

void MMU::lockedalloc(int pid){
      lock_guard<mutex> lock(allocamtx);
      Memory_allocate(pid);

}

void MMU::LRU_replace(int pid){

    /*
    LRU least recently used page replacement
    */
    Mlist *switcher = Mmu->mlist;
    while(switcher!=nullptr){
        if(switcher->mid == pid){
            int j = findMaxIndex(switcher->PT.last_used,sizeof(switcher->PT.last_used));
            if(j == -1)return;
            for(int k = 0 ;k < pagetablesize;k++){
                switcher->PT.last_used[k] ++;

            }
            for(int k = 0 ;k < tlbsize;k++){
                switcher->TLb.last_used[k]++;
            }
            int probaddr  = rand()/512 * 1024 *1024;
            for (int k=0;k<pagetablesize;k++){
                if (probaddr == switcher->PT.diskaddr[k])
                    break;
            }
            switcher->PT.diskaddr[j] = probaddr; 
            switcher->PT.last_used[j] = 0;
            for(int k = 0 ;k < tlbsize;k++){
                if(switcher->PT.page_num[k]==switcher->TLb.page_num[k]){
                    switcher->TLb.diskaddr[k]=switcher->PT.diskaddr[k];
                    switcher->TLb.last_used[k] = switcher->PT.last_used[k];
                }

            }
        }
    

    }
   free(switcher);

}


void MMU::Find_phyaddr(int pid){
    
    /*
    translate virtual address into physical address,and update TLB
    */
    int randompage = rand() % 64;
    int offset = rand() % 4096;
    uint32_t address = ((pid+1) * 64 + randompage) * 4096 + offset;
    struct v_address vitrualaddr;
    vitrualaddr.pagenum = address / 4096;
    vitrualaddr.offset = address % 4096;
    struct p_address phyaddress;
    phyaddress.offset = vitrualaddr.offset;
    //address overflow judgment
    if(address>4019304||address<0){
        cout << "Fail to addressing: Illegal Address"<<endl;
        return;
    }
    Mlist *search = Mmu->mlist;
    while(search!=nullptr){
        if((vitrualaddr.pagenum >= 64 * search->mid)&&(vitrualaddr.pagenum <= 64 * ( 1 + search->mid))){
            for(int i=0;i<tlbsize;i++){
                if((64 * search->mid + search->TLb.page_num[i]) == vitrualaddr.pagenum ){
                    cout << "Cache Hit!"<<endl;
                    phyaddress.framenum = search->TLb.page_phyaddr[i];
                    cout <<"Physical address is:"<<endl<<"framenum:" << phyaddress.framenum << "offset:"<<phyaddress.offset<<endl;
                    return ;
                }
            }
            for(int i=0;i<pagetablesize;i++){
                if((64 * search->mid + search->PT.page_num[i]) == vitrualaddr.pagenum )
                    phyaddress.framenum = search->PT.page_phyaddr[i];
                    cout <<"Physical address is:"<<endl<<"framenum:" << phyaddress.framenum << "offset:"<<phyaddress.offset<<endl;
                    search->PT.last_used[i] = 0;
                    int j = findMaxIndex(search->PT.last_used,sizeof(search->PT.last_used));
                    if(j == -1)return;
                    search->TLb.last_used[j] = 0;
                    search->TLb.page_phyaddr[j] = phyaddress.framenum;
                    search->TLb.page_num[j] = i;
                    search->TLb.diskaddr[j] = search->PT.diskaddr[j];
                    search->TLb.is_changed[j] = 0;
            }
        
        }
        search = search->next;
    }
    free(search);
}


void MMU::Memory_release(int pid){
    
    //Mlist *i = Mmu->mlist;
    while(Mmu->mlist!=nullptr){
        if(Mmu->mlist->mid == pid){
            Mmu->mlist->mid = -2;
            release_pagetable(Mmu->mlist->PT);
            release_tlb(Mmu->mlist->TLb);
            Mmu->mlist->next = Mmu->mlist->next->next;
            break;
        }
        Mmu->mlist = Mmu->mlist->next;
    }    
    Mmu->total_frame += 64;
    Mmu->total_memory += 64 * 4 * 1024;
    mem_available += 64 * 4 * 1024;
}

//init pagetable and TLB
Pagetable MMU::initPagetable(int pid){
    Pagetable pt; 
    pt.pagetable_id = pid;
    pt.page_num = new int[pagetablesize];
    pt.page_phyaddr = new int[pagetablesize];
    pt.is_changed = new int[pagetablesize];
    pt.diskaddr = new int[pagetablesize];
    pt.last_used = new int[pagetablesize];
    memset(pt.is_changed,0,pagetablesize);
    memset(pt.last_used,0,pagetablesize);
    for(int i = 0;i < pagetablesize;i++) {
        pt.page_phyaddr[i] = (1+pid) * 64 + i;
        pt.page_num[i] = i;
    }    
    set<int> randomSet;
    while (randomSet.size() < pagetablesize) {
        int randomNumber = rand()/512 * 1024 *1024;
        if (randomSet.find(randomNumber) == randomSet.end()) {
            randomSet.insert(randomNumber);
        }
    }
    int i = 0;
    for (int number : randomSet) {      
        pt.diskaddr[i] = number ;
        i++;
    }
    return pt;

}

TLB MMU::initTLB(int pid,Pagetable PT){
    TLB tlb;
    tlb.tlb_id = pid;
    tlb.page_num = new int[tlbsize];
    tlb.page_phyaddr = new int[tlbsize];
    tlb.is_changed = new int[tlbsize];
    tlb.diskaddr = new int[tlbsize];
    tlb.last_used = new int[tlbsize];
    memset(tlb.is_changed,0,tlbsize);
    memset(tlb.last_used,0,tlbsize);
    for(int i = 0;i < tlbsize;i++){
        tlb.page_num[i] = i;
        tlb.page_phyaddr[i] = (1+pid) * 64 + i;
        tlb.diskaddr[i] = PT.diskaddr[i];
    }
    return tlb;
}

void MMU::release_pagetable(Pagetable pt){
    pt.pagetable_id = -2;
    delete[] pt.page_num;
    delete[] pt.page_phyaddr;
    delete[] pt.is_changed;
    delete[] pt.diskaddr;
    delete[] pt.last_used;
}

void MMU::release_tlb(TLB tlb){
    tlb.tlb_id = -2;
    delete[] tlb.page_phyaddr;
    delete[] tlb.is_changed;
    delete[] tlb.diskaddr;
    delete[] tlb.last_used;
    delete[] tlb.page_num;
}

int findMaxIndex(int array[], int size) {
    if (size <= 0) {
        return -1;
    }
    int maxIndex = 0; 
    for (int i = 1; i < size; i++) {
        if (array[i] > array[maxIndex]) {
            maxIndex = i;
        }
    }
    return maxIndex;
}


void MMU::Query_memory(){
    float memo = float(total_memory/(1024.0*1024));
    std::cout << memo << "MB" << std::endl;    
    std::cout << "Total memory space is 4MB"<<endl;
}  



