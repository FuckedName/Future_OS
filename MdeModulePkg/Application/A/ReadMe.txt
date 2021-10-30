/** //by renqihong

ToDo:
1. Read File From Path; OK====> optimize point: when blocks get from FAT table are continuous, can read from disk once, it can reduce, disk io.
2. Fix trigger event with mouse and keyboard; OK
3. NetWork connect baidu.com; ==> driver not found
4. File System Simple Management; ==> current can read and write, Sub directory read,NTFS almost ok.
5. progcess; ==> current mouse move, keyboard input, read file they can similar to process, but the real process is very complex, for example: 
    a.register push and pop, 
    b.progress communicate, 
    c.progress priority,
    d.PCB
    e.semaphoe
    etc.
6. My Computer window(disk partition)  
    ==> finish partly
    ==> read partitions root path items need to save to cache, for fastly get next time, and can reduce disk io...
    ==> partition volumn name
7. Setting window(select file, delete file, modify file) ==> 0%
8. Memory Simple Management 
    ==> can get memory information, but it looks like something wrong.
    ==> get memory information successfully.
9. Multi Windows, button click event. ==> 10%
10. Application. ==>10%
11. How to Automated Testing? ==>0%
12. Graphics run slowly. ===> need to fix the bug.
13. Desk wallpaper display successfully..
14. Need to rule naming, about function name, variable name, struct name, and etc.
    a.function name ruled
    b.
    c.
    d.
    e.

current problems:
    1. display NTFS file system root path items system will go die...
    2. display FAT32 file system root path items almost ok, display format not very beautiful.
    3. keyboard input not ok.

常用网址
1、区位码查询 http://quwei.911cha.com/
2、C语言代码格式化 http://web.chacuo.net/formatc

注意事项
1、存放EFI目录的分区一定要是FAT32格式
2、存放EFI分区大小扇区数一定要替换宏STORE_EFI_PATH_PARTITION_SECTOR_COUNT
3、桌面图片文件和HZK16文件一定要存放在这个分区的根目录

常见问题
1、SecureCRT不能连接LINUX UBUNTU，PING不通，需要把LINUX 网络断开重连下
2、

一些参考资料
区位码:
http://witmax.cn/gb2312.html

汉字的区位码查询:
http://quwei.911cha.com/

中文显示：
https://blog.csdn.net/zenwanxin/article/details/8349124?utm_medium=distribute.pc_relevant.none-task-blog-2~default~baidujs_baidulandingword~default-0.control&spm=1001.2101.3001.4242

英文显示：
https://blog.csdn.net/czg13548930186/article/details/79861914
