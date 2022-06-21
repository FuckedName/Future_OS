
/*************************************************
    .
    File name:      	*.*
    Author��	        	������
    ID��					00001
    Date:          		202107
    Description:    	ͼ�δ����һЩ�����㷨������������ģ��
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

#include <Library/BaseLib.h>


typedef enum
{
    GRAPHICS_LAYER_DESK = 0,
    GRAPHICS_LAYER_START_MENU, //1
    GRAPHICS_LAYER_SYSTEM_SETTING_WINDOW, //2
    GRAPHICS_LAYER_MY_COMPUTER_WINDOW, //3
    GRAPHICS_LAYER_SYSTEM_LOG_WINDOW, //5
    GRAPHICS_LAYER_MEMORY_INFORMATION_WINDOW, //6
    GRAPHICS_LAYER_MOUSE_RIGHT_CLICK_WINDOW, //7
    GRAPHICS_LAYER_MOUSE, //��Զ���ᴥ������¼��㣬��Ϊ����ϱ�û�������¼���
    GRAPHICS_LAYER_MAX
}GRAPHICS_LAYER_ID;

unsigned char L1_GRAPHICS_Bilinear(double a, double blue, int uv, int u1v, int uv1, int u1v1);

void L1_GRAPHICS_ZoomImage(UINT8* pDest, unsigned int DestWidth, unsigned int DestHeight, UINT8* pSource, unsigned int SourceWidth, unsigned int SourceHeight );

