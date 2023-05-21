#include "interupt.h"

mutex mu;  // 中断队列互斥锁

queue<Interupt> interupt_queue;  // 中断队列

// 默认中断函数
void Interupt::do_nothing(int pid) { cout << "默认中断处理" << endl; }

// 异常默认处理函数
int Interupt::raise_panic(int pid, int type) {
    if (type == 1) {  // 设备请求失败
        cout << "异常: 设备请求失败" << endl;
    } else if (type == 2) {  // 设备释放失败
        cout << "异常: 设备释放失败" << endl;
    }
    this->handle_interupt();
    // throw "panic";
    return 233;  // panic specify return value
}

// 初始化中断
void Interupt::init_interupt() {
    while (!interupt_queue.empty()) interupt_queue.pop();
}

// 产生时钟中断给进程
// 然后加入中断队列
void Interupt::raise_time_interupt(int pid) {
    this->pid = pid;
    this->type = 1;  // 产生时钟中断
    mu.lock();
    interupt_queue.push(*this);
    mu.unlock();
    this->handle_interupt();
}

// 结束时钟中断
void Interupt::disable_time_interupt(int pid) {
    this->pid = pid;
    this->type = 2;  // 结束时钟
    mu.lock();
    interupt_queue.push(*this);
    mu.unlock();
    this->handle_interupt();
}

// 产生一个中断供外部设备使用
// 加入中断队列
// TODO: 返回给调度某个进程能否成功申请到设备
void Interupt::raise_device_interupt(int pid, int device_id) {
    this->device_id = device_id;
    this->pid = pid;
    this->type = 3;  // 请求设备
    mu.lock();
    interupt_queue.push(*this);
    mu.unlock();
    this->handle_interupt();
}

// 释放设备请求中断
void Interupt::disable_device_interupt(int pid, int device_id) {
    this->device_id = device_id;
    this->pid = pid;
    this->type = 4;  // 释放设备
    mu.lock();
    interupt_queue.push(*this);
    mu.unlock();
    this->handle_interupt();
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
    std::cout << "耗时: " << elapsed_seconds.count() << " 秒" << std::endl;
}

//  FCFS 处理队列中的中断请求
int Interupt::handle_interupt() {
    while (!interupt_queue.empty()) {
        auto task = interupt_queue.front();
        interupt_queue.pop();
        if (task.type == 1) {  // 请求时钟
            cout << "PID 为 " << this->pid << " 的进程开启时钟中断请求" << endl;
            this->timer();
            cout << "进程耗费经过一个时间片!" << endl;
            return 1;                 // 返回一个时间片信号
        } else if (task.type == 2) {  // 停止时钟
            cout << "PID 为 " << this->pid << " 的进程结束时钟中断请求" << endl;
        } else if (task.type == 3) {
            // 请求设备
            /*
                2: 直接使用设备
                0: 设备ID不存在
                1: 设备忙，进入该设备的等待队列
            */
            // int res = apply_device(pid, device_id);
            //  if(res == 2)
            cout << "PID 为 " << this->pid << " 的进程申请设备中断请求 "
                 << this->device_id << " 号设备" << endl;
            // return res;
        } else if (task.type == 4) {  // 释放设备
            // int res = release_device(pid, device_id);
            /*
                2: pid error
                1: ok
                0: device_id not exist
            */
            cout << "PID 为 " << this->pid << " 的进程释放设备中断请求 "
                 << this->device_id << " 号设备" << endl;
            // return res;               // 返回给调度
        } else if (task.type == 5) {  // 异常
            int res = this->raise_panic(this->pid, this->panic_type);
            return res;
        }
    }
    return 114514;  // ok
}