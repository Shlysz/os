#pragma once
#include <stdbool.h>
#include <mutex>
#include <thread>
#include <vector>
#include <iostream>
#include <iomanip>
#include "global.h"

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
typedef int PSTATE;      // 线程状态
#define READY          1 // 就绪
#define RUN            2 // 运行
#define SUSPEND        3 // 阻塞挂起
#define TERMINATED     4 // 死亡结束

//指令编码
#define CREAFILE       0 // 创建文件
#define DELEFILE       1 // 删除文件
#define APPLY          2 // 申请设备
#define REALESR        3 // 释放设备
#define READ           4 // 读文件
#define WRITE          5 // 写文件
#define MEMORY         6 // 输出进程占用内存信息
#define DEBUG          7 // 测试
#define BLANK          8 // 空转


struct CentralProcessingUnit { // 处理器
    unsigned int eax;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;

    //BYTE *share_addr;//共享内存首地址
};

struct ShareResource { // 共享资源占用标记
    bool using_eax;
    bool using_ebx;
    bool using_ecx;
    bool using_edx;
    bool is_working; // 没用到返回false，用了返回true
};

typedef struct cmd {//指令格式
	int num;//指令对应的编码
	int num2;//需要唤醒或阻塞的进程PID，文件size或申请的设备代码
	string name;//创建或删除文件的名字
    string code;//写入文件内容
}cmd;

typedef struct ProgramControlBlock { // PCB表结构
    int pid;            // pid
    int slice_use;      // 当前已在时间片中使用的时间
    int slice_cnt;      // 使用过的时间片数量
    int time_need;      // 预计还需要的时间单位
    int size;           // 大小
    int pagetable_addr; // 页表首地址
    int pagetable_pos;  // 当前载入内存中使用的页表序号
    int pagetable_len;  // 页表长度
    bool page_write;    // 每页是否可写，true则可写，否则仅可读
    int pagein_time;    // 页面存入内存时间
    PSTATE state;       // 进程状态
    PRIORITY priority;  // 优先级
    File *myFile;
    string name;   // 进程名称
    struct ProgramControlBlock *parent;   // 父进程
    struct CentralProcessingUnit *p_date; // 中断后进程存储此进程的共享资源数据
    int PC;//指令PC指针
    vector<cmd> cmdVector; // 指令数组
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
    void scheduler();                   // 内核进程，进制状态转换必须由中断进入到这个函数来处理
    //用户进程从创建到结束，状态的切换应该都由中断函数，并由父进程对象（内核进程）来调用这些状态切换函数
    int create(string);                 // 创建线程对象（进入就绪）
    void readyforward();                // 就绪状态进一步运行或者先挂起
    void run(PCB *runPCB);              // 开始运行进程
    bool runCmd(PCB *runPCB);           // 执行指令
    void passSlice(int);                // 执行完一个时间片
    void wait(int);                     // 由运行状态进程挂起
    void wakeup(int);                   // 唤醒挂起进程
    void terminate(int);                // 终结进程 
    void displayProc();                 // 展示进程信息
    void displayPcb(PCB *runPCB);       // 打印PCB信息
    void setTimer1(int*);               // 发起计时信号1
    void setTimer2(int*);               // 发起计时信号2
    void scheduledisplay();             // 调度展示程序
    // void checkProcess(int);          // 观察某个进程信息

    int signal_get();                   // 信号量获取
    void signal_add();                  // 信号量增加
    void signal_min();                  // 信号量减少
    void display_test();                // 测试函数： 打印pid对应的进程信息
};

//全局变量
extern int Userpid;
extern struct CentralProcessingUnit CPU;
extern struct ShareResource CPU_flag;
extern vector<int> RunQueue;            // 运行队列
extern vector<int> ReadyQueue;          // 准备队列
extern vector<int> WaitQueue;           // 等待队列
extern vector<int> DoneQueue;           // 完成队列
extern Process kernel;
extern vector<Process> Processes;       // 所有的进程对象
