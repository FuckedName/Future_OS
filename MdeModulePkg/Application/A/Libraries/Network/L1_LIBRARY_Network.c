
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




#include <L1_LIBRARY_Network.h>


// 小端模式
// byte转int  
/****************************************************************************
*
*  描述:   8个字节转换成无符号64位整数
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
unsigned long long L1_NETWORK_8BytesToUINT64(unsigned char *bytes)
{
    unsigned long long s = bytes[0];
    s += ((unsigned long long)bytes[1]) << 8;
    s += ((unsigned long long)bytes[2]) << 16;
    s += ((unsigned long long)bytes[3]) << 24;
    s += ((unsigned long long)bytes[4]) << 32;
    s += ((unsigned long long)bytes[5]) << 40;
    s += ((unsigned long long)bytes[6]) << 48;
    s += ((unsigned long long)bytes[7]) << 56;
    //printf("%llu\n",  s );

    return s;
}



// 小端模式
// byte转int  
/****************************************************************************
*
*  描述:   6个字节转换成无符号64位整数
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
unsigned long long L1_NETWORK_6BytesToUINT64(unsigned char *bytes)
{
    unsigned long long s = bytes[0];
    s += ((unsigned long long)bytes[1]) << 8;
    s += ((unsigned long long)bytes[2]) << 16;
    s += ((unsigned long long)bytes[3]) << 24;
    s += ((unsigned long long)bytes[4]) << 32;
    s += ((unsigned long long)bytes[5]) << 40;
    //printf("%llu\n",  s );

    return s;
}



// 小端模式
// byte转int  
/****************************************************************************
*
*  描述:   4个字节转换成无符号32位整数
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
unsigned long L1_NETWORK_4BytesToUINT32(unsigned char *bytes)
{
    unsigned long Result = bytes[0] & 0xFF;
    Result |= (bytes[1] << 8 & 0xFF00);
    Result |= ((bytes[2] << 16) & 0xFF0000);
    Result |= ((bytes[3] << 24) & 0xFF000000);
    return Result;
}



// 小端模式
// byte转int  
/****************************************************************************
*
*  描述:   3个字节转换成无符号32位整数
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/
unsigned long L1_NETWORK_3BytesToUINT32(unsigned char *bytes)
{
    //INFO_SELF("%x %x %x\n", bytes[0], bytes[1], bytes[2]);
    unsigned long Result = bytes[0] & 0xFF;
    Result |= (bytes[1] << 8 & 0xFF00);
    Result |= ((bytes[2] << 16) & 0xFF0000);
    return Result;
}



// 小端模式
// byte转int  
/****************************************************************************
*
*  描述:   2个字节转换成无符号16位整数
*
*  参数1： xxxxx
*  参数2： xxxxx
*  参数n： xxxxx
*
*  返回值： 成功：XXXX，失败：XXXXX
*
*****************************************************************************/

unsigned int L1_NETWORK_2BytesToUINT16(unsigned char *bytes)
{
    unsigned int Result = bytes[0] & 0xFF;
    Result |= (bytes[1] << 8 & 0xFF00);
    return Result;
}

