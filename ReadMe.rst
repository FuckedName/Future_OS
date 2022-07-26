**类操作系统：**
作者：任启红
日期：20211103

当前代码分支及目录：Split分支MdeModulePkg/Application/A目录

系统描述：当前整个系统分成以下几个模块：设备管理模块、图层管理模块、基础库模块、内存管理模块、分区管理模块、进程管理模块

    1、设备管理模块：代码在根目录下Devices目录，此模块包含的设备有键盘（Keyboard目录）、鼠标（Mouse目录）、存储设备（Mouse目录）、

       定时器（Timer目录），以后准备把显示器、网络也放过来，网络这块比较大或者在根目录新建一个目录。

    2、图层管理模块：代码在Graphics目录，

    3、基础库模块：代码在Libraries目录，主要数据结构、数学库、内存库、网络库、字符串库，整个系统的基础库存放到此目录

    4、内存管理模块：代码在Memory目录

    5、分区管理模块：代码在Partitions目录），主要有FAT32、NTFS文件系统结构，后续如果需要支持其他文件系统类型，存放到此目录

    6、进程管理模块：代码在Processes目录）

文件命名，即原因解释：

    1、语法格式Ln_MouduleName_SubMouduleName.*，例如：L2_DEVICE_Keyboard.c，L2表示是第二层，L1表示第一层；

       第二个DEVICE表示是DEVICE模块，第三个Keyboard表示子模块，如果没有子模块的命名格式：Ln_MouduleName.*，

       例如：L2_GRAPHICS.c，表示是Graphics根目录下的第2层c文件，如果是头文件则为L2_GRAPHICS.h.

    2、最前面的Ln表示模块内部的分层，当前结构要求为L1一般为一个模块基础数据结构，对其他任何模块不依赖；

    3、Ln其中n建议最大到9，所有模块可以自己确定对外提供的接口列表，默认为一个模块最大层对应的头文件；

    4、模块内部Lm调用Ln，必须要求m大于等于n，也即模块内部同层的函数可以调用，以及调用层数数字更小的层级；数字小的层不准调用

       数字大的层，防止系统模块间循环依赖。

**当前发现比较难解决的问题列表：**

    1、UEFI不支持中断，操作系统级实现，不太好为应用程序提供API接口，得想办法解决；

    2、基于UEFI APP开发的特权级可能有问题；    
    
待处理事项（优先级：XXX，事项描述）：

    1、优先级：高，事项描述：给绝大多数函数写注释。

    2、优先级：高，事项描述：在HP的PC机运行存在蓝屏问题。

    3、优先级：高，事项描述：在DELL的笔记本运行只能显示少数几个按钮。

    4、优先级：中，事项描述：支持对NTFS文件系统的读取。

    5、优先级：中，事项描述：网络连接百度的支持。

    6、优先级：中，事项描述：支持进程内存独立管理。

    7、优先级：中，事项描述：

    8、优先级：低，事项描述：桌面图标下的汉字显示不对问题。

    9、优先级：中，事项描述：

    10、优先级：中，创建一个可以运行的应用程序

    11、优先级：中，事项描述：如何进行高效的DEBUG？可以快速定位蓝屏这类内存问题？

    12、优先级：中，事项描述：如何进行自动化测试，并且可以覆盖的更加全面？

    13、优先级：中，事项描述：桌面在鼠标移动时，显示有点卡，性能问题。

#### 当前已经完成的功能：

1、中文显示、支持HZK16和HZK12（汉字的16*16像素和12*12像素显示）、英文显示：

2、键盘输入、鼠标移动和点击

3、FAT32文件系统的目录、子目录、文件内容查看、NTFS可以查看目录文件，子目录及文件，文件内容；

4、支持BMP格式图片显示，图片缩小算法

5、支持多窗口

6、支持关机动画

7、支持时间日期显示

8、支持多任务，支持不同组任务不同优先级

9、当前对HP的笔记本和HP的台式机支持的比较好，不过代码也有些BUG

10、当前正在支持TCP,当前只能单向数据发送


当前代码不支持在模拟器运行，在HP笔记本运行没有问题

**基线代码：**

基于Tag:edk2-stable202105

**编译及运行方法：**

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

**相关介绍视频地址：**

https://www.bilibili.com/video/BV17M4y1V7dh

https://www.bilibili.com/video/BV11q4y1M7S3

https://www.bilibili.com/video/BV1Uo4y1U7Au

https://www.bilibili.com/video/BV1Hh411i7NX

https://www.bilibili.com/video/BV18M4y1L7wJ

https://www.bilibili.com/video/BV1G64y1v7hy


**EDK2编译环境搭建、编译、在模拟器运行、在笔记本运行：**

https://blog.csdn.net/r77683962/article/details/118640380?spm=1001.2014.3001.5501

**配套代码地址：**
https://gitee.com/dream-os/edk22/tree/edk2-stable202105-base

**代码地址：**
https://gitee.com/dream-os/edk22/blob/edk2-stable202105-base/MdeModulePkg/Application/A/A.c

**参考书籍:**

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
