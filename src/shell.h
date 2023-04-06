//
// Created by 26656 on 2023/4/5.
//

#ifndef FILESYSTEM_SHELL_H
#define FILESYSTEM_SHELL_H
#include <utility>

#include "iostream"
#include "vector"
using namespace std;


class shell {
//shell解析器
public:
    shell()= default;
    //接受命令行参数
    shell(string order):order(std::move(order)){};


    //分析命令
    bool  analyse();

    //执行文件系统命令
    bool runFileOrder(const vector<string>&fileorder);

    //执行其他命令
    //TODO


    void setOrder(string order) {
        this->order=std::move(order);
    }
private:
    string order;

private:
    vector<string>split();//按字符串分割
};


#endif //FILESYSTEM_SHELL_H
