
/*************************************************
    .
    File name:          *.*
    Author��                ������
    ID��                    00001
    Date:                  202107
    Description:        
    Others:             ��

    History:            ��
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


// �ļ���¼ͷ
typedef struct
{
    /*+0x00*/ UINT8 Type[4];    // �̶�ֵ'FILE'
    /*+0x04*/ UINT8 USNOffset[2]; // �������к�ƫ��, �����ϵͳ�й�
    /*+0x06*/ UINT8 USNCount[2]; // �̶��б��СSize in words of Update Sequence Number & Array (S)
    /*+0x08*/ UINT8 Lsn[8]; // ��־�ļ����к�(LSN)
    /*+0x10*/ UINT8 SequenceNumber[2]; // ���к�(���ڼ�¼�ļ�������ʹ�õĴ���)
    /*+0x12*/ UINT8 LinkCount[2];// Ӳ������
    /*+0x14*/ UINT8 AttributeOffset[2]; // ��һ������ƫ��
    /*+0x16*/ UINT8 Flags[2];// flags, 00��ʾɾ���ļ�,01��ʾ�����ļ�,02��ʾɾ��Ŀ¼,03��ʾ����Ŀ¼
    /*+0x18*/ UINT8 BytesInUse[4]; // �ļ���¼ʵʱ��С(�ֽ�) ��ǰMFT�����,��FFFFFF�ĳ���+4
    /*+0x1C*/ UINT8 BytesAllocated[4]; // �ļ���¼�����С(�ֽ�)
    /*+0x20*/ UINT8 BaseFileRecord[8]; // = 0 �����ļ���¼ File reference to the base FILE record
    /*+0x28*/ UINT8 NextAttributeNumber[2]; // ��һ������ID��
    /*+0x2A*/ UINT8 Pading[2]; // �߽�
    /*+0x2C*/ UINT8 MFTRecordNumber[4]; // windows xp��ʹ��,��MFT��¼��
    /*+0x30*/ UINT8 USN[2]; // �������к�
    /*+0x32*/ UINT8 UpdateArray[0]; // ��������
 } NTFS_FILE_HEADER, *pNTFS_FILE_HEADER; 


// NTFS FILE ����ͷ
typedef struct  
{
    UINT8 Type[4];   //��������
    UINT8 Size[4];   //����ͷ����������ܳ���
    UINT8 ResidentFlag; //�Ƿ��ǳ�פ���ԣ�0��פ 1�ǳ�פ��
    UINT8 NameSize;   //�������ĳ���
    UINT8 NameOffset[2]; //��������ƫ�� ���������ͷ
    UINT8 Flags[2]; //��־��0x0001ѹ�� 0x4000���� 0x8000ϡ�裩
    UINT8 Id[2]; //����ΨһID
}NTFS_FILE_ATTRIBUTE_HEADER;


//��פ���� ����ͷ
typedef struct _ResidentAttributeHeader 
{
    //NTFS_FILE_ATTRIBUTE_HEADER ATTR_Common;
    UINT8 ATTR_DataSize[4]; //�������ݵĳ���
    UINT8 ATTR_DataOffset[2]; //�����������������ͷ��ƫ��
    UINT8 ATTR_Index; //����
    UINT8 ATTR_Reserved; //����
    UINT8 ATTR_AttrName[8];//��������Unicode����β��0
}ResidentAttributeHeader, *pResidentAttributeHeader;

//�ǳ�פ���� ����ͷ
typedef struct _NonResidentAttributeHeader 
{
    NTFS_FILE_ATTRIBUTE_HEADER ATTR_Common;
    UINT64 ATTR_StartVCN; //����������������ʼ����غ�
    UINT64 ATTR_EndVCN; //����������������ֹ����غ�
    UINT16 ATTR_DatOff; //�����б����������ͷ��ƫ��
    UINT16 ATTR_CmpSz; //ѹ����λ 2��N�η�
    UINT32 ATTR_Resvd;
    UINT64 ATTR_AllocSz; //���Է���Ĵ�С
    UINT64 ATTR_ValidSz; //���Ե�ʵ�ʴ�С
    UINT64 ATTR_InitedSz; //���Եĳ�ʼ��С
    UINT8 ATTR_AttrNam[0];
}NonResidentAttributeHeader, *pNonResidentAttributeHeader;

