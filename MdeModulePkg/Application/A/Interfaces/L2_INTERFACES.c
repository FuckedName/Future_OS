
/*************************************************
    .
    File name:          *.*
    Author��                ������
    ID��                    00001
    Date:                  202107
    Description:        ���ڸ�Ӧ���ṩ�ӿ�
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





#include <Library/UefiRuntimeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "L2_INTERFACES.h"

#include <Global/Global.h>
#include "L2_DEVICE_System.h"

#include <L2_PROCESS_Multi.h>


UINT32 ApplicationCallFlag = FALSE;
UINT32 *pApplicationCallFlag = 0x20000000;


double (*pAPPLICATION_CALL_ID)(double, double);

//���Ǹ�ϵͳ���ýṹ�������ڴ��ַ
APPLICATION_CALL_DATA *APPLICATION_CALL_FLAG_ADDRESS = (APPLICATION_CALL_DATA *)(0x40000000 - 0x1000);

//���Ǹ�Ӧ�ó��������ڴ��ַ
#define APPLICATION_DYNAMIC_MEMORY_ADDRESS_START 0x40000000

APPLICATION_CALL_DATA *pApplicationCallData;


typedef struct
{
    APPLICATION_CALL_ID             ApplicationCallID; //����Linuxϵͳ����
    EFI_STATUS                      (*pApplicationCallFunction)();  //���õ���ID��Ӧ�Ĵ�����
}APPLICATION_CALL_TABLE;


/****************************************************************************
*
*  ����:   ����ͼ�����¼�����ǰֻ�����½ǵĿ�ʼ�˵�����¼�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
****************************************************************************
MOUSE_CLICK_EVENT L2_GRAPHICS_DeskLayerClickEventGet()
{

    return MAX_CLICKED_EVENT;
}*/

EFI_STATUS L2_INTERFACES_PrintString()
{    

    UINT16 x, y;
    UINT8 S = "Test";
    x = DISPLAY_DESK_DATE_TIME_X - 200;
    y = DISPLAY_DESK_DATE_TIME_Y - 200;

    L2_DEBUG_Print1(x, y, "%d testfunction. %a",  __LINE__, pApplicationCallData->pApplicationCallInput);

    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d Status: %a\n", __LINE__, pApplicationCallData->pApplicationCallInput);
}


/****************************************************************************
*
*  ����:   ����Ӧ�õ��õĽӿ����ͣ��ҵ���Ӧ�Ĵ�������
*  ��һ�У��ӿ����Ͷ���
*  �ڶ��У�������
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
APPLICATION_CALL_TABLE InterfaceCallTable[] =
{
    {APPLICATION_CALL_ID_INIT,                   NULL},
    {APPLICATION_CALL_ID_SHUTDOWN,               L2_System_Shutdown},
    {APPLICATION_CALL_ID_PRINT_STRING,           L2_INTERFACES_PrintString},
    {APPLICATION_CALL_ID_MAX,                   NULL},
};


/****************************************************************************
*
*  ����:   ������ϵͳӦ�ó����ṩ���õĽӿ�
*
*  ApplicationCallID��     Ӧ�õ��ýӿڱ��
*  pParameters��           �����б�
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_INTERFACES_ApplicationCall (EFI_EVENT Event,  VOID           *Context)
{
    UINT16 x, y;
    UINT8 S = "Test";
    x = DISPLAY_DESK_DATE_TIME_X - 200;
    y = DISPLAY_DESK_DATE_TIME_Y - 300;    
    
    L2_DEBUG_Print1(x, y, "%d  L2_INTERFACES_ApplicationCall.\n", __LINE__);
    
    //����ϵͳ����
    if (APPLICATION_CALL_ID_INIT != pApplicationCallData->ID)
    {
        //ִ��ϵͳ����
        EFI_STATUS RanStatus = InterfaceCallTable[pApplicationCallData->ID].pApplicationCallFunction();

        //����ص�����ִ�гɹ������Ӧ�ó���һ��������������ԣ����ֻص���������ִ�гɹ�
        pApplicationCallData->RanStatus = RanStatus;

        //��ϵͳ������������Ϊ��ʼ������Ȼ�������ѭ��
        pApplicationCallData->ID = APPLICATION_CALL_ID_INIT;
    }    
    
    return;
}


/****************************************************************************
*
*  ����:   ������ϵͳӦ�ó����ṩ���õĽӿ�
*
*  ApplicationCallID��     Ӧ�õ��ýӿڱ��
*  pParameters��           �����б�
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_INTERFACES_Shutdown()
{
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: L2_ApplicationShutdown \n", __LINE__);
    
}


/****************************************************************************
*
*  ����:   ������ϵͳӦ�ó����ṩ���õĽӿ�
*
*  ApplicationCallID��     Ӧ�õ��ýӿڱ��
*  pParameters��           �����б�
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
VOID L2_INTERFACES_Initial()
{
    pApplicationCallData = APPLICATION_CALL_FLAG_ADDRESS;
    L1_MEMORY_Memset(pApplicationCallData, 0, sizeof(APPLICATION_CALL_DATA));
    pApplicationCallData->ID = APPLICATION_CALL_ID_INIT;
}


