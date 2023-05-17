//
// Created by 26656 on 2023/4/26.
//

#ifndef FILESYSTEM_DISKBLOCK_H
#define FILESYSTEM_DISKBLOCK_H


#define BLOCK_SIZE 16
#define BLOCK_NUM 1024

#include "FileSystem.h"
class diskBlock {//磁盘块分配,主要服务于文件系统从磁盘块中存取数据
    //为了方便演示,将一个块的大小设为16字节
public:
    diskBlock(){
        maxspace=BLOCK_SIZE*BLOCK_NUM;
        remainspce=maxspace;
        //初始化bitmap
        for(int & i : bitmap){
            i=0;
        }
        //初始化data
        for(char * i : data){
            for(int j=0;j<BLOCK_SIZE+1;j++){
                i[j]='\0';
            }
        }
    }
    bool diskallocation(void*data,inode *inode);
    char*diskread(int blocknum);
    bool diskdelete(int blocknum);
    char data[BLOCK_NUM][BLOCK_SIZE+1];
private:
    int maxspace;
    int remainspce;
    //磁盘块，使用+1是因为最后有/0
    int bitmap[BLOCK_NUM]{0};//位示图
    int get_free_block();//获取空闲块
};


#endif //FILESYSTEM_DISKBLOCK_H
