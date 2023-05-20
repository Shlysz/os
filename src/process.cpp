#include "process.h"

#include "memory.h"

// 全局变量
int Userpid = 0;
struct CentralProcessingUnit CPU;
struct ShareResource CPU_flag;
vector<Process> RunQueue;    // 运行队列
vector<Process> ReadyQueue;  // 准备队列
vector<Process> WaitQueue;   // 等待队列
vector<Process> DoneQueue;   // 完成队列

int Process::CPU_init() {  // CPU初始化
    CPU.eax = 0;
    CPU.ebx = 0;
    CPU.ecx = 0;
    CPU.edx = 0;
    CPU.pc = 0;
    CPU_flag.using_eax = false;
    CPU_flag.using_ebx = false;
    CPU_flag.using_ecx = false;
    CPU_flag.using_edx = false;
    CPU_flag.is_working = true;
    return 1;
}

Process::Process() {  // 基本的构造函数
    pcb.pid = Userpid + 1;
    pcb.slice_use = 0;
    pcb.slice_cnt = 0;
    pcb.time_need = 0;
    pcb.size = 0;
    /*页面相关待补充
    pcb.pagetable_addr =
    pcb.pagetable_pos =
    pcb.pagetable_len =
    */
    pcb.page_write = true;
    /*中断有关，待补充
    pcb.pagein_time =
    */
    pcb.parent = NULL;
    pcb.p_date = new struct CentralProcessingUnit;
    pcb.p_date->eax = 0;
    pcb.p_date->ebx = 0;
    pcb.p_date->ecx = 0;
    pcb.p_date->edx = 0;

    /*打开文件和当前工作的目录在这里补充*/
}

int Process::kernel_init() {  // 内核初始化
    int ret = 1;              // 1表示正常
    // 内核首先初始化CPU
    ret = CPU_init();
    /*
    内存初始化
    中断初始化
    */
    return ret;
}

void Process::runKernel(int flag) {  // 内核运行函数
    if (!flag) {                     // 初始化失败结束程序
        cout << "systemd init fail." << endl;
        exit(0);
    }

    int p_space = NPROC;  // 剩余的空闲线程空间
    // cout << "\n(Here is kernel running!)" << endl;
    while (1) {
        int request_type = 0;

        /*接收中断请求，存入request_type里*/
        if (request_type == 1) {  // 时钟中断
            // 检测正在运行的进程的运行时间

        } else if (request_type == 2) {  // 设备中断
            // 将进程挂起并切换,或者进程完成输入中断返回

        } else if (request_type == 3) {  // 异常中断
            //
        } else if (request_type == 4)  // 系统关闭，程序结束
            return;
        else {  // 没有中断时，自行将准备队列放入运行
            if (RunQueue.size() < NPROC && !ReadyQueue.empty()) readyforward();
        }
    }
    // cout << "kernel end!" << endl;
}

int Process::create(int parent_id) {
    if (WaitQueue.size() > MAXQUEUE) {  // 判断是否有空间创建进程
        cout << "Without enough space to create new thread!" << endl;
        return 0;  // 返回0创建进程失败
    }

    Process newProcess;
    ReadyQueue.push_back(newProcess);
    int jud = 0;
    // 调用内存分配，看内存是否有足够的内存分配（待补充）,结果返回给jud
    // 0:没有足够的内存空间
    // 1:有足够的内存空间
    if (jud == 1) {
        if (RunQueue.size() < NPROC) {  // 运行进程队列未满
            ;
        } else {
            ;
        }
    }

    return 1;
}

void Process::readyforward() {  // 准备进程进入工作
    RunQueue.push_back(ReadyQueue[0]);
    ReadyQueue.erase(ReadyQueue.begin());
    // 内存分配
}
