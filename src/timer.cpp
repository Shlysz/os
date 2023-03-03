#include"timer.h"

Timer::Timer()
{
    timer_struct* timer=new timer_struct;
    timer->next=timer;
    timer->id=-1;
    timer->period=defaultPeriod; 
    this->head=timer;
    timer_struct *tmp=timer;
    timer_struct *pre=NULL;   
    //初始化一个循环链表

    for(int i=0;i<defaultTimerNum-1;++i)
    {
        timer_struct*tmpNode=new timer_struct;
        tmpNode->id=-1;
        tmpNode->period=defaultPeriod;
        tmpNode->next=NULL;
        tmp->next=tmpNode;
        pre=tmp;
        tmp=tmp->next;  
    }
    pre->next=head;
    freePlace=head;
    tmp=NULL;
}

bool Timer::add_timer(void)//void 可能要改为PCB
{
    if(freePlace->id!=-1)
    return false;//满了 
    /*后续考虑动态增加队列*/

    /*************这里要改为pid**********/
    freePlace->id=0;
    /********这里改为cpu时间片的长度********/
    freePlace->period=0;
    freePlace->expire=(int)std::time(0)+freePlace->period;
    freePlace=freePlace->next;
    return true;
}


void Timer::timer_handler()//时间轮+FCFS
{
    
    
    timer_struct *p=head;
    //检查所有定时器是否到期
    while(1)//如果等于-1,说明当前定时器队列为空
    {
        int currentTime=std::time(0);
        if(p->id!=-1)
        {
            if(currentTime>=p->expire)//当前进程开始调度
            {
                //调用进程从 就绪->运行 的api
                //进程运行
                cout<<"进程"<<p->id<<"正在运行"<<endl;
                //运行的进程可能要阻塞
                // 可能要引入中断
            
            }
            p->period-=defaultPeriod;
            if(p->period<=0)
            {
                //调用 进程从 运行->结束 的api
                //进程结束
                p->id=-1;
            }
            else
            {
                //调用进程 从 运行->就绪 的api
                //进程挂起等待再分配
                p->id=-1;
                this->add_timer();//
            }
        }

        p=p->next;
    }
}