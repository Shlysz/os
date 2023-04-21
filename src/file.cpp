//
// Created by 26656 on 2023/3/19.
//

#include <cstring>
#include <algorithm>
#include "file.h"
#include "globalVariable.h"
#include <sstream>
#include <utility>
size_t File::read(void *buffer, size_t size) {//读指定文件
    if(this->fcb->inode.type!=1) {//不是文件类型
        return 0;
    }
    //从数据库中读取数据
    vector<int>block=disk->analysis(this->fcb->inode);
    string data;
    for(int i=0;i<block.size();i++)
    {
        data+=disk->getDiskInfo(block[i]);
    }
    memcpy(buffer,fcb->data,size);
    return size;
}

size_t File::write(void *buffer, size_t size) {//写指定文件
    if(disk->getremainSpace()<size) return 0;//磁盘空间不足
    if(fcb->inode.type!=1) return 0;
    //重新设置磁盘分配
    disk->deleteDiskInfo(fcb->inode);
    //重新设置inode
    fcb->inode.size=size;
    fcb->inode.link_num=1;
    fcb->inode.type=1;
    //根据大小确定类型
    if(size<=4*3)
        fcb->inode.indextype=1;
    else if(size<=4*4)
        fcb->inode.indextype=2;
    else
        fcb->inode.indextype=3;
    for(int i=0;i<3;i++)
    fcb->inode.direct_blocks[i]=-1;
    if(fcb->inode.indextype) free(fcb->inode.indirect_block);
    for(auto i:fcb->inode.double_indirect_block) free(i);
    fcb->inode.indirect_block= nullptr;
    for(auto i:fcb->inode.double_indirect_block) i= nullptr;
    vector<int>freeBlock = disk->getNullDiskTrack();
    //根据类型分配磁盘
    if(fcb->inode.indextype==1)
    {
        //向上取整

        for(int i=0;i<(size-1)/4+1;i++)
        {
            fcb->inode.direct_blocks[i]=freeBlock[i];
        }

    }
    else if(fcb->inode.indextype==2)
    {
        fcb->inode.indirect_block=new int [4];
        for(int i=0;i<4;i++)
        {
            fcb->inode.indirect_block[i]=freeBlock[i];
        }
    }
    else if(fcb->inode.indextype==3)
    {
        //根据size判断需要几个二级索引块
        int num=(size-1)/16+1;
        for(int i=0;i<num-1;i++)
        {
            fcb->inode.double_indirect_block[i]=new int [4];
            for(int j=0;j<4;j++)
            {
                fcb->inode.double_indirect_block[i][j]=freeBlock[i*4+j];
            }
        }
        //最后一个二级索引块
        fcb->inode.double_indirect_block[num-1]=new int [4];
        for(int i=0;i<(size-1)%16+1;i++)
        {
            fcb->inode.double_indirect_block[num-1][i]=freeBlock[(num-1)*4+i];
        }
    }
    //将数据写入数据库
    disk->diskAllocation(fcb->inode,buffer);
    //修改inode
    fcb->inode.modify_time=time(nullptr);
    //memcpy(fcb->data,buffer,size);
    return size;
}

File *FileSystem::open(string name) {//打开文件
    FileNode*node=findNode(current,std::move(name));//从当前目录寻找文件
    if(node && files.count(node)){
        return files[node];
    }
    else{
        return nullptr;
    }
}


bool FileSystem::remove(string name) {//删除文件或目录
    FileNode*node= findNode(current,std::move(name));//找到删除的节点
    if(node)
    {
       auto it= files.find(node);

       if(it !=files.end())
       {
           disk->deleteDiskInfo(it->second->fcb->inode);//删除磁盘分配
           delete it->second;
           files.erase(it);
       }
        FileNode* parent = getParent(node);//找到node的父亲节点
        //删除该该文件或目录的索引
        parent->children.erase(remove_if(parent->children.begin(),parent->children.end(),[node](FileNode*tmp){return tmp==node;}),parent->children.end());
        removeFileSystemTree(node);
        return true;
    }
    else return false;

}

bool FileSystem::create(string name) {//创建文件
    if(!findNode(current,name)){
        auto*node=new FileNode(name,1);
        node->parent=current;
        Inode inode;
        inode.type=1;
        inode.size=0;
        inode.indextype=1;
        auto*fcb=new FileControlBlock(inode);
        files[node]=new File(node,fcb);
        current->children.push_back(node);
        return true;
    }
    else
        return false;
}

bool FileSystem::mkdir(string name) {//创建目录
  if(!findNode(current,name))
  {
      FileNode*node=new FileNode(name,0);
      node->parent=current;
      current->children.push_back(node);
      return true;
  }
  else
      return false;

}

bool FileSystem::cd(string path) {
    if(path=="/"||path.empty())
    {
        current=root;
        return true;
    }
    if(path=="..")
    {
        if(current==root) return false;
        current = getParent(current);//获得父节点
        return true;
    }
    FileNode*node= findNode(current,path);//找到节点
    if(node && node->type==0)//如果是目录
    {
        current=node;
        return true;
    }
    else return false;

}

void FileSystem::ls() {//展示当前目录下所有文件和字母录
    for(auto child:current->children)
    {
        cout<<child->name;
        if(child->type==0)//如果是目录
            cout<<"/";
        cout<<endl;
    }
}
//TODO


void FileSystem::removeFileSystemTree(FileNode *node) {
    if(node->children.size()==0)
    {
        FileNode*parent = getParent(node);
        parent->children.erase(remove_if(parent->children.begin(),parent->children.end(),[node](FileNode*tmp){return tmp==node;}),parent->children.end());
        this->files.erase(node);
        delete node;

    }
    else
    {
        for(auto it:node->children)
        {
            removeFileSystemTree(it);
        }
    }
}

FileNode *FileSystem::getParent(FileNode *node) {//找到当前节点的父节点
    return node->parent;
}

FileNode *FileSystem::findNode(FileNode *node, string path) {//找到指定节点
    //分析path
    vector<string> result;
    stringstream ss(path);
    string item;
    while(getline(ss,item,'/')){
        result.push_back(item);
    }
    FileNode*res= nullptr;
    FileNode*tmp=current;//

    int i=0;
    for( i=0;i<result.size();i++)
    {
        int j=0;
        int size=tmp->children.size();
        for(j=0;j<size;j++){
           if(tmp->children[j]->name==result[i])//如果匹配
           {
               tmp=tmp->children[j];
               res=tmp;
               break;
           }
        }

        if(j>=size)
            return nullptr;
    }

   if(i>=result.size())
    return  res;

    return nullptr;
}


