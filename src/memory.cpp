#include "memory.h"
#include "global.h"

//BYTES mem[memory_size];

int oskernel_size = 64 * pagesize;//It means the memory for os code,it cannot be used;
int shell_size = 64 * pagesize;
int mem_available = memory_size;
int max_process = 14;
int now_process = 0;
mutex allocamtx;
int  findMaxIndex(int array[], int size);
void printQueue(std::queue<int> myQueue);
void removeprocessFromQueue(std::queue<int>& myQueue, int target);

void MMU::initMMU(){

    //cout << Mmu->total_frame<<endl;
    //Mlist *initkernel = new Mlist;
    // Mlist *initkernel = new Mlist ();
    // while(!initkernel){
    //     Mlist *initkernel = new Mlist();
    // }
    // Mlist *initshell = new Mlist();
    // while(!initshell){
    //     Mlist *initshell = new Mlist();
    // }
    // initkernel->mid = 0;
    // initkernel->PT = initPagetable(0);
    // initkernel->TLb = initTLB(0,initkernel->PT);
    // initkernel->next = nullptr;

    // initshell->mid = 1;
    // initshell->PT = initPagetable(1);
    // initshell->TLb = initTLB(1,initshell->PT);
    // initshell->next = nullptr;


    // //Mlist *neww = Mmu->mlist;
    // Mmu->total_frame -= 2 * 64;
    // Mmu->total_memory -= 2 * 64 * 4096;
    // initkernel->next = Mmu->mlist;
    // Mmu->mlist = initkernel;
    // initkernel->next = initshell;

    std::memset(Mmu->framearray, 0, 64*sizeof(int));
    int *ptr = &Mmu->framearray[63];
    std::memset(ptr, 1, 64*sizeof(int));
    //free(initkernel);
    //free(initshell);
    //free(neww);

}
void MMU::seeprocess(){
    cout << "Now the processes in memory are:"<<endl;
    
    printQueue(Mmu->MidQueue);
}

void printQueue(std::queue<int> myQueue) {
    while (!myQueue.empty()) {
        int frontElement = myQueue.front();
        std::cout << frontElement << " ";
        myQueue.pop();
    }
    std::cout << std::endl;
}


void MMU::lockedalloc(int pid){
      lock_guard<mutex> lock(allocamtx);
      Memory_allocate(pid,max_process);

}

void MMU::Memory_allocate(int pid,int &max_process){ 

    if(max_process <= 0){
        std::cout << "Error:No Enough Memory!" << std::endl;
        return;
    }
    max_process--;
    Mmu->MidQueue.push(pid);
    cout << "max processnum is:"<<endl<<max_process<<endl;
    //std::memset(Mmu->framearray+64*pid, 1, 64*sizeof(int));
    Mmu->total_frame -= 64;
    Mmu->total_memory -= 64 * 4 * 1024;
    mem_available -= 64 * 4 * 1024;
    int *ptr = &Mmu->framearray[64*pid-1];
    memset(ptr,pid,64*sizeof(int));
    cout << "Memory of this process is allocated!"<<endl;
    seeprocess();
    // Mlist *newmmu = new Mlist();
    // while(!newmmu){
    //         Mlist *mewmmu = new Mlist();
    // }
    // newmmu->mid = pid;
    // newmmu->PT = initPagetable(pid);
    // newmmu->TLb = initTLB(pid,newmmu->PT); 
    // newmmu->next = nullptr;
    // Mlist *addnode = Mmu->mlist;
    // while(addnode->next!=nullptr){
    //     if(addnode->mid == -1){
    //         break;
    //     }
    //     cout << addnode->mid <<endl;
    //     addnode = addnode->next;
    // }
    // addnode->next = newmmu;
    // Mmu->mlist = addnode;
    // for(int i = pid * 64;i <=pid * 64 + 64;i++ ){
    //     Mmu->framearray[i] = pid;
    // }
    // cout << "max processnum is:"<<endl<<max_process<<endl;
    // //Mmu->mlist->next = newmmu;
    
    // cout << "max processnum is:"<<endl<<max_process<<endl;
    // //cout << i->mid << endl;
    // free(newmmu);

}

