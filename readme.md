**IN PROGRESS** ...

## 说明

reference: [link](https://www.youtube.com/watch?v=XGSOl1QZVKI&list=PLHh55M_Kq4OApWScZyPl5HhgsTJS9MZ6M&index=5)

1. .section .multboot 段作为使用GRUB进行引导的前置条件，必须要设置
2. 设置编译选项，将所有的系统链接库全部关闭
3. linker初始化内存段，.bss没有初始化和初始化为0的全局变量，.data初始化的非0非const全局变量，rodata用const修饰的read only全局变量（ROM中可直接读，不需要载入RAM）
4. .init_array为存放构造函数的段；KEEP让其不被垃圾回收机制回收
5. gcc -m32 test.c  -fno-pie -no-pie 编译文件为 ELF 32-bit LSB executable; gcc -m32 test.c 编译文件为 ELF 32-bit LSB shared object。

其他说明放代码中。


### 镜像制作
```bash
sudo apt install grub-efi-amd64 xorriso mtools
```
使用 virtualbox 打开镜像文件
