//磁盘调度
#include"mysqlConfig.h"
#include<file.h>
class DiskManager {
public:
    DiskManager();
    ~DiskManager();
    void init();//初始化磁盘(获取到磁盘的空闲磁道号)
    //void initDisk();//初始化磁盘(获取到磁盘的空闲磁道号)
    string getDiskInfo(int num);//根据磁道号来寻找内容
    vector<int> getNullDiskTrack();//获取空闲磁道号
    //磁盘分配
    bool diskAllocation(Inode inode,void*data);
    bool deleteDiskInfo(Inode inode);//删除磁盘信息
    int getmaxspace();//获取磁盘总空间
    int getremainSpace();//获取磁盘剩余空间
    vector<int> analysis(Inode inode);//通过inode分析磁盘分配号
    
private:
    mysqlConfig *mysql;
    int maxspace;//总空间
    int remainSpace;//剩余空间
    int*bitmap;//位示图
    int getBestway(vector<int>diskTrack);//磁道数最少的寻道方案
};