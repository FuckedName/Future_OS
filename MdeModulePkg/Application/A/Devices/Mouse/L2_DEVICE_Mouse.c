
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
    if (0 == State.RelativeMovementX && 0 == State.RelativeMovementY && 0 == State.LeftButton && 0 == State.RightButton)
    {
        return;
    }
    
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

    ////DEBUG ((EFI_D_INFO, "X: %X, Y: %X ", x_move, y_move));
    L2_DEBUG_Print1(0, ScreenHeight - 30 -  8 * 16, "%d: X: %04d, Y: %04d move X: %d move Y: %d", __LINE__, iMouseX, iMouseY, x_move, y_move );
    
    //L2_DEBUG_Print1(0, ScreenHeight - 30 -  8 * 16, "%d: X move: %d Y move: %d left: %d right: %d", __LINE__, State.RelativeMovementX, State.RelativeMovementY, State.LeftButton, State.RightButton);
	
	iMouseX = iMouseX + x_move * 3;
	iMouseY = iMouseY + y_move * 3; 

	iMouseX = (iMouseX < 0) ? 0 : iMouseX;
	iMouseX = (iMouseX > ScreenWidth - 16) ? ScreenWidth - 16 : iMouseX;
	
	iMouseY = (iMouseY < 0) ? 0 : iMouseY;
	iMouseY = (iMouseY > ScreenHeight - 16) ? ScreenHeight - 16 : iMouseY;
	
    //Button
    if (State.LeftButton == MOUSE_LEFT_CLICKED)
    {
        //DEBUG ((EFI_D_INFO, "Left button clicked\n"));
        
        L2_GRAPHICS_RightClickMenu(iMouseX, iMouseY);
        
        MouseClickFlag = (MouseClickFlag == MOUSE_NO_CLICKED) ? MOUSE_LEFT_CLICKED : MOUSE_NO_CLICKED;
        
        //DrawAsciiCharUseBuffer(GraphicsOutput, 20 + process2_i * 8 + 16, 60, 'E', Color);  
    }
    
    if (State.RightButton == 0x01)
    {
        //DEBUG ((EFI_D_INFO, "Right button clicked\n"));
        //DrawAsciiCharUseBuffer(GraphicsOutput, 20 + process2_i * 8 + 16, 60, 'R', Color);

        L2_GRAPHICS_RightClickMenu(iMouseX, iMouseY);
        
        MouseClickFlag = 2;

        TCP4Test();
    }
    //L2_DEBUG_Print1(DISPLAY_X, DISPLAY_Y, "%d: HandleMouseEvent\n", __LINE__);
    ////DEBUG ((EFI_D_INFO, "\n"));
    L2_MOUSE_Move();
    
    gBS->WaitForEvent( 1, &gMouse->WaitForInput, &Index );
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
	MouseClickFlag 					   = MOUSE_NO_CLICKED;
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


