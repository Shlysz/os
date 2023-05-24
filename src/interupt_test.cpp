#include "interupt.h"

#include "device.h"
#include "shell.h"
#include "ui.h"

int test() {
    // init_device();
    //  cout << "init device" << endl;
    Interupt inte1, inte2;
    Interupt inte3, inte4, inte5;
    Interupt inte6;
    Interupt inte7;

    device tmp;
    tmp.init_device();

    inte1.raise_device_interupt(2, 0);  // pid =2, deviceid = 0;
    inte2.raise_device_interupt(99, 1);
    inte3.raise_device_interupt(88, 2);
    inte3.raise_device_interupt(100, 1);
    inte1.disable_device_interupt(2, 0);
    inte1.disable_device_interupt(1, 1);
    inte1.disable_device_interupt(99, 1);

    tmp.show_device_all();

    // while (!process_info_queue.empty()) {
    //     auto t = process_info_queue.front();
    //     process_info_queue.pop();
    //     cout << t.first << ' ' << t.second << endl;
    // }

    return 0;
}