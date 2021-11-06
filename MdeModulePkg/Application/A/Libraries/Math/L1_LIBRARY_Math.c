
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





#include <L1_LIBRARY_Math.h>

#define BitSet(x,y) x |= (0x01 << y)
#define BitSlr(x,y) x &= ~(0x01 << y)
#define BitReverse(x,y) x ^= (0x01 << y)
#define BitGet(x,y)  ((x) >> (y) & 0x01)
#define ByteSet(x)    x |= 0xff
#define ByteClear(x)  x |= 0x00


long long L1_MATH_Multi(long x, long y)
{
    return x * y;
}

int L1_MATH_DigitToInteger( int c)
{
  if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))) 
  {  
    /* If c is one of [A-Za-z]... */
    c = L1_STRING_ToUpper(c) - 7;   // Adjust so 'A' is ('9' + 1)
  }
  
  return c - '0';   // Value returned is between 0 and 35, inclusive.
}


// string transform into long long
long long
L1_MATH_StringToLongLong(const char * nptr, char ** endptr, int base)
{
  const char *pEnd;
  long long   Result = 0;
  long long   Previous;
  int         temp;
  BOOLEAN     Negative = FALSE;

  pEnd = nptr;
    
  base = 16; 
  ++nptr;
  ++nptr;
  //DEBUG ((EFI_D_INFO, "line:%d device->StartSectorNumber: %a base: %d \n", __LINE__, nptr, base));

  while( L1_STRING_IsAllNumber(*nptr) && ((temp = L1_MATH_DigitToInteger(*nptr)) < base)) 
  {
    //DEBUG ((EFI_D_INFO, "line:%d temp: %d, Result: %d \n", __LINE__, temp, Result));
    Previous = Result;
    Result = L1_MATH_Multi (Result, base) + (long long int)temp;
    if( Result <= Previous) 
    {   // Detect Overflow
      if(Negative) 
      {
        //DEBUG ((EFI_D_INFO, "line:%d Negative!!!!\n", __LINE__, temp, Result));
        Result = LLONG_MIN;
      }
      else 
      {
        Result = LLONG_MAX;
      }
      Negative = FALSE;
      break;
    }
    pEnd = ++nptr;
  }

  //DEBUG ((EFI_D_INFO, "line:%d device->StartSectorNumber: %d \n", __LINE__, Result));
  if(Negative) 
  {
    Result = -Result;
  }

  // Save pointer to final sequence
  if(endptr != NULL) 
  {
    *endptr = (char *)pEnd;
  }

  //DEBUG ((EFI_D_INFO, "line:%d temp: %d, Result: %d \n", __LINE__, temp, Result));
  return Result;
}


long L1_MATH_ABS(long v)
{
    return (v < 0) ? -v : v;
}

UINT8 L1_MATH_DayOfWeek(int y, int m, int d)
{
    UINT8 T;
    
    if ( m < 3 )
    {
        m += 12;
        y -= 1;
    }
    
    return ( (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400 + 1) % 7 + 7) % 7;        
}

