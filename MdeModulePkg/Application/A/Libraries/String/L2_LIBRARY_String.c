
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




#include <Library/PrintLib.h>


#include <L2_LIBRARY_String.h>

#include <Global/Global.h>

#include <Graphics/L1_GRAPHICS.h>


/****************************************************************************
*
*  描述:   生成字符串
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L2_STRING_Maker (UINT16 x, UINT16 y,
  IN  CONST CHAR8   *Format,
  IN  VA_LIST       VaList
  )
{
    
    
    // Note this api do not supported ("%f", float)
    AsciiVSPrint (AsciiBuffer, sizeof (AsciiBuffer), Format, VaList);
	
}



/****************************************************************************
*
*  描述:   生成字符串
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L2_STRING_Maker2 (UINT16 x, UINT16 y, WINDOW_LAYER_ITEM layer,
  IN  CONST CHAR8   *Format,
  IN  VA_LIST       VaList
  )
{

    //ASSERT (Format != NULL);

    AsciiVSPrint (AsciiBuffer, sizeof (AsciiBuffer), Format, VaList);    
}




/****************************************************************************
*
*  描述:   生成字符串
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
VOID L2_STRING_Maker3 (UINT16 x, UINT16 y, WINDOW_LAYER_ITEM layer,
  IN  CONST CHAR8   *Format,
  IN  VA_LIST       VaList
  )
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    UINT32 i = 0;
    
    for (i = 0; i < 0x100; i++)
        AsciiBuffer[i] = 0;
        
    Color.Blue = 0x00;
    Color.Red = 0x00;
    Color.Green = 0x00;
    Color.Reserved = layer.LayerID;

    //ASSERT (Format != NULL);

    AsciiVSPrint (AsciiBuffer, sizeof (AsciiBuffer), Format, VaList);

	//L1_QUEUE_In(&Queue, AsciiBuffer, (sizeof(AsciiBuffer) /sizeof(CHAR8)));

	UINT16 PrintLineCount = Queue.LineCount > 40 ? 40 : Queue.LineCount;

	for (UINT16 line = 0; line < PrintLineCount; line++)
	{
		UINT16 temp = (line + Queue.Front) % Queue.Size;
		for (i = 0; i < sizeof(Queue.Buffer[temp]) / sizeof(CHAR8); i++)
    	{
    		L2_GRAPHICS_AsciiCharDraw2(layer, i * 8, line * 16, Queue.Buffer[temp][i], Color);
	    }
	}

}


