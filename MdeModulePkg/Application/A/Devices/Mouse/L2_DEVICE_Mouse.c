
/*************************************************
    .
    File name:      	*.*
    Author：	        	任启红
    ID：					00001
    Date:          		202107
    Description:    	
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
    
    //左
    if (State.LeftButton == TRUE)
    {
    }

    //右
    if (State.RightButton == TRUE)
    {
    }
    
    stMouseState.ucX_RelativeMove = x_move;
    stMouseState.ucX_RelativeMove = y_move;

    //鼠标左键点击
    if (State.LeftButton == TRUE)
    {
        //如果原来是未点击状态，则改为点击状态，否则改为未点击状态
        MouseClickFlag = (MouseClickFlag == MOUSE_EVENT_TYPE_NO_CLICKED) ? MOUSE_EVENT_TYPE_LEFT_CLICKED : MOUSE_EVENT_TYPE_NO_CLICKED;        
    }
    
    //鼠标右键点击
    if (State.RightButton == TRUE)
    {        
        //如果原来是未点击状态，则改为点击状态，否则改为未点击状态
        MouseClickFlag = (MouseClickFlag == MOUSE_EVENT_TYPE_NO_CLICKED) ? MOUSE_EVENT_TYPE_RIGHT_CLICKED : MOUSE_EVENT_TYPE_NO_CLICKED;
    }
    

}



/****************************************************************************
*
*  描述:   鼠标事件的总入口，当鼠标有点击、移动事件时State值不为空，可以触发后续相关的绘图事件
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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
    
    //如果鼠标没有按键和鼠标移动，则直接返回。
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
*  描述:   鼠标图形事件相关变量初始化
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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

    if (NULL == pMouseClickBuffer)
    {
        return -1;
    }

    UINT8 *pBuffer = pMouseClickBuffer;
    UINT16 Width = MouseClickWindowWidth;


	for (UINT16 i = MouseClickWindowHeight / 2; i < MouseClickWindowHeight; i++)
	{
	    for (UINT16 j = 0; j < MouseClickWindowWidth; j++)
	    {
	        // WHITE
	        pBuffer[(i * Width + j) * 4 + 0] = 0xff;
	        pBuffer[(i * Width + j) * 4 + 1] = 0xff;
	        pBuffer[(i * Width + j) * 4 + 2] = 0xff;
	        pBuffer[(i * Width + j) * 4 + 3] = 0; 
	    }
	}
	
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;

	Color.Blue = 0xff;
	Color.Red  = 0xff;
	Color.Green= 0xff;
	Color.Reserved= 0;

	//汉字	区位码	汉字	区位码
    //打 	2082	开	3110
	UINT16 x = 3;
    UINT16 y = 3;

    L2_GRAPHICS_ChineseCharDraw2(pMouseClickBuffer, x , y,     20, 82, Color, MouseClickWindowWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pMouseClickBuffer, x , y,     31, 10, Color, MouseClickWindowWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pMouseClickBuffer, x , y,     12, 84, Color, MouseClickWindowWidth);   

    //汉字	区位码	汉字	区位码
    //运	 5243	行	4848
    //第二行
    x = 3;
    y += 16; 
    L2_GRAPHICS_ChineseCharDraw2(pMouseClickBuffer, x , y,     52, 43, Color, MouseClickWindowWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pMouseClickBuffer, x , y,     48, 48, Color, MouseClickWindowWidth);
    x += 16;
    
    L2_GRAPHICS_ChineseCharDraw2(pMouseClickBuffer, x , y,     12, 84, Color, MouseClickWindowWidth);  
}




/****************************************************************************
*
*  描述:   鼠标设备初始化，后续可以接收鼠标相关移动、点击事件
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
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

