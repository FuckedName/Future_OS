
/*************************************************
    .
    File name:      	*.*
    Author��	        	������
    ID��					00001
    Date:          		202107
    Description:    	
    Others:         	��

    History:        	��
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


#include <Library/UefiLib.h>
#include <string.h>
//#include <Device/Mouse/L1_DEVICE_Mouse.h>
//#include <Device/Screen/L1_DEVICE_Screen.h>


/*Ŀ¼�����ֽں���*/
typedef enum
{
    DirUnUsed            = 0x00,        /*������û��ʹ��*/
    DirCharE5            = 0x05,        /*���ַ�Ϊ0xe5*/
    DirisSubDir          = 0x2e,        /*��һ����Ŀ¼ .,..Ϊ��Ŀ¼*/
    DirFileisDeleted     = 0xe5        /*�ļ���ɾ��*/
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
	NTFS�ļ�ϵͳ�������ʷ�����
		1����һ�����������ݽṹDOLLAR_BOOT����ȡ��MFT_StartCluster��ֵ��
		2��ͨ��MFT_StartCluster * 8 ����MFT���кܶ�����ݽṹ��MFT_ITEM������ͨ��ƫ��10����������MFT_ITEM_DOLLAR_ROOT�
		3��ÿ��MFT��,��FILE��ASSCI�뿪ͷ��ռ��2��������1024�ֽڴ�С������MFTͷ�������ݽṹ��MFT_HEADER;
			���������кܶ���������(���ö��ֵ��MFT_ATTRIBUTE_TYPE)���������ݽṹ��NTFSAttribute
			������Ҫ�ҵ�0x90���Ժ�0xA0���ԣ�����ȡdata runs
			0X90���������ԣ�����Ÿ�Ŀ¼�µ���Ŀ¼�����ļ��������
			��ĳ��Ŀ¼�µ����ݱȽ϶࣬�Ӷ�����0X90�����޷���ȫ���ʱ��0XA0���Ի�ָ��һ����������
			���������������˸�Ŀ¼������ʣ�����ݵ������
		4��
		5��

		
*/


/*
    MFT ����һ���� MFT ��(��¼)����ɵģ�����ÿ���С�ǹ̶���(һ��Ϊ1KB = 2 * 512)��MFT������ǰ16�����������ļ���¼����ΪԪ���ݣ�
    Ԫ�����ڴ����������������ģ����Ϊ0~15�����$MFT��ƫ��Ϊ0x0C0000000, ��ô��һ���ƫ�ƾ���0x0C0000400������һ�����
    0x0C0000800�ȵȣ�
*/

//------------------  ����ͷͨ�ýṹ ----
typedef struct  //����ƫ������Ϊ������������� Type ��ƫ����
{
     UINT8 Type[4];           // �������� 0x10, 0x20, 0x30, 0x40,...,0xF0,0x100
     UINT8 Length[4];         // ���Եĳ���
     UINT8 NonResidentFiag;   // �Ƿ��Ƿǳ�פ���ԣ�l Ϊ�ǳ�פ���ԣ�0 Ϊ��פ���� 00
     UINT8 NameLength;        // �������Ƴ��ȣ�������������ƣ���ֵΪ 00
     UINT8 ContentOffset[2];  // �������ݵ�ƫ����  18 00
     UINT8 CompressedFiag[2]; // ���ļ���¼��ʾ���ļ������Ƿ�ѹ���� 00 00
     UINT8 Identify[2];       // ʶ���־  00 00
    //--- 0ffset: 0x10 ---
    //--------  ��פ���Ժͷǳ�פ���ԵĹ������� ----
    union
    {
    
        //---- ���������Ϊ ��פ ����ʱʹ�øýṹ ----
        struct 
        {
             UINT8 StreamLength[4];        // ����ֵ�ĳ���, �����Ծ������ݵĳ��ȡ�"48 00 00 00"
             UINT8 StreamOffset[2];        // ����ֵ��ʼƫ����  "18 00"
             UINT8 IndexFiag[2];           // �����Ƿ�����������������������һ������(��Ŀ¼)�Ļ������  00 00
        }CResident;
        
        //------- ���������Ϊ �ǳ�פ ����ʱʹ�øýṹ ----
        struct 
        {
             UINT8 StartVCN[8];            // ��ʼ�� VCN ֵ(����غţ���һ���ļ��е��ڲ��ر��,0��
             UINT8 LastVCN[8];             // ���� VCN ֵ
             UINT8 RunListOffset[2];       // �����б��ƫ����
             UINT8 CompressEngineIndex[2]; // ѹ�����������ֵ��ָѹ��ʱʹ�õľ������档
             UINT8 Reserved[4];
             UINT8 StreamAiiocSize[8];     // Ϊ����ֵ����Ŀռ� ����λΪB��ѹ���ļ�����ֵС��ʵ��ֵ
             UINT8 StreamRealSize[8];      // ����ֵʵ��ʹ�õĿռ䣬��λΪB
             UINT8 StreamCompressedSize[8]; // ����ֵ����ѹ����Ĵ�С, ��δѹ��, ��ֵΪʵ��ֵ
        }CNonResident;
     }CCommon;
}NTFSAttribute;


// MFT��¼��������������ļ� (����MFT���������ļ����ļ��еȵ�) ��Ϣ�������ռ�ռ�ã��ļ��������ԣ��ļ�λ������������ʱ
// MFT �ĵ�һ���¼$MFT����������������MFT�������ı��Ϊ0��MFT���ͷ���������½ṹ��
typedef struct {
     UINT8    mark[4];             // "FILE" ��־ 
     UINT8    UsnOffset[2];        // �������к�ƫ�� ��������30 00
     UINT8    usnSize[2];          // �������������С+1 �� 03 00
     UINT8    LSN[8];              // ��־�ļ����к�(ÿ�μ�¼�޸ĺ�ı�)  58 8E 0F 34 00 00 00 00
    // 0x10
     UINT8    SN[2];               // ���к� �����ļ����¼���ô���������
     UINT8    linkNum[2];          // Ӳ������ (����Ŀ¼ָ����ļ�) 01 00
     UINT8    firstAttr[2];        // ��һ�����Ե�ƫ�ơ���38 00
     UINT8    flags[2];            // 0��ɾ�� 1�����ļ� 2��ɾ��Ŀ¼ 3Ŀ¼��ʹ��
    // 0x18
     UINT8    MftUseLen[4];        // ��¼��Ч����   ��A8 01 00 00
     UINT8    maxLen[4];            // ��¼ռ�ó��� �� 00 04 00 00
    // 0x20
     UINT8    baseRecordNum[8];     // ����������¼, ����ǻ�����¼��Ϊ0
     UINT8    nextAttrId[2];        // ��һ����Id����07 00
     UINT8    border[2];            //
     UINT8    xpRecordNum[4];       // ����xp, ��¼��
    // 0x30
     UINT8    USN[8];                 // �������к�(2B) �� ������������
}MFT_HEADER;

