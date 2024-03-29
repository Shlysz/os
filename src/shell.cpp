#include "shell.h"
#include "process.h"
#include "FileMethod.h"
#include "FileSystem.h"
#include "device.h"
#include "global.h"

extern std::queue<std::pair<int, int> > process_info_queue;
int Shell::parse() {
    // 获取当前目录
    string currentdir = fs->getcurrent_dirent_location();
    std::cout << "\nuser@simulator " << currentdir << " % ";
    std::string params;
    std::getline(std::cin, params);
    if (params.empty()) return 1; // 空字符串直接返回不做处理
    std::vector<std::string> all_params;
    std::string temp;
    for (auto t : params) {
        if (t != ' ')
            temp += t;
        else {
            all_params.push_back(temp);
            temp = "";
        }
    }
    // 将当前回车输入的指令的参数放入一个 vector 里面
    // 比如输入 cd dir, 那么 all_params 里面放的就是 ["cd", "dir"]
    if (temp.size() != 0) all_params.push_back(temp);

    // debug
    // for (auto t : all_params) std::cout << t << ' ';
    // std::cout << std::endl;

    /*
        所有和 shell 交互的指令在下面实现
        继续加上一个 else if 代码块就行了, 然后 return 1 给 main.cpp
    */

    if (params == "help") {  // 输出所有可用的指令帮助信息
        std::cout << "\nCommands Available: \n";
        std::cout << "1. `ls -dir`   - list all files" << std::endl;
        std::cout << "2. `top`  - show all process" << std::endl;
        std::cout << "3. `mem`  - show size of free memory " << std::endl;
        std::cout << "4. cd dir - change nowdir to your dir " << std::endl;
        std::cout << "6. `write file` - write a file" << std::endl;
        std::cout << "7. `cat file` - read a file" << std::endl;
        std::cout << "5. `fork` - create a process" << std::endl;
        std::cout << "5. `exit` - exit the simulator" << std::endl;
        return 1;
    } else if (all_params[0] == "ls") {  // 列出当前目录下的所有文件
        fs->ls(all_params[1]);
        return 1;
    } else if (all_params[0] == "cd") {
        if (fs->cd(all_params[1]))
            return 1;
        else
            return 0;
    } else if (all_params[0] == "write") {
        if (FileMethod::writeByte(all_params[1])) return 1;
        cout << "write failed, may your file is not exist or the block is not "
                "enough"
             << endl;
        return 0;
    } else if (all_params[0] == "cat") {
        if (FileMethod::readByte(all_params[1]) > 0) return 1;
        cout << "read failed, may your file is not exist" << endl;
        return 0;
    } else if (all_params[0] == "mkdir") {
        if (fs->mkdir(all_params[1]))
            return 1;
        else
            return 0;
    } else if (all_params[0] == "touch") {
        if (fs->touch(all_params[1])) return 1;
        cout << "touch failed, may your file may is exist" << endl;
        return 0;
    } else if (all_params[0] == "rm") {
        if (fs->rm(all_params[1])) return 1;
        cout << "rm failed, may your file is not exist" << endl;
        return 0;
    } else if (params == "top") {  // 列出所有进程的信息
        std::cout << "\nCurrent Process: " << std::endl;
        kernel.displayProc();
        return 1;
    } else if (all_params[0] == "mem") {
        // TODO:显示剩余的内存大小
        // return memory剩余的大小

        Mmu->Query_memory();
        return 1;
    } else if (params == "fork") {
        kernel.create("blank");
        return 1;
    } else if (all_params[0] == "fork") {  // 创建一个进程
        for (int pos = 1; pos < all_params.size(); pos++) 
            kernel.create(all_params[pos]);
        kernel.scheduler();
        return 1;
    } else if (all_params[0] == "FCFS") {
        for (int pos=1; pos<all_params.size(); pos++) 
            kernel.create(all_params[pos]);
        kernel.FCFS();
        return 1;
    } else if (params == "wws") {
        cout << process_info_queue.size();
        return 1;
    }else if (params == "exit") {  // 退出程序
        /*产生一个特殊的中断信号，保证优先被处理*/
        return 0;
    }else if(params == "deviceinfo"){
        // test_init();     // 测试用设备初始化
        device test;
        test.show_device_all();
        return 1;
    }  else {  // 其余未实现的使用默认 Linux 系统 bash 功能
        system(params.c_str());
        return 1;
    }
}