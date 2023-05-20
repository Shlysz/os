#include "interupt.h"

mutex mu;  // 中断队列互斥锁
bool open_close_flag;

struct InteruptVector {
    InteruptFunc handler = nullptr;
    unsigned int priority;
};

InteruptVector InteruptVectorTable[InteruptVectorTableSize];

bool Interupt::operator<(const Interupt& b) const {
    if (type == b.type) {
        return interupt_time < b.interupt_time ||
               (interupt_time == b.interupt_time && device_id < b.device_id);
    } else {
        return InteruptVectorTable[static_cast<int>(type)].priority <
               InteruptVectorTable[static_cast<int>(b.type)].priority;
    }
}
bool Interupt::operator>(const Interupt& b) const {
    if (type == b.type) {
        return interupt_time > b.interupt_time ||
               (interupt_time == b.interupt_time && device_id > b.device_id);
    } else {
        return InteruptVectorTable[static_cast<int>(type)].priority >
               InteruptVectorTable[static_cast<int>(b.type)].priority;
    }
}

queue<Interupt> interupt_queue;  // 中断队列

// 默认中断函数
void do_nothing(int type, int, int64_t) {}

// 异常默认处理函数
void panic(int type, int64_t) { throw "panic"; }

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

// 关中断
void Interupt::push_off() {
    open_close_flag = false;  // 关闭
}

// 开中断
void Interupt::pop_off() {
    open_close_flag = true;  // 打开
}

// 安装中断处理程序
void Interupt::set_handler(int type, InteruptFunc f) {
    InteruptVectorTable[static_cast<int>(type)].handler = f;
}

void Interupt::send_alarm() {
    int time_cnt = 0;
    auto start = chrono::system_clock::now();
    // do something...
    auto end = chrono::system_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    // if()
    cout << "经过了 1 个时间片!" << endl;
}

// 处理中断; 由执行指令的部分调用
// FCFS 处理队列中的中断请求
void Interupt::handle_interupt() {
    while (!interupt_queue.empty()) {
        auto task = interupt_queue.front();
        interupt_queue.pop();
        if (task.type == 1) {  // 时钟中断
            cout << "时钟中断请求" << endl;
            Interupt::send_alarm();
        }
    }
}

void Interupt::set_priority(int type, int v) {
    InteruptVectorTable[static_cast<int>(type)].priority = v;
}

//  定时器相关

// void Interupt::enable_timer() { timer_is_valid = true; }
// void Interupt::disable_timer() { timer_is_valid = false; }
// void Interupt::stop_timer() { kill_timer = true; }
