#pragma once


//进程管理中的宏
#define TIME_SLICE 200  // 时间片大小
#define HIGH_PRI 0      // 高优先级
#define LOW_PRI 1       // 低优先级
#define MAX_SLICE_CNT 10// 最大占用的时间片数量
#define MAX_PROC 5      // 最大可调度的进程数量



//PCB表结构
typedef struct PCB
{

    int id;             // pid
    int status;         // 进程状态
    int priority;       // 优先级
    int slice_cnt;      // 使用过的时间片数量
    int time_need;      // 预计还需要的时间
    int pc;             // PC
    int size;           // 块大小
    int pagetable_addr; // 页表首地址
    int pagetable_len;  // 页表长度
    std::string name;   // 进程名称

} PCB;

class Process {
public:
    //进程需要用的变量以及函数
    int state; //进程当前的状态

    //...
};