//
// Created by 26656 on 2023/5/18.
//

#include "FileMethod.h"

#include <utility>
#include <sstream>
#include "global.h"
bool FileMethod::writeByte(string name) {
    File*file=fs->open(std::move(name),1);
    if(file== nullptr) return  false;
    //先使用stream流接受输入，然后赋值到char*上，并计算长度
    string buffer;
    stringstream ss;
    cout<<"please input your data:"<<endl;
    while(getline(cin,buffer)){
        if(buffer.empty())
            break;
        ss<<buffer<<endl;
    }
    string input=ss.str();
    char*data=new char[input.length()];
    strcpy(data,input.c_str());
    if(!fs->write(file,data,input.length())) {
        fs->close(file);
        delete[] data;
        data= nullptr;
        return false;
    }

    return true;
}

int FileMethod::readByte(string name) {
    File*file=fs->open(std::move(name),0);
    if(file== nullptr) return  -1;
    //得到文件的长度
    int length=file->inode->size;
    char*data=new char[length];
if(!fs->read(file,data)) {
        fs->close(file);
        delete[] data;
        data= nullptr;
        return -1;
    }
    cout<<"your data is:"<<endl;
    cout<<data;
    return length;
}
