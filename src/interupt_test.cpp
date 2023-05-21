#include "interupt.h"

#include "shell.h"
#include "ui.h"

int test() {
    // init_device();
    //  cout << "init device" << endl;
    Interupt inte1, inte2;
    Interupt inte3, inte4, inte5;
    inte3.raise_device_interupt(888, 2);  // pid =3 ,device_id = 2
    inte4.raise_device_interupt(999, 3);
    inte1.raise_time_interupt(55);
    inte2.disable_device_interupt(33, 3);
    Interupt inte6;
    inte6.disable_time_interupt(24);
    Interupt inte7;
    inte7.raise_panic(34, 2);
}