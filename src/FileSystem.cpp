//
// Created by 26656 on 2023/4/26.
//

#include "FileSystem.h"

#include <vector>

#include "global.h"

// 创建目录
bool FileSystem::mkdir(string name) {
    // 判断是否能继续挂载(如果原来有12个dirent就不能挂载了)
    int free_dirent_num;  // 空闲子项
    if ((free_dirent_num = get_current_inode_free_dirent()) == -1) {
        return false;
    }
    // 判断是否重名
    for (int i = 0; i < 12; i++) {
        if (this->inode_table[current_dirent->d_inode].i_dirent[i].d_name ==
                name &&
            this->inode_table[current_dirent->d_inode].i_dirent[i].d_inode !=
                -1)
            return false;
    }
    // 找到空的inode
    int free_inode_num = get_free_inode();
    if (free_inode_num == -1) {
        return false;
    }
    // 新建inode
    inode_table[free_inode_num] = inode(free_inode_num, 0, 0);
    // 当前目录下挂载这个dirent
    inode_table[current_dirent->d_inode].init_dirent(
        free_dirent_num, (char *)name.c_str(), 0, free_inode_num);
    // 在新建的目录下挂载 . 和 ..
    inode_table[free_inode_num].init_dirent(0, (char *)".", 0, free_inode_num);
    inode_table[free_inode_num].init_dirent(1, (char *)"..", 0,
                                            current_dirent->d_inode);
    // 修改bitmap
    bitmap[free_inode_num] = 1;
    return true;
}

// 展示该路径目录下的文件或子目录
void FileSystem::ls(string path) {
    // 获取该路径的dirent
    dirent *dir = get_dirent(path);
    if (dir == nullptr) {
        cout << "路径不存在" << endl;
        return;
    }
    for (int i = 0; i < 12; i++) {
        if (inode_table[dir->d_inode].i_dirent[i].d_inode== -1) {
            continue;
        }
        string type;
        if(inode_table[dir->d_inode].i_dirent[i].d_inode==0)  type="目录";
        else type="文件";
        cout << inode_table[dir->d_inode].i_dirent[i].d_name << " "
             << type << endl;
    }
}

bool FileSystem::cd(string path) {
    // 获取该路径的dirent
    dirent *dir = get_dirent(std::move(path));
    if (dir == nullptr) {
        cout << "路径不存在" << endl;
        return false;
    }
    if (dir->file_type != 0) {
        cout << "该路径不是目录" << endl;
        return false;
    }
    // 修改当前dirent
    current_dirent = dir;
    return true;
}

// 创建新文件
bool FileSystem::touch(const string& name) {
    // 判断是否能继续挂载
    int free_dirent_num;  // 空闲子项
    if ((free_dirent_num = get_current_inode_free_dirent()) == -1) {
        return false;
    }
    // 判断是否重名
    for (int i = 0; i < 12; i++) {
        if (this->inode_table[current_dirent->d_inode].i_dirent[i].d_name ==
                name &&
            this->inode_table[current_dirent->d_inode].i_dirent[i].d_inode !=
                -1)
            return false;
    }
    // 找到空的inode
    int free_inode_num = get_free_inode();
    if (free_inode_num == -1) {
        return false;
    }
    // 新建inode
    inode_table[free_inode_num] = inode(free_inode_num, 1, 0);
    // 当前目录下挂载这个dirent
    inode_table[current_dirent->d_inode].init_dirent(
        free_dirent_num, (char *)name.c_str(), 1, free_inode_num);
    // 修改bitmap
    bitmap[free_inode_num] = 1;
    return true;
}

bool FileSystem::rm(const string& name) {
    // 删除文件和目录
    // 判断是否存在
    int inode_num = -1;
    for (int i = 0; i < 12; i++) {
        if (this->inode_table[current_dirent->d_inode].i_dirent[i].d_name ==
                name &&
            this->inode_table[current_dirent->d_inode].i_dirent[i].d_inode !=
                -1) {
            inode_num =
                this->inode_table[current_dirent->d_inode].i_dirent[i].d_inode;
            break;
        }
    }
    if (inode_num == -1) {
        return false;
    }
    // if是目录
    if (inode_table[inode_num].mode == 0) {
        // 递归删除该目录以及目录下的文件/目录
        rmdir(inode_num);
        for (int i = 0; i < 12; i++) {
            if (this->inode_table[current_dirent->d_inode].i_dirent[i].d_name ==
                    name &&
                this->inode_table[current_dirent->d_inode]
                        .i_dirent[i]
                        .d_inode != -1) {
                this->inode_table[current_dirent->d_inode].i_dirent[i] =
                    dirent();
                break;
            }
        }
    }
    // 是文件
    else {
        // 删除dirent
        for (int i = 0; i < 12; i++) {
            if (this->inode_table[current_dirent->d_inode].i_dirent[i].d_name ==
                    name &&
                this->inode_table[current_dirent->d_inode]
                        .i_dirent[i]
                        .d_inode != -1) {
                this->inode_table[current_dirent->d_inode].i_dirent[i] =
                    dirent();
                break;
            }
        }
        // 修改bitmap
        bitmap[inode_num] = 0;
        // 删除文件内容
        // 计算文件需要的块数
        int block_num = inode_table[inode_num].size / 16;
        if (inode_table[inode_num].size % 16 != 0) {
            block_num++;
        }
        if (block_num <= 12) {
            for (int i = 0; i < block_num; i++) {
                diskblock->diskdelete(inode_table[inode_num].direct_blocks[i]);
            }
        } else {
            for (int direct_block : inode_table[inode_num].direct_blocks) {
                diskblock->diskdelete(direct_block);
            }
            int indirect_block_num = block_num - 12;
            for (int i = 0; i < indirect_block_num; i++) {
                diskblock->diskdelete(inode_table[inode_num].indirect_block[i]);
            }
        }

        // 删除inode
        inode_table[inode_num] = inode();
    }
    return true;  // added by rxb
}

