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

/*
中断处理队列结果: <pid, return_value>
return_value:
    //时钟
    0: 该进程消耗经过了一个时间片
    1: 该进程请求结束时钟中断成功(停止计时器成功，不再计时)

    //设备
    2: 该进程可以直接使用设备/
    3: 该设备ID不存在，异常中断
    4: 该设备被占用，进入设备等待队列
    5: 该进程释放设备的 pid 输入有误，异常中断
    6: 该进程成功释放设备
    7: 该进程释放设备的 device_id 输入有误，异常中断
*/
extern std::queue<std::pair<int, int> > process_info_queue;

class Interupt {
   public:
    int pid;  // 请求中断的进程 pid

    // 中断类型
    /*
        1: 时钟中断
        2: 停止时钟
        3: 请求设备
        4: 释放设备
        5: 异常中断
    */
    int type;
    // 请求中断的设备 id
    int device_id;

    int panic_type;  // 异常中断类型

    void do_nothing(int pid);            // 默认中断函数
    int raise_panic(int pid, int type);  // 异常中断

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