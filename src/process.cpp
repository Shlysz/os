#include "process.h"
#include "memory.h"

// 全局变量
int Userpid = 0;
struct CentralProcessingUnit CPU;
struct ShareResource CPU_flag;
vector<Process> RunQueue;            // 运行队列
vector<Process> ReadyQueue;          // 准备队列
vector<Process> WaitQueue;           // 等待队列
vector<Process> DoneQueue;           // 完成队列



int Process::CPU_init() { // CPU初始化
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

Process::Process() { // 基本的构造函数
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

int Process::kernel_init() { // 内核初始化
    int ret = 1; // 1表示正常
    // 内核首先初始化CPU
    ret = CPU_init();
    /*
    内存初始化
    中断初始化
    */
   return ret;
}
void Process::runKernel(int flag) { // 内核运行函数
    if (!flag) { //初始化失败结束程序
        cout << "systemd init fail." << endl;
        exit(0);
    }

    int p_space = NPROC;                 // 剩余的空闲线程空间

    if (1) { 
        int request_type = 0;
        /*接收中断请求，存入request_type里*/
        if (request_type == 1) { // 正常时间到了
            cout << "执行切换进程操作" << endl; 
        }
        else if (request_type == 2) { // 有进程提前完成
            cout << "进程提前完成，切换" << endl;
        }
        cout << "Here is kernel running!" << endl;

        //sleep(3);//给每个进程3秒，暂时用睡眠来设置，最后应该由中断来发出
    }
    cout << "kernel end!" << endl;
}

// void Process::run() { // 进程运行函数

/*
1.读取文件运行程序
2.时间中断（运行到时间打中断）
    2.1 到间隔时间打中断
    2.2 未到间隔时间进程结束中断
3.设备(IO)中断
4.资源占用（？）

运行过程：定期调用本函数，调用schedule()，
减少此进程还需运行的时间，并且判断是否需要阻塞其他进程，
是否需要申请设备，是否需要写文件。如果进程已经执行结束，
判断是否需要唤醒其他进程，是否需要释放设备，
是否需要删除文件
*/
// }

// int Process::create(int parent_id) {
//     if (WaitQueue.size() > MAXQUEUE) { // 判断是否有空间创建进程
//         cout << "Without enough space to create new thread!" << endl;
//         return 0;
//     }

//     Process newProcess;

//     // 调用内存分配，看内存是否有足够的内存分配（待补充）



//     return 1;
// }

