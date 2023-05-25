#include <stdlib.h>
#include <fstream>
#include <chrono>
#include "process.h"
#include "memory.h"
#include "global.h"
#include "device.h"
#include "interupt.h"
#include "FileMethod.h"

using namespace std;

// 全局变量
int Userpid = 2;         // 分配给进程的pid
int t1 = 0;
int t2 = 0;
struct CentralProcessingUnit CPU;
struct ShareResource CPU_flag;
vector<int> RunQueue;    // 运行队列
vector<int> ReadyQueue;  // 准备队列
vector<int> WaitQueue;   // 等待队列
vector<int> DoneQueue;   // 完成队列
vector<Process> Processes; // 所有的进程对象
Process kernel;          // 核心进程对象
mutex signal_mutex;      // 进程信号量锁
mutex output_mutex;      // 进程输入输出锁
Interupt fst_interupt;   // 进程一计时器
Interupt sec_interupt;   // 进程二计时器
extern mutex info_mu;

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
    device inidev;
    inidev.init_device();
    pcb.name = "kernel";
    pcb.slice_use = 99;                  // 内核进程的运行时间是系统启动至今的时间
    pcb.slice_cnt = 0;
    pcb.time_need = 99999 - pcb.slice_use;    
    int ret = 1;              // 1表示正常
    ret = CPU_init();     // 内核首先初始化CPU
    
    Mmu->initMMU();//内存初始化
    /*
    内存初始化
    中断初始化
    */
    return ret;
}

void Process::scheduler() { // RR调度运行函数
    //发出中断，请求当前系统时间存入变量
    int s_num = ReadyQueue.size() + WaitQueue.size() + RunQueue.size();
    cout << s_num << " processes start schedule:" << endl; 
    while (s_num) { 
        if (!process_info_queue.empty()) {
            auto t = process_info_queue.front();
            process_info_queue.pop();
            if (t.second==0 || t.second==1) { // 进程时间
                if (Processes[t.first-2].pcb.time_need == 0) {  // 如果进程用的时间够了
                    //if (Processes[t.first-2].pcb.state == RUN)
                        terminate(t.first);
                }
                else {
                    passSlice(t.first);
                    readyforward();
                }
            }
            else if (t.second == 2) { // 进程请求设备成功
                wakeup(t.first);
            }
            else if (t.second == 3) { // 设备请求异常：设备ID不存在
                output_mutex.lock();
                cout << "Kernel: Pid-" << t.first << " apply for a wrong device! End this process!" << endl;
                output_mutex.unlock();
                WaitQueue.erase(WaitQueue.begin()+t.first);
                DoneQueue.push_back(t.first);
                // 内存释放
            }
            else if (t.second == 4) { // 进程请求设备正在被占用
                output_mutex.lock();
                cout << "Kernel: Pid-" << t.first << " wait for device" << endl;
                output_mutex.unlock();
                wait(t.first);
            }
            else if (t.second==5 || t.second==7) { // 设备请求异常：设备ID和进程PID不匹配
                output_mutex.lock();
                cout << "Kernel: Pid-" << t.first << " don't match the device! End this process!" << endl;
                output_mutex.unlock();
                WaitQueue.erase(WaitQueue.begin()+t.first);
                DoneQueue.push_back(t.first);
                // 内存释放
            }
            //scheduledisplay();   // 可以实现每运行一次中断就打印     
        }

        if (RunQueue.size() < NPROC && !ReadyQueue.empty())
            readyforward();
        s_num = ReadyQueue.size() + WaitQueue.size() + RunQueue.size();
    }
    cout << "schedule done!" << endl;
}

