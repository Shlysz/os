#include "interupt.h"

mutex mu;  // 中断队列互斥锁

queue<Interupt> interupt_queue;  // 中断队列

// 默认中断函数
void Interupt::do_nothing(int pid) {}

// 异常默认处理函数
void Interupt::panic(int pid) { throw "panic"; }

// 初始化中断
void Interupt::init_interupt() {
    while (!interupt_queue.empty()) interupt_queue.pop();
}

// 产生时钟中断给进程
// 然后加入中断队列
void Interupt::raise_time_interupt(int pid) {
    Interupt time_task;
    time_task.pid = pid;
    time_task.type = 1;
    mu.lock();
    interupt_queue.push(time_task);
    mu.unlock();
}

// 结束时钟中断
void Interupt::disable_time_interupt(int pid) {}

// 产生一个中断供外部设备使用
// 加入中断队列
void Interupt::raise_device_interupt(int pid, int device_id) {
    Interupt device_task;
    device_task.device_id = device_id;
    device_task.pid = pid;
    device_task.type = 2;
    mu.lock();
    interupt_queue.push(device_task);
    mu.unlock();
}

// 释放设备请求中断
void Interupt::disable_device_interupt(int pid, int device_id) {}

unsigned int time_cnt = 0;  // 时间片

// 计时器 更新 time_cnt 全局变量
void Interupt::timer() {
    int time_cnt = 0;
    auto start = chrono::system_clock::now();
    // do something...
    auto end = chrono::system_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    // if()
    cout << "经过了 1 个时间片!" << endl;
}

// TODO: 返回给调度某个进程能否成功申请到设备

//  死循环处理
//  FCFS 处理队列中的中断请求
void Interupt::handle_interupt() {
    while (!interupt_queue.empty()) {
        auto task = interupt_queue.front();
        interupt_queue.pop();
        if (task.type == 1) {  // 时钟中断
            cout << "时钟中断请求" << endl;
        }
    }
}