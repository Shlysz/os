# pragma once
# include <map>
# include <list>
# include <algorithm>
# include <stdbool.h>
# include <iostream>
# define MAX_DCT_LEN  5//基础设备数量

using namespace std;

class device{// DCT表项
    public:

    struct DCTItem {
        int deviceID;
        int busy;
        list<int>pid_list;
    };

    DCTItem dct_item;

    // DCT 设备控制表，系统中的每台设备都有一张设备控制表DCT
    list<DCTItem>dct;


    // 初始化设备信息
    void init_device();
    DCTItem &get_device_item(int device_id);
    int have_device(int device_id);
    int device_register(int deviceID);

    /**
     * 进程请求设备
     * 返回 2 ：设备为空闲状态，进程直接使用设备
     * 返回 1 ：设备占用中，进程加入等待队列
     * 返回 0 ：设备 ID 不存在
    */
    int apply_device(int pid, int deviceID);

    /**
     * 进程释放设备
     * 返回 2 ：pid 错误（pid不存在或在等待队列中）
     * 返回 1 ：进程释放设备成功
     * 返回 0 ：设备 ID 不存在
    */
    int release_device(int pid, int deviceID);

    // 显示该设备信息
    void show_device(int deviceID);

    // 显示所有设备信息
    void show_device_all();

    // 设备测试
    void test_init();
};