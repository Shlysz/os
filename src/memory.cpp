#include "memory.h"
#include "global.h"
using namespace std;

int oskernel_size = 64 * pagesize;//It means the memory for os code,it cannot be used;
int shell_size = 64 * pagesize;

int mem_available = memory_size;
int max_process = 14;
int now_process = 0;
mutex allocamtx;
mutex releamtx;
int  findMaxIndex(int array[], int size);
void printQueue(std::queue<int> myQueue);
void removeprocessFromQueue(std::queue<int>& myQueue, int target);
int minvalue(int value1, int value2) {
    return (value1 < value2) ? value1 : value2;
}
int maxindex(const int arr[]) {
    int maxIndex = 0;  // 假设第一个元素是最大值
    for (int i = 1; i < 4; i++) {
        if (arr[i] > arr[maxIndex]) {
            maxIndex = i;  // 更新最大值的下标
        }
    }
    return maxIndex;
}




void MMU::initMMU(){

    int *ptra = &Mmu->framearray[0];
    std::memset(ptra, 0, 64*sizeof(int));
    int *ptrb = &Mmu->framearray[64];
    for(int i=0;i<64;i++){
        ptrb[i] = 1;
    }
    Mmu->PT1 = initPagetable();
    Mmu->PT2 = initPagetable();
    Mmu->PT1.isused = 0;
    Mmu->PT2.isused = 0;
}

void MMU::lockedalloc(int pid,string filename){
      lock_guard<mutex> lock(allocamtx);
      Memory_allocate(pid,max_process,filename);

}


void MMU::Memory_allocate(int pid,int &max_process,string filename){ 

    if(Mmu->total_memory < 64 * 4 * 1024){
        std::cout << "Error:No Enough Memory!" << std::endl;
        return;
    }
    Mmu->MidQueue.push(pid);

    /*
    读取thread.txt
    一条指令占一页，一个内存最大存有4条指令
    */
    std::ifstream file(filename);
    int lineCount = 0;
    std::string line;
    int lineIndex = 0;
    if(Mmu->PT1.isused == 0){
        Mmu->PT1.instrname = filename;
        //cout <<endl<<"now alloc PT1"<<endl;
        Mmu->PT1.isused = 1;
        while (std::getline(file, line)) {
            lineCount++;
            if (lineIndex < 4) {
                    std::strncpy(Mmu->PT1.instruc[lineIndex], line.c_str(), 49);
                    Mmu->PT1.instruc[lineIndex][49] = '\0'; // 确保字符串以终止符结尾
                    lineIndex++;
                    }       
        }
        file.close();
        //cout << "line of "+ filename +" is" <<lineCount<<endl;
        //cout << "instruc in memory is:"<<endl;
        for (int i = 0; i < 4; i++) {
                if (Mmu->PT1.instruc[i][0] != '\0') {
                std::cout << Mmu->PT1.instruc[i] << std::endl;
            } else {
                break; // 遇到空字符串，退出循环
            }
                
        }
    }else{
        Mmu->PT2.instrname = filename;
        Mmu->PT2.isused = 1;
        //cout <<endl<<"now alloc PT2"<<endl;
        
        while (std::getline(file, line)) {
            lineCount++;
            if (lineIndex < 4) {
                    std::strncpy(Mmu->PT2.instruc[lineIndex], line.c_str(), 49);
                    Mmu->PT2.instruc[lineIndex][49] = '\0'; // 确保字符串以终止符结尾
                    lineIndex++;
                    }       
        }
        file.close();
        //cout << "line of "+ filename +" is" <<lineCount<<endl;
    }
    
    if(lineCount >= 4){
        Mmu->total_frame -= 64;
        Mmu->total_memory -= 64 * 4 * 1024;
        mem_available -= 64 * 4 * 1024;
    }else if(lineCount > 0 ){
        Mmu->total_frame -= 16 * lineCount;
        Mmu->total_memory -= lineCount * 16 * 4 * 1024;
        mem_available -= lineCount * 16 * 4 * 1024;
    }

    int num = 0;
    for (int i = 0;i<1024;i++){
        if(Mmu->framearray[i]==-1){
            num++;
            Mmu->framearray[i]=pid;
        }
        if(num == minvalue(64,16*lineCount)){
            break;
        }
    }
    cout << "Memory of this process is allocated!"<<endl;
    // Query_memory();
    // seeprocess();
    
}

