#pragma once
#include "file.h"

//进程管理中的宏
#define TIME_SLICE   200 // 时间片大小
#define HIGH_PRI       0 // 高优先级
#define LOW_PRI        1 // 低优先级
#define MAX_SLICE_CNT 10 // 最大占用的时间片数量
#define MAX_PROC       5 // 最大可调度的进程数量

//定义各种状态代表的数值
#define int PRIORITY
#define int PSTATE       //线程状态
#define CREATE         0 //创建
#define READY          1 //就绪
#define RUN            2 //运行
#define SUSPEND        3 //阻塞挂起
#define TERMINATED     4 //死亡结束


//PCB表结构
typedef struct ProgramControlBlock
{

    int pid;            // pid
    int slice_cnt;      // 使用过的时间片数量
    int time_need;      // 预计还需要的时间
    int size;           // 块大小
    int pagetable_addr; // 页表首地址
    int pagetable_len;  // 页表长度
    int state;          // 进程状态
    int priority;       // 优先级
    std::string name;   // 进程名称
    struct ProgramControlBlock *parent; // 父进程
    /*这里应该补充打开文件，用一个结构ofile来保存所有在这个进程打开的文件*/


} PCB;

class Process {
public:
    //进程需要用的变量以及函数
    PCB pcb;                 // PCB表

    int createProcess();     // 创建线程对象
    int PCB(int pid, PRIORITY pri, PSTAT sta) : process_id(pid), priority(pri), status(sta){}
    int get_pid() const { return process_id; }
    int get_priority() const { return priority; }
    int get_status() const { return status; }
    void set_priority(int pri) { priority = pri; }
    void set_status(PSTATE sta) { status = sta; }
    void suspend() { status = SUSPENDED; }
    void resume() { status = READY; }
    void terminate() { status = TERMINATED; }
    //...
};