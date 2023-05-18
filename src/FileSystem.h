//
// Created by 26656 on 2023/4/26.
//

#ifndef FILESYSTEM_FILESYSTEM_H
#define FILESYSTEM_FILESYSTEM_H

#include <ctime>
#include <cstring>
#include "iostream"
using namespace std;
#define DIR_NUM 12


class dirent{//目录项

public:
    int d_inode{};//索引节点号
    int d_name_length{};//文件名长度
    int file_type{};//文件类型
    char d_name[256];//目录或文件中的文件名

public:
    dirent(){ d_inode=-1;};
    dirent(int inode, char*name,int file_type) {
        d_inode = inode;
        d_name_length = strlen(name);
        strcpy(d_name, name);
        this->file_type = file_type;
    }
};

struct inode{
    int id;//编号
    int mode;//文件类型0：目录 1：普通
    int size;//大小
    int link_num;//链接数
    time_t create_time;//创建时间
    time_t  modify_time;//最后访问时间
    //12个直接块指针
    int direct_blocks[12]{};
    //一级间接块指针
    int *indirect_block;
    //二级间接块指针
    int **double_indirect_block;
   dirent *i_dirent{nullptr};//目录项

    inode(){ //初始化
        id=-1;
        mode=0;
        size=0;
        link_num=0;
        create_time=0;
        modify_time=0;
        for(int & direct_block : direct_blocks){
            direct_block=-1;
        }
        indirect_block= nullptr;
        double_indirect_block= nullptr;

    }

    inode(int id,int mode,int size){
        this->id=id;
        this->mode=mode;
        this->size=size;
        this->link_num=1;
        this->create_time=time(nullptr);
        this->modify_time=time(nullptr);
        for(int & direct_block : direct_blocks){
            direct_block=-1;
        }
        indirect_block= nullptr;
        double_indirect_block= nullptr;
        if(mode==0) {
            i_dirent = new  dirent[DIR_NUM];
        }
    }

    //初始化一块dirent
     void init_dirent(int index,char*name,int file_type,int node_num){
        i_dirent[index]=dirent(node_num,name,file_type);
    }
};
struct File{
    struct inode* inode;//指向对应的inode;
    int offset;//文件偏移量(字节)
    string name;//文件名
    int open_mode;//打开方式(0为追加,1为写入)
    void*data;//暂时存放数据,以便将数据存入磁盘块中
};


class FileSystem {
    //文件系统
private:
    inode inode_table[128]{};//一共有128个inode
    //128位的bitmap
    int bitmap[128]{0};//0表示空闲，1表示占用

    dirent *root;//根节点指针
public:
    dirent *current_dirent{};//当前节点的指针
    FileSystem(){
        //初始化文件系统
        //初始化一个root节点
        string name="root";
        root=new dirent(0,(char*)name.c_str(),0);
        //初始化一个inode
        inode_table[0]=inode(0,0,0);
        for(int i=0;i<12;i++) {
            inode_table[0].i_dirent[i].d_inode = -1;
        }
        inode_table[0].init_dirent(0,(char*)".",0,0);
        //初始化一个bitmap
        bitmap[0]=1;
        current_dirent=root;
    }

    bool mkdir(string name);
    void ls(string path);
    bool cd(string path);
    bool touch(string name);
    bool rm(string name);
    bool rmdir(int inode_num);
    File* open(string name,int mode);
    bool close(File* file);
    bool write(File* file,char* content,int length);
    bool read(File* file,char* content);

     string getcurrent_dirent_location()
    {
        return this->current_dirent->d_name;
    }
private:
    //获取空闲的inode号
    int get_free_inode(){
        for(int i=0;i<128;i++){
            if(bitmap[i]==0){
                return i;
            }
        }
        return -1;
    }
    int get_current_inode_free_dirent();
    dirent* get_dirent(string path);
};

#endif //FILESYSTEM_FILESYSTEM_H
//生成数据结构说明