#include <cstring>
#include <iostream>

#define unsigned char BYTE
// #define unsigned int v_address
// #define unsigned int p_address
#define TLBsize 32
#define pagetablesize 1024
#define memory_size 4 * 1024 * 1024
#define disk_size 512 * 1024 * 1024

class pagetable {
   public:
    bool isallocated;
    int pagetableitem[pagetablesize] = {0};
    void init_pagetable();  // todo:当进程新加入时，初始化一个页表给他
};

class TranslookasideBuffer {
   public:
    int TLBitem[TLBsize];
};

typedef TranslookasideBuffer TLB;