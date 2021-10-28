
#include <L1_LIBRARY_String.h>

unsigned long long L1_STRING_Length(char *String)
{
    unsigned long long  Length;
    for (Length = 0; *String != '\0'; String++, Length++) ;
	
    return Length;
}

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


int L1_STRING_IsAllNumber (int c)
{
  return ((('0' <= (c)) && ((c) <= '9')) ||
          (('a' <= (c)) && ((c) <= 'z')) ||
          (('A' <= (c)) && ((c) <= 'Z')));
}


int L1_STRING_ToUpper( int c  )
{
	if ( (c >= 'a') && (c <= 'z') ) 
	{
		c = c - ('a' - 'A');
	}
  	return c;
}

int L1_STRING_IsSpace (int c)
{
  return ((c) == ' ') || ((c) == '\t') || ((c) == '\r') || ((c) == '\n') || ((c) == '\v')  || ((c) == '\f');
}


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

