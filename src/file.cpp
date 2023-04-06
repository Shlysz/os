//
// Created by 26656 on 2023/3/19.
//

#include <cstring>
#include <algorithm>
#include "file.h"
#include <sstream>
size_t File::read(void *buffer, size_t size) {//读指定文件
    if(this->fcb->type!=1) {//不是文件类型
        return 0;
    }
    if(size>fcb->size)
    {
        size=fcb->size;
    }
    memcpy(buffer,fcb->data,size);
    return size;
}

size_t File::write(void *buffer, size_t size) {//写指定文件
    if(fcb->type!=1) return 0;
    if(size>fcb->size)
    {
        size=fcb->size;
    }
    memcpy(fcb->data,buffer,size);
    return size;
}

File *FileSystem::open(string name) {//打开文件
    FileNode*node=findNode(current,name);//从当前目录寻找文件
    if(node && files.count(node)){
        return files[node];
    }
    else{
        return nullptr;
    }
}


bool FileSystem::remove(string name) {//删除文件或目录
    FileNode*node= findNode(current,name);//找到删除的节点
    if(node)
    {
       auto it= files.find(node);
       if(it !=files.end())
       {
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

bool FileSystem::create(string name, size_t size) {//创建文件
    if(!findNode(current,name)){
        FileNode*node=new FileNode(name,1);
        node->parent=current;
        FileControlBlock*fcb=new FileControlBlock(1,size);
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
void FileSystem::deleteFileSystemTree(FileNode *node) {

}

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


