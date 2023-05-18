#include "shell.h"

#include "Filesystem.h"

int Shell::parse() {
    std::cout << "\nroot@simulator > ";
    std::string params;
    std::getline(std::cin, params);

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
        std::cout << "1. `ls`   - list all files" << std::endl;
        std::cout << "2. `top`  - show all process" << std::endl;
        std::cout << "3. ..." << std::endl;
        std::cout << "4. `exit` - exit the simulator" << std::endl;
        return 1;
    } else if (params == "ls") {  // 列出当前目录下的所有文件
        // TODO: 待实现和文件系统对接
        // 在此处实现 ls 的功能
        return 1;
    } else if (all_params[0] == "cd") {
        // TODO: 切换目录，目前为使用 C++ std::filesystem
        // 实现的，可能需要修改为文件系统自己的实现
        auto path = std::filesystem::current_path();          // getting path
        std::filesystem::current_path("./" + all_params[1]);  // setting path
        return 1;
    } else if (params == "top") {  // 列出所有进程的信息
        std::cout << "\nCurrent Process: " << std::endl;
        // TODO: 在这里实现列出所有当前进程的信息
        return 1;
    } else if (params == "exit") {  // 退出程序
        return 0;
    } else {  // 其余未实现的使用默认 Linux 系统 bash 功能
        system(params.c_str());
        return 1;
    }
}