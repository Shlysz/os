#include <stdbool.h>
#include <mutex>
#include <thread>
#include <vector>
#include <iostream>
#include <iomanip>

using namespace std;

// 一些需要使用到的参数
#define NPROC         2  // 最大支持的线程数量
#define MAXQUEUE     10  // 最大队列数量
#define NFILE       100  // 最大打开文件数量


//进程管理中的宏
#define TIME_SLICE   200 // 时间片大小
#define MAX_SLICE_CNT 10 // 最大占用的时间片数量


//定义各种状态代表的数值
typedef int PRIORITY;
typedef int PSTATE;      //线程状态
#define READY          1 //就绪
#define RUN            2 //运行
#define SUSPEND        3 //阻塞挂起
#define TERMINATED     4 //死亡结束

struct CentralProcessingUnit { // 处理器
    unsigned int eax;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;
    unsigned int pc;
    //BYTE *share_addr;//共享内存首地址
};

struct ShareResource { // 共享资源占用标记
    bool using_eax;
    bool using_ebx;
    bool using_ecx;
    bool using_edx;
    bool is_working;
};

typedef struct ProgramControlBlock { // PCB表结构
    int pid;            // pid
    int slice_use;      // 当前已在时间片中使用的时间
    int slice_cnt;      // 使用过的时间片数量
    int time_need;      // 预计还需要的时间
    int size;           // 大小
    int pagetable_addr; // 页表首地址
    int pagetable_pos;  // 当前载入内存中使用的页表序号
    int pagetable_len;  // 页表长度
    bool page_write;    // 每页是否可写，true则可写，否则仅可读
    int pagein_time;    // 页面存入内存时间
    PSTATE state;       // 进程状态
    PRIORITY priority;  // 优先级

    std::string name;   // 进程名称
    struct ProgramControlBlock *parent;   // 父进程
    struct CentralProcessingUnit *p_date; // 中断后进程存储此进程的共享资源数据
    /*这里应该补充打开文件，用一个结构ofile来保存所有在这个进程打开的文件*/
    /*还得有一个变量指向当前进程的工作目录*/
} PCB, *PCBptr;

class Process {
public:
    //进程需要用的变量以及函数
    PCB pcb;                            // PCB表
    vector<int> a;

    Process();                          // 基本的构造函数

    int CPU_init();                     // CPU初始化
    int kernel_init();                  // 内核初始化
    void runKernel(int flag);           // 内核进程，进制状态转换必须由中断进入到这个函数来处理

    //用户进程从创建到结束，状态的切换应该都由中断函数，并由父进程对象（内核进程）来调用这些状态切换函数
    int create(int, string);            // 创建线程对象（进入就绪）
    
    void wait();                     // 由运行状态进程挂起
    void wakeup();                   // 唤醒挂起进程
    void readyforward();                // 就绪状态进一步运行或者先挂起
    void terminate();                   // 终结进程 
    void run(int);//for debug

    void displayProc();                 // 展示进程信息
    // void checkProcess(int);             // 观察某个进程信息
};

//全局变量
extern int Userpid;
extern struct CentralProcessingUnit CPU;
extern struct ShareResource CPU_flag;
extern vector<Process> RunQueue;            // 运行队列
extern vector<Process> ReadyQueue;          // 准备队列
extern vector<Process> WaitQueue;           // 等待队列
extern vector<Process> DoneQueue;           // 完成队列
extern Process kernel;