// 90���Ե���������3���ֹ��ɣ�������������ͷ�������������Щ�����90�������ǲ������������(��ͼ��90���Բ����������
// ͼ8�е�90���԰���2��������)�����ʱ���Ŀ¼����������A0�����е�data runsָ����90������Ľṹ����(����������ͷ)��
typedef struct _INDEX_HEADER 
{
    UINT8 IH_EntryOff[4];//��һ��Ŀ¼���ƫ��
    UINT8 IH_TalSzOfEntries[4];//Ŀ¼����ܳߴ�(��������ͷ�������������)
    UINT8 IH_AllocSize[4];//Ŀ¼�����ĳߴ�
    UINT8 IH_Flags;/*��־λ����ֵ���������º�ֵ֮һ��
                    0x00 СĿ¼(���ݴ���ڸ��ڵ����������)
                    0x01 ��Ŀ¼(��ҪĿ¼��洢����������λͼ)*/
    UINT8 IH_Resvd[3];
}INDEX_HEADER,*pINDEX_HEADER;

//INDEX_ROOT 0X90������
typedef struct _INDEX_ROOT 
{
    //������
    UINT8 IR_AttributeType[4];//���Ե�����
    UINT8 IR_ColRule[4];//�������
    UINT8 IR_EntrySize[4];//Ŀ¼�����ߴ�
    UINT8 IR_ClusterPerRec;//ÿ��Ŀ¼��ռ�õĴ���
    UINT8 IR_Reserved[3];
    //����ͷ
    INDEX_HEADER IndexHeader;
    //������ ���ܲ�����
    UINT8 IR_IndexEntry[0];
}INDEX_ROOT,*pINDEX_ROOT;

//���У�30���Ե�������ṹ��������(����������ͷ)��
//FILE_NAME 0X30������
typedef struct _FILE_NAME 
{
    UINT64 FN_ParentFR; /*��Ŀ¼��MFT��¼�ļ�¼������
    ע�⣺��ֵ�ĵ�6�ֽ���MFT��¼�ţ���2�ֽ��Ǹ�MFT��¼�����к�*/
    UINT8 FN_CreatTime[4];
    UINT8 FN_AlterTime[4];
    UINT8 FN_MFTChg[4];
    UINT8 FN_ReadTime[4];
    UINT64 FN_AllocSz;
    UINT64 FN_ValidSz;//�ļ�����ʵ�ߴ�
    UINT32 FN_DOSAttr;//DOS�ļ�����
    UINT32 FN_EA_Reparse;//��չ����������
    UINT8 FN_NameSz;//�ļ������ַ���
    UINT8 FN_NamSpace;/*�����ռ䣬��ֵ��Ϊ����ֵ�е�����һ��
    0��POSIX������ʹ�ó�NULL�ͷָ�����/��֮�������UNICODE�ַ���������ʹ��255���ַ���ע�⣺�������ǺϷ��ַ�����Windows������ʹ�á�
    1��Win32��Win32��POSIX��һ���Ӽ��������ִ�Сд������ʹ�ó�������������������?��������������/������<������>������/������|��֮�������UNICODE�ַ��������ֲ����ԡ�.����ո��β��
    2��DOS��DOS�����ռ���Win32���Ӽ���ֻ֧��ASCII����ڿո��8BIT��д�ַ����Ҳ�֧�������ַ�������������������?��������������/������<������>������/������|������+������,������;������=����ͬʱ���ֱ��밴���¸�ʽ������1~8���ַ���Ȼ���ǡ�.����Ȼ������1~3���ַ���
    3��Win32&DOS����������ռ���ζ��Win32��DOS�ļ����������ͬһ���ļ��������С�*/
    UINT8 FN_FileName[0];
}FILE_NAME,*pFILE_NAME;


// Index Header
typedef struct {
    UINT8 Flag[4]; //�̶�ֵ "INDX"
    UINT8 USNOffset[2];//�������к�ƫ��
    UINT8 USNSize[2];//�������кź͸��������С
    UINT8 LogSequenceNumber[8]; // ��־�ļ����к�(LSN)
    UINT8 IndexCacheVCN[8];//�����������������������е�VCN
    UINT8 IndexEntryOffset[4];//�������ƫ�� ����ڵ�ǰλ��
    UINT8 IndexEntrySize[4];//������Ĵ�С
    UINT8 IndexEntryAllocSize[4];//���������Ĵ�С
    UINT8 HasLeafNode;//��һ ��ʾ���ӽڵ�
    UINT8 Fill[3];//���
    UINT8 USN[2];//�������к�
    UINT8 USNArray[0];//������������
}NTFS_INDEX_HEADER;

