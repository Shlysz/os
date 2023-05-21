#include "device.h"

DCT* DCTPtr = (DCTptr)malloc(sizeof(DCTptr));

void init_device() {
	int i;
	// SDTPtr = (SDT *)memPtr->selMem;
    // SDT * SDTPtr;
	// SDTPtr->size = 0;
	// DCTPtr = (DCT*)(memPtr->selMem + sizeof(SDT));
    // DCT * DCTPtr;
	DCTPtr->size = 0;
	for (i = 0; i < MAX_DCT_LEN; i++) {
		// registe_device(i);
		device_register(i);
	}

}

// int registe_device(int deviceID) {
// 	SDTItem* tmpSDTItemPtr;
// 	tmpSDTItemPtr = &(SDTPtr->SDTitem[SDTPtr->size]);
// 	tmpSDTItemPtr->deviceID = deviceID;
// 	tmpSDTItemPtr->DCTItemPtr = &(DCTPtr->DCTitem[DCTPtr->size]);
// 	tmpSDTItemPtr->DCTItemPtr->busy = false;
// 	tmpSDTItemPtr->DCTItemPtr->deviceID = deviceID;
// 	tmpSDTItemPtr->DCTItemPtr->QueProInfo->QueueHead = (PCBPtrQueue*)malloc(sizeof(PCBPtrQueue)); 
// 	// tmpSDTItemPtr->DCTItemPtr->QueProInfo->Qplist = (PCBPtrQueue*)malloc(sizeof(PCBPtrQueue)); 
// 	// if (tmpSDTItemPtr->DCTItemPtr->waitingProInfo->waitingQueueHead != NULL) {
// 	tmpSDTItemPtr->DCTItemPtr->QueProInfo->QueueHead->next = NULL;
// 	// tmpSDTItemPtr->DCTItemPtr->waitingProInfo->wplist->next = NULL;
// 	// }
// 	tmpSDTItemPtr->DCTItemPtr->QueProInfo->QueueEnd = tmpSDTItemPtr->DCTItemPtr->QueProInfo->QueueHead;
// 	SDTPtr->size += 1;
// 	DCTPtr->size += 1;
// 	return 1;
// }

int device_register(int deviceID){
	DCTItem * tmpDCTItemPtr = &(DCTPtr->DCTitem[DCTPtr->size]);
	tmpDCTItemPtr->busy = 0;
	tmpDCTItemPtr->deviceID = deviceID;

	tmpDCTItemPtr->QueProInfo = (QInfoptr)malloc(sizeof(QInfoptr));
	tmpDCTItemPtr->QueProInfo->QueueHead = (PCBPtrQueue*)malloc(sizeof(PCBPtrQueueptr));
	tmpDCTItemPtr->QueProInfo->QueueEnd = (PCBPtrQueue*)malloc(sizeof(PCBPtrQueueptr));
	tmpDCTItemPtr->QueProInfo->QueueHead->next = nullptr;
	tmpDCTItemPtr->QueProInfo->size = 0;
	DCTPtr->size += 1;
	// printf("%d  and  %d\n", deviceID, DCTPtr->size);
	// show_device(deviceID);
	
	return 0;
}

int get_index_by_deviceID(int deviceID) {
	int i;
	for (i = 0; i < DCTPtr->size; i++) {
		if (DCTPtr->DCTitem[i].deviceID == deviceID)
			return i;
	}
	return -1;
}

