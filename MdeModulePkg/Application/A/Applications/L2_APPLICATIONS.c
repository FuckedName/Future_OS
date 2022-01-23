
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

#include "L2_APPLICATIONS.h"

#include <Global/Global.h>


/****************************************************************************
*
*  描述:   操作系统运行应用程序
*
*  ApplicationCallID：     应用调用接口编号
*  pParameters：           参数列表
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L2_ApplicationRun(UINT8 *pPath)
{
    //1. Read FileName File into Buffer1;
    //2. Analysis Buffer1 ELF and get obj Code1;
    //3. Allocate Memory Buffer2 for Application;
    //4. Copy Code1 into  Buffer2;
    //5. Run Buffer2 after apply cpu time slice;
}



