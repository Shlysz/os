//
// Created by 26656 on 2023/5/2.
//

#ifndef FILESYSTEM_GLOBAL_H
#define FILESYSTEM_GLOBAL_H
#include <queue>

#include "FileSystem.h"
#include "diskBlock.h"
#include "memory.h"

extern diskBlock *diskblock;
extern FileSystem *fs;
extern MMU *Mmu;

#endif  // FILESYSTEM_GLOBAL_H