typedef struct {
     UINT8 MFTReferNumber[8];//�ļ���MFT�ο���, first 6 Bytes * 2 + MFT table sector = file sector 
     UINT8 IndexEntrySize[2];//������Ĵ�С
     UINT8 FileNameAttriBodySize[2];//�ļ���������Ĵ�С
     UINT8 IndexFlag[2];//������־:     0x00 ��ͨ�ļ���;         0x01 ������ 0x02 ��ǰ�������һ��Ŀ¼��
     UINT8 Fill[2];//���
     UINT8 FatherDirMFTReferNumber[8];//��Ŀ¼MFT�ļ��ο���
     UINT8 CreatTime[8];//�ļ�����ʱ�� 8
     UINT8 AlterTime[8];//�ļ�����޸�ʱ��
     UINT8 MFTChgTime[8];//�ļ���¼����޸�ʱ��
     UINT8 ReadTime[8];//�ļ�������ʱ��
     UINT8 FileAllocSize[8];//�ļ������С
     UINT8 FileRealSize[8];//�ļ�ʵ�ʴ�С
     UINT8 FileFlag[8];//�ļ���־
     UINT8 FileNameSize;//�ļ�������
     UINT8 FileNamespace;//�ļ������ռ�
     UINT8 FileNameAndFill[0];//�ļ��������
}NTFS_INDEX_ITEM;

// Master File Table
typedef enum
{
    MFT_ATTRIBUTE_DOLLAR_STANDARD_INFORMATION         = 0x10,
    MFT_ATTRIBUTE_DOLLAR_ATTRIBUTE_LIST             = 0x20,
    MFT_ATTRIBUTE_DOLLAR_FILE_NAME                    = 0x30,
    MFT_ATTRIBUTE_DOLLAR_OBJECT_ID                    = 0x40,
    MFT_ATTRIBUTE_DOLLAR_SECURITY_DESCRIPTOR        = 0x50,
    MFT_ATTRIBUTE_DOLLAR_VOLUME_NAME                = 0x60,
    MFT_ATTRIBUTE_DOLLAR_VOLUME_INFORMATION            = 0x70,
    MFT_ATTRIBUTE_DOLLAR_DATA                        = 0x80,    
    MFT_ATTRIBUTE_DOLLAR_INDEX_ROOT                    = 0x90,
    MFT_ATTRIBUTE_DOLLAR_INDEX_ALLOCATION            = 0xA0,
    MFT_ATTRIBUTE_DOLLAR_BITMAP                        = 0xB0,
    MFT_ATTRIBUTE_DOLLAR_REPARSE_POINT                = 0xC0,
    MFT_ATTRIBUTE_DOLLAR_EA_INFORMATION                = 0xD0,
    MFT_ATTRIBUTE_DOLLAR_EA                            = 0xE0,
    MFT_ATTRIBUTE_INVALID                            = 0xFF,
    MFT_ATTRIBUTE_DOLLAR_LOGGED_UTILITY_STREAM        = 0x100,
    MFT_ATTRIBUTE_DOLLAR_MAX                        
}MFT_ATTRIBUTE_TYPE;

/****���涨��ľ���������Ľṹ ����������ͷ****/
//STANDARD_INFORMATION 0X10������
/*
    SI_DOSAttrȡֵ��
    0x0001ֻ��
    0x0002����
    0x0004ϵͳ
    0x0020�鵵
    0x0040�豸
    0x0080����
    0x0100��ʱ�ļ�
    0x0200ϡ���ļ�
    0x0400�ؽ�����
    0x0800ѹ��
    0x1000����
    0x2000����������
    0x4000����
*/
typedef struct _STANDARD_INFORMATION 
{
    UINT8 SI_CreatTime[4];//����ʱ��
    UINT8 SI_AlterTime[4];//����޸�ʱ��
    UINT8 SI_MFTChgTime[4];//�ļ���MFT�޸ĵ�ʱ��
    UINT8 SI_ReadTime[4];//������ʱ��
    UINT32 SI_DOSAttr;//DOS�ļ�����
    UINT32 SI_MaxVer;//�ļ����õ����汾�� 0��ʾ����
    UINT32 SI_Ver;//�ļ��汾�� �����汾��Ϊ0 ��ֵΪ0
    UINT32 SI_ClassId;//??
    //UINT64 SI_OwnerId;//�ļ�ӵ����ID
    //UINT64 SI_SecurityId;//��ȫID
    //UINT64 SI_QuotaCharged;//�ļ�����ʹ�õĿռ���� 0��ʾ������
    //UINT64 SI_USN;//�ļ����һ�θ��µļ�¼��
#if 0
    uint32 QuotaId;
    uint32 SecurityId;
    uint64 QuotaCharge;
    USN Usn;
#endif
}STANDARD_INFORMATION,*pSTANDARD_INFORMATION;

