#include "interupt.h"

mutex mu;  // 中断队列互斥锁

void Interupt::init_interupt() {}

// 关中断
void Interupt::push_off() {}

// 开中断
void Interupt::pop_off() {}

// 安装中断处理程序
void Interupt::set_handler(int type, InteruptFunc f) {}

// 产生一个中断；供外部设备使用
void Interupt::raise_device_interupt(int type, int device_id,
                                     unsigned int priority_value) {}

// 处理中断; 由执行指令的部分调用
// 为防止中断过多，这里会处理全部可处理的中断
void Interupt::handle_interupt() {}

//  定时器相关
void Interupt::enable_timer() {}
void Interupt::disable_timer() {}
void Interupt::stop_timer() {}