int apply_device(int pid, int deviceID) {
	int index = get_index_by_deviceID(deviceID);
	DCTItem* tmpDCTPtr;

	if (index == -1) {
		printf("UNKNOW DEVICE : %d\n", deviceID);
		return 0;
	}
	else {
		tmpDCTPtr = &(DCTPtr->DCTitem[index]);
		
		if(tmpDCTPtr->busy == 0){
			// printf("%d\n", index);
			tmpDCTPtr->QueProInfo->QueueHead = (PCBPtrQueue*)malloc(sizeof(PCBPtrQueue));
			tmpDCTPtr->QueProInfo->QueueHead->next = (PCBPtrQueue*)malloc(sizeof(PCBPtrQueue));
			// show_device_all();
			tmpDCTPtr->QueProInfo->QueueHead->next->pcbid = pid;
			tmpDCTPtr->QueProInfo->QueueHead->next->next = nullptr;
			tmpDCTPtr->QueProInfo->QueueEnd = tmpDCTPtr->QueProInfo->QueueHead->next;
			
			tmpDCTPtr->QueProInfo->size += 1;
			tmpDCTPtr->busy += 1;
			printf("PID: %d APPLY\n", pid);
			return 2;		// 进程直接开始使用设备
		}
		else if (tmpDCTPtr->busy != 0) {

			tmpDCTPtr->QueProInfo->QueueEnd->next = (PCBPtrQueue*)malloc(sizeof(PCBPtrQueue));
			tmpDCTPtr->QueProInfo->QueueEnd->next->pcbid = pid;
			tmpDCTPtr->QueProInfo->QueueEnd->next->next = nullptr;
			tmpDCTPtr->QueProInfo->QueueEnd = tmpDCTPtr->QueProInfo->QueueEnd->next;

			tmpDCTPtr->QueProInfo->size += 1;
			tmpDCTPtr->busy += 1;
			printf("PID: %d ADD\n", pid);
			return 1;		// 进程加入等待队列
		}
		else {
			return 0;		// 理论不会进入这个else
		}
	}
}

int release_device(int pid, int deviceID) {
	int index = get_index_by_deviceID(deviceID);
	DCTItem* tmpDCTPtr;

	if (index == -1) {
		printf("UNKNOW DEVICE : %d\n", deviceID);
		return 0;
	}
	else {
		tmpDCTPtr = &(DCTPtr->DCTitem[index]);
		if (tmpDCTPtr->busy != 0 && tmpDCTPtr->QueProInfo->QueueHead->next->pcbid == pid) {
			
			// 进程移出队列
			tmpDCTPtr->QueProInfo->QueueHead->next = tmpDCTPtr->QueProInfo->QueueHead->next->next;
			tmpDCTPtr->QueProInfo->size -= 1;
			printf("PID: %d RELEASE\n", pid);
			// 进程释放后设备空闲
			if (tmpDCTPtr->QueProInfo->QueueHead->next == nullptr) {
				tmpDCTPtr->QueProInfo->QueueEnd = tmpDCTPtr->QueProInfo->QueueHead;
				tmpDCTPtr->busy -= 1;
			}
			// 进程释放后等待队列下一进程开始占用设备
			else {
				tmpDCTPtr->busy -= 1;
				printf("PID: %d APPLY\n", tmpDCTPtr->QueProInfo->QueueHead->next->pcbid);
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

void show_device(int deviceID){
	// printf("show device: %d", deviceID);
	int index = get_index_by_deviceID(deviceID);
	if (index == -1) {
		printf("UNKNOW DEVICE : %d\n", deviceID);
	}

	DCTItem * tmpDCTItemPtr = &(DCTPtr->DCTitem[index]);
	PCBPtrQueue * tmpPCBPtr = (PCBPtrQueue*)malloc(sizeof(PCBPtrQueueptr));

	printf("deviceID: \t%d\n", deviceID);

	printf("status: \t");
	if (tmpDCTItemPtr->busy == 0){
		printf("free\n");
	}else{
		printf("busy\n");
	}

	printf("device Queue: \t");
	tmpPCBPtr = tmpDCTItemPtr->QueProInfo->QueueHead;
	for (int i = 0; i < tmpDCTItemPtr->busy; i++){
		tmpPCBPtr = tmpPCBPtr->next;
		printf("%d->", tmpPCBPtr->pcbid);
	}
	printf("NULL\n");
	// }
}

void show_device_all(){
	for (int i = 0; i < MAX_DCT_LEN; i++){
		show_device(DCTPtr->DCTitem[i].deviceID);
	}
}

void test_init(){
	init_device();
    // show_device_all();

    apply_device(5, 1);
    apply_device(6, 3);
    apply_device(7, 2);
    apply_device(8, 4);
    apply_device(9, 1);
    apply_device(10, 1);
    // show_device_all();

    apply_device(1, 1);
    // show_device_all();

    apply_device(2, 1);
    // show_device_all();

    apply_device(3, 2);
    // show_device_all();

    apply_device(4, 3);
    show_device_all();
    printf("-------------------\n");

    release_device(5, 1);
    // show_device_all();

    release_device(7, 2);
    // show_device_all();

    release_device(4, 2);
    // show_device_all();

    release_device(9, 1);
    // show_device_all();

    release_device(6, 3);
	release_device(8, 4);
    show_device_all();
}