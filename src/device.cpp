#include "device.h"

SDT* SDTPtr;
DCT* DCTPtr;

// extern MemPtr memPtr;
map<int, PCBptr> pidPool;

void init_device() {
	int i;
	// SDTPtr = (SDT *)memPtr->selMem;
    SDT * SDTPtr;
	SDTPtr->size = 0;
	// DCTPtr = (DCT*)(memPtr->selMem + sizeof(SDT));
    DCT * DCTPtr;
	DCTPtr->size = 0;
	for (i = 0; i < MAX_SDT_LEN; i++) {
		registe_device(i);
	}

}

int registe_device(int deviceID) {
	SDTItem* tmpSDTItemPtr;
	tmpSDTItemPtr = &(SDTPtr->SDTitem[SDTPtr->size]);
	tmpSDTItemPtr->deviceID = deviceID;
	tmpSDTItemPtr->DCTItemPtr = &(DCTPtr->DCTitem[DCTPtr->size]);

	tmpSDTItemPtr->DCTItemPtr->busy = false;
	tmpSDTItemPtr->DCTItemPtr->deviceID = deviceID;
	tmpSDTItemPtr->DCTItemPtr->QueProInfo->QueueHead = (PCBPtrQueue*)malloc(sizeof(PCBPtrQueue)); 
	tmpSDTItemPtr->DCTItemPtr->QueProInfo->Qplist = (PCBPtrQueue*)malloc(sizeof(PCBPtrQueue)); 

	// if (tmpSDTItemPtr->DCTItemPtr->waitingProInfo->waitingQueueHead != NULL) {
	tmpSDTItemPtr->DCTItemPtr->QueProInfo->QueueHead->next = NULL;
	// tmpSDTItemPtr->DCTItemPtr->waitingProInfo->wplist->next = NULL;
	// }
	
	tmpSDTItemPtr->DCTItemPtr->QueProInfo->QueueEnd = tmpSDTItemPtr->DCTItemPtr->QueProInfo->QueueHead;
	SDTPtr->size += 1;
	DCTPtr->size += 1;
	return 1;
}

int get_index_by_deviceID(int deviceID) {
	int i;
	for (i = 0; i < SDTPtr->size; i++) {
		if (SDTPtr->SDTitem[i].deviceID == deviceID)
			return i;
	}
	return -1;
}

int apply_device(int pid, int deviceID) {
	int index = get_index_by_deviceID(deviceID);
	DCTItem* tmpDCTPtr;
	PCBPtrQueue* tmpPCBPtr;

	if (index == -1) {
		printf("UNKNOW DEVICE : %d", deviceID);
		return 0;
	}
	else {
		tmpDCTPtr = SDTPtr->SDTitem[index].DCTItemPtr;
		tmpPCBPtr = (PCBPtrQueue*)malloc(sizeof(PCBPtrQueue));

		if(tmpDCTPtr->QueProInfo->QueueHead->next == NULL && tmpPCBPtr != NULL){
			tmpPCBPtr->pcbptr = pidPool[pid];
			tmpPCBPtr->next = NULL;

			tmpDCTPtr->QueProInfo->size += 1;
			tmpDCTPtr->QueProInfo->QueueEnd = tmpPCBPtr;
			tmpDCTPtr->QueProInfo->QueueHead->next = tmpPCBPtr;
			tmpDCTPtr->busy == true;
			printf("PID: %d APPLY", pid);
			return 2;		// 进程直接开始使用设备
		}
		else if (tmpDCTPtr->QueProInfo->QueueHead->next != NULL && tmpPCBPtr != NULL) {
			tmpPCBPtr->pcbptr = pidPool[pid];
			tmpPCBPtr->next = NULL;

			tmpDCTPtr->QueProInfo->size += 1;
			tmpDCTPtr->QueProInfo->QueueEnd->next = tmpPCBPtr;
			tmpDCTPtr->QueProInfo->QueueEnd = tmpPCBPtr;
			printf("PID: %d ADD", pid);
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
	PCBPtrQueue* tmpPCBPtr;

	if (index == -1) {
		printf("UNKNOW DEVICE : %d", deviceID);
		return 0;
	}
	else {
		tmpDCTPtr = SDTPtr->SDTitem[index].DCTItemPtr;
		if (tmpDCTPtr->QueProInfo->QueueHead->next->pcbptr->pid == pid) {
			// 进程移出队列
			tmpDCTPtr->QueProInfo->QueueHead->next = tmpDCTPtr->QueProInfo->QueueHead->next->next;
			tmpDCTPtr->QueProInfo->size -= 1;
			printf("PID: %d RELEASE", pid);
			// 进程释放后设备空闲
			if (tmpDCTPtr->QueProInfo->QueueHead->next == NULL) {
				tmpDCTPtr->QueProInfo->QueueEnd = tmpDCTPtr->QueProInfo->QueueHead;
				tmpDCTPtr->busy = false;
			}
			// 进程释放后等待队列下一进程开始占用设备
			else {
				tmpDCTPtr->busy = true;
				printf("PID: %d APPLY", tmpDCTPtr->QueProInfo->QueueHead->next->pcbptr->pid);
			}
			
			return 1;
		}
		else {
			printf("Wrong PID or process is waiting");
			return 2;
		}
	}
}