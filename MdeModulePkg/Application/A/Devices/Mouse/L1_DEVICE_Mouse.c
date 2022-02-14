
/*************************************************
    .
    File name:      	*.*
    Author：	        	任启红
    ID：					00001
    Date:          		202107
    Description:    	鼠标处理的初始化操作
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




#include <L1_DEVICE_Mouse.h>

EFI_SIMPLE_POINTER_PROTOCOL  *gMouse;
UINT16 mouse_count = 0;
UINT8 MouseClickFlag = MOUSE_EVENT_TYPE_NO_CLICKED;

CHAR8 x_move = 0;
CHAR8 y_move = 0;

INT16 iMouseX = 0;
INT16 iMouseY = 0;


UINT16 MouseRightButtonClickWindowWidth = 140;
UINT16 MouseRightButtonClickWindowHeight = 300;

//用于记录鼠标移动时光标所指的菜单、图标、文件夹、文件等等
//并保存可点击菜单、图标的位置信息
MOUSE_MOVEOVER_OBJECT MouseMoveoverObject;

//鼠标是否在一个可点击按钮上边标志
BOOLEAN bMouseMoveoverObject;

/****************************************************************************
*
*  描述:   xxxxx
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/


