#pragma once

#include <string.h>

#include <Library/UefiLib.h>
//#include <Device/Mouse/L1_DEVICE_Mouse.h>
//#include <Device/Screen/L1_DEVICE_Screen.h>


/*目录项首字节含义*/
typedef enum
{
    DirUnUsed            = 0x00,        /*本表项没有使用*/
    DirCharE5            = 0x05,        /*首字符为0xe5*/
    DirisSubDir          = 0x2e,        /*是一个子目录 .,..为父目录*/
    DirFileisDeleted     = 0xe5        /*文件已删除*/
}DirFirstChar;


typedef struct {
  UINTN               Signature;
  EFI_FILE_PROTOCOL   Handle;
  UINT64              Position;
  BOOLEAN             ReadOnly;
  LIST_ENTRY          Tasks;                  // List of all FAT_TASKs
  LIST_ENTRY          Link;                   // Link to other IFiles
} FAT_IFILE;



/*
	NTFS文件系统分区访问方法：
		1、第一个扇区；数据结构DOLLAR_BOOT，获取：MFT_StartCluster的值；
		2、通过MFT_StartCluster * 8 访问MFT表，有很多项，数据结构：MFT_ITEM，访问通过偏移10个扇区访问MFT_ITEM_DOLLAR_ROOT项；
		3、每个MFT项,以FILE的ASSCI码开头（占用2个扇区，1024字节大小）包含MFT头部：数据结构：MFT_HEADER;
			还包含还有很多个属性组成(多个枚举值：MFT_ATTRIBUTE_TYPE)，属性数据结构：NTFSAttribute
			我们需要找到0x90属性和0xA0属性，并获取data runs
			0X90索引根属性，存放着该目录下的子目录和子文件的索引项；
			当某个目录下的内容比较多，从而导致0X90属性无法完全存放时，0XA0属性会指向一个索引区域，
			这个索引区域包含了该目录下所有剩余内容的索引项。
		4、
		5、

		
*/


/*
    MFT 是由一条条 MFT 项(记录)所组成的，而且每项大小是固定的(一般为1KB = 2 * 512)，MFT保留了前16项用于特殊文件记录，称为元数据，
    元数据在磁盘上是物理连续的，编号为0~15；如果$MFT的偏移为0x0C0000000, 那么下一项的偏移就是0x0C0000400，在下一项就是
    0x0C0000800等等；
*/

//------------------  属性头通用结构 ----
typedef struct  //所有偏移量均为相对于属性类型 Type 的偏移量
{
     UINT8 Type[4];           // 属性类型 0x10, 0x20, 0x30, 0x40,...,0xF0,0x100
     UINT8 Length[4];         // 属性的长度
     UINT8 NonResidentFiag;   // 是否是非常驻属性，l 为非常驻属性，0 为常驻属性 00
     UINT8 NameLength;        // 属性名称长度，如果无属性名称，该值为 00
     UINT8 ContentOffset[2];  // 属性内容的偏移量  18 00
     UINT8 CompressedFiag[2]; // 该文件记录表示的文件数据是否被压缩过 00 00
     UINT8 Identify[2];       // 识别标志  00 00
    //--- 0ffset: 0x10 ---
    //--------  常驻属性和非常驻属性的公共部分 ----
    union CCommon
    {
    
        //---- 如果该属性为 常驻 属性时使用该结构 ----
        struct CResident
        {
             UINT8 StreamLength[4];        // 属性值的长度, 即属性具体内容的长度。"48 00 00 00"
             UINT8 StreamOffset[2];        // 属性值起始偏移量  "18 00"
             UINT8 IndexFiag[2];           // 属性是否被索引项所索引，索引项是一个索引(如目录)的基本组成  00 00
        };
        
        //------- 如果该属性为 非常驻 属性时使用该结构 ----
        struct CNonResident
        {
             UINT8 StartVCN[8];            // 起始的 VCN 值(虚拟簇号：在一个文件中的内部簇编号,0起）
             UINT8 LastVCN[8];             // 最后的 VCN 值
             UINT8 RunListOffset[2];       // 运行列表的偏移量
             UINT8 CompressEngineIndex[2]; // 压缩引擎的索引值，指压缩时使用的具体引擎。
             UINT8 Reserved[4];
             UINT8 StreamAiiocSize[8];     // 为属性值分配的空间 ，单位为B，压缩文件分配值小于实际值
             UINT8 StreamRealSize[8];      // 属性值实际使用的空间，单位为B
             UINT8 StreamCompressedSize[8]; // 属性值经过压缩后的大小, 如未压缩, 其值为实际值
        };
    };
}NTFSAttribute;


// MFT记录了整个卷的所有文件 (包括MFT本身、数据文件、文件夹等等) 信息，包括空间占用，文件基本属性，文件位置索引，创建时
// MFT 的第一项记录$MFT描述的是主分区表MFT本身，它的编号为0，MFT项的头部都是如下结构：
typedef struct {
     UINT8    mark[4];             // "FILE" 标志 
     UINT8    UsnOffset[2];        // 更新序列号偏移 　　　　30 00
     UINT8    usnSize[2];          // 更新序列数组大小+1 　 03 00
     UINT8    LSN[8];              // 日志文件序列号(每次记录修改后改变)  58 8E 0F 34 00 00 00 00
    // 0x10
     UINT8    SN[2];               // 序列号 随主文件表记录重用次数而增加
     UINT8    linkNum[2];          // 硬连接数 (多少目录指向该文件) 01 00
     UINT8    firstAttr[2];        // 第一个属性的偏移　　38 00
     UINT8    flags[2];            // 0已删除 1正常文件 2已删除目录 3目录正使用
    // 0x18
     UINT8    MftUseLen[4];        // 记录有效长度   　A8 01 00 00
     UINT8    maxLen[4];            // 记录占用长度 　 00 04 00 00
    // 0x20
     UINT8    baseRecordNum[8];     // 索引基本记录, 如果是基本记录则为0
     UINT8    nextAttrId[2];        // 下一属性Id　　07 00
     UINT8    border[2];            //
     UINT8    xpRecordNum[4];       // 用于xp, 记录号
    // 0x30
     UINT8    USN[8];                 // 更新序列号(2B) 和 更新序列数组
}MFT_HEADER;

