#include "memory.h"

void init(){
//连续分页
}


int read(int page_num,bool IsUsed,v_address address,pagetable *page){


}
int write (int page_num,bool IsUsed,bool IsReadonly,bool IsDirty, pagetable *page){


}
int Findphyaddr(int page_num,bool IsUsed,v_address address,pagetable *page){


    TLB_renew();
}
int Free(int page_num,pagetable *page){

    
}

int TLB_renew(TLB tlb){


}