void Process::FCFS() { // FCFS调度函数 
    int to_do = ReadyQueue.size() + WaitQueue.size(); // 总共需要调度的进程
    cout << "FCFS schedule for " << to_do << " processes." << endl;
    while (to_do) {
        if (!process_info_queue.empty()) { // 有中断
            auto t = process_info_queue.front();
            process_info_queue.pop();
            if (t.second == 2) { //可用
                wakeup(t.first);
            }
            else if (t.second==3 || t.second==5 || t.second==7) { // 异常中断
                wrong (t.first);
            }
            else if (t.second == 4) { // 设备进入等待
                wait(t.first);
            }
            to_do = ReadyQueue.size()+WaitQueue.size()+RunQueue.size();
        }
        else if (!ReadyQueue.empty()) {
            RunQueue.push_back(ReadyQueue[0]);
            ReadyQueue.erase(ReadyQueue.begin());
            FCFS_run(&Processes[RunQueue[0]-2].pcb);
        }
       to_do = ReadyQueue.size(); 
    }
    cout << "FCFS schedule done" << endl;
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
            if (instuction->num==0 || instuction->num==1||instuction->num == 4)
                file >> instuction->name;
             if (instuction->num==5){
                 file >> instuction->name;
                 file >> instuction->code;
             }
            newProcess.pcb.cmdVector.push_back(*instuction);
            // cout << "debug info, cmd read:" << newProcess.pcb.cmdVector.back().num << newProcess.pcb.cmdVector.back().num2 << endl;
        }
        newProcess.pcb.size = 100 * newProcess.pcb.cmdVector.size() * 1024; // 每条指令100Kb
        newProcess.pcb.time_need =  newProcess.pcb.cmdVector.size();
        newProcess.pcb.name = p_name;
    }
    else {
        newProcess.pcb.name = "NULL"; // 不指定内容的进程就是一个死循环进程
        cmd* instuction = new cmd;
        instuction->num = 8;
        instuction->num2 = 0;
        newProcess.pcb.cmdVector.push_back(*instuction);
    }
    newProcess.pcb.PC = 0;
    newProcess.pcb.parent = &(this->pcb);
    newProcess.pcb.pid = Userpid++;
    newProcess.pcb.state = READY;
    //分配一块内存

    Mmu->lockedalloc(newProcess.pcb.pid);
    

    Processes.push_back(newProcess);
    if (ReadyQueue.size() < MAXQUEUE) // 如果准备队列有空进入准备队列，否则进入等待队列
        ReadyQueue.push_back(newProcess.pcb.pid);
    else 
        WaitQueue.push_back(newProcess.pcb.pid);
    return 1;
}

void Process::readyforward() { // 准备进程进入工作
    int *a = new int;
    *a = ReadyQueue[0];
    Processes[*a-2].pcb.state = RUN;
    RunQueue.push_back(*a);
    ReadyQueue.erase(ReadyQueue.begin());
    // 每个进程开始 run 线程后再开始计时线程
    if (t1 == 0) {
        t1 = *a;
        thread sub_proc1(&Process::run, kernel, &Processes[t1-2].pcb); 
        sub_proc1.detach();
        thread sub1_time_set(&Process::setTimer1, kernel, a);
        sub1_time_set.detach();
    }
    else if (t2 == 0) {
        t2 = *a;
        thread sub_proc0(&Process::run, kernel, &Processes[t2-2].pcb);
        sub_proc0.detach();
        thread sub2_time_set(&Process::setTimer2, kernel, a);
        sub2_time_set.detach();
    } 
}

void Process::passSlice(int id) {
    Processes[id-2].pcb.state = READY;
    ReadyQueue.push_back(id);
    if (t1 == id)
        t1 = 0;
    else 
        t2 = 0;
    for (int i=0; i<RunQueue.size(); i++) {
        if (RunQueue[i] == id) {
            RunQueue.erase(RunQueue.begin()+i);
            break;
        }
    }
    // output_mutex.lock();
    // cout << "Pid:" << id << " Process:" << Processes[id-2].pcb.name << " finishes a slice." << endl;
    // output_mutex.unlock();
}

void Process::wait(int id) { // 中断进程
    Processes[id-2].pcb.state = SUSPEND;
    WaitQueue.push_back(id);
    if (t1 == id)
        t1 = 0;
    else 
        t2 = 0;
    for (int i=0; i<RunQueue.size(); i++) {
        if (RunQueue[i] == id) {
            RunQueue.erase(RunQueue.begin()+i);
            break;
        }
    }
    cout << "Pid:" << id << " Process:" << Processes[id-2].pcb.name << " is interupted." << endl;
}

void Process::wrong(int id) { // 从等待终止
    Processes[id-2].pcb.pid = TERMINATED;
    DoneQueue.push_back(id);
    for (int i=0; i<WaitQueue.size(); i++) {
        if (WaitQueue[i] == id) {
            WaitQueue.erase(WaitQueue.begin()+i);
            break;
        }
    }
    output_mutex.lock();
    cout << "Pid:" << id << " Process:" << Processes[id-2].pcb.name << " wrong and quit." << endl;
    output_mutex.unlock();
}

