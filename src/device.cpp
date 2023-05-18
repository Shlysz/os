#include "device.h"

#define dIDlist[32];

// 获取设备数量
int getDCTSize(DCT dct){
    return dct.size;
}

// 获取设备ID
int getDeviceID(DCTItem *ditem){
    return ditem->deviceID;
}

// 获取设备状态
bool getDeviceBusy(DCTItem ditem){
    return ditem.busy;
}

// 获取等待队列信息
wPro getWPro(DCTItem ditem){
    return ditem.wpro;
}

// 获取等待队列
wpList *getwpList(DCTItem ditem){
    return getWPro(ditem).wplist;
}

// 设备快初始化
void init_device(DCT dct){
    dct.size = 0;
    dct.DCTitem = NULL;
}

// 连接设备
int register_device(DCT dct, int deviceID){
    for(int i = 0; i < getDCTSize(dct); i++){
        if(getDeviceID(dct.DCTitem) == deviceID){
            return 0;
        }
        dct.DCTitem = dct.DCTitem->next;
    }

    dct.DCTitem->busy = false;
    dct.DCTitem->deviceID = deviceID;
    dct.DCTitem->usingPid = NULL;

    wPro wpro;
    wpro.size = 0;
    wpro.waitingEnd = NULL;
    wpro.waitingHead = NULL;
    wpro.wplist = NULL;
    dct.DCTitem->wpro = wpro;

    dct.DCTitem->next = NULL;
}