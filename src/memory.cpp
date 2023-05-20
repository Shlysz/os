#include <iostream>
#include "memory.h"
using namespace std;
BYTES mem[memory_size];
int remaining_memory = memory_size;
//byte disk[disk_size];

struct v_address{
    int pagenum;
    int offset;
};

struct p_address{
    int framenum;
    int offfset;
};

void Pagetable::init_pagetable(){
    Pagetable pt;
    pt.isallocated = 1;
    /*
    get pid, set pt.pagetable_id = pid;
    randomly create pageitems
    */

}



