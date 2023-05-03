#include "device.h"


// 获取设备数量
int getDCTSize(DCT dct){
    return dct.size;
}

// 获取设备ID
int getDeviceID(DCTItem ditem){
    return ditem.deviceID;
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
wpList getwpList(DCTItem ditem){
    return getWPro(ditem).wplist;
}
