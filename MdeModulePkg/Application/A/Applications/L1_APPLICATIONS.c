
/*************************************************
    .
    File name:      	*.*
    Author：	        	任启红
    ID：					00001
    Date:          		202201
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

#include "L1_APPLICATIONS.h"

#include <Global/Global.h>


/****************************************************************************
*
*  描述:   操作系统运行应用程序
*
*  pELF_Buffer     ELF文件缓冲内存存储
*  pReturnCode：    返回从ELF文件解析的机器码
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
EFI_STATUS L2_ApplicationAnalysisELF(UINT8 *pELF_Buffer, UINT8 *pReturnCode)
{
    UINT8 j = 0;
    for(UINT16 i = 64; i < 64 + 0x1f; i++)
        pReturnCode[j++] = pELF_Buffer[i];
        
}



