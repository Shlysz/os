#include <cstring>
#include "diskManager.h"
#include "globalVariable.h"

DiskManager::DiskManager() {
    mysql = new mysqlConfig();
    //找到磁盘的总空间
    this->maxspace= mysql->execSelectSQL("select * from disktrack").size()*4;
    //找到磁盘的剩余空间
    this->remainSpace= mysql->execSelectSQL("select * from disktrack where state=0").size()*4;
    this->init();
}

DiskManager::~DiskManager() {
    delete mysql;
}

void DiskManager::init() {
    //设置bitmap
    this->bitmap=new int[this->maxspace/4];
    //初始化bitmap
    for(int i=0;i<this->maxspace/4;i++)
    {
        this->bitmap[i]=0;
    }
    //获取磁盘的空闲磁道号
    vector<vector<string>> res=mysql->execSelectSQL("select * from disktrack where state=0");
    for(int i=0;i<res.size();i++)
    {
        //设置bitmap
        this->bitmap[stoi(res[i][0])/4]=1;
    }
}

string DiskManager::getDiskInfo(int num) {
    //根据磁道号来寻找内容
    vector<vector<string>> res=mysql->execSelectSQL("select * from disktrack where id="+to_string(num));
    if(res.empty())
    {
        return "";
    }
    else
    {
        return res[0][2];
    }
}

vector<int> DiskManager::getNullDiskTrack() {
    //获取空闲磁道号
    vector<int>diskTrack;
    for(int i=0;i<this->maxspace/4;i++)
    {
        if(this->bitmap[i]==0)
        {
            diskTrack.push_back(i);
        }
    }
    return diskTrack;
}

bool DiskManager::diskAllocation(Inode inode, void *data) {
    vector<int>block= analysis(inode);
    //将data分为每四个字节的大小
    vector<string>dataBlock;
    string temp;
    for(int i=0;i<sizeof(data);i++)
    {
        temp+=((char*)data)[i];
        if(temp.size()==4)
        {
            dataBlock.push_back(temp);
            temp="";
        }
    }
    if(!temp.empty())
    dataBlock.push_back(temp);
    //将dataBlock写入磁盘
    for(int i=0;i<dataBlock.size();i++)
    {
        //更新磁盘
        mysql->execUpdateSQL("update disktrack set state=1,info='"+dataBlock[i]+"' where id="+to_string(block[i]));
        //更新bitmap
        this->bitmap[block[i]]=1;
    }

    //更新剩余空间
    this->remainSpace-=block.size()*4;
    return true;

}

bool DiskManager::deleteDiskInfo(Inode inode) {
    vector<int>block = analysis(inode);
    //删除磁盘信息
    for(int i=0;i<block.size();i++)
    {
        //更新磁盘
        mysql->execUpdateSQL("update disktrack set state=0,info='' where id="+to_string(block[i]));
        //更新bitmap
        this->bitmap[block[i]]=0;
    }
    //更新剩余空间
    this->remainSpace+=block.size()*4;
    return true;
}

int DiskManager::getmaxspace() {
    return this->maxspace;
}

int DiskManager::getremainSpace() {
    return this->remainSpace;
}

vector<int> DiskManager::analysis(Inode inode) {
    vector<int>diskTrack;
    if(inode.indextype==0) //直接块
    {
        for(int direct_block : inode.direct_blocks)
            if(direct_block!=-1)
                diskTrack.push_back(direct_block);
    }
    else if(inode.indextype==1) //一级块
    {
        //获取一级块的内容
      for(int i=0;i<4;i++)
        if(inode.indirect_block[i]!=-1)
            diskTrack.push_back(inode.indirect_block[i]);
    }
    else if(inode.indextype==2) //二级块
    {
        //获取一级块的内容
        for(auto & i : inode.double_indirect_block)
            for(int j=0;j<4;j++)
            {
                if(i[j]!=-1)
                    diskTrack.push_back(i[j]);
            }
    }
    return diskTrack;
}


