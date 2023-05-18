#include <iostream>
#include <memory>
#include <pthread.h>
#include "process.h"
#include "timer.h"
#include "globalVariable.h"
#include "shell.h"
#include "param.h"

void* systemed() {//内核进程
    init();//进程初始化

}

int main() {
    //初始化
    BYTE* runningMemory = new BYTE[MEMORYSIZE+1];   // 模拟计算机的内存
    
    


    std::cout<<"请键入命令"<<std::endl;
    while (true)
    {
        std::string command;
        if(fs!= nullptr)
            std::cout<<"root:"<<fs->current->name<<"/ $";
        std::getline(std::cin,command);

        sh->setOrder(command);

        if(!sh->analyse()){
            cout<<"命令解析错误,请输入正确命令"<<endl;
        }
        else cout<<"success"<<endl;
    }


    return 0;
}
