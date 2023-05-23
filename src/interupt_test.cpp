#include "interupt.h"

#include "shell.h"
#include "ui.h"

int test() {
    // init_device();
    //  cout << "init device" << endl;
    Interupt inte1, inte2;
    Interupt inte3, inte4, inte5;
    Interupt inte6;
    Interupt inte7;
    inte1.raise_time_interupt(55);
    inte2.raise_time_interupt(55);
    inte3.raise_time_interupt(55);
    inte4.disable_time_interupt(55);
    inte5.disable_time_interupt(55);

    while (!process_info_queue.empty()) {
        auto t = process_info_queue.front();
        process_info_queue.pop();
        cout << t.first << ' ' << t.second << endl;
    }

    return 0;
}