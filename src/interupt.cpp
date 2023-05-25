#include "interupt.h"

mutex mu;  // 中断队列互斥锁

queue<Interupt> interupt_queue;  // 中断队列

// 默认中断函数
void Interupt::do_nothing(int pid) { cout << "默认中断处理" << endl; }

// 异常默认处理函数
void Interupt::raise_panic(int pid, int type) {
    this->pid = pid;
    this->panic_type = type;
    if (type == 1) {  // 设备请求失败
        cout << "异常: 设备请求失败" << endl;
    } else if (type == 2) {  // 设备释放失败
        cout << "异常: 设备释放失败" << endl;
    }
    // this->handle_interupt();
    //  throw "panic";
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

bool timer_flag = true;
void Interupt::timer() {
    // 计时开始
    auto start = std::chrono::steady_clock::now();

    // 持续计时3秒
    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(3)) {
        // 继续计时
        // 收到结束计时的信号就停止计时器中断
        if (timer_flag == false) break;
    }

    // 计时结束
    auto end = std::chrono::steady_clock::now();

    // 计算耗时并输出结果
    std::chrono::duration<double> elapsed_seconds = end - start;
    // std::cout << "耗时: " << elapsed_seconds.count() << " 秒" << std::endl;
}

//  FCFS 处理队列中的中断请求
// <PID, return_value>
/*
return_value:
    0: 该进程消耗经过了一个时间片
    1: 成功停止计时器
    2: 该进程可以直接使用设备
    3: 该设备ID不存在，异常中断
    4: 该设备被占用，进入设备等待队列
    5: 该进程释放设备的 pid 输入有误，异常中断
    6: 该进程成功释放设备
    7: 该进程释放设备的 device_id 输入有误，异常中断
*/

std::queue<std::pair<int, int> > process_info_queue;
mutex info_mu;  // 中断处理结果消息队列锁
void Interupt::handle_interupt() {
    while (!interupt_queue.empty()) {
        auto task = interupt_queue.front();
        interupt_queue.pop();
        if (task.type == 1) {  // 请求时钟
            timer_flag = true;
            // cout << "PID 为 " << task.pid << " 的进程开启时钟中断!" << endl;
            task.timer();
            cout << "PID 为 " << task.pid << " 的进程耗费经过一个时间片!"
                 << endl;
            info_mu.lock();
            process_info_queue.push({task.pid, 0});  // 经过了一个时间片
            info_mu.unlock();
        } else if (task.type == 2) {  // 停止时钟
            timer_flag = false;
            cout << "PID 为 " << task.pid << " 的进程停止时钟中断!" << endl;
            // task.disable_time_interupt();
            info_mu.lock();
            process_info_queue.push({task.pid, 1});
            info_mu.unlock();
        } else if (task.type == 3) {
            cout << "PID 为 " << task.pid << " 的进程发来请求使用设备ID为 "
                 << task.device_id << " 的中断!" << endl;
            // 请求设备
            /*
                2: 直接使用设备
                0: 设备ID不存在
                1: 设备忙，进入该设备的等待队列
            */
            device tmp_device;
            int re = tmp_device.apply_device(task.pid, task.device_id);
            if (re == 2) {
                info_mu.lock();
                process_info_queue.push({task.pid, 2});  // 直接使用设备
                info_mu.unlock();
                cout << "PID 为 " << task.pid
                     << " 的进程成功申请设备，直接使用!" << endl;
            } else if (re == 1) {
                cout << "PID 为 " << task.pid
                     << " 的进程申请的设备忙，加入设备等待队列!" << endl;
                info_mu.lock();
                process_info_queue.push({task.pid, 4});  // 设备忙加入等待队列
                info_mu.unlock();
            }
        } else if (task.type == 4) {  // 释放设备
            /*
                2: pid error
                1: ok
                0: device_id not exist
            */
            cout << "PID 为 " << task.pid << " 的进程设备中断请求释放 "
                 << task.device_id << " 号设备" << endl;
            device tmp_device;
            int re = tmp_device.release_device(task.pid, task.device_id);
            if (re == 1) {  // ok 成功释放
                info_mu.lock();
                process_info_queue.push({task.pid, 6});
                info_mu.unlock();
                cout << "PID 为 " << task.pid << " 的进程成功释放设备!" << endl;
            }
        } else if (task.type == 5) {  // 异常
            // task.raise_panic(task.pid, task.panic_type);
        }
    }
    return;
}