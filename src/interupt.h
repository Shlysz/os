#include <mutex>
#include <queue>
#include <vector>

#include "process.h"

using namespace std;

typedef void (*InteruptFunc)(int type, int, int64_t);
const unsigned int TIMER_INTERUPT_INTERVAL = 100;  // 100ms
const int InteruptVectorTableSize = 32;

class Interupt {
   public:
    // 中断类型
    const int TIMER = 1;   // 时钟中断
    const int DEVICE = 2;  // 设备请求中断
    const int PANIC = 3;   // 异常中断

    // 请求中断的设备 id
    int device_id;

    // 中断时间
    int64_t time;

    // 初始化中断
    void init_interupt();

    // 关中断
    void push_off();

    // 开中断
    void pop_off();

    // 安装一个中断处理程序
    void set_handler(int type, InteruptFunc f);

    // 设置优先级
    // void set_priority(int type, int);

    // 产生一个中断；供外部设备使用
    void raise_interupt(int type, int device_id, int64_t value);

    // 处理中断; 由执行指令的部分调用
    // 为防止中断过多，这里会处理全部可处理的中断
    void handle_interupt();

    //  定时器相关
    void enable_timer();
    void disable_timer();
    void stop_timer();
};