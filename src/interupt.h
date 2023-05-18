#pragma once
#include <ctime>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

// #include "process.h"

using namespace std;

typedef void (*InteruptFunc)(int type, int, int64_t);  // 中断服务程序
const unsigned int TIMER_INTERUPT_INTERVAL = 100;      // 100ms
const int InteruptVectorTableSize = 32;                // 中断向量表长

class Interupt {
   public:
    // 中断类型
    int type;
    // 请求中断的设备 id
    int device_id;
    // 中断优先级
    unsigned int priority_value;
    // 中断时间
    int64_t interupt_time;

    const int TIMER = 1;   // 时钟中断
    const int DEVICE = 2;  // 设备请求中断
    const int PANIC = 3;   // 异常中断

    // 中断是否有效
    bool valid;

    bool operator<(const Interupt&) const;  // 重载优先级比较运算符
    bool operator>(const Interupt&) const;
    // bool is_blocking() const;  // 是否阻塞?

    // 初始化中断
    void init_interupt();

    // 关中断
    void push_off();

    // 开中断
    void pop_off();

    // 安装一个中断处理程序
    void set_handler(int type, InteruptFunc f);

    // 设置优先级
    void set_priority(int type, int priority);

    // 产生一个中断；供外部设备使用
    void raise_device_interupt(int type, int device_id,
                               unsigned int priority_value);

    // 处理中断; 由执行指令的部分调用
    // 为防止中断过多，这里会处理全部可处理的中断
    void handle_interupt();

    //  定时器相关
    bool timer_is_valid;
    bool kill_timer;
    void enable_timer();
    void disable_timer();
    void stop_timer();
};