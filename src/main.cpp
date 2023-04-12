#include <iostream>
<<<<<<< HEAD
#include <memory>
#include "process.h"
#include "timer.h"
=======
#include "globalVariable.h"
#include "shell.h"
int main() {

    std::cout<<"请键入命令"<<std::endl;
    while (true)
    {
        std::string command;
        if(fs!= nullptr)
            std::cout<<"root:"<<fs->current->name<<"/ $";
        std::getline(std::cin,command);

        sh->setOrder(command);
>>>>>>> master

        if(!sh->analyse()){
            cout<<"命令解析错误,请输入正确命令"<<endl;
        }
        else cout<<"success"<<endl;
    }


    return 0;
}
