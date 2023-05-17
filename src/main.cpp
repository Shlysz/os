#include <iostream>

#include "shell.h"
#include "ui.h"

int main() {
    // system("clear");
    UI menu;
    menu.start_shell();

    Shell shell;
    while (true) {
        int re = shell.parse();
        if (re == 0) return 0;  // shell 返回 0 表示退出程序
    }
    return 0;
}