#include "device.h"

device dcb;

void device::init_device() {
	int i;
	for (i = 0; i < MAX_DCT_LEN; i++) {
		dcb.device_register(i);
	}
}

int device::device_register(int deviceID){

	device tmpdev;
	tmpdev.dct_item.deviceID = deviceID;
	tmpdev.dct_item.busy = 0;

	dcb.dct.push_back(tmpdev.dct_item);
	return 0;
}

// int get_index_by_deviceID(int deviceID) {
// 	int i;
// 	for (i = 0; i < DCTPtr->size; i++) {
// 		if (DCTPtr->DCTitem[i].deviceID == deviceID)
// 			return i;
// 	}
// 	return -1;
// }

device::DCTItem & device::get_device_item(int device_id){
	DCTItem tmpitem;
	for (auto dct_item = dcb.dct.begin(); dct_item != dcb.dct.end(); dct_item++){
		if ((*dct_item).deviceID == device_id)
			return (*dct_item);
	}
	return tmpitem;
}

int device::have_device(int device_id){
	for (auto dct_item = dcb.dct.begin(); dct_item != dcb.dct.end(); dct_item++){
		if ((*dct_item).deviceID == device_id)
			return 1;
	}
	return 0;
}

int device::apply_device(int pid, int deviceID) {
	if (dcb.have_device(deviceID) == 0) {
		printf("UNKNOW DEVICE : %d\n", deviceID);
		return 0;
	}
	else {
		device::DCTItem &item = dcb.get_device_item(deviceID);
		item.pid_list.push_back(pid);
		item.busy++;
		if (item.busy == 0){
			printf("PID: %d APPLY\n", pid);
			return 2;		// 进程直接开始使用设备
		}
		else if (item.busy != 0) {
			printf("PID: %d ADD\n", pid);
			return 1;		// 进程加入等待队列
		}
	}
	return 0;
}

int device::release_device(int pid, int deviceID) {
	if (dcb.have_device(deviceID) == 0) {
		printf("UNKNOW DEVICE : %d\n", deviceID);
		return 0;
	}
	else {
		device::DCTItem &item = dcb.get_device_item(deviceID);
		if (item.busy != 0 && *(item.pid_list.begin()) == pid) {	
			// 进程移出队列
			item.pid_list.pop_front();
			item.busy--;
			printf("PID: %d RELEASE\n", pid);
			// 进程释放后设备空闲
			if (item.busy == 0) {
				printf("deviceID %d free\n", deviceID);
			}
			// 进程释放后等待队列下一进程开始占用设备
			else {
				printf("PID: %d APPLY\n", *(item.pid_list.begin()));
			}
			return 1;
		}
		else {
			printf("Wrong PID or process is waiting\n");
			return 2;
		}
	}
	printf("access\n");
}

void device::show_device(int deviceID){
	// printf("show device: %d", deviceID);
	if (dcb.have_device(deviceID) == 0) {
		printf("UNKNOW DEVICE : %d\n", deviceID);
	}else{
		DCTItem item = dcb.get_device_item(deviceID);

		printf("deviceID: \t%d\n", deviceID);

		printf("status: \t");
		if (item.busy == 0){
			printf("free\n");
		}else{
			printf("busy\n");
		}

		printf("device Queue: \t");
		for (auto pid_item = item.pid_list.begin(); pid_item != item.pid_list.end(); pid_item++){
			printf("%d->", (*pid_item));
		}
		printf("NULL\n");
	}
}

void device::show_device_all(){
	for (auto dct_item = dcb.dct.begin(); dct_item != dcb.dct.end(); dct_item++){
		printf("deviceID: \t%d\n", (*dct_item).deviceID);

		printf("status: \t");
		if ((*dct_item).busy == 0){
			printf("free\n");
		}else{
			printf("busy\n");
		}

		printf("device Queue: \t");
		for (auto pid_item = (*dct_item).pid_list.begin(); pid_item != (*dct_item).pid_list.end(); pid_item++){
			printf("%d->", (*pid_item));
		}
		printf("NULL\n");
	}
}

void device::test_init(){
	device test;
	test.init_device();


	test.apply_device(6, 3);
    test.apply_device(7, 2);
    test.apply_device(8, 4);
    test.apply_device(9, 1);
    test.apply_device(10, 1);
    // show_device_all();

    test.apply_device(1, 1);
    // show_device_all();

    test.apply_device(2, 1);
    // show_device_all();

    test.apply_device(3, 2);
    // show_device_all();

    test.apply_device(4, 3);
    test.show_device_all();
    printf("-------------------\n");

    test.release_device(5, 1);
    // show_device_all();

   	test.release_device(7, 2);
    // show_device_all();

    test.release_device(4, 2);
    // show_device_all();

    test.release_device(9, 1);
    // show_device_all();

    test.release_device(6, 3);
    test.show_device_all();
}