
/*************************************************
    .
    File name:      	*.*
    Author：	        	任启红
    ID：					00001
    Date:          		202107
    Description:    	
    Others:         	无

    History:        	无
	    1.  Date:
		    Author: 
		    ID:
		    Modification:
		    
	    2.  Date:
		    Author: 
		    ID:
		    Modification:
*************************************************/




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


//常驻属性 属性头
typedef struct _ResidentAttributeHeader 
{
	NTFS_FILE_ATTRIBUTE_HEADER ATTR_Common;
	UINT32 ATTR_DatSz; //属性数据的长度
	UINT16 ATTR_DatOff; //属性数据相对于属性头的偏移
	UINT8 ATTR_Indx; //索引
	UINT8 ATTR_Resvd; //保留
	UINT8 ATTR_AttrNam[0];//属性名，Unicode，结尾无0
}ResidentAttributeHeader, *pResidentAttributeHeader;

//非常驻属性 属性头
typedef struct _NonResidentAttributeHeader 
{
	NTFS_FILE_ATTRIBUTE_HEADER ATTR_Common;
	UINT64 ATTR_StartVCN; //本属性中数据流起始虚拟簇号
	UINT64 ATTR_EndVCN; //本属性中数据流终止虚拟簇号
	UINT16 ATTR_DatOff; //簇流列表相对于属性头的偏移
	UINT16 ATTR_CmpSz; //压缩单位 2的N次方
	UINT32 ATTR_Resvd;
	UINT64 ATTR_AllocSz; //属性分配的大小
	UINT64 ATTR_ValidSz; //属性的实际大小
	UINT64 ATTR_InitedSz; //属性的初始大小
	UINT8 ATTR_AttrNam[0];
}NonResidentAttributeHeader, *pNonResidentAttributeHeader;

// 90属性的属性体由3部分构成：索引根、索引头和索引项。但是有些情况下90属性中是不存在索引项的(上图的90属性不包含索引项，
// 图8中的90属性包含2个索引项)，这个时候该目录的索引项由A0属性中的data runs指出。90属性体的结构如下(不包含属性头)：
typedef struct _INDEX_HEADER 
{
	UINT32 IH_EntryOff;//第一个目录项的偏移
	UINT32 IH_TalSzOfEntries;//目录项的总尺寸(包括索引头和下面的索引项)
	UINT32 IH_AllocSize;//目录项分配的尺寸
	UINT8 IH_Flags;/*标志位，此值可能是以下和值之一：
					0x00 小目录(数据存放在根节点的数据区中)
					0x01 大目录(需要目录项存储区和索引项位图)*/
	UINT8 IH_Resvd[3];
}INDEX_HEADER,*pINDEX_HEADER;

//INDEX_ROOT 0X90属性体
typedef struct _INDEX_ROOT 
{
	//索引根
	UINT32 IR_AttrType;//属性的类型
	UINT32 IR_ColRule;//整理规则
	UINT32 IR_EntrySz;//目录项分配尺寸
	UINT8 IR_ClusPerRec;//每个目录项占用的簇数
	UINT8 IR_Resvd[3];
	//索引头
	INDEX_HEADER IH;
	//索引项 可能不存在
	UINT8 IR_IndexEntry[0];
}INDEX_ROOT,*pINDEX_ROOT;

