//
// Created by 26656 on 2023/4/26.
//

#include "diskBlock.h"

bool diskBlock::diskdelete(int blocknum) {
    //删除块
    for(int i=0;i<BLOCK_SIZE;i++){
        data[blocknum][i]='\0';
    }
    //修改bitmap
    bitmap[blocknum]=0;
    this->remainspce+=BLOCK_SIZE;
    return true;

}

char *diskBlock::diskread(int blocknum) {
    return data[blocknum];
}

bool diskBlock::diskallocation(void *data,inode*inode){
   //将data 转为 char* 类型 并且按照每BLOCK_SIZE分割
   char*char_data=(char*)data;
   int size=inode->size;
   int block_num=size/BLOCK_SIZE;
    if(size%BLOCK_SIZE!=0){
         block_num++;
    }
    //判断是否有足够的空间
    if(block_num*BLOCK_SIZE>remainspce){
        return false;
    }
    //根据块数选择哪一级索引
    if(block_num<=12){
        //直接索引
        for(int i=0;i<block_num;i++){
            int free_block=get_free_block();
            if(free_block==-1){
                return false;
            }
            bitmap[free_block]=1;
            inode->direct_blocks[i]=free_block;
            //将数据写入该磁盘块,不够的补0
            for(int j=0;j<BLOCK_SIZE;j++){
                if(i*BLOCK_SIZE+j>=size){
                    this->data[free_block][j]='\0';
                }
                else{
                    this->data[free_block][j]=char_data[i*BLOCK_SIZE+j];
                }
            }
        }
    }
    //二级索引
    else if(block_num<=12+16){
        //一级索引
        int free_block=get_free_block();
        if(free_block==-1){
            return false;
        }
        bitmap[free_block]=1;
        inode->indirect_block=&bitmap[free_block];
        //直接索引
        for(int i=0;i<12;i++){
            int free_block=get_free_block();
            if(free_block==-1){
                return false;
            }
            bitmap[free_block]=1;
            inode->direct_blocks[i]=free_block;
            //将数据写入该磁盘块,不够的补0
            for(int j=0;j<BLOCK_SIZE;j++){
                if(i*BLOCK_SIZE+j>=size){
                    this->data[free_block][j]='\0';
                }
                else{
                    this->data[free_block][j]=char_data[i*BLOCK_SIZE+j];
                }
            }
        }
        //间接索引
        //如果没有分配，则分配
        //申请一块长度为16的数组空间
        if(inode->indirect_block== nullptr)
        inode->indirect_block =new int[16];
        //分配
        for(int i=0;i<block_num-12;i++){
            int free_block=get_free_block();
            if(free_block==-1){
                return false;
            }
            bitmap[free_block]=1;
            inode->indirect_block[i]=free_block;
            //将数据写入该磁盘块,不够的补0
            for(int j=0;j<BLOCK_SIZE;j++){
                if((i+12)*BLOCK_SIZE+j>=size){
                    this->data[free_block][j]='\0';
                }
                else{
                    this->data[free_block][j]=char_data[(i+12)*BLOCK_SIZE+j];
                }
            }
        }

    }
    this->remainspce-=block_num*BLOCK_SIZE;
    return true;

}

int diskBlock::get_free_block() {
    for(int i=0;i<BLOCK_NUM;i++){
        if(bitmap[i]==0){
            return i;
        }
    }
    return -1;
}
