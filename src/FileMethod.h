//
// Created by 26656 on 2023/5/18.
//

#ifndef MYOS_FILEMETHOD_H
#define MYOS_FILEMETHOD_H
#include "FileSystem.h"

class FileMethod{
    public:
        //静态类
        static bool writeByte(string name);
        static int readByte(string name);
};


#endif //MYOS_FILEMETHOD_H