void MMU::Memory_release(int pid){

    lock_guard<mutex> lock(releamtx);

    removeprocessFromQueue(Mmu->MidQueue,pid);
    if(Mmu->PT1.isused == 1){
        Mmu->PT1.instrname = "\0";
        //Mmu->PT1.isused = 0;
        //cout <<endl<<"now release PT1"<<endl;
        int deletenum = 0;
        Mmu->PT1.isused = 0;
        for (int i = 0; i < 1024; i++) {
        if(Mmu->framearray[i] == pid){
            Mmu->framearray[i]=-1;
            deletenum ++;
        }
            if(deletenum > 63)break;
        }
        //cout<<endl<<Mmu->PT1.isused<<endl;
        //cout<<endl<<Mmu->PT2.isused<<endl;
        Mmu->total_frame += deletenum ;
        Mmu->total_memory += deletenum * 4 * 1024;
        for (int i = 0; i < 4; ++i) {
            Mmu->PT1.instruc[i][0] = '\0'; // 将每行的第一个字符设置为字符串终止符
        }
        release_pagetable(Mmu->PT1);
    }
    else if(Mmu->PT2.isused == 1){
        Mmu->PT2.instrname = "\0";
        Mmu->PT2.isused = 0;
        //cout<<endl<<Mmu->PT1.isused<<endl;
        //cout <<endl<<"now release PT2"<<endl;
        int deletenum = 0;
        for (int i = 0; i < 1024; i++) {
        if(Mmu->framearray[i]==pid){
            Mmu->framearray[i]=-1;
            deletenum ++;
        }
            if(deletenum > 63)break;
        }
        //cout<<endl<<Mmu->PT2.isused<<endl;
        Mmu->total_frame += deletenum ;
        Mmu->total_memory += deletenum * 4 * 1024;
        for (int i = 0; i < 4; ++i) {
            Mmu->PT2.instruc[i][0] = '\0'; // 将每行的第一个字符设置为字符串终止符
        }
        release_pagetable(Mmu->PT2);
    }
    
    //cout << "Memory of this process is released!"<<endl;
    
   
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
Pagetable MMU::initPagetable(){
    Pagetable pt; 
    
    pt.instrname = "\0";
    
    pt.page_num = new int[pagetablesize];
    pt.page_phyaddr = new int[pagetablesize];
    pt.is_changed = new int[pagetablesize];
    pt.diskaddr = "";
    pt.last_used = new int[pagetablesize];
    memset(pt.is_changed,0,4*sizeof(int));
    memset(pt.last_used,0,4*sizeof(int));
    return pt;
}

TLB MMU::initTLB(int pid,Pagetable PT){
    TLB tlb;
    tlb.tlb_id = pid;
    tlb.page_num = new int[tlbsize];
    tlb.page_phyaddr = new int[tlbsize];
    tlb.is_changed = new int[tlbsize];
    
    tlb.last_used = new int[tlbsize];
    memset(tlb.is_changed,0,4*sizeof(int));
    memset(tlb.last_used,0,4*sizeof(int));
    for(int i = 0;i < tlbsize;i++){
        tlb.page_num[i] = i;
        tlb.page_phyaddr[i] = PT.page_phyaddr[i];
        
    }
    return tlb;
}

void MMU::release_pagetable(Pagetable pt){
    //pt.pagetable_id = -1;
    pt.isused = 0;
    delete[] pt.page_num;
    delete[] pt.page_phyaddr;
    delete[] pt.is_changed;
    //delete[] pt.diskadd
    pt.last_used = {0};
}

void MMU::release_tlb(TLB tlb){
    tlb.tlb_id = -1;
    delete[] tlb.page_phyaddr;
    delete[] tlb.is_changed;
    tlb.diskaddr = "";
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


void MMU::LRU_replace(int pid,string filename1){

    /*
    LRU least recently used page replacement
    */
    string filename = filename1 + ".txt";
    //cout<<"\n\n"+filename+"\n\n";
    std::ifstream pfs(filename);
    int lineCount1 = 0;
    std::string line;
    while (std::getline(pfs, line)) {
        lineCount1++;
    }
    pfs.close();
    if(lineCount1<5)cout<<"指令全部在内存中，不需要页面置换"<<endl;
    else if(lineCount1>=5){
        if((Mmu->PT1.isused == 1)&&(Mmu->PT1.instrname == filename)){
            std::ifstream cpfs(filename);
            int lineCount = 0;
            std::string line;
            while (std::getline(cpfs, line)) {
                lineCount++;
                for(int i=0;i<4;i++)Mmu->PT1.last_used[i]++;
                if(lineCount > 4){
                    int index = maxindex(Mmu->PT1.last_used);
                    std::strncpy(Mmu->PT1.instruc[index], line.c_str(), 49);
                    Mmu->PT1.instruc[index][49] = '\0';
                    Mmu->PT1.last_used[index] = 0;
                }
            }
            cout << "instruc1 in memory is:"<<endl;
            for (int i = 0; i < 4; i++) {
                    if (Mmu->PT1.instruc[i][0] != '\0') {
                    std::cout << Mmu->PT1.instruc[i] << std::endl;
                } else 
                    break; // 遇到空字符串，退出循环
                }

            cpfs.close();
        }
        else if((Mmu->PT2.isused == 1)&&(Mmu->PT2.instrname == filename)){
            std::ifstream cpfs(filename);
            int lineCount = 0;
            std::string line;
            while (std::getline(cpfs, line)) {
                lineCount++;
                for(int i=0;i<4;i++)Mmu->PT2.last_used[i]++;
                if(lineCount > 4){
                    int index = maxindex(Mmu->PT2.last_used);
                    std::strncpy(Mmu->PT2.instruc[index], line.c_str(), 49);
                    Mmu->PT2.instruc[index][49] = '\0';
                    Mmu->PT2.last_used[index] = 0;
                }
            }
            cout << "instruc2 in memory is:"<<endl;
            for (int i = 0; i < 4; i++) {
                    if (Mmu->PT2.instruc[i][0] != '\0') {
                    std::cout << Mmu->PT2.instruc[i] << std::endl;
                } else 
                    break; // 遇到空字符串，退出循环
            }
            cpfs.close();
        }
    }
    // Mlist *switcher = Mmu->mlist;
    // while(switcher!=nullptr){
    //     if(switcher->mid == pid){
    //         int j = findMaxIndex(switcher->PT.last_used,sizeof(switcher->PT.last_used));
    //         if(j == -1)return;
    //         for(int k = 0 ;k < pagetablesize;k++){
    //             switcher->PT.last_used[k] ++;

    //         }
    //         for(int k = 0 ;k < tlbsize;k++){
    //             switcher->TLb.last_used[k]++;
    //         }
    //         string probaddr  = "getline";
    //         for (int k=0;k<pagetablesize;k++){
    //             if (probaddr == switcher->PT.diskaddr)
    //                 break;
    //         }
    //         switcher->PT.diskaddr = probaddr; 
    //         switcher->PT.last_used[j] = 0;
    //         for(int k = 0 ;k < tlbsize;k++){
    //             if(switcher->PT.page_num[k]==switcher->TLb.page_num[k]){
    //                 switcher->TLb.diskaddr[k]=switcher->PT.diskaddr[k];
    //                 switcher->TLb.last_used[k] = switcher->PT.last_used[k];
    //             }

    //         }
    //     }
    

    // }
   //free(switcher);

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

void MMU::seeprocess(){

    cout << "Now the processes in memory are:";
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

void MMU::Report_realtime(){
    cout << "Memory info:"<<endl;
    cout << "memavailbale"<< mem_available <<endl;
    Query_memory();
    seeprocess();
    // Mlist *p = Mmu->mlist;
    // while(p!=nullptr){
    //     cout << p->mid << endl;
    //     p = p->next;
    // }

}

void MMU::Query_memory(){
    cout <<"pbuse:PT1: "<<Mmu->PT1.isused +" PT2:"<<Mmu->PT2.isused<<endl;
    float memo = float(Mmu->total_memory/(1024.0*1024));
    cout << "Size of free memory is "<< memo << "MB" << endl;    
    cout << "Total memory space is 4MB"<<endl;
}  