//ATTRIBUTE_LIST 0X20������
typedef struct _ATTRIBUTE_LIST 
{
    UINT32 AL_RD_Type;
    UINT16 AL_RD_Len;
    UINT8 AL_RD_NamLen;
    UINT8 AL_RD_NamOff;
    UINT64 AL_RD_StartVCN;//����������������ʼ�Ĵغ�
    UINT64 AL_RD_BaseFRS;/*�����Լ�¼������MFT��¼�ļ�¼��
    ע�⣺��ֵ�ĵ�6�ֽ���MFT��¼�ţ���2�ֽ��Ǹ�MFT��¼�����к�*/
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

//OBJECT_ID 0X40������
typedef struct _OBJECT_ID 
{
    UINT8 OID_ObjID[16];//�ļ���GUID
    UINT8 OID_BirthVolID[16];//�ļ�����ʱ���ھ��ID
    UINT8 OID_BirthID[16];//�ļ���ԭʼID
    UINT8 OID_DomainID[16];//����������ʱ�������ID
}OBJECT_ID, *pOBJECT_ID;

//SECRUITY_DESCRIPTOR 0X50������
typedef struct _SECRUITY_DESCRIPTOR 
{
    //??
}SECRUITY_DESCRIPTOR,*pSECRUITY_DESCRIPTOR;

//VOLUME_NAME 0X60������
typedef struct _VOLUME_NAME 
{
    UINT8 VN_Name[0];
}VOLUME_NAME,*pVOLUME_NAME;

//VOLUME_INFORMATION 0X70������
typedef struct _VOLUME_INFORMATION
{
    UINT64 VI_Resvd;
    UINT8 VI_MajVer;//�����汾��
    UINT8 VI_MinVer;//���Ӱ汾��
    UINT16 VI_Flags;/*��־λ�����������¸�ֵ���
    0x0001��λ������ֵ������ʱWindows�������´�����ʱ����chkdsk/F���
    0x0002��־�ļ��ı�ߴ�
    0x0004��ҽ�ʱ����
    0x0008��Windows NT 4�ҽ�
    0x0010����ʱɾ��USN
    0x0020�޸�����ID
    0x8000��chkdsk�޸Ĺ�*/
}VOLUME_INFORMATION,*pVOLUME_INFORMATION;
    
//DATA 0X80������
typedef struct _DATA 
{
    //??
    /*+0x10*/ UINT64 StartVcn; // LowVcn ��ʼVCN ��ʼ�غ�
    /*+0x18*/ UINT64 LastVcn; // HighVcn ����VCN �����غ�
    /*+0x20*/ UINT16 RunArrayOffset;// �������е�ƫ��
    /*+0x22*/ UINT16 CompressionUnit; // ѹ������
    /*+0x24*/ UINT32 Padding0; // ���
    /*+0x28*/ UINT32 IndexedFlag;// Ϊ����ֵ�����С(������Ĵص��ֽ�������)
    /*+0x30*/ UINT64 AllocatedSize; // ����ֵʵ�ʴ�С
    /*+0x38*/ UINT64 DataSize; // ����ֵѹ����С
    /*+0x40*/ UINT64 InitializedSize; // ʵ�����ݴ�С
    /*+0x48*/ UINT64 CompressedSize;// ѹ�����С
    UINT8 D_data[0];
}DATA,*pDATA;

typedef struct _INDEX_ENTRY 
{
    UINT8 IE_MftReferNumber[8];/*���ļ���MFT�ο��š�ע�⣺��ֵ�ĵ�6�ֽ���MFT��¼�ţ���2�ֽ��Ǹ�MFT��¼�����к�*/
    UINT8 IE_Size[2];//������Ĵ�С ����������ʼ��ƫ����
    UINT8 IE_FileNAmeAttriBodySize[2];//�ļ���������Ĵ�С
    UINT8 IE_Flags[2];    /*��־����ֵ����������ֵ֮һ��
                        0x00 ��ͨ�ļ���
                        0x01 ������
                        0x02 ��ǰ�������һ��Ŀ¼��
                        �ڶ�ȡ����������ʱӦ�����ȼ��ó�Ա��ֵ��ȷ����ǰ�������*/
    UINT8 IE_Fill[2];//��� ������
    UINT8 IE_FatherDirMftReferNumber[8];//��Ŀ¼��MFT�ļ��ο���
    UINT8 IE_CreatTime[8];//�ļ�����ʱ��
    UINT8 IE_AlterTime[8];//�ļ�����޸�ʱ��
    UINT8 IE_MFTChgTime[8];//�ļ���¼����޸�ʱ��
    UINT8 IE_ReadTime[8];//�ļ�������ʱ��
    UINT8 IE_FileAllocSize[8];//�ļ������С
    UINT8 IE_FileRealSize[8];//�ļ�ʵ�ʴ�С
    UINT8 IE_FileFlag[8];//�ļ���־
    UINT8 IE_FileNameSize;//�ļ�������
    UINT8 IE_FileNamespace;//�ļ������ռ�
    UINT8 IE_FileNameAndFill[0];//�ļ��������
    //UINT8 IE_Stream[0];//Ŀ¼�����ݣ��ṹ���ļ������Ե�������ͬ
    //UINT64 IE_SubNodeFR;//����ļ�¼��������ֵ�ĵ�6�ֽ���MFT��¼�ţ���2�ֽ��Ǹ�MFT��¼�����к�
}INDEX_ENTRY,*pINDEX_ENTRY;

/*     0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  0123456789ABCDEF
00   90 00 00 00 B8 00 00 00 00 04 18 00 00 00 01 00  ................
10   98 00 00 00 20 00 00 00 24 00 49 00 33 00 30 00  .... ...$.I.3.0.
20   30 00 00 00 01 00 00 00 00 10 00 00 01 00 00 00  0...............
30   10 00 00 00 88 00 00 00 88 00 00 00 00 00 00 00  ................
ƫ�� ��С ����
0X00 4 ���Ժ� 90 00 00 00 
0X04 4 ���Գ��� B8 00 00 00
0X08 1 ��פ��־ 00
0X09 1 ���Ƴ��� 04
0X0A 2 ����ƫ�� 18 00
0X0C 2 ��־(��פ���Բ���ѹ��) 00 00 
0X0E 2 ����ID 01 00
0X10 4 ���Գ���(����ͷ) 98 00 00 00
0X14 2 ����ƫ�� 20 00 00 00
0X16 1 ������־ 00
0X17 1 ���  00
0X18 8 ������ 24 00 49 00 33 00 30 00
0X20 4 ������������ 30 00 00 00
0X24 4 ������� 01 00 00 00
0X28 4 ����������С 00 10 00 00
0X2C 1 ÿ������¼�Ĵ��� 01
0X2D 3 ���  00 00 00
0X30 4 ÿ������ƫ�� 10 00 00 00 
0X34 4 ��������ܴ�С 88 00 00 00
0X38 4 ������ķ��� 88 00 00 00
0X3C 1 ��־����0X01��������  00
0X3C 3 ��� 00 00 00

// NTFS FILE ����ͷ
typedef struct  
{
    UINT8 Type[4];   //�������� 90 00 00 00
    UINT8 Size[4];   //����ͷ����������ܳ��� B8 00 00 00
    UINT8 ResidentFlag; //�Ƿ��ǳ�פ���ԣ�0��פ 1�ǳ�פ�� 00
    UINT8 NameSize;   //�������ĳ��� 04 
    UINT8 NameOffset[2]; //��������ƫ�� ���������ͷ 18 00
    UINT8 Flags[2]; //��־��0x0001ѹ�� 0x4000���� 0x8000ϡ�裩 00 00
    UINT8 Id[2]; //����ΨһID 01 00
}NTFS_FILE_ATTRIBUTE_HEADER;

//��פ���� ����ͷ
typedef struct _ResidentAttributeHeader 
{
    NTFS_FILE_ATTRIBUTE_HEADER ATTR_Common;
    UINT32 ATTR_DatSz; //�������ݵĳ��� 98 00 00 00
    UINT16 ATTR_DatOff; //�����������������ͷ��ƫ�� 20 00
    UINT8 ATTR_Indx; //���� 00
    UINT8 ATTR_Resvd; //���� 00
    UINT8 ATTR_AttrNam[8];//��������Unicode����β��0 24
}ResidentAttributeHeader, *pResidentAttributeHeader;

//INDEX_ROOT 0X90������
typedef struct _INDEX_ROOT 
{
    //������
    UINT32 IR_AttrType;//���Ե�����  30 00 00 00
    UINT32 IR_ColRule;//������� 01 00 00 00
    UINT32 IR_EntrySz;//Ŀ¼�����ߴ� 00 10 00 00
    UINT8 IR_ClusPerRec;//ÿ��Ŀ¼��ռ�õĴ��� 01
    UINT8 IR_Resvd[3]; 00 00 00 
    //����ͷ
    INDEX_HEADER IH;
    //������ ���ܲ�����
    UINT8 IR_IndexEntry[0];
}INDEX_ROOT,*pINDEX_ROOT;



// 90���Ե���������3���ֹ��ɣ�������������ͷ�������������Щ�����90�������ǲ������������(��ͼ��90���Բ����������
// ͼ8�е�90���԰���2��������)�����ʱ���Ŀ¼����������A0�����е�data runsָ����90������Ľṹ����(����������ͷ)��
typedef struct _INDEX_HEADER 
{
    UINT32 IH_EntryOff;//��һ��Ŀ¼���ƫ�� 10 00 00
    UINT32 IH_TalSzOfEntries;//Ŀ¼����ܳߴ�(��������ͷ�������������) 88 00 00 00
    UINT32 IH_AllocSize;//Ŀ¼�����ĳߴ� 88 00 00 00
    UINT8 IH_Flags;��־λ����ֵ���������º�ֵ֮һ��
                    0x00 СĿ¼(���ݴ���ڸ��ڵ����������)
                    0x01 ��Ŀ¼(��ҪĿ¼��洢����������λͼ) 00
    UINT8 IH_Resvd[3]; 00 00 00
}INDEX_HEADER,*pINDEX_HEADER;


     0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  0123456789ABCDEF
00   27 00 00 00 00 00 01 00 68 00 54 00 00 00 00 00  '.......h.T.....
10   26 00 00 00 00 00 01 00 5B AE 88 D6 EE D6 04 00  &.......[.......
20   C5 F9 BC 2E E6 C0 D7 01 C5 F9 BC 2E E6 C0 D7 01  ................
30   5B AE 88 D6 EE D6 D7 01 18 00 00 00 00 00 00 00  [...............
40   18 00 00 00 00 00 00 00 20 00 00 00 00 00 00 00  ........ .......
50   09 03 74 00 65 00 73 00 74 00 32 00 2E 00 74 00  ..t.e.s.t.2...t.
60   78 00 74 00 00 00 00 00 00 00 00 00 00 00 00 00  x.t.............
70   10 00 00 00 02 00 00 00 
��11 ����������������ͷ���ֽṹ
ƫ�� ��С ����
0X00 8 �ļ���MFT��¼�� 27 00 00 00 00 00 01 00
0X08 2 �������С 68 00
0X0A 2 ����ƫ�� 54 00
0X0C 4 ������־�����  00 00 00 00
0X10 8 ��Ŀ¼��MFT�ļ��ο��� 26 00 00 00 00 00 01 00
0X18 8 �ļ�����ʱ�� 5B AE 88 D6 EE D6 04 00
0X20 8 �ļ��޸�ʱ�� C5 F9 BC 2E E6 C0 D7 01
0X28 8 �ļ�����޸�ʱ�� C5 F9 BC 2E E6 C0 D7 01
0X30 8 �ļ�������ʱ�� 5B AE 88 D6 EE D6 D7 01
0X38 8 �ļ������С 18 00 00 00 00 00 00 00
0X40 8 �ļ�ʵ�ʴ�С 18 00 00 00 00 00 00 00
0X48 8 �ļ���־ 20 00 00 00 00 00 00 00 �ļ������ļ���
0X50 1 �ļ������ȣ�F�� 09 
0X51 1 �ļ��������ռ� 03 
0X52 2F �ļ�������䵽8�ֽڣ�
0X52+2F P
0X52 +P+2F 8 �ӽڵ����������VCL
��12 ������ṹ
���������Ծ���������ͷ����


typedef struct _INDEX_ENTRY 
{
    UINT64 IE_MftReferNumber;���ļ���MFT�ο��š�ע�⣺��ֵ�ĵ�6�ֽ���MFT��¼�ţ���2�ֽ��Ǹ�MFT��¼�����к�
    UINT16 IE_Size;//������Ĵ�С ����������ʼ��ƫ����
    UINT16 IE_FileNAmeAttriBodySize;//�ļ���������Ĵ�С
    UINT16 IE_Flags;��־����ֵ����������ֵ֮һ��
                    0x00 ��ͨ�ļ���
                    0x01 ������
                    0x02 ��ǰ�������һ��Ŀ¼��
                    �ڶ�ȡ����������ʱӦ�����ȼ��ó�Ա��ֵ��ȷ����ǰ�������
    UINT16 IE_Fill;//��� ������
    UINT64 IE_FatherDirMftReferNumber;//��Ŀ¼��MFT�ļ��ο���
    UINT8 IE_CreatTime[4];//�ļ�����ʱ��
    UINT8 IE_AlterTime[4];//�ļ�����޸�ʱ��
    UINT8 IE_MFTChgTime[4];//�ļ���¼����޸�ʱ��
    UINT8 IE_ReadTime[4];//�ļ�������ʱ��
    UINT64 IE_FileAllocSize;//�ļ������С
    UINT64 IE_FileRealSize;//�ļ�ʵ�ʴ�С
    UINT64 IE_FileFlag;//�ļ���־
    UINT8 IE_FileNameSize;//�ļ�������
    UINT8 IE_FileNamespace;//�ļ������ռ�
    UINT8 IE_FileNameAndFill[0];//�ļ��������
    //UINT8 IE_Stream[0];//Ŀ¼�����ݣ��ṹ���ļ������Ե�������ͬ
    //UINT64 IE_SubNodeFR;//����ļ�¼��������ֵ�ĵ�6�ֽ���MFT��¼�ţ���2�ֽ��Ǹ�MFT��¼�����к�
}INDEX_ENTRY,*pINDEX_ENTRY;
*/


//INDEX_ALLOCATION 0XA0������
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
    UINT32 RP_Type;/*�ؽ����������ͣ���ֵ����������ֵ֮һ
                    0x20000000����
                    0x40000000��ߵȴ�ʱ��
                    0x80000000΢��ʹ��
                    0x68000005NSS
                    0x68000006NSS�ָ�
                    0x68000007SIS
                    0x68000008DFS
                    0x88000003��ҽӵ�
                    0xA8000004 HSM
                    0xE8000000 Ӳ����*/
    UINT16 RP_DatSz;//�ؽ������ݳߴ�
    UINT16 RP_Resvd;//
    UINT8 RP_Data[0];// �ؽ�������
}REPARSE_POINT,*pREPARSE_POINT;

