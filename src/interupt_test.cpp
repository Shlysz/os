#include "device.h"
#include "interupt.h"
#include "process.h"
#include "shell.h"
#include "ui.h"

int test() {
    init_device();
    // cout << "init device" << endl;
    Interupt inte1;
    // inte1.type = 3; // require
    // inte2.type = 4;  //release
    inte1.raise_device_interupt(888, 2);  // pid =3 ,device_id = 2
    cout << "raise 888";
    // inte2.raise_device_interupt(999, 3);
}