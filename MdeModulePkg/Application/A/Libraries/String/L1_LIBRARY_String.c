
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





#include <L1_LIBRARY_String.h>

unsigned char AsciiBuffer[0x100] = {0};



/****************************************************************************
*
*  描述:   字符串长度计算
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
unsigned long long L1_STRING_Length(char *String)
{
    unsigned long long  Length;
    for (Length = 0; *String != '\0'; String++, Length++) ;
	
    return Length;
}

L1_STRING_AsciiStringToWchar(CHAR8          *pSource, unsigned short *pDest)
{
    if (pSource == NULL || pDest == NULL)
    {
        return NULL;
    }

    unsigned long long  i;
    for (i = 0; *pSource != '\0'; pSource++, i++)
    {
        pDest[i] = pSource[i];
    }
}


/****************************************************************************
*
*  描述:   字符串比较
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
int L1_STRING_Compare(unsigned char *p1, unsigned char *p2, unsigned int length)
{
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: StrCmpSelf\n", __LINE__);
    
    for (int i = 0; i < length; i++)
    {
        //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%02X %02X ", p1[i], p2[i]);
        if (p1[i] != p2[i])
            return -1;
    }
    return 0;
}




/****************************************************************************
*
*  描述:   字符是否是数字、大小写字母
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
int L1_STRING_IsAllNumber (int c)
{
  return ((('0' <= (c)) && ((c) <= '9')) ||
          (('a' <= (c)) && ((c) <= 'z')) ||
          (('A' <= (c)) && ((c) <= 'Z')));
}





/****************************************************************************
*
*  描述:   小写字母转成大写字母
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
int L1_STRING_ToUpper( int c  )
{
	if ( (c >= 'a') && (c <= 'z') ) 
	{
		c = c - ('a' - 'A');
	}
  	return c;
}



/****************************************************************************
*
*  描述:   字符是否是空白字符
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
int L1_STRING_IsSpace (int c)
{
  return ((c) == ' ') || ((c) == '\t') || ((c) == '\r') || ((c) == '\n') || ((c) == '\v')  || ((c) == '\f');
}




/****************************************************************************
*
*  描述:   小数转成字符串
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
char *L1_STRING_FloatToString(float val, int precision, char *buf)
{
    char *cur, *end;
    float temp = 0;
    int count = 0;

    if (val == 0)
    {
        buf[count++] = '0';
        buf[count++] = '\0';
        return;
    }

    if (val > 0)
    {
    	buf[count++] = (int)val + '0'; 
    }

    buf[count++] = '.'; 

    temp = val - (int)val;
    //printf("%d: %f\n", __LINE__, temp);

    while(precision--) 
    {
        temp = temp * 10;
        buf[count++] = (int)temp + '0';
        temp = temp - (int)temp;
    }
    return buf;
}