void Process::wakeup(int id) { // 唤醒进程
    Processes[id-2].pcb.state = READY;
    for (int i=0; i<WaitQueue.size(); i++)
        if (WaitQueue[i] == id) {
            ReadyQueue.push_back(i);
            WaitQueue.erase(WaitQueue.begin()+i);
            break;
        }
    
    cout << "Pid:" << id << " Process:" << Processes[id-2].pcb.name << " is waken up." << endl;
}

void Process::terminate(int id) { // 从运行进程终结进程
    Processes[id-2].pcb.state = TERMINATED;
    DoneQueue.push_back(id);
    if (t1 == id)
        t1 = 0;
    else
        t2 = 0;
    for (int i=0; i<RunQueue.size(); i++) {
        if (RunQueue[i] == id) {
            RunQueue.erase(RunQueue.begin()+i);
            break;
        }
    }
    //内存释放
    // cout << "release"<<endl;
    Mmu->Memory_release(id);
    // //Mmu->Report_realtime();
    // cout << "released"<<endl;
    output_mutex.lock();
    cout << "Pid:" << id << " (name:" << Processes[id-2].pcb.name << ") has done, state:" << Processes[id-2].pcb.state << endl; 
    output_mutex.unlock();
}

void Process::displayProc() { // 观察进程信息
    // 中断提供的时间打印在第一行***
    // 展示内容：pid 用户 优先级 占用内存百分比 进程状态 进程总时间 进程名字
    cout << "PID\tUSER\tPR\t%MEM\tSTATE\tTIME\tname" << endl;
    int d = kernel.pcb.time_need + kernel.pcb.slice_cnt;
    string s;
    cout << "0\troot\t99\t0\trun\t" << 99999 << "\tsystemd" << endl;
    cout << "1\troot\t99\t0\trun\t" << 99999 << "\tshell" << endl;
    for (auto e : Processes) {
        d = e.pcb.time_need + e.pcb.slice_cnt;
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
    //runPCB->PC = 0;
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
            //intertemp = false;
            //TODO:schedule:block
            //cout << "Apply for device:" << runPCB->cmdVector[(runPCB->PC)].num2 << endl;
            break;
        case REALESR:
            tmp_interupt.disable_device_interupt(runPCB->pid,runPCB->cmdVector[(runPCB->PC)].num2);
           // intertemp = false;
            //cout << "Release device:" << runPCB->cmdVector[(runPCB->PC)].num2 << endl;
            break;
        case READ:
            FileMethod::readByte(runPCB->cmdVector[(runPCB->PC)].name);
            break;
        case WRITE:
            temfile = fs->open(runPCB->cmdVector[(runPCB->PC)].name,1);           
            strcpy(content, runPCB->cmdVector[(runPCB->PC)].code.c_str());            
            fs->write(temfile,content,runPCB->cmdVector[(runPCB->PC)].code.length());           
            fs->close(temfile);
            delete[] content;
            break;
        case MEMORY:
            // cout << "Memory info:"<<endl;
            // Mmu ->Query_memory();
            // Mmu->seeprocess();
            Mmu->Report_realtime();
            //TODO:输出进程占用内存信息
            break;
        case DEBUG:
            output_mutex.lock();
            cout << "pid:" << runPCB->pid << ", this is a test proc! " << runPCB->PC << endl;
            output_mutex.unlock();
            break;
        case BLANK:
            runPCB->PC--;
            break;
        default:
            output_mutex.lock();
            cout << runPCB->pid << ": Instruction error: " << runPCB->cmdVector[(runPCB->PC)].num << endl;
            output_mutex.unlock();
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
    //Todo : paging
    if(runCmd(runPCB)){
        //cout << "debug info, after r:running process PID:" << runPCB->pid <<"  silece_cnt:" << runPCB->slice_cnt << endl;//输出程序完成，时间等等
    }else{
        //cout << "running process PID:" << runPCB->pid << " running fail" << endl;
    }
    if (!runPCB->time_need)
    {//TODO:释放内存
    }
    if (runPCB->pid == t1 && runPCB->slice_use!=0) { // 如果当前进程在t1运行,而且时间片没跑完,t1计时器结束
        fst_interupt.disable_time_interupt(runPCB->pid);//解除中断定时器
    }
        
    else if (runPCB->slice_use!=0) {
        output_mutex.lock();
        cout << "PID" << runPCB->pid << " time2 end send" <<endl;
        output_mutex.unlock();
        sec_interupt.disable_time_interupt(runPCB->pid);
    }
    return ;
}

void Process::FCFS_run(PCB *runPCB) { // FCFS的运行函数
    Interupt tmp_interupt;
    File* temfile = nullptr;
    char* content = new char[runPCB->cmdVector[(runPCB->PC)].code.length()+1];
    bool intertemp = true; // 判断是否申请释放设备中断
    while (runPCB->PC != (runPCB->cmdVector.size())) {                           
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
            return;
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
            cout << "memory info:"<<endl;
            Mmu->seeprocess();
            Mmu->Query_memory();
            //TODO:输出进程占用内存信息
            break;
        case DEBUG:
            output_mutex.lock();
            cout << "pid:" << runPCB->pid << ", this is a test proc! " << runPCB->PC << endl;
            output_mutex.unlock();
            break;
        case BLANK:
            runPCB->PC--;
            break;
        default:
            output_mutex.lock();
            cout << runPCB->pid << ": Instruction error: " << runPCB->cmdVector[(runPCB->PC)].num << endl;
            output_mutex.unlock();
            break;
        }
        runPCB->PC++;
        this_thread::sleep_for(std::chrono::seconds(1));
    }
    runPCB->state = TERMINATED;
    RunQueue.pop_back();
}

