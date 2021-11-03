#include <L1_LIBRARY_Memory.h>
/*************************************************
    .
    File name:      	A.c
    Author：	        任启红
    ID：		00001
    Version: Date:  	202107
    Description:    	整个模块的主入口函数
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




#include <Library/BaseLib.h>


VOID L1_MEMORY_Memset(void *s, UINT8 c, UINT32 n)
{
  UINT8 *d;

  d = s;

  while (n-- != 0) 
  {
    *d++ = c;
  }

  return s;
}


//InternalMemSetMem
void L1_MEMORY_SetValue(UINT8 *pBuffer, UINT32 Length, UINT8 Value)
{
    //
  // Declare the local variables that actually move the data elements as
  // volatile to prevent the optimizer from replacing this function with
  // the intrinsic memset()
  //
  volatile UINT8                    *Pointer8;
  volatile UINT32                   *Pointer32;
  volatile UINT64                   *Pointer64;
  UINT32                            Value32;
  UINT64                            Value64;

  if ((((UINTN)pBuffer & 0x7) == 0) && (Length >= 8)) 
  {
    // Generate the 64bit value
    Value32 = (Value << 24) | (Value << 16) | (Value << 8) | Value;
    Value64 = LShiftU64 (Value32, 32) | Value32;

    Pointer64 = (UINT64*)pBuffer;
    while (Length >= 8) 
    {
      *(Pointer64++) = Value64;
      Length -= 8;
    }

    // Finish with bytes if needed
    Pointer8 = (UINT8*)Pointer64;
  } 
  else if ((((UINTN)pBuffer & 0x3) == 0) && (Length >= 4)) 
  {
    // Generate the 32bit value
    Value32 = (Value << 24) | (Value << 16) | (Value << 8) | Value;

    Pointer32 = (UINT32*)pBuffer;
    while (Length >= 4) 
    {
      *(Pointer32++) = Value32;
      Length -= 4;
    }

    // Finish with bytes if needed
    Pointer8 = (UINT8*)Pointer32;
  } 
  else 
  {
    Pointer8 = (UINT8*)pBuffer;
  }
  
  while (Length-- > 0) 
  {
    *(Pointer8++) = Value;
  }
  return pBuffer;
}

void *L1_MEMORY_Copy(UINT8 *dest, const UINT8 *src, UINT8 count)
{
    UINT8 *d;
    const UINT8 *s;

    d = dest;
    s = src;
    while (count--)
        *d++ = *s++;        
    
    return dest;
}

