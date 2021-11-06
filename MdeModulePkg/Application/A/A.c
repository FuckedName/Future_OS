
/*************************************************
    .
    File name:      	*.*
    Author：	        	任启红
    ID：					00001
    Date:          		202107
    Description:    	整个模块的主入口函数
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




#define Note ""

#include <Library/MemoryAllocationLib.h>


#include <Libraries/Math/L1_LIBRARY_Math.h>
#include <Libraries/Network/L1_LIBRARY_Network.h>
#include <Libraries/String/L2_LIBRARY_String.h>
#include <Libraries/String/L1_LIBRARY_String.h>
#include <Libraries/Memory/L1_LIBRARY_Memory.h>
#include <Libraries/DataStructure/L1_LIBRARY_DataStructure.h>
#include <Partitions/FAT32/L2_PARTITION_FAT32.h>
#include <Graphics/L3_GRAPHICS.h>
#include <Devices/Mouse/L2_DEVICE_Mouse.h>
#include <Devices/Keyboard/L2_DEVICE_Keyboard.h>
#include <Devices/Timer/L2_DEVICE_Timer.h>
#include <Devices/Store/L2_DEVICE_Store.h>
#include <Devices/Screen/L2_DEVICE_Screen.h>

#include <Memory/L2_MEMORY.h>
#include <Memory/L1_MEMORY.h>
#include <Global/Global.h>
#include <Partitions/L2_PARTITION.h>
#include <Processes/L2_PROCESS_Multi.h>


//https://blog.csdn.net/goodwillyang/article/details/45559925
//注意：FAT32分区的卷标存放在第2个簇前几个字符
//      NTFS分区的卷标存放在MFT表项$VOLUME表里

// 用于测试内存分配的地址测试
char *p1;   
extern const UINT8 sASCII[][16];




/****************************************************************************
*
*  描述:     整个系统总入口函数
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS EFIAPI Main (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS  Status;    

    //For test AllocatePool allocated memory memory phyics address.
    char *pBuffer = (UINT8 *)AllocatePool(4); 
    if (NULL == pBuffer)
    {
        //DEBUG ((EFI_D_INFO, "ScreenInit AllocatePool pDeskBuffer NULL\n"));
        return -1;
    }

	// 测试内存地址
    INFO_SELF(L"Main: 0x%X Status: 0x%X sASCII: 0x%X p1: 0x%X pBuffer: 0x%X \r\n", Main, &Status, sASCII, p1, pBuffer);  

	L2_SCREEN_Init();
	
    L2_COMMON_MemoryAllocate();

	L2_GRAPHICS_BootScreenInit();

    L2_GRAPHICS_ParameterInit();

    L2_MOUSE_Init();
        
    // get partitions use api
    L2_STORE_PartitionAnalysis();
    
    L2_COMMON_MultiProcessInit();
	
    L2_GRAPHICS_ScreenInit();
    
    L2_GRAPHICS_StartMenuInit();

    L2_GRAPHICS_SystemSettingInit();

	L3_APPLICATION_WindowsInitial();
    
    L2_TIMER_IntervalInit();    
        
    return EFI_SUCCESS;
}


