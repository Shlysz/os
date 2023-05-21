#include "interupt.h"

mutex mu;  // 中断队列互斥锁

queue<Interupt> interupt_queue;  // 中断队列

// 默认中断函数
void Interupt::do_nothing(int pid) {}

// 异常默认处理函数
void Interupt::panic(int pid, int type) {
    if (type == 1) {  // 设备请求失败
        cout << "异常：设备请求失败" << endl;
    } else if (type == 2) {  // 设备释放失败
        cout << "异常：设备释放失败" << endl;
    }
    throw "panic";
}

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
int Interupt::raise_device_interupt(int pid, int device_id) {
    Interupt device_task;
    // cout << "create!!" << endl;
    device_task.device_id = device_id;
    device_task.pid = pid;
    device_task.type = 3;  // 请求设备
    mu.lock();
    cout << "lock" << endl;
    interupt_queue.push(device_task);
    mu.unlock();
}

// 释放设备请求中断
int Interupt::disable_device_interupt(int pid, int device_id) {
    Interupt device_task;
    device_task.device_id = device_id;
    device_task.pid = pid;
    device_task.type = 4;  // 释放设备
    mu.lock();
    interupt_queue.push(device_task);
    mu.unlock();
}

unsigned int time_cnt = 0;  // 时间片

void Interupt::timer() {
    // 计时开始
    auto start = std::chrono::steady_clock::now();

    // 持续计时3秒
    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(3)) {
        // 继续计时
        // 可以在这里添加其他操作
    }

    // 计时结束
    auto end = std::chrono::steady_clock::now();

    // 计算耗时并输出结果
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "耗时：" << elapsed_seconds.count() << " 秒" << std::endl;
}

// TODO: 返回给调度某个进程能否成功申请到设备

//  死循环处理
//  FCFS 处理队列中的中断请求
int Interupt::handle_interupt() {
    while (!interupt_queue.empty()) {
        auto task = interupt_queue.front();
        interupt_queue.pop();
        if (task.type == 1) {  // 请求时钟
            cout << "时钟中断请求" << endl;
            while (1) {
                Interupt::timer();
            }
        } else if (task.type == 2) {  // 停止时钟

        } else if (task.type == 3) {
            // 请求设备
            /*
                2: 直接使用设备
                0: 设备ID不存在
                1: 设备忙，进入该设备的等待队列
            */
            int res = apply_device(pid, device_id);
            // if(res)
            cout << "调用设备请求函数!!!" << endl;
            return res;
        } else if (task.type == 4) {  // 释放设备
            int res = release_device(pid, device_id);
            /*
                2: pid error
                1: ok
                0: device_id not exist
            */
            cout << "调用设备释放函数" << endl;
            return res;               // 返回给调度
        } else if (task.type == 5) {  // 异常
        }
    }
}