void Process::setTimer1(int* id) { // 发起计时信号
    output_mutex.lock();
    cout << "PID" << *id << " time1 begin send" <<endl;
    output_mutex.unlock();
    fst_interupt.raise_time_interupt(*id);
}

void Process::setTimer2(int* id) { // 发起计时信号
    output_mutex.lock();
    cout << "PID" << *id << " time2 begin send" <<endl;
    output_mutex.unlock();
    sec_interupt.raise_time_interupt(*id);
}

void Process::displayPcb(PCB *runPCB){ // 测试函数：打印PCB的id和命令
    // cout << runPCB->pid <<runPCB->slice_use<<  runPCB->slice_cnt<< runPCB->time_need
    // << runPCB->time_need << runPCB->size << runPCB->pagetable_addr <<runPCB->pagetable_pos
    // <<runPCB->pagetable_len<< runPCB->page_write << runPCB->pagein_time<<endl;

    // cout << "cmd0:" << runPCB->cmdVector[(runPCB->PC)].num <<"  "<< runPCB->cmdVector[0].num<<endl;
    // cout << "cmd1:" << runPCB->cmdVector[(runPCB->PC)+1].num <<"  "<< runPCB->cmdVector[1].num<<endl;
    // cout << runPCB;
    cout << "pid:" << runPCB->pid << " name:" << runPCB->name << " state:" << runPCB->state << " PC:" << runPCB->PC << " cmd:" << endl;
    for (auto e : runPCB->cmdVector) 
        cout << e.num << e.num2 << endl;
    
}

void Process::display_test() { // 打印所有进程信息
    cout << "All proc and its cmd:" << endl;
    for (auto e : Processes) {
        cout << e.pcb.pid << endl;
        for (int i=0; i<e.pcb.cmdVector.size(); i++)
            cout << "\t" << e.pcb.cmdVector[i].num << " " << e.pcb.cmdVector[i].num2 << endl;
    }
}

void Process::scheduledisplay() {
    int needTime = 0;
    output_mutex.lock();
    system("cls");
    cout << "\tPID\tNAME\tTIME\tSLCIE" << endl;
    for (int i=0; i<Processes.size(); i++) {
        needTime = Processes[i].pcb.time_need + Processes[i].pcb.slice_use;
        cout << "\t" << Processes[i].pcb.pid << "\t" << Processes[i].pcb.name << "\t" << needTime << "\t|";
        for (int j=0; j<Processes[i].pcb.slice_cnt+Processes[i].pcb.slice_use-3; j++)
            cout << "\033[32;1m=\033[0m";   // 绿色代表是已完成的
        for (int k=0; k<Processes[i].pcb.slice_use; k++)
            cout << "\033[33;1m=\033[0m";   // 黄色代表新完成的
        cout << endl;
    }
    output_mutex.unlock();
    
}