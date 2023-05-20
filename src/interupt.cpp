#include "interupt.h"

mutex mu;  // 中断队列互斥锁

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
void raise_time_interupt() {}

// 产生一个中断供外部设备使用
// 加入中断队列
void Interupt::raise_device_interupt(int type, int device_id,
                                     unsigned int priority_value) {}

// 关中断
void Interupt::push_off() {}

// 开中断
void Interupt::pop_off() {}

// 安装中断处理程序
void Interupt::set_handler(int type, InteruptFunc f) {
    InteruptVectorTable[static_cast<int>(type)].handler = f;
}

// 处理中断; 由执行指令的部分调用
// 为防止中断过多, 这里会处理全部可处理的中断
void Interupt::handle_interupt() {}

void Interupt::set_priority(int type, int v) {
    InteruptVectorTable[static_cast<int>(type)].priority = v;
}

//  定时器相关
void Interupt::enable_timer() { timer_is_valid = true; }
void Interupt::disable_timer() { timer_is_valid = false; }
void Interupt::stop_timer() { kill_timer = true; }