//其中，30属性的属性体结构定义如下(不包含属性头)：
//FILE_NAME 0X30属性体
typedef struct _FILE_NAME 
{
	UINT64 FN_ParentFR; /*父目录的MFT记录的记录索引。
	注意：该值的低6字节是MFT记录号，高2字节是该MFT记录的序列号*/
	UINT8 FN_CreatTime[4];
	UINT8 FN_AlterTime[4];
	UINT8 FN_MFTChg[4];
	UINT8 FN_ReadTime[4];
	UINT64 FN_AllocSz;
	UINT64 FN_ValidSz;//文件的真实尺寸
	UINT32 FN_DOSAttr;//DOS文件属性
	UINT32 FN_EA_Reparse;//扩展属性与链接
	UINT8 FN_NameSz;//文件名的字符数
	UINT8 FN_NamSpace;/*命名空间，该值可为以下值中的任意一个
	0：POSIX　可以使用除NULL和分隔符“/”之外的所有UNICODE字符，最大可以使用255个字符。注意：“：”是合法字符，但Windows不允许使用。
	1：Win32　Win32是POSIX的一个子集，不区分大小写，可以使用除““”、“＊”、“?”、“：”、“/”、“<”、“>”、“/”、“|”之外的任意UNICODE字符，但名字不能以“.”或空格结尾。
	2：DOS　DOS命名空间是Win32的子集，只支持ASCII码大于空格的8BIT大写字符并且不支持以下字符““”、“＊”、“?”、“：”、“/”、“<”、“>”、“/”、“|”、“+”、“,”、“;”、“=”；同时名字必须按以下格式命名：1~8个字符，然后是“.”，然后再是1~3个字符。
	3：Win32&DOS　这个命名空间意味着Win32和DOS文件名都存放在同一个文件名属性中。*/
	UINT8 FN_FileName[0];
}FILE_NAME,*pFILE_NAME;


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
}NTFS_INDEX_HEADER;

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
}NTFS_INDEX_ITEM;

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
	MFT_ATTRIBUTE_INVALID							= 0xFF,
	MFT_ATTRIBUTE_DOLLAR_LOGGED_UTILITY_STREAM		= 0x100,
	MFT_ATTRIBUTE_DOLLAR_MAX						
}MFT_ATTRIBUTE_TYPE;

/****下面定义的均是属性体的结构 不包括属性头****/
//STANDARD_INFORMATION 0X10属性体
/*
	SI_DOSAttr取值：
	0x0001只读
	0x0002隐藏
	0x0004系统
	0x0020归档
	0x0040设备
	0x0080常规
	0x0100临时文件
	0x0200稀疏文件
	0x0400重解析点
	0x0800压缩
	0x1000离线
	0x2000无内容索引
	0x4000加密
*/
typedef struct _STANDARD_INFORMATION 
{
	UINT8 SI_CreatTime[4];//创建时间
	UINT8 SI_AlterTime[4];//最后修改时间
	UINT8 SI_MFTChgTime[4];//文件的MFT修改的时间
	UINT8 SI_ReadTime[4];//最后访问时间
	UINT32 SI_DOSAttr;//DOS文件属性
	UINT32 SI_MaxVer;//文件可用的最大版本号 0表示禁用
	UINT32 SI_Ver;//文件版本号 若最大版本号为0 则值为0
	UINT32 SI_ClassId;//??
	//UINT64 SI_OwnerId;//文件拥有者ID
	//UINT64 SI_SecurityId;//安全ID
	//UINT64 SI_QuotaCharged;//文件最大可使用的空间配额 0表示无限制
	//UINT64 SI_USN;//文件最后一次更新的记录号
#if 0
	uint32 QuotaId;
	uint32 SecurityId;
	uint64 QuotaCharge;
	USN Usn;
#endif
}STANDARD_INFORMATION,*pSTANDARD_INFORMATION;

//ATTRIBUTE_LIST 0X20属性体
typedef struct _ATTRIBUTE_LIST 
{
	UINT32 AL_RD_Type;
	UINT16 AL_RD_Len;
	UINT8 AL_RD_NamLen;
	UINT8 AL_RD_NamOff;
	UINT64 AL_RD_StartVCN;//本属性中数据流开始的簇号
	UINT64 AL_RD_BaseFRS;/*本属性记录所属的MFT记录的记录号
	注意：该值的低6字节是MFT记录号，高2字节是该MFT记录的序列号*/
	UINT16 AL_RD_AttrId;
	//UINT8 AL_RD_Name[0];
	UINT16 AlignmentOrReserved[3];
}ATTRIBUTE_LIST,*pATTRIBUTE_LIST;


