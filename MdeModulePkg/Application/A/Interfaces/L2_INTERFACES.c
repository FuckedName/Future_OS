
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


typedef struct
{
    APPLICATION_CALL_ID             ApplicationCallID;
    EFI_STATUS                      (*pApplicationCallFunction)(); 
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



/****************************************************************************
*
*  描述:   根据鼠标光标所在图层，找到图层点击事件，并根据点击事件找到对应的响应处理函数。
*  第一列：不同图层走到对应处理函数
*  第二列：根据图层ID获取不同图层的点击事件XXXXXClickEventGet
*  第三列：处理不同事件XXXXXClickEventHandle
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
APPLICATION_CALL_TABLE ApplicationCallTable[] =
{
    {APPLICATION_CALL_ID_INIT,       		    NULL},
    {APPLICATION_CALL_ID_SHUTDOWN,       		L2_System_Shutdown},
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
VOID L2_ApplicationCall(APPLICATION_CALL_ID ApplicationCallID)
{
    if (0 != *APPLICATION_CALL_FLAG_ADDRESS)
        return ApplicationCallTable[*APPLICATION_CALL_FLAG_ADDRESS].pApplicationCallFunction();
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
VOID L2_ApplicationInitial()
{
    *APPLICATION_CALL_FLAG_ADDRESS = APPLICATION_CALL_ID_INIT;
}


