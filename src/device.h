# include <time.h>
# include <sys/timeb.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <map>
# include <windows.h>
# include <list>
# include <algorithm>
# include <ctime>
# include <vector>
# include <iostream>
# include <cstring>
# include <string>
# include <fstream>
# include <sstream>

# define MAX_DCT_LEN 32 //基础设备数量


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

// 设备控制表
typedef struct DeviceControlTable{
    int size;                       // 当前设备表长度
    DCTItem * DCTitem;   // 设备列表
}DCT, * DCTptr;

// 设备控制表项
typedef struct DeviceControlTableItem{
    int deviceID;               // 设备ID
    bool busy;                  // 设备是否占用
    int usingPid;               // 占用设备的进程ID 0代表空闲
    wPro wpro;                  // 等待进程队列
    struct DeviceControlTableItem * next;
}DCTItem, * DCTItemptr;

// 等待队列信息
typedef struct waitingPro{
    int size;                   // 等待进程数量
    PCB * waitingHead;            // 队列头
    PCB * waitingEnd;             // 队列尾
    wpList * wplist;              // 队列
}wPro;
// 等待队列
typedef struct waitingProList{
    PCB process;
    struct waitingProList * next;
}wpList;

// 