//VOLUME_VERSION
typedef struct _VOLUME_VERSION 
{
	//??
}
VOLUME_VERSION,*pVOLUME_VERSION;

//OBJECT_ID 0X40属性体
typedef struct _OBJECT_ID 
{
	UINT8 OID_ObjID[16];//文件的GUID
	UINT8 OID_BirthVolID[16];//文件建立时所在卷的ID
	UINT8 OID_BirthID[16];//文件的原始ID
	UINT8 OID_DomainID[16];//对象所创建时所在域的ID
}OBJECT_ID, *pOBJECT_ID;

//SECRUITY_DESCRIPTOR 0X50属性体
typedef struct _SECRUITY_DESCRIPTOR 
{
	//??
}SECRUITY_DESCRIPTOR,*pSECRUITY_DESCRIPTOR;

//VOLUME_NAME 0X60属性体
typedef struct _VOLUME_NAME 
{
	UINT8 VN_Name[0];
}VOLUME_NAME,*pVOLUME_NAME;

//VOLUME_INFORMATION 0X70属性体
typedef struct _VOLUME_INFORMATION
{
	UINT64 VI_Resvd;
	UINT8 VI_MajVer;//卷主版本号
	UINT8 VI_MinVer;//卷子版本号
	UINT16 VI_Flags;/*标志位，可以是以下各值组合
	0x0001脏位，当该值被设置时Windows将会在下次启动时运行chkdsk/F命令。
	0x0002日志文件改变尺寸
	0x0004卷挂接时升级
	0x0008由Windows NT 4挂接
	0x0010启动时删除USN
	0x0020修复过的ID
	0x8000被chkdsk修改过*/
}VOLUME_INFORMATION,*pVOLUME_INFORMATION;
	
//DATA 0X80属性体
typedef struct _DATA 
{
	//??
	/*+0x10*/ UINT64 StartVcn; // LowVcn 起始VCN 起始簇号
	/*+0x18*/ UINT64 LastVcn; // HighVcn 结束VCN 结束簇号
	/*+0x20*/ UINT16 RunArrayOffset;// 数据运行的偏移
	/*+0x22*/ UINT16 CompressionUnit; // 压缩引擎
	/*+0x24*/ UINT32 Padding0; // 填充
	/*+0x28*/ UINT32 IndexedFlag;// 为属性值分配大小(按分配的簇的字节数计算)
	/*+0x30*/ UINT64 AllocatedSize; // 属性值实际大小
	/*+0x38*/ UINT64 DataSize; // 属性值压缩大小
	/*+0x40*/ UINT64 InitializedSize; // 实际数据大小
	/*+0x48*/ UINT64 CompressedSize;// 压缩后大小
	UINT8 D_data[0];
}DATA,*pDATA;

typedef struct _INDEX_ENTRY 
{
	UINT64 IE_MftReferNumber;/*该文件的MFT参考号。注意：该值的低6字节是MFT记录号，高2字节是该MFT记录的序列号*/
	UINT16 IE_Size;//索引项的大小 相对于索引项开始的偏移量
	UINT16 IE_FileNAmeAttriBodySize;//文件名属性体的大小
	UINT16 IE_Flags;/*标志。该值可能是以下值之一：
	0x00 普通文件项
	0x01 有子项
	0x02 当前项是最后一个目录项
	在读取索引项数据时应该首先检查该成员的值以确定当前项的类型*/
	UINT16 IE_Fill;//填充 无意义
	UINT64 IE_FatherDirMftReferNumber;//父目录的MFT文件参考号
	UINT8 IE_CreatTime[4];//文件创建时间
	UINT8 IE_AlterTime[4];//文件最后修改时间
	UINT8 IE_MFTChgTime[4];//文件记录最后修改时间
	UINT8 IE_ReadTime[4];//文件最后访问时间
	UINT64 IE_FileAllocSize;//文件分配大小
	UINT64 IE_FileRealSize;//文件实际大小
	UINT64 IE_FileFlag;//文件标志
	UINT8 IE_FileNameSize;//文件名长度
	UINT8 IE_FileNamespace;//文件命名空间
	UINT8 IE_FileNameAndFill[0];//文件名和填充
	//UINT8 IE_Stream[0];//目录项数据，结构与文件名属性的数据相同
	//UINT64 IE_SubNodeFR;//子项的记录索引。该值的低6字节是MFT记录号，高2字节是该MFT记录的序列号
}INDEX_ENTRY,*pINDEX_ENTRY;
	

