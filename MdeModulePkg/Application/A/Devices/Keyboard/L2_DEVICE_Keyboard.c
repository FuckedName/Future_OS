
/*************************************************
    .
    File name:          *.*
    Author��                ������
    ID��                    00001
    Date:                  202107
    Description:        
    Others:             ��

    History:            ��
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

//��ǰ�ն˴�����ʾ����:
UINT16 TerminalCurrentLineCount = 0;

//Line 2
#define DISPLAY_KEYBOARD_X (0) 
#define DISPLAY_KEYBOARD_Y (ScreenHeight - 16 * 3  - 30)


//���ڼ�¼�Ѿ����������ַ�
char pKeyboardInputBuffer[KEYBOARD_BUFFER_LENGTH] = {0};
char pCommandLinePrefixBuffer[COMMAND_LINE_PREFIX_BUFFER_LENGTH] = {0};

EFI_HANDLE                        *Handles;
UINTN                             HandleCount;

#define KEYBOARD_KEY_ENTER (13) 


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
UINT32 L2_KEYBOARD_ParametersGet(UINT8 *ps, UINT8 parameters[PARAMETER_COUNT][PARAMETER_LENGTH], UINT8 *pParameterCount)
{
    UINT32 i = 0;    
    while(*ps != '\0')
    {
        // for example: clear -a aaa -b bbb -c ccc
        // for example: cd /, cd .., cd xxx
        if (' ' == *ps)
        {
            parameters[*pParameterCount][i] = '\0';    
            (*pParameterCount)++;
            i = 0;
        }
        
        parameters[*pParameterCount][i++] = *ps++;
    }

    parameters[*pParameterCount][i] = '\0';    
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
VOID L2_KEYBOARD_CommandHandle()
{    
    int parametercount = 0;
    char parameters[PARAMETER_COUNT][PARAMETER_LENGTH] = {0};
    
    L2_KEYBOARD_ParametersGet(pKeyboardInputBuffer, parameters, &parametercount);
    
    if (L1_STRING_Compare2(parameters[0], "address") == 0) // Enter pressed����ʾ�������ַ��ʼ��һ���ڴ��ַ
    {
        L2_System_PrintMemoryBuffer();                        
    }    
    else if (L1_STRING_Compare2(parameters[0], "clear") == 0) //When click 'clear' then Clear log window content
    {
        L2_GRAPHICS_SystemLogBufferClear();        
    }
    else if (L1_STRING_Compare2(parameters[0], "shutdown") == 0)  //When click 'shutdown' then Shutdown System
    {
        L2_System_Shutdown();
    }
    else if (L1_STRING_Compare2(parameters[0], "cd") == 0)  //When click 'shutdown' then Shutdown System
    {
        L2_APPLICATIONS_Command_cd(parameters);
    }
    else if (L1_STRING_Compare2(parameters[0], "ls") == 0)  //When click 'shutdown' then Shutdown System
    {
        L2_APPLICATIONS_Command_ls(parameters);
    }
    else if (L1_STRING_Compare2(parameters[0], "curl") == 0)  //When click 'shutdown' then Shutdown System
    {
        L2_APPLICATIONS_Command_curl(parameters);
    }
    else if (L1_STRING_Compare2(parameters[0], "ping") == 0)  //When click 'shutdown' then Shutdown System
    {
        L2_APPLICATIONS_Command_ping(parameters);
    }
    else if (L1_STRING_Compare2(parameters[0], "http") == 0)  //When click 'shutdown' then Shutdown System
    {
        L2_APPLICATIONS_Command_http(parameters);
    }

    //��ʾ����İ���
    L2_DEBUG_Print1(DISPLAY_KEYBOARD_X, DISPLAY_KEYBOARD_Y, "%a keyboard_input_count: %04d ", pKeyboardInputBuffer, keyboard_input_count);      
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
            L1_STRING_Copy(pCommandLinePrefixBuffer, "[root@Notepad /home/Jason/]# ");
            //char pCommandLinePrefixBuffer[COMMAND_LINE_PREFIX_BUFFER_LENGTH] = ;
            //����
            int TerminalWindowMaxLineCount = 0;
            TerminalWindowMaxLineCount = (ScreenHeight - 23) / 2;
            TerminalWindowMaxLineCount /= 16;
            
            //д����̻��浽�ն˴��ڡ�
            L2_DEBUG_Print3(3, 23 + TerminalCurrentLineCount % TerminalWindowMaxLineCount * 16, WindowLayers.item[GRAPHICS_LAYER_TERMINAL_WINDOW], "%a%a", pCommandLinePrefixBuffer, pKeyboardInputBuffer);
        }

        if (KEYBOARD_KEY_ENTER == uniChar)
        {                        
            L2_KEYBOARD_CommandHandle();
            
            //��ʼ�����������ַ�����
            keyboard_input_count = 0;

            //����ûس������ʾ��Ҫ����һ��
            TerminalCurrentLineCount++;

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

