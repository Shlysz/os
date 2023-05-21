#pragma once
#include <chrono>
#include <ctime>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "device.h"
#include "process.h"

using namespace std;

class Interupt {
   public:
    int pid;  // 请求中断的进程 pid

    // 中断类型
    int type;
    // 请求中断的设备 id
    int device_id;

    const int TIMER = 1;   // 时钟中断
    const int DEVICE = 2;  // 设备请求中断
    const int PANIC = 3;   // 异常中断

    void do_nothing(int pid);  // 默认中断函数
    void panic(int pid);       // 异常中断

    // 初始化中断
    void init_interupt();

    // 产生一个中断；供外部设备使用
    void raise_device_interupt(int pid, int device_id);
    void disable_device_interupt(int pid, int device_id);

    void raise_time_interupt(int pid);
    void disable_time_interupt(int pid);

    // 处理中断; 由执行指令的部分调用
    // 为防止中断过多，这里会处理全部可处理的中断
    void handle_interupt();

    //  定时器相关
    void timer();  // 中断发送时间片提醒
};