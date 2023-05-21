#include "process.h"
#include "memory.h"
#include "global.h"
#include "device.h"
#include "interupt.h"

using namespace std;

// 全局变量
int Userpid = 2;
struct CentralProcessingUnit CPU;
struct ShareResource CPU_flag;
vector<Process> RunQueue;    // 运行队列
vector<Process> ReadyQueue;  // 准备队列
vector<Process> WaitQueue;   // 等待队列
vector<Process> DoneQueue;   // 完成队列
vector<PCB*> PCBlist;
Process kernel;

int Process::CPU_init() {  // CPU初始化
    CPU.eax = 0;
    CPU.ebx = 0;
    CPU.ecx = 0;
    CPU.edx = 0;
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
    pcb.name = "kernel";
    pcb.name = "kernel";
    pcb.slice_use = 99;                  // 内核进程的运行时间是系统启动至今的时间
    pcb.slice_cnt = 0;
    pcb.time_need = 99999 - pcb.slice_use;    
    int ret = 1;              // 1表示正常
    ret = CPU_init();         // 内核首先初始化CPU
    /*
    内存初始化
    中断初始化
    */
    return ret;
}

void Process::runKernel(int flag) { // 内核运行函数
    if (!flag) { //初始化失败结束程序
        cout << "\nsystemd init fail." << endl;
        exit(0);
    }

    //发出中断，请求当前系统时间存入变量
    
    int p_space = NPROC;                 // 剩余的空闲线程空间
             
    // cout << "\n(Here is kernel running!)" << endl;
    while (1) { 
        int request_type = 0;
        /*接收中断请求，存入request_type里***/

        if (! request_type && RunQueue.size() < NPROC && ReadyQueue.size()>0)
            {readyforward();
            cout << "end kernel" << endl;
            return;
            }
        if (request_type == 1) { // 时钟中断
            // 检测正在运行的进程的运行时间

        } else if (request_type == 2) {  // 设备中断
            // 将进程挂起并切换,或者进程完成输入中断返回

        }
        else if (request_type == 3) { // 异常中断
            // 
        }
        else if (request_type == 4) { // 控制中断
            // 根据不同的控制命令使用不同的功能

        }
        else if (request_type == 5)  // 系统关闭，程序结束
            return;
        else { // 没有中断时，自行将准备队列放入运行
        }
    }
    // cout << "kernel end!" << endl;
}

int Process::create(int parent_id, string p_name) { //创建进程
    if (WaitQueue.size() > MAXQUEUE) { // 判断是否有空间创建进程
        cout << "Without enough space to create new thread!" << endl;
        this->pcb.p_date->ebx++; // kernel的ebx记录创建失败的进程
        return 0; // 返回0创建进程失败
    }

    this->pcb.p_date->eax++; // kernel的eax记录创建成功的进程
    this->pcb.p_date->ecx++; // kernel的ecx记录创建的所有进程
    Process newProcess;
    PCBlist.push_back(&newProcess.pcb);
    ReadyQueue.push_back(newProcess);

    //PCB 初始化
    // 打开对应的进程文件给PCB初始化
    newProcess.pcb.name = p_name;
    newProcess.pcb.parent = PCBlist[parent_id];
    newProcess.pcb.pid = Userpid++;
    newProcess.pcb.state = READY;
    return 1;
}

void Process::readyforward() { // 准备进程进入工作
    cout << "kernel in ready forward" << endl;
    RunQueue.push_back(ReadyQueue[0]);
    ReadyQueue.erase(ReadyQueue.begin());
    //运行
    
}

void Process::wait() { // 中断进程
    
}

void Process::wakeup() { // 唤醒进程

}

void Process::terminate() { // 终结进程

}

void Process::displayProc() { // 观察进程信息
    // 中断提供的时间打印在第一行***
    // 展示内容：pid 用户 优先级 占用内存百分比 进程状态 进程总时间 进程名字
    cout << "PID\tUSER\tPR\t%MEM\tSTATE\tTIME\tname" << endl;
    int d = kernel.pcb.time_need + kernel.pcb.slice_use;
    float dd;
    string s;
    cout << "0\troot\t99\t0\trunning\t" << 99999 << "\tsystemd" << endl;
    cout << "1\troot\t99\t0\trunning\t" << 99999 << "\tshell" << endl;
    for (auto e : PCBlist) {
        d = (*e).time_need + (*e).slice_use;
        switch ((*e).state)
        {
        case READY:
            s = "ready";
            break;
        case RUN:
            s = "run";
            break;
        case SUSPEND:
            s = "wait";
            break;
        case TERMINATED:
            s = "end";
            break;
        default:
            s = "unknown";
            break;
        }
        dd = (*e).size / memory_size * 1.0;
        cout << (*e).pid << "\troot\t" << (*e).priority << "\t"  << dd << "%\t"
             << s << "\t" << d << "\t"<< (*e).name << endl; 
    }
    cout << "CPU\tUsingthreads:" << RunQueue.size()+2 << "/" << NPROC+2 << endl;
    cout << "debug info:\nrunqueue:" << RunQueue.size() << endl;
    cout << "waitqueue:" << WaitQueue.size() << endl;
    cout << "readyqueue:" << ReadyQueue.size() << endl;
    cout << "PCBlist:" << PCBlist.size() << endl;
}

bool runCmd(PCB *runPCB){//运行进程的指令，如果没有被中断等情况则返回1，否则返回0
    int num = runPCB->PC - &(runPCB->cmdVector[0]); //运行到的指令数
    Interupt tmp_interupt;
    while (runPCB->time_need!=0 && runPCB->slice_use % 3 == 0){                           
        runPCB->PC = &runPCB->cmdVector[num];       
        switch (runPCB->cmdVector[num].num)
        {
        case CREAFILE:
            if(fs->mkdir(runPCB->cmdVector[num].name)){
                cout << "File created successfully" << endl;
            }else{
                cout << "File creation failure" <<endl;
            }
            break;
        case DELEFILE:
            if (fs->rm(runPCB->cmdVector[num].name)){
                cout << "Deleted file successfully" << endl;
            } else{
                cout << "File deletion failure" << endl;
            }
            break;
        case APPLY:
            tmp_interupt.raise_device_interupt(runPCB->pid,runPCB->cmdVector[num].num2);
            //TODO:schedule:block
            cout << "Apply for device:" << runPCB->cmdVector[num].num2 << endl;
            break;
        case REALESR:
            tmp_interupt.disable_device_interupt(runPCB->pid,runPCB->cmdVector[num].num2);
            cout << "Release device:" << runPCB->cmdVector[num].num2 << endl;
            break;
        default:
            cout << "Instruction error" << endl;
            break;
        }
        num++;
        runPCB->PC = &(runPCB->cmdVector[num]);
        runPCB->time_need--;
        runPCB->slice_use++;

        this_thread::sleep_for(std::chrono::seconds(1));
    }
    return true;
}

void run(PCB *runPCB){//运行函数
    //TODO:申请内存
    Interupt tmp_interupt;
    tmp_interupt.raise_time_interupt(runPCB->pid);//申请中断定时器
    cout << "running process PID:" << runPCB->pid << "needTime:" << runPCB->time_need << endl;
    if(runCmd(runPCB)){
        cout << "running process PID:" << runPCB->pid <<"silece_use:" << runPCB->slice_use << endl;//输出程序完成，时间等等
        //TODO:调度（？）schedule:block
    }else{
        cout << "running false" <<endl;
    }
    //TODO:释放内存
    tmp_interupt.disable_time_interupt(runPCB->pid);//解除中断定时器
    return ;
}