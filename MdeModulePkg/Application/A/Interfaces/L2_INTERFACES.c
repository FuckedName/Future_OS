
/*************************************************
    .
    File name:      	*.*
    Author：	        	任启红
    ID：					00001
    Date:          		202107
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

#include "L2_INTERFACES.h"

#include <Global/Global.h>
#include "L2_DEVICE_System.h"



UINT32 ApplicationCallFlag = FALSE;
UINT32 *pApplicationCallFlag = 0x20000000;


double (*pAPPLICATION_CALL_ID)(double, double);
UINT32 *APPLICATION_CALL_FLAG_ADDRESS = (UINT32 *)(0x20000000 - 0x1000);


APPLICATION_CALL_DATA *pApplicationCallData;


typedef struct
{
    APPLICATION_CALL_ID             ApplicationCallID; //类似Linux系统调用
    EFI_STATUS                      (*pApplicationCallFunction)();  //不用调用ID对应的处理函数
}APPLICATION_CALL_TABLE;


/****************************************************************************
*
*  描述:   桌面图层点击事件，当前只有左下角的开始菜单点击事件
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
****************************************************************************
MOUSE_CLICK_EVENT L2_GRAPHICS_DeskLayerClickEventGet()
{

	return MAX_CLICKED_EVENT;
}*/

VOID L2_INTERFACES_PrintString()
{    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %a\n", __LINE__, pApplicationCallData->pApplicationCallInput);
}


/****************************************************************************
*
*  描述:   根据应用调用的接口类型，找到对应的处理函数。
*  第一列：接口类型定义
*  第二列：处理函数
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
APPLICATION_CALL_TABLE InterfaceCallTable[] =
{
    {APPLICATION_CALL_ID_INIT,       		    NULL},
    {APPLICATION_CALL_ID_SHUTDOWN,       		L2_System_Shutdown},
    {APPLICATION_CALL_ID_PRINT_STRING,       	L2_INTERFACES_PrintString},
    {APPLICATION_CALL_ID_MAX,                   NULL},
};


/****************************************************************************
*
*  描述:   给操作系统应用程序提供调用的接口
*
*  ApplicationCallID：     应用调用接口编号
*  pParameters：           参数列表
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L2_INTERFACES_ApplicationCall(APPLICATION_CALL_ID ApplicationCallID)
{
    //进入系统调用
    if (APPLICATION_CALL_ID_INIT != pApplicationCallData->ID)
    {
        //执行系统调用
        InterfaceCallTable[pApplicationCallData->ID].pApplicationCallFunction();

        //把系统调用类型设置为初始化，不然会进入死循环
        pApplicationCallData->ID = APPLICATION_CALL_ID_INIT;
    }    
}


/****************************************************************************
*
*  描述:   给操作系统应用程序提供调用的接口
*
*  ApplicationCallID：     应用调用接口编号
*  pParameters：           参数列表
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L2_INTERFACES_Shutdown()
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_ApplicationShutdown \n", __LINE__);
    
}


/****************************************************************************
*
*  描述:   给操作系统应用程序提供调用的接口
*
*  ApplicationCallID：     应用调用接口编号
*  pParameters：           参数列表
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L2_INTERFACES_Initial()
{
    pApplicationCallData = APPLICATION_CALL_FLAG_ADDRESS;
    pApplicationCallData->ID = APPLICATION_CALL_ID_INIT;
}


