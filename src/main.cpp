#include "process.h"
#include "shell.h"
#include "ui.h"

int main() {
    Process kernel;
    int kernel_flag = kernel.kernel_init();
    thread systemd(kernel.runKernel, kernel_flag); // 创建内核进程

    cout << "back to main" << endl;

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