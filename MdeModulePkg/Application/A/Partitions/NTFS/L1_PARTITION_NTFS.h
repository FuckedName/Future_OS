#pragma once


typedef struct 
{
    UINT8 OccupyCluster;
    UINT64 Offset; //Please Note: The first item is start offset, and next item is relative offset....
}IndexInformation;


// 文件记录头
typedef struct
{
    /*+0x00*/ UINT8 Type[4];    // 固定值'FILE'
    /*+0x04*/ UINT8 USNOffset[2]; // 更新序列号偏移, 与操作系统有关
    /*+0x06*/ UINT8 USNCount[2]; // 固定列表大小Size in words of Update Sequence Number & Array (S)
    /*+0x08*/ UINT8 Lsn[8]; // 日志文件序列号(LSN)
    /*+0x10*/ UINT8 SequenceNumber[2]; // 序列号(用于记录文件被反复使用的次数)
    /*+0x12*/ UINT8 LinkCount[2];// 硬连接数
    /*+0x14*/ UINT8 AttributeOffset[2]; // 第一个属性偏移
    /*+0x16*/ UINT8 Flags[2];// flags, 00表示删除文件,01表示正常文件,02表示删除目录,03表示正常目录
    /*+0x18*/ UINT8 BytesInUse[4]; // 文件记录实时大小(字节) 当前MFT表项长度,到FFFFFF的长度+4
    /*+0x1C*/ UINT8 BytesAllocated[4]; // 文件记录分配大小(字节)
    /*+0x20*/ UINT8 BaseFileRecord[8]; // = 0 基础文件记录 File reference to the base FILE record
    /*+0x28*/ UINT8 NextAttributeNumber[2]; // 下一个自由ID号
    /*+0x2A*/ UINT8 Pading[2]; // 边界
    /*+0x2C*/ UINT8 MFTRecordNumber[4]; // windows xp中使用,本MFT记录号
    /*+0x30*/ UINT8 USN[2]; // 更新序列号
    /*+0x32*/ UINT8 UpdateArray[0]; // 更新数组
 } NTFS_FILE_HEADER, *pNTFS_FILE_HEADER; 


// NTFS FILE 属性头
typedef struct  
{
    UINT8 Type[4];   //属性类型
    UINT8 Size[4];   //属性头和属性体的总长度
    UINT8 ResidentFlag; //是否是常驻属性（0常驻 1非常驻）
    UINT8 NameSize;   //属性名的长度
    UINT8 NameOffset[2]; //属性名的偏移 相对于属性头
    UINT8 Flags[2]; //标志（0x0001压缩 0x4000加密 0x8000稀疏）
    UINT8 Id[2]; //属性唯一ID
}NTFS_FILE_ATTRIBUTE_HEADER;

// Index Header
typedef struct {
    UINT8 Flag[4]; //固定值 "INDX"
    UINT8 USNOffset[2];//更新序列号偏移
    UINT8 USNSize[2];//更新序列号和更新数组大小
    UINT8 LogSequenceNumber[8]; // 日志文件序列号(LSN)
    UINT8 IndexCacheVCN[8];//本索引缓冲区在索引分配中的VCN
    UINT8 IndexEntryOffset[4];//索引项的偏移 相对于当前位置
    UINT8 IndexEntrySize[4];//索引项的大小
    UINT8 IndexEntryAllocSize[4];//索引项分配的大小
    UINT8 HasLeafNode;//置一 表示有子节点
    UINT8 Fill[3];//填充
    UINT8 USN[2];//更新序列号
    UINT8 USNArray[0];//更新序列数组
}INDEX_HEADER;

typedef struct {
     UINT8 MFTReferNumber[8];//文件的MFT参考号, first 6 Bytes * 2 + MFT table sector = file sector 
     UINT8 IndexEntrySize[2];//索引项的大小
     UINT8 FileNameAttriBodySize[2];//文件名属性体的大小
     UINT8 IndexFlag[2];//索引标志:	 0x00 普通文件项;    	 0x01 有子项 0x02 当前项是最后一个目录项
     UINT8 Fill[2];//填充
     UINT8 FatherDirMFTReferNumber[8];//父目录MFT文件参考号
     UINT8 CreatTime[8];//文件创建时间 8
     UINT8 AlterTime[8];//文件最后修改时间
     UINT8 MFTChgTime[8];//文件记录最后修改时间
     UINT8 ReadTime[8];//文件最后访问时间
     UINT8 FileAllocSize[8];//文件分配大小
     UINT8 FileRealSize[8];//文件实际大小
     UINT8 FileFlag[8];//文件标志
     UINT8 FileNameSize;//文件名长度
     UINT8 FileNamespace;//文件命名空间
     UINT8 FileNameAndFill[0];//文件名和填充
}INDEX_ITEM;

// Master File Table
typedef enum
{
	MFT_ATTRIBUTE_DOLLAR_STANDARD_INFORMATION 		= 0x10,
	MFT_ATTRIBUTE_DOLLAR_ATTRIBUTE_LIST 			= 0x20,
	MFT_ATTRIBUTE_DOLLAR_FILE_NAME					= 0x30,
	MFT_ATTRIBUTE_DOLLAR_OBJECT_ID					= 0x40,
	MFT_ATTRIBUTE_DOLLAR_SECURITY_DESCRIPTOR		= 0x50,
	MFT_ATTRIBUTE_DOLLAR_VOLUME_NAME				= 0x60,
	MFT_ATTRIBUTE_DOLLAR_VOLUME_INFORMATION			= 0x70,
	MFT_ATTRIBUTE_DOLLAR_DATA						= 0x80,	
	MFT_ATTRIBUTE_DOLLAR_INDEX_ROOT					= 0x90,
	MFT_ATTRIBUTE_DOLLAR_INDEX_ALLOCATION			= 0xA0,
	MFT_ATTRIBUTE_DOLLAR_BITMAP						= 0xB0,
	MFT_ATTRIBUTE_DOLLAR_REPARSE_POINT				= 0xC0,
	MFT_ATTRIBUTE_DOLLAR_EA_INFORMATION				= 0xD0,
	MFT_ATTRIBUTE_DOLLAR_EA							= 0xE0,
	MFT_ATTRIBUTE_DOLLAR_LOGGED_UTILITY_STREAM		= 0x100,
	MFT_ATTRIBUTE_DOLLAR_MAX						
}MFT_ATTRIBUTE_TYPE;

