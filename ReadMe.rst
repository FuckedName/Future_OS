**类操作系统：**
当前代码在模拟器运行有问题，在笔记本运行没有问题（有些BUG）

**基线代码：**
基于Tag:edk2-stable202105

编译及运行方法：
1、进入代码目录:
cd /home/x/UEFI/edk22;

2、编译：
EmulatorPkg/build.sh -a X64

3、把A_xxx.efi拷贝到U盘（U盘需要有启动shell.efi用于进入命令行）
Build/EmulatorX64/DEBUG_GCC5/X64/A.efi

4、进入A.efi所在分区：
比如我的是fs3:
shell>fs3:回车

5、运行A.efi
shell>A.efi回车

相关介绍视频地址：
https://www.bilibili.com/video/BV17M4y1V7dh
https://www.bilibili.com/video/BV11q4y1M7S3
https://www.bilibili.com/video/BV1Uo4y1U7Au
https://www.bilibili.com/video/BV1Hh411i7NX
https://www.bilibili.com/video/BV18M4y1L7wJ
https://www.bilibili.com/video/BV1G64y1v7hy

EDK2编译环境搭建、编译、在模拟器运行、在笔记本运行：
https://blog.csdn.net/r77683962/article/details/118640380?spm=1001.2014.3001.5501
配套代码地址：https://gitee.com/dream-os/edk22/tree/edk2-stable202105-base
代码地址：https://gitee.com/dream-os/edk22/blob/edk2-stable202105-base/MdeModulePkg/Application/A/A.c

参考书籍:
《30天自制操作系统》
《OrangeS:一个操作系统的实现》
linux 内存看一篇就够了(多图) https://www.jianshu.com/p/a563a5565705
《操作系统概念 第7版》
《中华人民共和国汉字区位码表(GB2312-80).pdf》
《华为技术有限公司c语言编程规范》
《553657 UEFI原理与编程 戴正华(著) 带书签目录》
《NTFS文件系统根目录结构_孙维连》
《NTFS文件系统结构分析》
《数据恢复技术(第2版)-戴士剑》
《大话设计模式》
《FAT32文件系统格式详解》
《计算机图形学》
《数据结构》