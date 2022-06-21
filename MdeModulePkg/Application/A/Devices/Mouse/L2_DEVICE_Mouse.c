
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




#include <Library/BaseLib.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Global/Global.h>

#include <L2_DEVICE_Mouse.h>

#include <Graphics/L2_GRAPHICS.h>

typedef struct
{
    CHAR8 ucX_RelativeMove;
    CHAR8 ucY_RelativeMove;
    BOOLEAN LeftButton;
    BOOLEAN RightButton;
    BOOLEAN MiddleButton;
}MOUSE_STATE;

VOID L2_MOUSE_StateAnaysis(EFI_SIMPLE_POINTER_STATE State)
{
    MOUSE_STATE stMouseState;
        
    //X
    if (State.RelativeMovementX < 0)
    {
        int temp = State.RelativeMovementX >> 16;
        x_move = (0xFFFF - temp) & 0xff;  
        x_move = - x_move;
    }
    else if(State.RelativeMovementX > 0)
    {
        x_move = (State.RelativeMovementX >> 16) & 0xff;
    }

    //Y
    if (State.RelativeMovementY < 0)
    {       
        int temp = State.RelativeMovementY >> 16;
        y_move = (0xFFFF - temp) & 0xff;  
        y_move = - y_move;
    }
    else if(State.RelativeMovementY > 0)
    {
        y_move = (State.RelativeMovementY >> 16) & 0xff;
    }
        
    stMouseState.ucX_RelativeMove = x_move;
    stMouseState.ucX_RelativeMove = y_move;

    //���������
    if (State.LeftButton == TRUE)
    {
        //���ԭ����δ���״̬�����Ϊ���״̬�������Ϊδ���״̬
        MouseClickFlag = (MouseClickFlag == MOUSE_EVENT_TYPE_NO_CLICKED) ? MOUSE_EVENT_TYPE_LEFT_CLICKED : MOUSE_EVENT_TYPE_NO_CLICKED;        
    }
    
    //����Ҽ����
    if (State.RightButton == TRUE)
    {        
        //���ԭ����δ���״̬�����Ϊ���״̬�������Ϊδ���״̬
        MouseClickFlag = (MouseClickFlag == MOUSE_EVENT_TYPE_NO_CLICKED) ? MOUSE_EVENT_TYPE_RIGHT_CLICKED : MOUSE_EVENT_TYPE_NO_CLICKED;
    }
    

}



