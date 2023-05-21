#include <stdlib.h>
#include <fstream>
#include "process.h"
#include "memory.h"
#include "interupt.h"

using namespace std;

// 全局变量
int Userpid = 2;
struct CentralProcessingUnit CPU;
struct ShareResource CPU_flag;
vector<int> RunQueue;    // 运行队列
vector<int> ReadyQueue;  // 准备队列
vector<int> WaitQueue;   // 等待队列
vector<int> DoneQueue;   // 完成队列

vector<Process> Processes; // 所有的进程对象
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
    pcb.priority = 1;
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
            {
            //readyforward();
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



int Process::create(string p_name) { //创建进程
    if (WaitQueue.size() > MAXQUEUE) { // 判断是否有空间创建进程
        cout << "Without enough space to create new thread!" << endl;
        this->pcb.p_date->ebx++; // kernel的ebx记录创建失败的进程
        return 0; // 返回0创建进程失败
    }

    this->pcb.p_date->eax++; // kernel的eax记录创建成功的进程
    this->pcb.p_date->ecx++; // kernel的ecx记录创建的所有进程
    Process newProcess;

    //PCB 初始化
    // 打开对应的进程文件给PCB初始化
    fstream file;
    file.open(p_name+".txt", ios::in);
    string buff;
    cmd instuction;
    while (file >> buff) {   // 存好进程的指令栈
        instuction.num << atoi(buff.c_str());
        file >> buff;
        instuction.num2 << atoi(buff.c_str());
        if (instuction.num==0 || instuction.num==1) 
            file >> instuction.name;
        newProcess.pcb.cmdVector.push_back(instuction);
    }
    newProcess.pcb.PC = &newProcess.pcb.cmdVector[0];
    newProcess.pcb.size = newProcess.pcb.cmdVector.size() * 100 * 1024; // 每条指令使得进程大小增大100Kb
    newProcess.pcb.time_need = newProcess.pcb.cmdVector.size(); // 需要的时间单位等于指令的数量 
    newProcess.pcb.name = p_name;
    newProcess.pcb.parent = &(this->pcb);
    newProcess.pcb.pid = Userpid++;
    newProcess.pcb.state = READY;
    Processes.push_back(newProcess);
    ReadyQueue.push_back(newProcess.pcb.pid);
    return 1;
}

void Process::readyforward() { // 准备进程进入工作
    Processes[ReadyQueue[0]].pcb.state = RUN;
    RunQueue.push_back(ReadyQueue[0]);
    ReadyQueue.erase(ReadyQueue.begin());
    if (RunQueue.size() == 0)
        thread sub_proc1(&Process::run, Processes[RunQueue.front()], &Processes[RunQueue.front()].pcb); 
    else 
        thread sub_proc0(&Process::run, Processes[RunQueue.back()], &Processes[RunQueue.back()].pcb);    
}

void Process::wait(int id) { // 中断进程
    Processes[id-2].pcb.state == SUSPEND;
    WaitQueue.push_back(id);
    for (int i=0; i<RunQueue.size(); i++) {
        if (RunQueue[i] == id) {
            RunQueue.erase(RunQueue.begin()+i);
            break;
        }
    }
    cout << "No." << id << " Process:" << Processes[id-2].pcb.name << " is interupted." << endl;
}

void Process::wakeup(int id) { // 唤醒进程
    Processes[id-2].pcb.state == READY;
    for (int i=0; i<WaitQueue.size(); i++)
        if (WaitQueue[i] == id) {
            ReadyQueue.push_back(i);
            WaitQueue.erase(WaitQueue.begin()+i);
            break;
        }
    
    cout << "No." << id << " Process:" << Processes[id-2].pcb.name << " is waken up." << endl;
}

void Process::terminate(int id) { // 终结进程
    Processes[id-2].pcb.state == TERMINATED;
    DoneQueue.push_back(id);
    for (int i=0; i<RunQueue.size(); i++) {
        if (RunQueue[i] == id) {
            RunQueue.erase(RunQueue.begin()+i);
            break;
        }
    }
    //内存释放

    cout << "No." << id << " Process:" << Processes[id-2].pcb.name << " has done." << endl; 
}

void Process::displayProc() { // 观察进程信息
    // 中断提供的时间打印在第一行***
    // 展示内容：pid 用户 优先级 占用内存百分比 进程状态 进程总时间 进程名字
    cout << "PID\tUSER\tPR\t%MEM\tSTATE\tTIME\tname" << endl;
    int d = kernel.pcb.time_need + kernel.pcb.slice_use;
    string s;
    cout << "0\troot\t99\t0\trun\t" << 99999 << "\tsystemd" << endl;
    cout << "1\troot\t99\t0\trun\t" << 99999 << "\tshell" << endl;
    for (auto e : Processes) {
        d = e.pcb.time_need + e.pcb.slice_use;
        switch (e.pcb.state)
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
        cout << e.pcb.pid << "\troot\t" << e.pcb.priority << "\t";
        cout <<  setprecision(2) << 1.0 * (e.pcb.size/1024) * 100 / (memory_size/1024);
        cout << "%\t" << s << "\t" << d << "\t"<< e.pcb.name << endl; 
    }
    cout << "CPU\tUsingthreads:" << RunQueue.size()+2 << "/" << NPROC+2 << endl;
    //debug
    cout << "debug info:\nrunqueue:" << RunQueue.size() << endl;
    cout << "waitqueue:" << WaitQueue.size() << endl;
    cout << "readyqueue:" << ReadyQueue.size() << endl;
    return;
}

bool runCmd(PCB *runPCB) { // 运行进程的指令，如果没有被中断等情况则返回1，否则返回0
//    Interupt interupt;
    bool interupt = true;//TODO:中断是否需要这个变量
    int num = runPCB->PC - &runPCB->cmdVector[0]; //运行到的指令数
//    runPCB->PC = &runPCB->cmdVector[0];//PC指向指令数组的指令
    //TODO:上面这行放到PCB初始化中
    while (&runPCB->cmdVector.back() != runPCB->PC && interupt){
        runPCB->PC = &runPCB->cmdVector[num];
        switch (runPCB->PC->num)
        {
        case CREAFILE:
            if(fs->mkdir(runPCB->PC->name)){
                cout << "创建成功" << endl;
            }else{
                cout << "创建失败" <<endl;
            }
            break;
        case DELEFILE:
            if (fs->rm(runPCB->PC->name)){
                cout << "删除成功" << endl;
            } else{
                cout << "删除失败" << endl;
            }
            break;
        case APPLY:
            // if(apply_device(runPCB->pid,nowCmd.num2)==1){
            //TODO:要不要给中断一个设备占用信息
            // }
            cout << "申请设备" << endl;
            break;
        case REALESR:
            //release_device(runPCB->pid, nowCmd.num2);
            cout << "释放设备" << endl;
            break;
        default:
            cout << "指令错误" << endl;
            break;
        }
        num++;
//       interupt.handle_interupt();
        //TODO:获得中断判断时间片是否用完的信息以及切断
    }
    return interupt;
}

void Process::run(PCB *runPCB) { // 运行函数
    //TODO:申请内存
    //TODO:申请中断定时器
    cout << "running process PID:" << runPCB->pid << "needTime:" << runPCB->time_need << endl;
    if(!runCmd(runPCB)){
        //TODO:调度（？）
    }
    //TODO:释放内存
    //TODO:解除中断定时器

}