File *FileSystem::open(string name, int mode) {
    // 打开文件,并返回指针
    // 判断是否存在
    int inode_num = -1;
    for (int i = 0; i < 12; i++) {
        if (this->inode_table[current_dirent->d_inode].i_dirent[i].d_name ==
                name &&
            this->inode_table[current_dirent->d_inode].i_dirent[i].d_inode !=
                -1) {
            inode_num =
                this->inode_table[current_dirent->d_inode].i_dirent[i].d_inode;
            break;
        }
    }
    if (inode_num == -1) {
        cout << "文件不存在" << endl;
        return nullptr;
    }
    // 判断是否是文件
    if (inode_table[inode_num].mode == 0) {
        cout << "该路径是目录" << endl;
        return nullptr;
    }
    // 找到inode并封装到File*
    File *file = new File();
    file->inode = &inode_table[inode_num];
    file->open_mode = mode;
    file->name = name;
    if (mode == 0) {
        file->offset = file->inode->size + 1;
    } else {
        file->offset = 0;
    }
    return file;
}

bool FileSystem::close(File *file) {
    // 关闭文件
    file->inode = nullptr;
    file->open_mode = -1;
    file->offset = -1;
    file->name = "";
    file = nullptr;
    return true;
}

bool FileSystem::rmdir(int inode_num) {
    // 删除当前目录下的挂载这个目录
    // 递归删除目录
    // 删除dirent
    for (int i = 2; i < 12; i++) {
        if (inode_table[inode_num].i_dirent[i].d_inode != -1) {
            if (inode_table[inode_num].i_dirent[i].file_type == 0) {
                rmdir(inode_table[inode_num].i_dirent[i].d_inode);
                // 修改bitmap
                bitmap[inode_table[inode_num].i_dirent[i].d_inode] = 0;
            } else {
                // 删除文件
                int file_inode = inode_table[inode_num].i_dirent[i].d_inode;
                // 删除文件内容
                // 计算文件需要的块数
                int block_num = inode_table[file_inode].size / 16;
                if (inode_table[file_inode].size % 16 != 0) {
                    block_num++;
                }
                if (block_num <= 12) {
                    for (int k = 0; k < block_num; k++) {
                        diskblock->diskdelete(
                            inode_table[file_inode].direct_blocks[k]);
                    }
                } else {
                    for (int direct_block :
                         inode_table[file_inode].direct_blocks) {
                        diskblock->diskdelete(direct_block);
                    }
                    int indirect_block_num = block_num - 12;
                    for (int k = 0; k < indirect_block_num; k++) {
                        diskblock->diskdelete(
                            inode_table[file_inode].indirect_block[k]);
                    }
                }

                // 删除inode
                inode_table[file_inode] = inode();
                // 修改bitmap
                bitmap[file_inode] = 0;
                // 删除dirent
                inode_table[inode_num].i_dirent[i] = dirent();
            }
        }
    }
    return true;
}

bool FileSystem::write(File *file, char *content, int length) {
    file->inode->size = length;
    if (diskblock->diskallocation(content, file->inode)) {
        cout << "成功写入" << endl;
        file->offset = length + 1;
        return true;
    } else
        return false;
}

bool FileSystem::read(File *file, char *content) {
    // 从inode找出block的数量
    int block_num = file->inode->size / 16;
    if (file->inode->size % 16 != 0) block_num++;
    // content=new char[file->inode->size];
    if (block_num <= 12) {
        // 从直接块中读取并写入content中
        for (int i = 0; i < block_num; i++) {
            char *ret = diskblock->diskread(file->inode->direct_blocks[i]);
            for (int j = 0; j < 16; j++) {
                content[i * 16 + j] = ret[j];
            }
        }
    } else {
        // 从一级间接块中读取并写入content中
        for (int i = 0; i < 12; i++) {
            char *ret = diskblock->diskread(file->inode->direct_blocks[i]);
            for (int j = 0; j < 16; j++) {
                content[i * 16 + j] = ret[j];
            }
        }
        // 从二级间接块中读取并写入content中
        for (int i = 0; i < block_num - 12; i++) {
            char *ret = diskblock->diskread(file->inode->indirect_block[i]);
            for (int j = 0; j < 16; j++) {
                content[(i + 12) * 16 + j] = ret[j];
            }
        }
    }
    file->offset = file->inode->size + 1;
    // cout<<"read"<<content<<endl;
    return true;
}

int FileSystem::get_current_inode_free_dirent() {
    int node_num = current_dirent->d_inode;
    for (int i = 0; i < 12; i++) {
        if (inode_table[node_num].i_dirent[i].d_inode == -1) {
            return i;
        }
    }
    return -1;
}

dirent *FileSystem::get_dirent(string path) {
    // 解析path
    vector<string> path_list;
    string temp;
    for (int i = 0; i < path.length(); i++) {
        if (path[i] == '/') {
            path_list.push_back(temp);
            temp = "";
        } else {
            temp += path[i];
        }
    }
    path_list.push_back(temp);
    // 从根目录开始查找
    dirent *dir = root;
    int i = 0;
    for (; i < path_list.size(); i++) {
        for (int j = 0; j < 12; j++) {
            if (inode_table[dir->d_inode].i_dirent[j].d_name == path_list[i]) {
                dir = &inode_table[dir->d_inode].i_dirent[j];
                break;
            }
        }
    }
    if (i != path_list.size()) {
        return nullptr;
    }
    return dir;
}
