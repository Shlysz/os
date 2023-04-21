//
// Created by 26656 on 2023/4/5.
//

#include <sstream>
#include <cstring>
#include "globalVariable.h"

bool shell::analyse() {
    //将order按照空格分割
    vector<string> res = split();
    if (res.empty())
        return false;
    if (res[0] == "file")
        if (runFileOrder(res)) return true;
        else return false;

    return false;
}


vector<string> shell::split() {
    vector<string> res;
    stringstream ss(order);
    string tmp;
    while (ss >> tmp) {
        res.push_back(tmp);
    }

    return res;
}

bool shell::runFileOrder(const vector<string> &fileorder) {
    if (fileorder.size() < 2) return false;
    if (fileorder[1] == "mkdir") {
        if (fileorder.size() < 3 || fileorder.size() >= 4) return false;
        const string &name = fileorder[2];//文件夹名称
        if (fs->mkdir(name)) return true;
        else return false;
    }
    if (fileorder[1] == "create") {
        if (fileorder.size() < 3 || fileorder.size() >= 4) return false;
        if (fs->create(fileorder[2]) )return true;
        else return false;
    }
    if (fileorder[1] == "cd") {
        string path;
        if (fileorder.size() < 3)
            path = "/";
        else
            path = fileorder[2];
        cout << "path: " << path << endl;
        if (fs->cd(path)) return true;
        else return false;
    }
    if (fileorder[1] == "ls") {
        if (fileorder.size() != 2) return false;
        fs->ls();
        return true;
    }
    if (fileorder[1] == "rm") {
        if (fileorder.size() < 3 || fileorder.size() >= 4) return false;
        if (fs->remove(fileorder[2])) return true;
        else return false;
    }
    if (fileorder[1] == "write") {
        if (fileorder.size() < 3 || fileorder.size() >= 4) return false;
        //输入内容

        File *file = fs->open(fileorder[2]);
        if(file== nullptr) return false;
        cout<<"input:"<<endl;
        char *buffer;
        buffer = new char[64];
        cin.getline(buffer, 64);
        size_t length = strlen(buffer) ;
        file->write(buffer, length);
        delete[] buffer;
        return true;

    }
    if(fileorder[1]=="cat")
    {
        if(fileorder.size()<3||fileorder.size()>=4) return false;
        File*file=fs->open(fileorder[2]);
        char*buffer=new char[file->fcb->inode.size];
        file->read(buffer,file->fcb->inode.size);
        cout<<buffer<<endl;
        delete[]buffer;
        return true;
    }
    return false;
}




