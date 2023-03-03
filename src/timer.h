#include<iostream>
#include<ctime>
using namespace std;

#define defaultPeriod 3//默认时间片
#define defaultTimerNum 10//默认定时器数量



struct  timer_struct
{
    int id;
    int period;//周期
    int expire;//下一次到期的时间
    //这里可能放一些有关进程的信息
    
    timer_struct *next;//记录下一个节点位置
};
class Timer
{
public:
    //一个循环链表
    timer_struct* head;//记录现在链表的位置
    timer_struct*freePlace;//记录空闲位置
public:
    Timer();
    Timer(int period,int num);
    void timer_handler();//核心函数，用来处理到期的任务
    bool add_timer(void);//每当有进程进入就使用这个函数

};
