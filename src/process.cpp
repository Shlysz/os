#include "process.h"
#include "memory.h"
#include "param.h"

void CPU_init() { // CPU初始化
    CPU.eax = 0;
    CPU.ebx = 0;
    CPU.ecx = 0;
    CPU.edx = 0;
    CPU.pc = 0; // 在内核状态不关心PC的值，只有在进程中才关心
    CPU_flag.using_eax = false;
    CPU_flag.using_ebx = false;
    CPU_flag.using_ecx = false;
    CPU_flag.using_edx = false;
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

void Process::runKernel() { // 内核运行函数
    while (1) { 
        int p_space = NPROC;            // 剩余的空闲线程空间
        // for (int i=0; i < RunQueue.size(); i++) {
        //     if (RunQueue[i].PCB->time_need == 0) {
        //         if (InterruptQueue.size() > 0)
        //             wakeup();
        //         /*内存分配待补充*/
        //         RunQueue.push_back(WaitQueue.front());  // 运行队列尾加入一个等待队列
        //         DoneQueue.push_back(RunQueue[i]);       // 完成队列尾加入完成进程
        //         RunQueue.erase(RunQueue.begin() + i);   // 运行队列删除该完成的进程

        //         RunQueue.back().
        //     } // 以上是一个进程完成时的情况

        //     else {
        //         /*内存分配待补充*/
                
        //     } // 以上是没有进程完成，时间片到限制了主动更换
        // }
        cout << "这里是kernel" << endl;

        //sleep(3);//给每个进程3秒，暂时用睡眠来设置，最后应该由中断来发出
    }
}

void Process::run() { // 进程运行函数

}

int Process::create(int parent_id) {
    if (WaitQueue.size() > MAXQUEUE) { // 判断是否由空间创建进程
        cout << "Without enough space to create new thread!" << endl;
        return 0;
    }

    // 调用内存分配，看内存是否有足够的内存分配（待补充）
    return 1;
}

