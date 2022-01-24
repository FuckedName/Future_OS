
/*************************************************
    .
    File name:      	*.*
    Author：	        	任启红
    ID：					00001
    Date:          		202201
    Description:    	用于给应用提供接口
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





#include <Library/UefiRuntimeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "L2_APPLICATIONS.h"

#include <Global/Global.h>


/****************************************************************************
*
*  描述:   操作系统运行应用程序
*
*  ApplicationCallID：     应用调用接口编号
*  pParameters：           参数列表
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L2_ApplicationRun(UINT8 *pPath)
{
    EFI_STATUS Status;
    UINT8 ucCode[0x1f];

    //Allocate buffer for test.o
    UINT8 *pApplicationBuffer = L2_MEMORY_Allocate("System Icon Buffer", MEMORY_TYPE_APPLICATION, 1704);
    
    //1. Read FileName File into Buffer1;
    Status = L3_APPLICATION_ReadFile("TESTO", 5, pApplicationBuffer); //test.o
    if (EFI_ERROR(Status))
    {
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_ApplicationRun.\n", __LINE__);
    } 
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_ApplicationRun. %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n", __LINE__,
                pApplicationBuffer[0],
                pApplicationBuffer[1],
                pApplicationBuffer[2],
                pApplicationBuffer[3],
                pApplicationBuffer[4],
                pApplicationBuffer[5],
                pApplicationBuffer[6],
                pApplicationBuffer[7],
                pApplicationBuffer[8],
                pApplicationBuffer[9],
                pApplicationBuffer[10],
                pApplicationBuffer[11],
                pApplicationBuffer[12],
                pApplicationBuffer[13],
                pApplicationBuffer[14]);

    for (UINT32 i = 0; i < 0x1f; i++)
        pApplication[i] = pApplicationBuffer[64 + i]; // 64:  elf64 header length. 
    return;            
    //2. Analysis Buffer1 ELF and get obj Code1, copy code to pApplication;
    L2_ApplicationAnalysisELF(pApplicationBuffer, pApplication);
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_ApplicationRun. %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n", __LINE__,
                    pApplication[0],
                    pApplication[1],
                    pApplication[2],
                    pApplication[3],
                    pApplication[4],
                    pApplication[5],
                    pApplication[6],
                    pApplication[7],
                    pApplication[8],
                    pApplication[9],
                    pApplication[10],
                    pApplication[11],
                    pApplication[12],
                    pApplication[13],
                    pApplication[14]);
    
    //3. Allocate Memory Buffer2 for Application;
    //4. Copy Code1 into  Buffer2;
    //5. Run Buffer2 after apply cpu time slice;
}



