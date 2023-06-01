## Operating System Simulator

2023 Spring BUPT OS Curriculum Design

### Dependencies

- GNU C Compiler
- Unix environment

### Build & Compile

```cpp
cd src/
g++ -std=c++17 -w -O2 *.cpp -o os && ./os
```
Note: If you use a Windows system, please add `-fexec-charset=GBK` to the compilation parameters so that Chinese can be displayed normally in the terminal.

### License
The GNU General Public License Version 3
