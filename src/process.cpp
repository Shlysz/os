#include <stdlib.h>
#include <fstream>
#include "process.h"
#include "memory.h"
#include "global.h"
#include "device.h"
#include "interupt.h"
#include "FileMethod.h"
#include <chrono>

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
mutex signal_mutex;       // 进程信号量锁

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
    ret = CPU_init();     // 内核首先初始化CPU
    cout<<"init cpu successfully"<<endl;
    //MMU();
    Mmu->initMMU();
    cout<<"init mmu successfully"<<endl;
    /*
    内存初始化
    中断初始化
    */
    return ret;
}

void Process::scheduler() { // 调度运行函数
    //发出中断，请求当前系统时间存入变量
    int s_num = ReadyQueue.size() + WaitQueue.size() + RunQueue.size();
    cout << s_num << " processes start schedule:" << endl; 
    while (s_num) { 
        mutex tmpmu;
        tmpmu.lock();
        if (!process_info_queue.empty()) {
            auto t = process_info_queue.front();
            process_info_queue.pop();
            tmpmu.unlock();
            if (t.second==0 || t.second==1) { // 进程时间
                if (Processes[t.first-2].pcb.time_need == 0)  // 如果进程用的时间够了
                    terminate(t.first);
                else {
                    passSlice(t.first);
                    readyforward();
                }
            }
            else if (t.second == 2) { // 进程请求设备成功
                wakeup(t.first);
            }
            else if (t.second == 3) { // 设备请求异常：设备ID不存在
                cout << "Pid:" << t.first << " apply for a wrong device! End this process!" << endl;
                WaitQueue.erase(WaitQueue.begin()+t.first);
                DoneQueue.push_back(t.first);
                // 内存释放
            }
            else if (t.second==5 || t.second==7) { // 设备请求异常：设备ID和进程PID不匹配
                cout << "Pid:" << t.first << " don't match the device! End this process!" << endl;
                WaitQueue.erase(WaitQueue.begin()+t.first);
                DoneQueue.push_back(t.first);
                // 内存释放
            }            
        }

        if (RunQueue.size() < NPROC)
            readyforward();
        
        
    }
    // cout << "kernel end!" << endl;
}



