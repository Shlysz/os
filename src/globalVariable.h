//
// Created by 26656 on 2023/4/5.
//

#ifndef FILESYSTEM_GLOBALVARIABLE_H
#define FILESYSTEM_GLOBALVARIABLE_H

#include "file.h"
#include "shell.h"
#include "diskManager.h"
//定义全局文件系统
extern FileSystem*fs;
//定义shell命令解析器
extern shell*sh;
//定义全局disk
extern DiskManager*disk;
#endif //FILESYSTEM_GLOBALVARIABLE_H
