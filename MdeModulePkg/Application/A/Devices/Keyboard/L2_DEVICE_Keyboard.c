
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




#include <Library/UefiLib.h>
#include <Library/BaseLib.h>

#include <string.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Global/Global.h>



#include <L2_DEVICE_Keyboard.h>
//#include <Device/Mouse/L2_DEVICE_Mouse.h>

//#include <Graphics/L2_GRAPHICS.h>

int keyboard_input_count = 0;

UINT16 display_sector_number = 0;

UINT16 keyboard_count = 0;

//Line 2
#define DISPLAY_KEYBOARD_X (0) 
#define DISPLAY_KEYBOARD_Y (ScreenHeight - 16 * 3  - 30)

#define KEYBOARD_BUFFER_LENGTH (30) 

//���ڼ�¼�Ѿ����������ַ�
char pKeyboardInputBuffer[KEYBOARD_BUFFER_LENGTH] = {0};

EFI_HANDLE                        *Handles;
UINTN                             HandleCount;

#define KEYBOARD_KEY_ENTER (13) 




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
VOID L2_KEYBOARD_KeyPressed()
{
    //DEBUG ((EFI_D_INFO, "%d HandleEnterPressed\n", __LINE__));
    
        
    EFI_STATUS Status;
    //PartitionUSBReadSynchronous();
    //PartitionUSBReadAsynchronous();

    // char buffer[SYSTEM_ICON_WIDTH * SYSTEM_ICON_HEIGHT * 3 + 0x36] = {0};
    
    //DEBUG ((EFI_D_INFO, "%d HandleEnterPressed\n", __LINE__));
}


VOID EFIAPI L2_KEYBOARD_Init (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
    EFI_STATUS Status;
        
    Status = gBS->LocateHandleBuffer ( ByProtocol,
                                        &gEfiSimpleTextInputExProtocolGuid,
                                        NULL,
                                        &HandleCount,
                                        &Handles
                                        );    
    if(EFI_ERROR (Status))
        return ;

}



/****************************************************************************
*
*  ����:   ������������������
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_KEYBOARD_CommandHandle (UINT16 uniChar)
{
    // Enter pressed����ʾ�������ַ��ʼ��һ���ڴ��ַ
    //if (0x0D == uniChar)
    if (0x0D == 0x0c)
    {
        UINT64 Sumary = 0;
        
        for (UINT16 i = 0; '\0' != pKeyboardInputBuffer[i]; i++)
        {
            if(pKeyboardInputBuffer[i] >= '0' && pKeyboardInputBuffer[i] <= '9')
            {
                Sumary = Sumary * 10 + (pKeyboardInputBuffer[i] - '0');
            }
        }
        
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Sumary: %d \n", __LINE__, Sumary);
        

        UINT8 *pAddress = (UINT8 *)Sumary;
        
        for (int j = 0; j < 256; j++)
        {
            L2_DEBUG_Print1(DISK_READ_BUFFER_X + ScreenWidth * 3 / 4 + (j % 16) * 8 * 3, DISK_READ_BUFFER_Y + 16 * (j / 16), "%02X ", pAddress[j] & 0xff);
        }
                        
        L2_KEYBOARD_KeyPressed();
    }
    //When click 'clear' then Clear log window content
    else if (L1_STRING_Compare2(pKeyboardInputBuffer, "clear") == 0)
    {
        for (UINT32 i = 23; i < SystemLogWindowHeight - 3; i++)
        {
            for (UINT32 j = 3; j < SystemLogWindowWidth - 3; j++)
            {
                pSystemLogWindowBuffer[4 * (i * SystemLogWindowWidth + j)] = 0;
                pSystemLogWindowBuffer[4 * (i * SystemLogWindowWidth + j) + 1] = 0;
                pSystemLogWindowBuffer[4 * (i * SystemLogWindowWidth + j) + 2] = 0;
            }
        }   

        //��ʼ���󣬴ӵ�1�п�ʼ��ʾ
        LogStatusErrorCount = 0;

        //��ʾ����İ���
        L2_DEBUG_Print1(DISPLAY_KEYBOARD_X, DISPLAY_KEYBOARD_Y, "%a keyboard_input_count: %04d ", pKeyboardInputBuffer, keyboard_input_count);
    }
    else if (L1_STRING_Compare2(pKeyboardInputBuffer, "shutdown") == 0)
    {
    	L2_System_Shutdown();
    }
	else
	{
	
		//��ʾ����İ���
		L2_DEBUG_Print1(DISPLAY_KEYBOARD_X, DISPLAY_KEYBOARD_Y, "%a keyboard_input_count: %04d ", pKeyboardInputBuffer, keyboard_input_count);
	}

}



/****************************************************************************
*
*  ����:   ���̰����¼���Ӧ����ں���
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID EFIAPI L2_KEYBOARD_Event (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
    EFI_STATUS Status;
    UINTN                             HandleIndex;
    UINTN                             Index;
    EFI_KEY_TOGGLE_STATE toggleState;

    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *SimpleEx;
    EFI_KEY_DATA                      KeyData;
    UINT16 scanCode = 0;
    UINT16 uniChar = 0;
    UINT32 shiftState;
    
    keyboard_count++;
        
    for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) 
    {
        Status = gBS->HandleProtocol (Handles[HandleIndex], &gEfiSimpleTextInputExProtocolGuid, (VOID **) &SimpleEx);
        if (EFI_ERROR(Status))  
        {    
            continue;
        }
        
        Status = gBS->CheckEvent(SimpleEx->WaitForKeyEx);
        if (Status == EFI_NOT_READY)
            continue;
          
        gBS->WaitForEvent(1, &(SimpleEx->WaitForKeyEx), &Index);

        Status = SimpleEx->ReadKeyStrokeEx(SimpleEx, &KeyData);
        if(EFI_ERROR(Status))
        {
            continue;
        }
        
        uniChar     = KeyData.Key.UnicodeChar;
        L2_DEBUG_Print1(0, ScreenHeight - 30 - 2 * 16, "%d: L2_KEYBOARD_Event input uniChar: %d", __LINE__, uniChar);

		if (KEYBOARD_KEY_ENTER != uniChar)
	    {
	    	pKeyboardInputBuffer[keyboard_input_count++] = uniChar;
	    }

		if (KEYBOARD_KEY_ENTER == uniChar)
	    {			
	    	L2_KEYBOARD_CommandHandle(uniChar);
			
	        //��ʼ�����������ַ�����
	        keyboard_input_count = 0;
	        
	        for (UINT16 i = 0; i < KEYBOARD_BUFFER_LENGTH; i++)
	            pKeyboardInputBuffer[i] = '\0';
		}
        
    }  
    
     //DrawAsciiCharUseBuffer(pDeskBuffer, DISPLAY_KEYBOARD_X, DISPLAY_KEYBOARD_Y, uniChar, Color);

	//����ͼ����Ϣ������������Ż��£����û�а����¼�������Բ�����ͼ�㣬�Խ�ʡCPU��Դ
	//������Ļ��ʱ���ڲ�ͣ���ߣ�ʱ��Ƭ��ϢҲ�ڲ��ϵı仯�����Բ�������ȫ�Ż�
	//����������������Ϣ����ֻˢ�¶�Ӧ���������ǿ��Ե�
    L2_GRAPHICS_LayerCompute(iMouseX, iMouseY, 0);
}

