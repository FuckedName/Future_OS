
/*************************************************
    .
    File name:      	*.*
    Author��	        	������
    ID��					00001
    Date:          		202107
    Description:    	��괦��ĳ�ʼ������
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

//���ڼ�¼����ƶ�ʱ�����ָ�Ĳ˵���ͼ�ꡢ�ļ��С��ļ��ȵ�
//������ɵ���˵���ͼ���λ����Ϣ
MOUSE_MOVEOVER_OBJECT MouseMoveoverObject;

//����Ƿ���һ���ɵ����ť�ϱ߱�־
BOOLEAN bMouseMoveoverObject;

/****************************************************************************
*
*  ����:   xxxxx
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/