//INDEX_ALLOCATION 0XA0属性体
typedef struct _INDEX_ALLOCATION 
{
	//UINT64 IA_DataRuns;
	UINT8 IA_DataRuns[0];
}INDEX_ALLOCATION,*pINDEX_ALLOCATION;

//BITMAP
typedef struct _MFT_ATTR_BITMAP 
{
	//??
}MFT_ATTR_BITMAP,*pMFT_ATTR_BITMAP;
	
//SYMBOL_LINK
typedef struct _SYMBOL_LINK 
{
	//??
}SYMBOL_LINK,*pSYMBOL_LINK;

//REPARSE_POINT
typedef struct _REPARSE_POINT
{
	UINT32 RP_Type;/*重解析数据类型，该值可以是以下值之一
					0x20000000别名
					0x40000000最高等待时间
					0x80000000微软使用
					0x68000005NSS
					0x68000006NSS恢复
					0x68000007SIS
					0x68000008DFS
					0x88000003卷挂接点
					0xA8000004 HSM
					0xE8000000 硬连接*/
	UINT16 RP_DatSz;//重解析数据尺寸
	UINT16 RP_Resvd;//
	UINT8 RP_Data[0];// 重解析数据
}REPARSE_POINT,*pREPARSE_POINT;

//EA_INFORMATION
typedef struct _EA_INFORMATION 
{
	UINT16 EI_PackedSz;// 压缩扩展属性尺寸
	UINT16 EI_NumOfEA;//拥有NEED_EA记录的扩展属性个数
	UINT32 EI_UnpackedSz;//未压缩扩展属性尺寸
}EA_INFORMATION,*pEA_INFORMATION;

//EA
typedef struct _EA 
{
	UINT32 EA_Next;//下一个扩展属性的偏移(本记录的尺寸)
	UINT8 EA_Flags;//标志位，值取0x80表示需要EA
	UINT8 EA_NamLen;//名字数据的长度(M)
	UINT16 EA_ValLen;//值数据的长度
	UINT8 EA_NameVal[0];//名字数据和值数据
}EA,*pEA;


//NTFS_FILE_HEADER Switched
typedef struct 
{
    UINT16 AttributeOffset;
}NTFS_FILE_HEADER_SWITCHED;    


//NTFS_FILE_ATTRIBUTE_HEADER
typedef struct 
{
    UINT8 Type;
    UINT32 Size;
    UINT8 ResidentFlag; //是否是常驻属性（0常驻 1非常驻）
    UINT8 NameSize;   //属性名的长度
    UINT16 NameOffset; //属性名的偏移 相对于属性头
    UINT8 Data[20]; //用于存放属性的数据
    UINT16 DataSize; //用于存放属性的数据
}NTFS_FILE_ATTRIBUTE_HEADER_SWITCHED;    


typedef struct 
{
    NTFS_FILE_HEADER_SWITCHED NTFSFileHeaderSwitched;
    NTFS_FILE_ATTRIBUTE_HEADER_SWITCHED NTFSFileAttributeHeaderSwitched[10];
}NTFS_FILE_SWITCHED;    