void MMU::Report_realtime(){

    Mlist *p = Mmu->mlist;
    while(p!=nullptr){
        cout << p->mid << endl;
        p = p->next;
    }

}
void MMU::Memory_release(int pid){
    removeprocessFromQueue(Mmu->MidQueue,pid);
    int *ptr = &Mmu->framearray[pid*64-1];
    memset(ptr,-1,64*sizeof(int));
        
    cout <<endl << "release memory successfully"<<endl;    
    Mmu->total_frame += 64;
    Mmu->total_memory += 64 * 4 * 1024;
    max_process++;
    cout << "Memory of this process is released!"<<endl;
    seeprocess();
    // while(Mmu->mlist->next!=nullptr)
    // {
    //     Mmu->mlist = Mmu->mlist->next;
    //     cout << Mmu->mlist->mid <<endl;
    // }
    
    // if(Mmu->mlist->mid == pid){
    //         Mmu->mlist->mid = -1;
    //         release_pagetable(Mmu->mlist->PT);
    //         release_tlb(Mmu->mlist->TLb);
    //         //Mmu->mlist = Mmu->mlist->next;
    //        // break;
    //     }
    //     //Mmu->mlist = Mmu->mlist->next;
   
}
void removeprocessFromQueue(std::queue<int>& myQueue, int target) {
    std::queue<int> tempQueue;  // 创建一个临时队列

    while (!myQueue.empty()) {
        int frontElement = myQueue.front();
        myQueue.pop();

        if (frontElement != target) {
            tempQueue.push(frontElement);
        }
    }

    // 将临时队列中的元素重新放回原队列
    while (!tempQueue.empty()) {
        int frontElement = tempQueue.front();
        tempQueue.pop();
        myQueue.push(frontElement);
    }
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
    /*
    randomly allocate disk address
    */
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
        tlb.page_phyaddr[i] = PT.page_phyaddr[i];
        tlb.diskaddr[i] = PT.diskaddr[i];
    }
    return tlb;
}

void MMU::release_pagetable(Pagetable pt){
    pt.pagetable_id = -1;
    delete[] pt.page_num;
    delete[] pt.page_phyaddr;
    delete[] pt.is_changed;
    delete[] pt.diskaddr;
    delete[] pt.last_used;
}

void MMU::release_tlb(TLB tlb){
    tlb.tlb_id = -1;
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
    cout << "Size of free memory is "<< memo << "MB" << endl;    
    cout << "Total memory space is 4MB"<<endl;
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


// void MMU::Find_phyaddr(int pid){
    
//     /*
//     translate virtual address into physical address,and update TLB
//     */
//     int randompage = rand() % 64;
//     int offset = rand() % 4096;
//     uint32_t address = ((pid+1) * 64 + randompage) * 4096 + offset;
//     struct v_address vitrualaddr;
//     vitrualaddr.pagenum = address / 4096;
//     vitrualaddr.offset = address % 4096;
//     struct p_address phyaddress;
//     phyaddress.offset = vitrualaddr.offset;
//     //address overflow judgment
//     if(address>4019304||address<0){
//         cout << "Fail to addressing: Illegal Address"<<endl;
//         return;
//     }
//     Mlist *search = Mmu->mlist;
//     while(search!=nullptr){
//         if((vitrualaddr.pagenum >= 64 * search->mid)&&(vitrualaddr.pagenum <= 64 * ( 1 + search->mid))){
//             for(int i=0;i<tlbsize;i++){
//                 if((64 * search->mid + search->TLb.page_num[i]) == vitrualaddr.pagenum ){
//                     cout << "Cache Hit!"<<endl;
//                     phyaddress.framenum = search->TLb.page_phyaddr[i];
//                     cout <<"Physical address is:"<<endl<<"framenum:" << phyaddress.framenum << "offset:"<<phyaddress.offset<<endl;
//                     return ;
//                 }
//             }
//             for(int i=0;i<pagetablesize;i++){
//                 if((64 * search->mid + search->PT.page_num[i]) == vitrualaddr.pagenum )
//                     phyaddress.framenum = search->PT.page_phyaddr[i];
//                     cout <<"Physical address is:"<<endl<<"framenum:" << phyaddress.framenum << "offset:"<<phyaddress.offset<<endl;
//                     search->PT.last_used[i] = 0;
//                     int j = findMaxIndex(search->PT.last_used,sizeof(search->PT.last_used));
//                     if(j == -1)return;
//                     search->TLb.last_used[j] = 0;
//                     search->TLb.page_phyaddr[j] = phyaddress.framenum;
//                     search->TLb.page_num[j] = i;
//                     search->TLb.diskaddr[j] = search->PT.diskaddr[j];
//                     search->TLb.is_changed[j] = 0;
//             }
        
//         }
//         search = search->next;
//     }
//     free(search);
// }



