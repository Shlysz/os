//
// Created by 26656 on 2023/3/19.
//

#ifndef FILESYSTEM_FILE_H
#define FILESYSTEM_FILE_H

#include <vector>
#include <unordered_map>
#include "iostream"
using  namespace std;

//新增inode结构
struct Inode {
    time_t create_time; // 创建时间
    time_t modify_time; // 修改时间
    size_t size; // 文件大小
    int type; // 文件类型 0-目录, 1-文件, 2-链接
    int link_num; // 链接数
    int direct_blocks[3]; // 直接块索引
    int* indirect_block; // 一次间接块索引
    int* double_indirect_block[4]; // 二次间接块索引
    int indextype;//索引类型

    Inode(){
        create_time = time(nullptr);
        modify_time = create_time;
        size = 0;
        type = 0;
        link_num = 0;
        for(int & direct_block : direct_blocks) direct_block = -1;
        indirect_block= nullptr;
        indextype=-1;
        for(auto i:double_indirect_block) i= nullptr;
        //double_indirect_block = nullptr;
    };
    ~Inode(){
        if(indirect_block) free(indirect_block);
        for(auto i:double_indirect_block) if(i) free(i);
    }
};


//文件节点
struct FileNode
{
    string name;//文件名字
    vector<FileNode*>children;//子节点
    FileNode*parent;
    int type;//文件类型, 0表示目录,1表示文件,2表示链接

    //其他属性
    FileNode(string name,int type):name(name),type(type){
        //inode.double_indirect_block = nullptr;
    };
};

//fcb
class FileControlBlock
{
public:
    Inode inode;//inode结构
    void*data;//数据
    //其他属性

    FileControlBlock(Inode inode):inode(inode){
      
    };
    ~FileControlBlock(){
       
    }
};

//文件类
class File{
public:
    FileNode*node;//文件节点
    FileControlBlock*fcb;//文件控制块
    File(FileNode*node,FileControlBlock*fcb):node(node),fcb(fcb){};
    ~File(){delete fcb;};
public:
    size_t read(void*buffer,size_t size);//读文件
    size_t write(void*buffer,size_t size);//写文件

};


//文件系统
class FileSystem
{
public:
    FileNode*root;//根节点
    FileNode*current;//当前节点
    unordered_map<FileNode*,File*>files;//文件表

    FileSystem():root(new FileNode("/",0)){
        root->parent= nullptr;
        current=root;
    };

    //TODO
    ~FileSystem(){ removeFileSystemTree(root);};//删除文件树
public:
    void ls();//展示当前目录下的文件/子目录
    bool cd(string path);//转到指定目录
    bool mkdir(string name);//创建目录
    bool create(string name,size_t size);//创建文件
    bool remove(string name);//删除
    File*open(string name);//打开指定文件
private:
    void deleteFileSystemTree(FileNode*node);//删除指定目录及其子目录
    void removeFileSystemTree(FileNode*node);//删除指定目录
    FileNode*getParent(FileNode*node);//获取父节点
    FileNode*findNode(FileNode*node,string path);//寻找指定节点

    //功能函数
    //bool hasTheNode(FileNode*node,FileNode*Parent);//查看是否有这个节点
};


#endif //FILESYSTEM_FILE_H


