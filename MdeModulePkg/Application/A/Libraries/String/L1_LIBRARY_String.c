
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





#include <L1_LIBRARY_String.h>

unsigned char AsciiBuffer[0x100] = {0};



/****************************************************************************
*
*  ����:   �ַ������ȼ���
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
unsigned long long L1_STRING_Length(char *String)
{
    unsigned long long  Length;
    for (Length = 0; *String != '\0'; String++, Length++) ;
	
    return Length;
}



/****************************************************************************
*
*  ����:  ��һ���ַ�������
*
*  ����1�� ʾ����"/OS/resource/zhufeng.bmp"������/OS��ָϵͳĿ¼
*  ����2�� pBuffer
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
UINT16 L1_STRING_Reverse(UINT8 *s)
{
    UINT16 len = AsciiStrLen(s);
    UINT16 i;
    UINT16 t;
    for (i = 0; i < len / 2; i++)
    {
        t = s[i];
        s[i] = s[len - 1 - i];
        s[len - 1 - i] = t;
    }
}



/****************************************************************************
*
*  ����:  �ѵ��ֽ��ַ�����ת����˫�ֽ��ַ���
*
*  ����1�� ʾ����"/OS/resource/zhufeng.bmp"������/OS��ָϵͳĿ¼
*  ����2�� pBuffer
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
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
*  ����:  ��Сд�ַ���Ϊ��д
*
*  ����1�� 
*  ����2�� 
*  ����n�� 
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
char L1_STRING_UpperCaseString(char c)
{
	if ('a' <= c && c <= 'z') 
	{
		return (c - 'a' + 'A');
	}

	return c;
}


/**
  Return if the name char is valid.

  @param[in] NameChar    The name char to be checked.

  @retval TRUE   The name char is valid.
  @retval FALSE  The name char is invalid.
**/
BOOLEAN L1_STRING_IsValidNameChar (CHAR8      NameChar)
{
  if (NameChar >= 'a' && NameChar <= 'z') 
  {
    return TRUE;
  }
  
  if (NameChar >= 'A' && NameChar <= 'Z') 
  {
    return TRUE;
  }
  
  if (NameChar >= '0' && NameChar <= '9') 
  {
    return TRUE;
  }
  
  if (NameChar == '_') 
  {
    return TRUE;
  }
  
  if (NameChar == '.') 
  {
    return TRUE;
  }
  return FALSE;
}


/****************************************************************************
*
*  ����:   �ַ����Ƚ�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
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
*  ����:   �ַ����Ƚ�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
int L1_STRING_Compare2(unsigned char *p1, unsigned char *p2)
{
	while ((*p1 != '\0') && (*p1 == *p2))
	{
		p1++;
		p2++;
	}
	
    return *p1 - *p2;
}



/****************************************************************************
*
*  ����:   �ַ��Ƿ������֡���Сд��ĸ
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
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
*  ����:   Сд��ĸת�ɴ�д��ĸ
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
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
*  ����:   �ַ��Ƿ��ǿհ��ַ�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
int L1_STRING_IsSpace (int c)
{
  return ((c) == ' ') || ((c) == '\t') || ((c) == '\r') || ((c) == '\n') || ((c) == '\v')  || ((c) == '\f');
}




/****************************************************************************
*
*  ����:   С��ת���ַ���
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
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


char *L1_STRING_ParametersGet()
{
}


