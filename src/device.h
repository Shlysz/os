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
# include "process.h"

# define MAX_DCT_LEN 32 //基础设备数量
# define MAX_SDT_LEN 32 //基础设备数量

using namespace std;

//PCB表结构
// typedef struct ProgramControlBlock
// {
// 
//     int pid;            // pid
//     int slice_cnt;      // 使用过的时间片数量
//     int time_need;      // 预计还需要的时间
//     int size;           // 块大小
//     int pagetable_addr; // 页表首地址
//     int pagetable_len;  // 页表长度
//     int state;          // 进程状态
//     int priority;       // 优先级
//     std::string name;   // 进程名称
//     struct ProgramControlBlock *parent; // 父进程
//     /*这里应该补充打开文件，用一个结构ofile来保存所有在这个进程打开的文件*/
// 
// 
// } PCB, *PCBptr;

typedef struct PCBPtrQueue{
    PCBptr pcbptr;
    struct PCBPtrQueue *next;
}PCBPtrQueue, *PCBPtrQueueptr;

// 进程队列信息
typedef struct QueueInfo{
    int size;                     // 进程数量
    PCBPtrQueue * QueueHead;            // 队列头
    PCBPtrQueue * QueueEnd;             // 队列尾
    PCBPtrQueue * Qplist;      // 队列
}QInfo, * QInfoptr;

// DCT表项
typedef struct DeviceControlTableItem {
	int deviceID;
	bool busy;
    QInfoptr QueProInfo;
}DCTItem, * DCTItemptr;

// DCT 设备控制表，系统中的每台设备都有一张设备控制表DCT
typedef struct DeviceControlTable {
	int size;
	DCTItem DCTitem[MAX_DCT_LEN];
}DCT, * DCTptr;

// SDT系统设备表项
typedef struct SystemDeviceTableItem {
	int deviceID;
	DCTItem* DCTItemPtr;

}SDTItem, * SDTItemptr;

// SDT 系统设备表
typedef struct SystemDeviceTable {
	int size;
	SDTItem	SDTitem[MAX_SDT_LEN];
}SDT, * SDTptr;


// 初始化设备信息
void init_device();
int registe_device(int deviceID);
int get_index_by_deviceID(int deviceID);

/**
 * 进程请求设备
 * 返回 2 ：设备为空闲状态，进程直接使用设备
 * 返回 1 ：设备占用中，进程加入等待队列
 * 返回 0 ：设备 ID 不存在
*/
int apply_device(int pid, int deviceID);
int release_device(int pid, int deviceID);