int Process::create(string p_name) { //创建进程
    if (WaitQueue.size()>MAXQUEUE && ReadyQueue.size()>MAXQUEUE) { // 判断是否有空间创建进程
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
    if (file.is_open())
    {
        while (file >> buff) {   // 存好进程的指令栈
            cmd* instuction = new cmd;
            instuction->num = atoi(buff.c_str());
            file >> buff;
            instuction->num2 = atoi(buff.c_str());
            if (instuction->num==0 || instuction->num==1) 
                file >> instuction->name;
            // if (instuction.num==4){
            //     file >> instuction.name;
            //     file >> instuction.code;
            // }             
            newProcess.pcb.cmdVector.push_back(*instuction);
            // cout << "debug info, cmd read:" << newProcess.pcb.cmdVector.back().num << newProcess.pcb.cmdVector.back().num2 << endl;
        }
    }
    else {
        newProcess.pcb.name = "NULL"; // 不指定内容的进程就是一个死循环进程
        cmd* instuction = new cmd;
        instuction->num = 5;
        instuction->num2 = 0;
        newProcess.pcb.cmdVector.push_back(*instuction);
    }
    newProcess.pcb.PC = 0;
    newProcess.pcb.parent = &(this->pcb);
    newProcess.pcb.pid = Userpid++;
    newProcess.pcb.state = READY;
    Processes.push_back(newProcess);
    if (ReadyQueue.size() < MAXQUEUE) // 如果准备队列有空进入准备队列，否则进入等待队列
        ReadyQueue.push_back(newProcess.pcb.pid);
    else 
        WaitQueue.push_back(newProcess.pcb.pid);
    return 1;
}

void Process::readyforward() { // 准备进程进入工作
    Processes[ReadyQueue[0]].pcb.state = RUN;
    RunQueue.push_back(ReadyQueue[0]);
    ReadyQueue.erase(ReadyQueue.begin());
    if (RunQueue.size() == 1) {
        thread sub_proc1(&Process::run, kernel, &Processes[RunQueue.front()-2].pcb); 
        sub_proc1.detach();
    }
    else if (RunQueue.size() == 2) {
        thread sub_proc0(&Process::run, kernel, &Processes[RunQueue.back()-2].pcb);
        sub_proc0.detach();
    } 
}

void Process::passSlice(int id) {
    Processes[id-2].pcb.state == READY;
    ReadyQueue.push_back(id);
    for (int i=0; i<RunQueue.size(); i++) {
        if (RunQueue[i] == id) {
            RunQueue.erase(RunQueue.begin()+i);
            break;
        }
    }
    cout << "Pid:" << id << " Process:" << Processes[id-2].pcb.name << " finishes a slice." << endl;
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
    cout << "Pid:" << id << " Process:" << Processes[id-2].pcb.name << " is interupted." << endl;
}

void Process::wakeup(int id) { // 唤醒进程
    Processes[id-2].pcb.state == READY;
    for (int i=0; i<WaitQueue.size(); i++)
        if (WaitQueue[i] == id) {
            ReadyQueue.push_back(i);
            WaitQueue.erase(WaitQueue.begin()+i);
            break;
        }
    
    cout << "Pid:" << id << " Process:" << Processes[id-2].pcb.name << " is waken up." << endl;
}

void Process::terminate(int id) { // 从运行进程终结进程
    Processes[id-2].pcb.state == TERMINATED;
    DoneQueue.push_back(id);
    for (int i=0; i<RunQueue.size(); i++) {
        if (RunQueue[i] == id) {
            RunQueue.erase(RunQueue.begin()+i);
            break;
        }
    }
    //内存释放
    cout << "Pid:" << id << " Process:" << Processes[id-2].pcb.name << " has done." << endl; 
    cout << "leaving read proc:" << ReadyQueue.size() << ", run proc:" << RunQueue.size() << endl;
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

int Process::signal_get() { // 获得一个信号量
    signal_mutex.lock();
    if (!CPU_flag.using_edx) {
        unsigned int ret = CPU.edx;
        signal_mutex.unlock();
        return ret;
    }
    else
        return -1;
}

void Process::signal_add() { // 信号量+
    signal_mutex.lock();
    CPU.edx++;
    signal_mutex.unlock();
}

void Process::signal_min() { // 信号量-
    signal_mutex.lock();
    if (!CPU.edx)
        CPU.edx--;
    signal_mutex.unlock();
}

bool Process::runCmd(PCB *runPCB){//运行进程的指令，如果没有被中断等情况则返回1，否则返回0
    runPCB->PC = 0;
    Interupt tmp_interupt;
    File* temfile = nullptr;
    char* content = new char[runPCB->cmdVector[(runPCB->PC)].code.length()+1];
    bool intertemp = true; // 判断是否申请释放设备中断
    while (runPCB->time_need!=0 && runPCB->slice_use < 3&& intertemp){                           
        switch (runPCB->cmdVector[(runPCB->PC)].num)
        {
        case CREAFILE:
            if(fs->touch(runPCB->cmdVector[(runPCB->PC)].name)){
                cout << "File created successfully" << endl;
            }else{
                cout << "File creation failure" <<endl;
            }
            break;
        case DELEFILE:
            if (fs->rm(runPCB->cmdVector[(runPCB->PC)].name)){
                cout << "Deleted file successfully" << endl;
            } else{
                cout << "File deletion failure" << endl;
            }
            break;
        case APPLY:
            tmp_interupt.raise_device_interupt(runPCB->pid,runPCB->cmdVector[(runPCB->PC)].num2);
            intertemp = false;
            //TODO:schedule:block
            cout << "Apply for device:" << runPCB->cmdVector[(runPCB->PC)].num2 << endl;
            break;
        case REALESR:
            tmp_interupt.disable_device_interupt(runPCB->pid,runPCB->cmdVector[(runPCB->PC)].num2);
            intertemp = false;
            cout << "Release device:" << runPCB->cmdVector[(runPCB->PC)].num2 << endl;
            break;
        case READ:
            FileMethod::readByte(runPCB->cmdVector[(runPCB->PC)].name);
            break;
        case WRITE:
            temfile = fs->open(runPCB->cmdVector[(runPCB->PC)].name,0);           
            strcpy(content, runPCB->cmdVector[(runPCB->PC)].code.c_str());            
            fs->write(temfile,content,runPCB->cmdVector[(runPCB->PC)].code.length());           
            fs->close(temfile);
            delete[] content;
            break;
        case MEMORY:
            //TODO:输出进程占用内存信息
            break;
        case DEBUG:
            cout << "This is a test proc!" << endl;
            break;
        case BLANK:
            runPCB->PC--;
            break;
        default:
            cout << "Instruction error" << endl;
            break;
        }
        runPCB->PC++;
        runPCB->slice_cnt++;
        runPCB->time_need--;
        runPCB->slice_use++;

        this_thread::sleep_for(std::chrono::seconds(1));
    }
    runPCB->slice_use = 0;
    return true;
}

void Process::run(PCB *runPCB) { // 运行函数


    //TODO:申请内存


    Interupt tmp_interupt;
    tmp_interupt.raise_time_interupt(runPCB->pid);//申请中断定时器
    if(runCmd(runPCB)){
        cout << "debug info, after r:running process PID:" << runPCB->pid <<"  silece_cnt:" << runPCB->slice_cnt << endl;//输出程序完成，时间等等
        //TODO:调度（？）schedule:block
    }else{
        cout << "running process PID:" << runPCB->pid << " running fail" << endl;
    }
    if (!runPCB->time_need)
    {//TODO:释放内存
    }
    tmp_interupt.disable_time_interupt(runPCB->pid);//解除中断定时器
    return ;
}

void Process::displayPcb(PCB *runPCB){
    cout << runPCB->pid <<runPCB->slice_use<<  runPCB->slice_cnt<< runPCB->time_need
    << runPCB->time_need << runPCB->size << runPCB->pagetable_addr <<runPCB->pagetable_pos
    <<runPCB->pagetable_len<< runPCB->page_write << runPCB->pagein_time<<endl;

    cout << "cmd0:" << runPCB->cmdVector[(runPCB->PC)].num <<"  "<< runPCB->cmdVector[0].num<<endl;
    cout << "cmd1:" << runPCB->cmdVector[(runPCB->PC)+1].num <<"  "<< runPCB->cmdVector[1].num<<endl;
    cout << runPCB;
}