//EA_INFORMATION
typedef struct _EA_INFORMATION 
{
    UINT16 EI_PackedSz;// ѹ����չ���Գߴ�
    UINT16 EI_NumOfEA;//ӵ��NEED_EA��¼����չ���Ը���
    UINT32 EI_UnpackedSz;//δѹ����չ���Գߴ�
}EA_INFORMATION,*pEA_INFORMATION;

//EA
typedef struct _EA 
{
    UINT32 EA_Next;//��һ����չ���Ե�ƫ��(����¼�ĳߴ�)
    UINT8 EA_Flags;//��־λ��ֵȡ0x80��ʾ��ҪEA
    UINT8 EA_NamLen;//�������ݵĳ���(M)
    UINT16 EA_ValLen;//ֵ���ݵĳ���
    UINT8 EA_NameVal[0];//�������ݺ�ֵ����
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
    UINT8 ResidentFlag; //�Ƿ��ǳ�פ���ԣ�0��פ 1�ǳ�פ��
    UINT8 NameSize;   //�������ĳ���
    UINT16 NameOffset; //��������ƫ�� ���������ͷ
    UINT8 Data[20]; //���ڴ�����Ե�����
    UINT16 DataSize; //���ڴ�����Ե�����
}NTFS_FILE_ATTRIBUTE_HEADER_SWITCHED;    


typedef struct 
{
    NTFS_FILE_HEADER_SWITCHED NTFSFileHeaderSwitched;
    NTFS_FILE_ATTRIBUTE_HEADER_SWITCHED NTFSFileAttributeHeaderSwitched[30];
}NTFS_FILE_SWITCHED;    