/****************************************************************************
*
*  ����:   ����¼�������ڣ�������е�����ƶ��¼�ʱStateֵ��Ϊ�գ����Դ���������صĻ�ͼ�¼�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID EFIAPI L2_MOUSE_Event (IN EFI_EVENT Event, IN VOID *Context)
{
    mouse_count++;
    //L2_DEBUG_Print1(DISPLAY_X, DISPLAY_Y, "%d: HandleMouseEvent\n", __LINE__);
    EFI_STATUS Status;
    UINTN Index;
    EFI_SIMPLE_POINTER_STATE State;

    Status = gBS->CheckEvent(gMouse->WaitForInput); 
    if (Status == EFI_NOT_READY)
    {
        //return ;
    }
    
    Status = gMouse->GetState(gMouse, &State);
    if (Status == EFI_DEVICE_ERROR)
    {
        return ;
    }
    
    //������û�а���������ƶ�����ֱ�ӷ��ء�
    if (0 == State.RelativeMovementX && 0 == State.RelativeMovementY && 0 == State.LeftButton && 0 == State.RightButton)
    {
        return;
    }

    L2_MOUSE_StateAnaysis(State);

    ////DEBUG ((EFI_D_INFO, "X: %X, Y: %X ", x_move, y_move));
    L2_DEBUG_Print1(0, ScreenHeight - 30 -  8 * 16, "%d: X: %04d, Y: %04d move X: %d move Y: %d", __LINE__, iMouseX, iMouseY, x_move, y_move );
    
    //L2_DEBUG_Print1(0, ScreenHeight - 30 -  8 * 16, "%d: X move: %d Y move: %d left: %d right: %d", __LINE__, State.RelativeMovementX, State.RelativeMovementY, State.LeftButton, State.RightButton);
	
	iMouseX = iMouseX + x_move * 3;
	iMouseY = iMouseY + y_move * 3; 

	iMouseX = (iMouseX < 0) ? 0 : iMouseX;
	iMouseX = (iMouseX > ScreenWidth - 16) ? ScreenWidth - 16 : iMouseX;
	
	iMouseY = (iMouseY < 0) ? 0 : iMouseY;
	iMouseY = (iMouseY > ScreenHeight - 16) ? ScreenHeight - 16 : iMouseY;
	
    L2_MOUSE_Move();
    
    gBS->WaitForEvent( 1, &gMouse->WaitForInput, &Index );
}




/****************************************************************************
*
*  ����:   ���ͼ���¼���ر�����ʼ��
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_MOUSE_GraphicsEventInit()
{    
    MouseMoveoverObject.StartX = 0;
    MouseMoveoverObject.EndX   = 0;
    MouseMoveoverObject.StartY = 0;
    MouseMoveoverObject.EndY   = 0;
    MouseMoveoverObject.GraphicsLayerID = 0;
    bMouseMoveoverObject = FALSE;

    if (NULL == pMouseRightButtonClickWindowBuffer)
    {
        return -1;
    }

    UINT16 Width = MouseRightButtonClickWindowWidth;


    //��ʵ����е㲻�Ͻ�����Ϊ����һ������ж��ֵ����˵������磬��ѡ�е�Ŀ�꣬û��ѡ�е�Ŀ��
    UINT16 MouseRightButtonClickMenuChineseName[MOUSE_RIGHT_BUTTON_CLICK_MENU_MAX][8] = 
    {
        {20,82,31,10,12, 84,0,0}, //���򿪡��ļ�
        {41,30,19,93,12, 84,0,0}, //��ɾ�����ļ�
        {48,34,52,86,12, 84,0,0}, //���������ļ�
        {48,62,24,36,12, 84,0,0}, //���޸ġ��ļ�
    };
    
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    Color.Blue  = 0x00;
    Color.Red   = 0x00;
    Color.Green = 0x00;
    Color.Reserved = GRAPHICS_LAYER_MOUSE_RIGHT_CLICK_WINDOW;
        
    //������ɫ��ʼ��
	for (UINT16 i = 0; i < MouseRightButtonClickWindowHeight; i++)
	{
	    for (UINT16 j = 0; j < MouseRightButtonClickWindowWidth; j++)
	    {
	        pMouseRightButtonClickWindowBuffer[(i * Width + j) * 4 + 0] = 0xee;
	        pMouseRightButtonClickWindowBuffer[(i * Width + j) * 4 + 1] = 0xee;
	        pMouseRightButtonClickWindowBuffer[(i * Width + j) * 4 + 2] = 0xee;
	        pMouseRightButtonClickWindowBuffer[(i * Width + j) * 4 + 3] = GRAPHICS_LAYER_MOUSE_RIGHT_CLICK_WINDOW; 
	    }
	}
	
    UINT16 x1, y1;
    y1 = 10;

    for (UINT32 j = 0; j < SYSTEM_ICON_MAX; j++)
    {
        x1 = 10;
        

        for (UINT16 i = 0; i < 4; i++)
        {
            UINT16 AreaCode = MouseRightButtonClickMenuChineseName[j][2 * i];
            UINT16 BitCode = MouseRightButtonClickMenuChineseName[j][2 * i + 1];
            
            if (0 != AreaCode && 0 != BitCode)
                L2_GRAPHICS_ChineseCharDraw(pMouseRightButtonClickWindowBuffer, x1, y1, (AreaCode - 1) * 94 + BitCode - 1, Color, MouseRightButtonClickWindowWidth);
                
            x1 += 16;
        }        
        y1 += 16;
    }        

    return;    
}




/****************************************************************************
*
*  ����:   ����豸��ʼ�����������Խ����������ƶ�������¼�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
EFI_STATUS L2_MOUSE_Init()
{
    EFI_STATUS                         Status;
    EFI_HANDLE                         *PointerHandleBuffer = NULL;
    UINTN                              i = 0;
	MouseClickFlag 					   = MOUSE_EVENT_TYPE_NO_CLICKED;
    UINTN                              HandleCount = 0;
        
    //get the handles which supports
    Status = gBS->LocateHandleBuffer(
                                    ByProtocol,
                                    &gEfiSimplePointerProtocolGuid,
                                    NULL,
                                    &HandleCount,
                                    &PointerHandleBuffer
                                    );
        
    if (EFI_ERROR(Status))  return Status;      //unsupport
    
    for (i = 0; i < HandleCount; i++)
    {
        Status = gBS->HandleProtocol(PointerHandleBuffer[i],
                                    &gEfiSimplePointerProtocolGuid,
                                    (VOID**)&gMouse);
            
        if (EFI_ERROR(Status))  
            continue;
        
        else
        {
            return EFI_SUCCESS;
        }
    }   
        
    return EFI_SUCCESS;
}

